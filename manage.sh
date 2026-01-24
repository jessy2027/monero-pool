#!/bin/bash
set -e

# =============================================================================
# Monero Pool - Unified Management Script
# =============================================================================

# Configuration
DEFAULT_DATA_DIR="/opt/monero-pool"
COMPOSE_FILE="docker-compose.linux.yml"
ENV_FILE=".env"
BACKUP_RETENTION_DAYS=7

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# -----------------------------------------------------------------------------
# Helper Functions
# -----------------------------------------------------------------------------

log_info() { echo -e "${GREEN}[INFO] $1${NC}"; }
log_warn() { echo -e "${YELLOW}[WARN] $1${NC}"; }
log_error() { echo -e "${RED}[ERROR] $1${NC}"; }
log_header() { echo -e "\n${BLUE}=== $1 ===${NC}"; }

check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "This script must be run as root."
        exit 1
    fi
}

load_env() {
    if [ -f "$ENV_FILE" ]; then
        # Load env vars safely
        export $(grep -v '^#' "$ENV_FILE" | xargs)
    fi
    # Set default if not in .env
    export DATA_DIR="${DATA_DIR:-$DEFAULT_DATA_DIR}"
}

check_docker() {
    if ! command -v docker &> /dev/null; then
        log_error "Docker is not installed. Please install Docker and Docker Compose."
        exit 1
    fi
}

# -----------------------------------------------------------------------------
# Subcommands
# -----------------------------------------------------------------------------

cmd_setup() {
    log_header "Setup Monero Pool"
    check_root
    check_docker

    # Prompt for Data Directory
    read -p "Install location [${DEFAULT_DATA_DIR}]: " USER_DIR
    DATA_DIR=${USER_DIR:-$DEFAULT_DATA_DIR}

    # Save to .env
    echo "DATA_DIR=$DATA_DIR" > "$ENV_FILE"
    echo "COMPOSE_FILE=$COMPOSE_FILE" >> "$ENV_FILE"
    log_info "Configuration saved to $ENV_FILE"

    # Create Directories
    log_info "Creating directories in $DATA_DIR..."
    mkdir -p "$DATA_DIR/blockchain"
    mkdir -p "$DATA_DIR/wallet"
    mkdir -p "$DATA_DIR/pool-data"
    mkdir -p "$DATA_DIR/config/certs"
    mkdir -p "$DATA_DIR/backups"
    mkdir -p "$DATA_DIR/lottery-data"
    mkdir -p "$DATA_DIR/lottery-output"

    # Tari directories
    mkdir -p "$DATA_DIR/tari-data"
    mkdir -p "$DATA_DIR/tari-wallet"

    # Copy Configs
    log_info "Copying configuration files..."
    if [ ! -f "$DATA_DIR/config/pool.conf" ]; then
        if [ -f "pool.conf" ]; then
            cp pool.conf "$DATA_DIR/config/pool.conf"
            log_info "Copied pool.conf"
        else
            log_warn "pool.conf not found in current directory!"
        fi
    else
        log_info "pool.conf already exists, skipping..."
    fi

    if [ ! -f "$DATA_DIR/config/haproxy.cfg" ] && [ -f haproxy.cfg ]; then
        cp haproxy.cfg "$DATA_DIR/config/haproxy.cfg"
        log_info "Copied haproxy.cfg"
    fi

    if [ ! -f "$DATA_DIR/config/wallet-password.txt" ]; then
        echo "CHANGE_THIS_PASSWORD" > "$DATA_DIR/config/wallet-password.txt"
        log_info "Created wallet-password.txt placeholder"
    fi

    # Wallet Address Check
    if [ -f "$DATA_DIR/config/pool.conf" ]; then
        if grep -q "46hcZEDqKfbEzEcchonDsbHpSKK2AkQ3X2ozg3Je78r7Xx5X4dVnaakVDjKgLU2qiHggadYM9fWcce95uCPNjz1MAJ5CyHU" "$DATA_DIR/config/pool.conf"; then
            log_warn "You are using the default wallet address in pool.conf!"
            log_warn "Please edit $DATA_DIR/config/pool.conf immediately."
        fi
    fi

    # Build Images
    log_info "Building Docker images..."
    docker compose -f "$COMPOSE_FILE" build

    log_info "Setup complete!"
    log_info "Next steps:"
    echo "  1. Edit config: nano $DATA_DIR/config/pool.conf"
    echo "  2. Create wallet: ./manage.sh create-wallet"
    echo "  3. Start pool: ./manage.sh start"
}

cmd_start() {
    log_header "Starting Monero Pool"
    check_docker
    load_env
    docker compose -f "$COMPOSE_FILE" up -d
    log_info "Services started."
    docker compose -f "$COMPOSE_FILE" ps
}

