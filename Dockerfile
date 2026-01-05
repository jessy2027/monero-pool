# =============================================================================
# Monero Pool - Multi-stage Docker Build
# =============================================================================
# This Dockerfile builds monero-pool with all dependencies in a Linux container
# Compatible with Docker Desktop for Windows
# =============================================================================

# -----------------------------------------------------------------------------
# Stage 1: Build Monero from source
# -----------------------------------------------------------------------------
FROM ubuntu:22.04 AS monero-builder

ARG MONERO_VERSION=v0.18.3.4
ARG DEBIAN_FRONTEND=noninteractive

# Install Monero build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libboost-all-dev \
    libssl-dev \
    libzmq3-dev \
    libunbound-dev \
    libsodium-dev \
    libreadline-dev \
    libhidapi-dev \
    libusb-1.0-0-dev \
    libprotobuf-dev \
    protobuf-compiler \
    ccache \
    && rm -rf /var/lib/apt/lists/*

# Clone and build Monero
WORKDIR /build
RUN git clone --recursive --depth 1 --branch ${MONERO_VERSION} \
    https://github.com/monero-project/monero.git

WORKDIR /build/monero
RUN make -j4 release

# -----------------------------------------------------------------------------
# Stage 2: Build monero-pool
# -----------------------------------------------------------------------------
FROM ubuntu:22.04 AS pool-builder

ARG DEBIAN_FRONTEND=noninteractive

# Install pool build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    liblmdb-dev \
    libevent-dev \
    libjson-c-dev \
    uuid-dev \
    libssl-dev \
    libsodium-dev \
    libboost-all-dev \
    libunbound-dev \
    libhidapi-dev \
    xxd \
    && rm -rf /var/lib/apt/lists/*

# Copy Monero build artifacts from previous stage
COPY --from=monero-builder /build/monero /monero

# Find the actual build path and create symlink to expected location
# The Makefile expects /monero/build/Linux/master/release but Monero builds to /monero/build/Linux/<branch>/release
RUN mkdir -p /monero/build/Linux/master && \
    BUILD_DIR=$(find /monero/build -type d -name "release" -path "*/Linux/*" | head -1) && \
    if [ -n "$BUILD_DIR" ]; then \
        ln -sf "$BUILD_DIR" /monero/build/Linux/master/release; \
    else \
        echo "Monero build directory not found, listing structure:" && \
        find /monero/build -type d | head -50 && \
        exit 1; \
    fi

# Set environment for pool build
ENV MONERO_ROOT=/monero
ENV MONERO_BUILD_ROOT=/monero/build/Linux/master/release

# Copy pool source code
WORKDIR /pool
COPY . .

# Build the pool
RUN make release

# -----------------------------------------------------------------------------
# Stage 3: Runtime image (minimal)
# -----------------------------------------------------------------------------
FROM ubuntu:22.04 AS runtime

ARG DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies only
RUN apt-get update && apt-get install -y --no-install-recommends \
    liblmdb0 \
    libevent-2.1-7 \
    libevent-pthreads-2.1-7 \
    libjson-c5 \
    libuuid1 \
    libssl3 \
    libsodium23 \
    libunbound8 \
    libhidapi-libusb0 \
    libboost-system1.74.0 \
    libboost-filesystem1.74.0 \
    libboost-thread1.74.0 \
    libboost-date-time1.74.0 \
    libboost-chrono1.74.0 \
    libboost-regex1.74.0 \
    libboost-serialization1.74.0 \
    libboost-program-options1.74.0 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Create non-root user for security
RUN useradd -m -s /bin/bash pool

# Create data directories
RUN mkdir -p /app/data /app/config && chown -R pool:pool /app

# Copy pool binary from builder
COPY --from=pool-builder /pool/build/release/monero-pool /app/
COPY --from=pool-builder /pool/build/release/pool.conf /app/config/pool.conf.example

# Set working directory
WORKDIR /app

# Switch to non-root user
USER pool

# Expose ports
# 4242 - Stratum (miners connect here)
# 80   - Web UI
EXPOSE 4242 80

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
    CMD curl -f http://localhost:80/ || exit 1

# Default command
CMD ["./monero-pool", "--config-file", "/app/config/pool.conf", "--data-dir", "/app/data"]
