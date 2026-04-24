# syntax=docker/dockerfile:1.6
ARG ALPINE_VERSION=3.22

############################################
# Build stage: toolchain + static builds
############################################
FROM alpine:${ALPINE_VERSION} AS build

# Pinned versions (override with --build-arg ...)
ARG OPENSSL_VERSION=3.5.4
ARG LIBCPUCYCLES_VERSION=20240318
ARG LIBRANDOMBYTES_VERSION=20240318
ARG LIBMCELIECE_VERSION=20250507
# OpenSSH built from local sources with Classic McEliece support

# Install only what's needed to build from source.
# - build-base: gcc, g++, make, libc headers
# - linux-headers: required by some builds
# - perl: OpenSSL build system requires it
# - zlib-dev + zlib-static: OpenSSH compression support, statically
# - curl + ca-certificates + tar: fetch and unpack sources
RUN apk add --no-cache \
      bash \
      build-base \
      linux-headers \
      perl \
      python3 \
      valgrind-dev \
      zlib-dev zlib-static \
      curl ca-certificates tar \
      pkgconf \
      autoconf \
      automake \
      libtool

WORKDIR /src

# -------------------------
# Build OpenSSL (static)
# -------------------------
# We build static libs and disable OpenSSL "modules" to avoid runtime-loaded provider .so files.
# This yields libcrypto.a / libssl.a suitable for fully static linking.
RUN set -eux; \
    OPENSSL_TGZ="openssl-${OPENSSL_VERSION}.tar.gz"; \
    ( curl -fsSLo "${OPENSSL_TGZ}" "https://www.openssl.org/source/${OPENSSL_TGZ}" \
      || curl -fsSLo "${OPENSSL_TGZ}" "https://github.com/openssl/openssl/releases/download/openssl-${OPENSSL_VERSION}/${OPENSSL_TGZ}" ); \
    tar -xzf "${OPENSSL_TGZ}"; \
    cd "openssl-${OPENSSL_VERSION}"; \
    ./Configure linux-x86_64 \
      no-shared no-module no-asm no-shared \
      --prefix=/opt/openssl \
      --openssldir=/opt/openssl/ssl; \
    make -j"$(nproc)"; \
    make install_sw; \
    # Remove any stray shared objects if they appear (defensive).
    find /opt/openssl -type f \( -name '*.so' -o -name '*.so.*' \) -delete; \
    ( cd /opt/openssl && ln -s lib64 lib ); \
    # Sanity check: static libs should exist.
    test -f /opt/openssl/lib/libcrypto.a; \
    test -f /opt/openssl/lib/libssl.a

# -------------------------
# Build libcpucycles (static)
# -------------------------
# Required dependency for libmceliece
RUN set -eux; \
    curl -fsSLo "libcpucycles-${LIBCPUCYCLES_VERSION}.tar.gz" \
      "https://cpucycles.cr.yp.to/libcpucycles-${LIBCPUCYCLES_VERSION}.tar.gz"; \
    tar -xzf "libcpucycles-${LIBCPUCYCLES_VERSION}.tar.gz"; \
    cd "libcpucycles-${LIBCPUCYCLES_VERSION}"; \
    ./configure --prefix=/opt/libcpucycles; \
    make -j"$(nproc)"; \
    make install; \
    # Remove shared objects, keep only static
    find /opt/libcpucycles -type f \( -name '*.so' -o -name '*.so.*' \) -delete; \
    # Sanity check
    test -f /opt/libcpucycles/lib/libcpucycles.a

# -------------------------
# Build librandombytes (static)
# -------------------------
# Required dependency for libmceliece
RUN set -eux; \
    curl -fsSLo "librandombytes-${LIBRANDOMBYTES_VERSION}.tar.gz" \
      "https://randombytes.cr.yp.to/librandombytes-${LIBRANDOMBYTES_VERSION}.tar.gz"; \
    tar -xzf "librandombytes-${LIBRANDOMBYTES_VERSION}.tar.gz"; \
    cd "librandombytes-${LIBRANDOMBYTES_VERSION}"; \
    export CPATH="/opt/openssl/include"; \
    export LIBRARY_PATH="/opt/openssl/lib"; \
    ./configure --prefix=/opt/librandombytes; \
    make -j"$(nproc)"; \
    make install; \
    # Remove shared objects, keep only static
    find /opt/librandombytes -type f \( -name '*.so' -o -name '*.so.*' \) -delete; \
    # Sanity check
    test -f /opt/librandombytes/lib/librandombytes.a