cmd_start_lottery() {
    log_header "Starting Lottery Service"
    check_docker
    load_env
    docker compose -f "$COMPOSE_FILE" --profile lottery up -d
    log_info "Lottery service started."
}

cmd_start_tari() {
    log_header "Starting Tari Merge Mining Services"
    check_docker
    load_env
    docker compose -f "$COMPOSE_FILE" --profile tari up -d
    log_info "Tari services started."
}

cmd_stop() {
    log_header "Stopping Monero Pool"
    check_docker
    load_env
    docker compose -f "$COMPOSE_FILE" down
    log_info "Services stopped."
}

cmd_restart() {
    cmd_stop
    cmd_start
}

cmd_logs() {
    check_docker
    load_env
    if [ -z "$1" ]; then
        docker compose -f "$COMPOSE_FILE" logs -f --tail=100
    else
        docker compose -f "$COMPOSE_FILE" logs -f --tail=100 "$1"
    fi
}

cmd_create_wallet() {
    log_header "Create Pool Wallet"
    check_docker
    load_env

    if [ -f "$DATA_DIR/wallet/pool-wallet" ]; then
        log_error "Wallet already exists at $DATA_DIR/wallet/pool-wallet"
        read -p "Do you want to overwrite it? (y/N) " confirm
        if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
            exit 0
        fi
    fi

    read -s -p "Enter new wallet password: " WALLET_PASS
    echo ""
    read -s -p "Confirm password: " WALLET_PASS_CONFIRM
    echo ""

    if [ "$WALLET_PASS" != "$WALLET_PASS_CONFIRM" ]; then
        log_error "Passwords do not match!"
        exit 1
    fi

    # Save password to file
    echo "$WALLET_PASS" > "$DATA_DIR/config/wallet-password.txt"

    log_info "Generating wallet..."
    docker compose -f "$COMPOSE_FILE" run --rm monero-wallet-rpc \
        --daemon-host=monerod \
        --generate-new-wallet=/home/monero/wallet/pool-wallet \
        --password="$WALLET_PASS" \
        --command "exit"

    log_info "Wallet created successfully!"
}

cmd_backup() {
    log_header "Backup Monero Pool"
    check_root
    load_env

    TIMESTAMP=$(date +"%Y-%m-%d_%H-%M")
    BACKUP_DIR="$DATA_DIR/backups"
    TEMP_DIR="$BACKUP_DIR/backup_$TIMESTAMP"

    mkdir -p "$TEMP_DIR"

    # Function to copy if exists
    safe_copy() {
        if [ -d "$1" ]; then
            mkdir -p "$2"
            cp -r "$1/." "$2/"
        else
            log_warn "Directory $1 not found, skipping..."
        fi
    }

    log_info "Backing up data..."
    safe_copy "$DATA_DIR/pool-data" "$TEMP_DIR/pool-data"
    safe_copy "$DATA_DIR/wallet" "$TEMP_DIR/wallet"
    safe_copy "$DATA_DIR/config" "$TEMP_DIR/config"
    safe_copy "$DATA_DIR/lottery-data" "$TEMP_DIR/lottery-data"
    safe_copy "$DATA_DIR/lottery-output" "$TEMP_DIR/lottery-output"

    # Backup Tari data
    safe_copy "$DATA_DIR/tari-data" "$TEMP_DIR/tari-data"
    safe_copy "$DATA_DIR/tari-wallet" "$TEMP_DIR/tari-wallet"

    # Compress
    log_info "Compressing..."
    ARCHIVE_NAME="backup_$TIMESTAMP.tar.gz"
    cd "$BACKUP_DIR"
    tar -czf "$ARCHIVE_NAME" "backup_$TIMESTAMP"
    rm -rf "backup_$TIMESTAMP"

    log_info "Backup created: $BACKUP_DIR/$ARCHIVE_NAME"

    # Cleanup
    log_info "Cleaning old backups (>$BACKUP_RETENTION_DAYS days)..."
    find "$BACKUP_DIR" -name "backup_*.tar.gz" -mtime +$BACKUP_RETENTION_DAYS -delete
}

