# ---------------------------
# Build Stage
# ---------------------------
FROM alpine:3.19 AS builder

RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    ncurses-dev \
    linux-headers

WORKDIR /build
COPY . .

RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)


# ---------------------------
# Runtime Stage
# ---------------------------
FROM alpine:3.19

RUN apk add --no-cache \
    ncurses \
    libstdc++ \
    man-db \
    procps

WORKDIR /app

COPY --from=builder /build/build/hawkpro /usr/local/bin/hawkpro

ENTRYPOINT ["hawkpro"]
