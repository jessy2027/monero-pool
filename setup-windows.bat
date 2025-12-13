@echo off
REM =============================================================================
REM Monero Pool - Windows Setup Script
REM =============================================================================
REM This script creates the required directory structure and prepares
REM the configuration for Docker deployment.
REM
REM Run this script as Administrator!
REM =============================================================================

echo.
echo ============================================
echo   Monero Pool - Docker Setup for Windows
echo ============================================
echo.

REM Create main directory
echo [1/5] Creating main directory structure...
if not exist "C:\MoneroPool" mkdir "C:\MoneroPool"
if not exist "C:\MoneroPool\blockchain" mkdir "C:\MoneroPool\blockchain"
if not exist "C:\MoneroPool\wallet" mkdir "C:\MoneroPool\wallet"
if not exist "C:\MoneroPool\pool-data" mkdir "C:\MoneroPool\pool-data"
if not exist "C:\MoneroPool\config" mkdir "C:\MoneroPool\config"
if not exist "C:\MoneroPool\config\certs" mkdir "C:\MoneroPool\config\certs"
if not exist "C:\MoneroPool\backups" mkdir "C:\MoneroPool\backups"
echo    Done!
echo.

REM Copy configuration
echo [2/5] Copying configuration files...
if not exist "C:\MoneroPool\config\pool.conf" (
    copy "%~dp0pool.docker.conf" "C:\MoneroPool\config\pool.conf"
    echo    Configuration copied to C:\MoneroPool\config\pool.conf
    echo    IMPORTANT: Edit this file and set your wallet address!
) else (
    echo    Pool configuration already exists, skipping...
)

REM Copy HAProxy config
if not exist "C:\MoneroPool\config\haproxy.cfg" (
    if exist "%~dp0haproxy.cfg" (
        copy "%~dp0haproxy.cfg" "C:\MoneroPool\config\haproxy.cfg"
        echo    HAProxy config copied to C:\MoneroPool\config\haproxy.cfg
    )
) else (
    echo    HAProxy configuration already exists, skipping...
)

REM Create wallet password file template
if not exist "C:\MoneroPool\config\wallet-password.txt" (
    echo CHANGEZ_CE_MOT_DE_PASSE> "C:\MoneroPool\config\wallet-password.txt"
    echo    Wallet password file created: C:\MoneroPool\config\wallet-password.txt
    echo    IMPORTANT: Edit this file and set a secure password!
) else (
    echo    Wallet password file already exists, skipping...
)
echo.

REM Check Docker
echo [3/5] Checking Docker Desktop...
docker --version >nul 2>&1
if errorlevel 1 (
    echo    ERROR: Docker is not installed or not running!
    echo    Please install Docker Desktop from: https://www.docker.com/products/docker-desktop/
    echo.
    pause
    exit /b 1
)
echo    Docker is installed and running.
echo.

REM Build the pool image
echo [4/5] Building Monero Pool image...
echo    This may take 15-30 minutes on first build...
echo.
cd /d "%~dp0"
docker-compose build
if errorlevel 1 (
    echo    ERROR: Failed to build Docker images!
    pause
    exit /b 1
)
echo    Build complete!
echo.

REM Final instructions
echo [5/5] Setup Complete!
echo.
echo ============================================
echo   NEXT STEPS:
echo ============================================
echo.
echo 1. Edit your pool configuration:
echo    notepad C:\MoneroPool\config\pool.conf
echo.
echo 2. Set your Monero wallet address in pool.conf:
echo    pool-wallet = YOUR_WALLET_ADDRESS_HERE
echo.
echo 3. Start the pool:
echo    docker-compose up -d
echo.
echo 4. View logs:
echo    docker-compose logs -f
echo.
echo 5. Access Web UI:
echo    http://localhost:80 or http://euroxmr.eu
echo.
echo 6. Miners connect to:
echo    stratum+tcp://euroxmr.eu:4242
echo    stratum+ssl://euroxmr.eu:4343 (SSL)
echo.
echo ============================================
echo   SSL SETUP (Optional):
echo ============================================
echo    1. Place your certificate in:
echo       C:\MoneroPool\config\certs\euroxmr.pem
echo    2. Start with SSL:
echo       docker-compose --profile ssl up -d
echo ============================================
echo.
echo ============================================
echo   DATA LOCATIONS (for backup):
echo ============================================
echo    Pool Data:   C:\MoneroPool\pool-data
echo    Wallet:      C:\MoneroPool\wallet
echo    Blockchain:  C:\MoneroPool\blockchain
echo    Config:      C:\MoneroPool\config
echo    SSL Certs:   C:\MoneroPool\config\certs
echo ============================================
echo.
pause

