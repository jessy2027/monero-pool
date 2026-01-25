@echo off
REM =============================================================================
REM Monero Pool - Windows Setup Script
REM =============================================================================
REM This script creates the required directory structure and prepares
REM the configuration for Docker deployment.
REM =============================================================================

echo.
echo ============================================
echo   Monero Pool - Docker Setup for Windows
echo ============================================
echo.

REM [0/5] Check for Administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo    ERROR: This script requires Administrator privileges!
    echo    Please right-click and select "Run as Administrator".
    echo.
    pause
    exit /b 1
)

REM [1/5] Creating main directory structure...
echo [1/5] Creating main directory structure...
if not exist "C:\MoneroPool" mkdir "C:\MoneroPool"
if not exist "C:\MoneroPool\blockchain" mkdir "C:\MoneroPool\blockchain"
if not exist "C:\MoneroPool\wallet" mkdir "C:\MoneroPool\wallet"
if not exist "C:\MoneroPool\pool-data" mkdir "C:\MoneroPool\pool-data"
if not exist "C:\MoneroPool\config" mkdir "C:\MoneroPool\config"
if not exist "C:\MoneroPool\config\certs" mkdir "C:\MoneroPool\config\certs"
if not exist "C:\MoneroPool\backups" mkdir "C:\MoneroPool\backups"
if not exist "C:\MoneroPool\lottery-data" mkdir "C:\MoneroPool\lottery-data"
if not exist "C:\MoneroPool\lottery-output" mkdir "C:\MoneroPool\lottery-output"
if not exist "C:\MoneroPool\tari-data" mkdir "C:\MoneroPool\tari-data"
if not exist "C:\MoneroPool\tari-wallet" mkdir "C:\MoneroPool\tari-wallet"
echo    Done!
echo.
echo.



REM [2/5] Copying configuration files...
echo [2/5] Copying configuration files...
if not exist "C:\MoneroPool\config\pool.conf" (
    copy "%~dp0pool.conf" "C:\MoneroPool\config\pool.conf"
    echo    Configuration copied to C:\MoneroPool\config\pool.conf
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
    echo    Wallet password file created.
) else (
    echo    Wallet password file already exists, skipping...
)
echo.

REM Validate Wallet Address
echo    Checking configuration...
findstr "46hcZEDqKfbEzEcchonDsbHpSKK2AkQ3X2ozg3Je78r7Xx5X4dVnaakVDjKgLU2qiHggadYM9fWcce95uCPNjz1MAJ5CyHU" "C:\MoneroPool\config\pool.conf" >nul
if %errorLevel% equ 0 (
    echo.
    echo    WARNING: You are using the default wallet address in pool.conf!
    echo    Minig rewards will go to the developer/example address.
    echo    Please edit C:\MoneroPool\config\pool.conf immediately after setup.
    echo.
)

REM [3/5] Checking Docker Desktop...
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

REM [4/5] Building Monero Pool images...
echo [4/5] Building Monero Pool images...
echo    This may take 15-30 minutes on first build...
echo.
cd /d "%~dp0"
docker compose -f docker-compose.windows.yml build
echo    Building Lottery service...
docker compose -f docker-compose.windows.yml --profile lottery build
echo    Building SSL Gateway (HAProxy)...
docker compose -f docker-compose.windows.yml --profile ssl build
if errorlevel 1 (
    echo    ERROR: Failed to build Docker images!
    pause
    exit /b 1
)
echo    Build complete!
echo.

REM [5/5] Setup Complete!
echo [5/5] Setup Complete!
echo.
echo ============================================
echo   NEXT STEPS:
echo ============================================
echo.
echo 1. Edit your pool configuration:
echo    notepad C:\MoneroPool\config\pool.conf
echo.
echo 2. Set your Monero wallet address in pool.conf!
echo.
echo 3. Start the pool:
echo    docker compose -f docker-compose.windows.yml up -d
echo.
echo ============================================
pause
