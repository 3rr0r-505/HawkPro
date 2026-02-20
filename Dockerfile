# -------------------------
# Stage 1: Build
# -------------------------
FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libncurses-dev \
    man-db \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# -------------------------
# Stage 2: Runtime
# -------------------------
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libncurses6 \
    man-db \
    && rm -rf /var/lib/apt/lists/*

# Copy binary
COPY --from=builder /build/build/hawkpro /usr/local/bin/hawkpro

# Copy man page (adjust path if needed)
COPY hawkpro.1 /usr/local/share/man/man1/hawkpro.1

RUN mandb >/dev/null 2>&1 || true

# Copy lifecycle manager (same logic as installer)
RUN printf '%s\n' \
'#!/usr/bin/env bash' \
'BINARY="/usr/local/bin/hawkpro"' \
'MANPAGE="/usr/local/share/man/man1/hawkpro.1"' \
'' \
'case "$1" in' \
'  --uninstall)' \
'    rm -f "$BINARY" "$MANPAGE"' \
'    mandb >/dev/null 2>&1 || true' \
'    echo "HawkPro removed."' \
'    ;;' \
'  --install)' \
'    if [[ -f "$BINARY" ]]; then' \
'      echo "HawkPro already installed."' \
'    else' \
'      echo "Binary missing."' \
'    fi' \
'    ;;' \
'  *)' \
'    exec "$BINARY" "$@"' \
'    ;;' \
'esac' \
> /usr/local/bin/hawkproctl \
&& chmod +x /usr/local/bin/hawkproctl

ENTRYPOINT ["hawkproctl"]