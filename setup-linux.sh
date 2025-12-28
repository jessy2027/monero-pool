#!/bin/bash
set -e

# =============================================================================
# Monero Pool - Linux Setup Script
# =============================================================================

echo ""
echo "============================================"
echo "  Monero Pool - Docker Setup for Linux"
echo "============================================"
echo ""

# Check for root
if [ "$EUID" -ne 0 ]; then
  echo "ERROR: Please run as root (required for /opt/monero-pool installation)"
  exit 1
fi

# Configuration
# Hardcoded installation path
DATA_DIR="/opt/monero-pool"

# [1/5] Checking Docker...
echo "[1/5] Checking Docker..."
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed!"
    echo "Please install Docker and Docker Compose."
    exit 1
fi
echo "   Docker is present."

# [2/5] Creating directory structure...
echo "[2/5] Creating directory structure in $DATA_DIR..."
mkdir -p "$DATA_DIR/blockchain"
mkdir -p "$DATA_DIR/wallet"
mkdir -p "$DATA_DIR/pool-data"
mkdir -p "$DATA_DIR/config/certs"
mkdir -p "$DATA_DIR/backups"
mkdir -p "$DATA_DIR/lottery-data"
mkdir -p "$DATA_DIR/lottery-output"
echo "   Done!"

# Configure .env
echo "   Configuring environment..."
echo "DATA_ROOT=$DATA_DIR" > .env
echo "   Created .env file with DATA_ROOT=$DATA_DIR"

# [3/5] Copying configuration files...
echo "[3/5] Copying configuration files..."
if [ ! -f "$DATA_DIR/config/pool.conf" ]; then
    cp pool.conf "$DATA_DIR/config/pool.conf"
    echo "   Configuration copied to $DATA_DIR/config/pool.conf"
else
    echo "   Pool configuration already exists, skipping..."
fi

if [ ! -f "$DATA_DIR/config/haproxy.cfg" ] && [ -f haproxy.cfg ]; then
    cp haproxy.cfg "$DATA_DIR/config/haproxy.cfg"
    echo "   HAProxy config copied."
fi

if [ ! -f "$DATA_DIR/config/wallet-password.txt" ]; then
    echo "CHANGE_THIS_PASSWORD" > "$DATA_DIR/config/wallet-password.txt"
    echo "   Wallet password file created."
fi

# Validate Wallet Address
if grep -q "46hcZEDqKfbEzEcchonDsbHpSKK2AkQ3X2ozg3Je78r7Xx5X4dVnaakVDjKgLU2qiHggadYM9fWcce95uCPNjz1MAJ5CyHU" "$DATA_DIR/config/pool.conf"; then
    echo ""
    echo "WARNING: You are using the default wallet address in pool.conf!"
    echo "Please edit $DATA_DIR/config/pool.conf immediately."
    echo ""
fi

# [4/5] Building Docker images...
echo "[4/5] Building Monero Pool images..."
echo "   This may take a while..."
docker compose build
echo "   Building Lottery service..."
docker compose --profile lottery build
echo "   Building SSL Gateway (HAProxy)..."
docker compose --profile ssl build

# [5/5] Setup Complete
echo ""
echo "============================================"
echo "  Setup Complete!"
echo "============================================"
echo "1. Edit your pool configuration:"
echo "   nano $DATA_DIR/config/pool.conf"
echo ""
echo "2. Start the pool:"
echo "   docker compose up -d"
echo ""
echo "============================================"
