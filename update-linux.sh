#!/bin/bash
set -e

# =============================================================================
# Monero Pool - Linux Update Script
# =============================================================================

echo ""
echo "============================================"
echo "  Monero Pool - Update / Redeploy"
echo "============================================"
echo ""

# Check environment
# Enforce .env
echo "[0/6] Enforcing environment..."
echo "DATA_ROOT=/opt/monero-pool" > .env
echo "   Enforced .env file with DATA_ROOT=/opt/monero-pool"

# [1/6] Pulling latest code...
echo "[1/6] Pulling latest code from git..."
git pull || echo "WARNING: Git pull failed. Continuing..."
echo ""

# Menu
echo "Select what to rebuild and deploy:"
echo "   [A] ALL services (monerod, wallet-rpc, pool, lottery, haproxy)"
echo "   [C] CORE only (monerod, monero-wallet-rpc, monero-pool)"
echo "   [P] POOL only (monero-pool)"
echo "   [L] LOTTERY only (lottery-cron)"
echo "   [Q] QUIT"
echo ""
read -p "Choose option [A/C/P/L/Q]: " choice
choice=${choice^^} # uppercase

case $choice in
    Q)
        echo "Cancelled."
        exit 0
        ;;
    A)
        echo "[2/6] Rebuilding ALL services..."
        docker compose build --no-cache monerod monero-wallet-rpc monero-pool
        docker compose --profile lottery build --no-cache lottery-cron || true
        docker compose --profile ssl build --no-cache haproxy || true
        
        echo "[3/6] Restarting ALL services..."
        docker compose up -d monerod monero-wallet-rpc monero-pool
        docker compose --profile lottery up -d lottery-cron
        # Only start haproxy if profile active or certs exist? 
        # Compose handles profile logic.
        docker compose --profile ssl up -d haproxy 2>/dev/null || true
        ;;
    C)
        echo "[2/6] Rebuilding CORE services..."
        docker compose build --no-cache monerod monero-wallet-rpc monero-pool
        
        echo "[3/6] Restarting CORE services..."
        docker compose up -d monerod monero-wallet-rpc monero-pool
        ;;
    P)
        echo "[2/6] Rebuilding Pool..."
        docker compose build --no-cache monero-pool
        
        echo "[3/6] Restarting Pool..."
        docker compose up -d monero-pool
        ;;
    L)
        echo "[2/6] Rebuilding Lottery..."
        docker compose --profile lottery build --no-cache lottery-cron
        
        echo "[3/6] Restarting Lottery..."
        docker compose --profile lottery up -d lottery-cron
        ;;
    *)
        echo "Invalid option."
        exit 1
        ;;
esac

echo ""
echo "[4/6] Waiting for services..."
sleep 5

echo "[5/6] Status:"
docker compose ps

echo ""
echo "============================================"
echo "  Update Complete!"
echo "============================================"
echo "Useful commands:"
echo "  docker compose logs -f monero-pool"
echo ""
