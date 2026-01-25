#!/bin/bash
set -e

# =============================================================================
# Monero Pool - Linux Backup Script
# =============================================================================

# Default config
# Hardcoded path
DATA_ROOT="/opt/monero-pool"

# Read from .env only if needed for other vars, but force DATA_ROOT
if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi
# Re-enforce DATA_ROOT just in case
DATA_ROOT="/opt/monero-pool"

BACKUP_DIR="$DATA_ROOT/backups"
TIMESTAMP=$(date +"%Y-%m-%d_%H-%M")
CURRENT_BACKUP="$BACKUP_DIR/backup_$TIMESTAMP"
KEEP_DAYS=7

echo ""
echo "============================================"
echo "  Monero Pool Backup - $TIMESTAMP"
echo "============================================"
echo "  Data Root: $DATA_ROOT"
echo ""

# Create directories
mkdir -p "$BACKUP_DIR"
mkdir -p "$CURRENT_BACKUP"

# Backup functions
backup_dir() {
    src="$1"
    dest="$CURRENT_BACKUP/$2"
    name="$3"

    if [ -d "$src" ]; then
        echo "[*] Backing up $name..."
        mkdir -p "$dest"
        cp -r "$src/"* "$dest/"
    else
        echo "[!] WARNING: $name not found at $src"
    fi
}

# Perform backups
backup_dir "$DATA_ROOT/pool-data" "pool-data" "Pool Data"
backup_dir "$DATA_ROOT/wallet" "wallet" "Wallet"
backup_dir "$DATA_ROOT/config" "config" "Configuration"
backup_dir "$DATA_ROOT/lottery-data" "lottery-data" "Lottery Data"
backup_dir "$DATA_ROOT/tari-data" "tari-data" "Tari Node Data"
backup_dir "$DATA_ROOT/tari-wallet" "tari-wallet" "Tari Wallet"

if [ -d "$DATA_ROOT/lottery-output" ]; then
    mkdir -p "$CURRENT_BACKUP/lottery-output"
    cp -r "$DATA_ROOT/lottery-output/"* "$CURRENT_BACKUP/lottery-output/"
fi

# Create Archive
echo ""
echo "[*] Compressing backup..."
cd "$BACKUP_DIR"
tar -czf "backup_$TIMESTAMP.tar.gz" "backup_$TIMESTAMP"
rm -rf "backup_$TIMESTAMP"

echo "    Created: $BACKUP_DIR/backup_$TIMESTAMP.tar.gz"

# Cleanup old backups
echo ""
echo "[*] Cleaning old backups (> $KEEP_DAYS days)..."
find "$BACKUP_DIR" -name "backup_*.tar.gz" -mtime +$KEEP_DAYS -delete
echo "    Cleanup complete."

echo ""
echo "============================================"
echo "  Backup Complete!"
echo "============================================"
echo ""