# -------------------------
# Build libmceliece (static)
# -------------------------
# Post-quantum cryptography library using Classic McEliece
RUN set -eux; \
    curl -fsSLo "libmceliece-${LIBMCELIECE_VERSION}.tar.gz" \
      "https://lib.mceliece.org/libmceliece-${LIBMCELIECE_VERSION}.tar.gz"; \
    tar -xzf "libmceliece-${LIBMCELIECE_VERSION}.tar.gz"; \
    cd "libmceliece-${LIBMCELIECE_VERSION}"; \
    export CPATH="/opt/libcpucycles/include:/opt/librandombytes/include"; \
    export LIBRARY_PATH="/opt/libcpucycles/lib:/opt/librandombytes/lib"; \
    ./configure --prefix=/opt/libmceliece; \
    make -j"$(nproc)"; \
    make install; \
    # Remove shared objects, keep only static
    find /opt/libmceliece -type f -name '*.so'  -delete; \
    find /opt/libmceliece -type f -name '*.so.*' -delete; \
    # Sanity check
    test -f /opt/libmceliece/lib/libmceliece.a

# -------------------------
# Build OpenSSH (static) with Classic McEliece support
# -------------------------
# Copy local OpenSSH sources with Classic McEliece modifications
COPY . /src/openssh

# Configure as a minimal client-focused build:
# - no PAM / Kerberos / libedit
# - link statically against OpenSSL + zlib + libmceliece
RUN set -eux; \
    cd /src/openssh; \
    export CPPFLAGS="-I/opt/openssl/include -I/opt/libmceliece/include"; \
    export LDFLAGS="-L/opt/openssl/lib -L/opt/libmceliece/lib -L/opt/libcpucycles/lib -L/opt/librandombytes/lib -static"; \
    export LIBS="-lz -lmceliece -lcpucycles -lrandombytes"; \
    autoreconf -fi; \
    ./configure \
      --prefix=/opt/openssh \
      --sysconfdir=/opt/openssh/etc \
      --with-ssl-dir=/opt/openssl \
      --with-zlib \
      --without-pam \
      --without-kerberos5 \
      --without-libedit \
      --with-libmceliece=/opt/libmceliece; \
    make -j"$(nproc)"; \
    # Strip to reduce size (best effort; strip is in build-base/binutils)
    strip ssh ssh-keygen ssh-agent ssh-add ssh-keyscan scp sftp sshd || true; \
    make -j install

# Verify static linkage (build-time check)
RUN set -eux; \
    file /opt/openssh/bin/ssh | grep -qi "statically linked"; \
    (ldd /opt/openssh/bin/ssh && exit 1) || true

############################################
# Runtime stage: minimal Alpine + ssh client
############################################
FROM alpine:${ALPINE_VERSION} AS runtime

# CA bundle is useful if you later add tools that do HTTPS (optional for ssh itself).
RUN apk add --no-cache ca-certificates
RUN apk add --no-cache bash
# RUN apk add --no-cache build-base autoconf automake libtool
# RUN apk add --no-cache bash build-base linux-headers perl python3 valgrind-dev zlib-dev zlib-static curl tar pkgconf

# Copy the openssh installation from build stage (binaries + config dir structure)
COPY --from=build /opt/openssh /opt/openssh

# Create keys directory and update config - all under /opt
RUN set -eux; \
    mkdir -p /opt/openssh/keys; \
    chmod 700 /opt/openssh/keys; \
    printf '%s\n' \
      'Host *' \
      '  ServerAliveInterval 30' \
      '  ServerAliveCountMax 4' \
      '  HashKnownHosts yes' \
      '  UserKnownHostsFile /opt/openssh/keys/known_hosts' \
      '  IdentityFile /opt/openssh/keys/id_rsa' \
      '  IdentityFile /opt/openssh/keys/id_ecdsa' \
      '  IdentityFile /opt/openssh/keys/id_ed25519' \
      > /opt/openssh/etc/ssh_config

RUN set -eux; \
    mkdir -p /root/.ssh; \
    chmod 700 /root/.ssh; \
    rm -vf /root/.ssh/authorized_keys; \
    /opt/openssh/bin/ssh-keygen -t rsa -b 4096 -f /root/.ssh/id_rsa; \
    cat /root/.ssh/id_rsa.pub >>/root/.ssh/authorized_keys; \
    /opt/openssh/bin/ssh-keygen -t ed25519 -f /root/.ssh/id_ed25519; \
    cat /root/.ssh/id_ed25519.pub >>/root/.ssh/authorized_keys; \
    chmod 600 /root/.ssh/authorized_keys; \
    mkdir -p /opt/openssh/keys; \
    cp -av /root/.ssh/id_rsa /root/.ssh/id_ed25519 /opt/openssh/keys;

# /opt/openssh/sbin/sshd -ddd -e -D -p 1222
# /opt/openssh/bin/ssh -v -p 1222 localhost -oKexAlgorithms=mceliece8192128x25519-sha512@openssh.com date
# Default to ssh client
# ENTRYPOINT ["/opt/openssh/bin/ssh"]
# CMD ["-V"]
# Default to bash
ENTRYPOINT ["/bin/bash"]