cmd_restore() {
    log_header "Restore Monero Pool"
    check_root
    load_env

    BACKUP_FILE="$1"

    if [ -z "$BACKUP_FILE" ]; then
        log_error "Usage: ./manage.sh restore <backup_file.tar.gz>"
        echo "Available backups:"
        ls -lh "$DATA_DIR/backups/"*.tar.gz 2>/dev/null || echo "No backups found."
        exit 1
    fi

    if [ ! -f "$BACKUP_FILE" ]; then
        # Try finding it in backup dir
        if [ -f "$DATA_DIR/backups/$BACKUP_FILE" ]; then
            BACKUP_FILE="$DATA_DIR/backups/$BACKUP_FILE"
        else
            log_error "Backup file not found: $BACKUP_FILE"
            exit 1
        fi
    fi

    log_warn "This will OVERWRITE current data in $DATA_DIR!"
    log_warn "Services will be stopped."
    read -p "Are you sure? (y/N) " confirm
    if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
        exit 0
    fi

    cmd_stop

    log_info "Restoring from $BACKUP_FILE..."

    # Extract to temp location
    TEMP_RESTORE="/tmp/monero_restore_$(date +%s)"
    mkdir -p "$TEMP_RESTORE"
    tar -xzf "$BACKUP_FILE" -C "$TEMP_RESTORE"

    # Identify the backup folder name (it was inside the tar)
    BACKUP_CONTENT_DIR=$(ls "$TEMP_RESTORE" | head -n 1)
    SOURCE_DIR="$TEMP_RESTORE/$BACKUP_CONTENT_DIR"

    if [ ! -d "$SOURCE_DIR/pool-data" ]; then
        log_error "Invalid backup structure. Aborting."
        rm -rf "$TEMP_RESTORE"
        exit 1
    fi

    # Restore files
    cp -r "$SOURCE_DIR/pool-data/"* "$DATA_DIR/pool-data/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/wallet/"* "$DATA_DIR/wallet/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/config/"* "$DATA_DIR/config/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/lottery-data/"* "$DATA_DIR/lottery-data/" 2>/dev/null || true

    # Restore Tari data
    cp -r "$SOURCE_DIR/tari-data/"* "$DATA_DIR/tari-data/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/tari-wallet/"* "$DATA_DIR/tari-wallet/" 2>/dev/null || true

    rm -rf "$TEMP_RESTORE"
    log_info "Restore complete."

    read -p "Start services now? (y/N) " start_confirm
    if [[ $start_confirm == [yY] || $start_confirm == [yY][eE][sS] ]]; then
        cmd_start
    fi
}

cmd_schedule_backups() {
    log_header "Schedule Daily Backups"
    check_root
    load_env

    SCRIPT_PATH=$(readlink -f "$0")
    CRON_JOB="0 3 * * * $SCRIPT_PATH backup >> $DATA_DIR/backups/backup.log 2>&1"

    # Check if already scheduled
    if crontab -l 2>/dev/null | grep -q "$SCRIPT_PATH backup"; then
        log_warn "Backup already scheduled in crontab."
    else
        (crontab -l 2>/dev/null; echo "$CRON_JOB") | crontab -
        log_info "Added daily backup at 03:00 to crontab."
    fi
}

cmd_update() {
    log_header "Update Monero Pool"
    check_docker
    load_env

    log_info "Pulling latest changes..."
    git pull

    log_info "Rebuilding images..."
    docker compose -f "$COMPOSE_FILE" build

    log_info "Restarting services..."
    docker compose -f "$COMPOSE_FILE" up -d

    log_info "Update complete."
}

# -----------------------------------------------------------------------------
# Main Switch
# -----------------------------------------------------------------------------

case "$1" in
    setup)
        cmd_setup
        ;;
    start)
        cmd_start
        ;;
    start-lottery)
        cmd_start_lottery
        ;;
    start-tari)
        cmd_start_tari
        ;;
    stop)
        cmd_stop
        ;;
    restart)
        cmd_restart
        ;;
    logs)
        cmd_logs "$2"
        ;;
    create-wallet)
        cmd_create_wallet
        ;;
    backup)
        cmd_backup
        ;;
    restore)
        cmd_restore "$2"
        ;;
    schedule-backups)
        cmd_schedule_backups
        ;;
    update)
        cmd_update
        ;;
    *)
        echo -e "${BLUE}Monero Pool Management CLI${NC}"
        echo "Usage: ./manage.sh <command>"
        echo ""
        echo "Commands:"
        echo "  setup             Initial setup of the pool"
        echo "  start             Start the pool services"
        echo "  start-lottery     Start the lottery service"
        echo "  start-tari        Start the Tari merge mining services"
        echo "  stop              Stop all services"
        echo "  restart           Restart services"
        echo "  logs [service]    View logs (optional: monerod, monero-pool)"
        echo "  create-wallet     Interactive wallet creation"
        echo "  backup            Create an immediate backup"
        echo "  restore <file>    Restore from a backup file"
        echo "  schedule-backups  Add daily backup to crontab"
        echo "  update            Git pull, rebuild and restart"
        exit 1
        ;;
esac
