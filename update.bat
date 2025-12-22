@echo off
REM =============================================================================
REM Monero Pool - Update Script
REM =============================================================================
REM Use this script after pulling new code from git to rebuild and redeploy
REM Supports deploying all services or individual components
REM =============================================================================

setlocal enabledelayedexpansion

echo.
echo ============================================
echo   Monero Pool - Update / Redeploy
echo ============================================
echo.

cd /d "%~dp0"

echo [1/6] Pulling latest code from git...
git pull
if errorlevel 1 (
    echo    WARNING: Git pull failed. Continuing with local code...
)
echo.

echo ============================================
echo   Select what to rebuild and deploy:
echo ============================================
echo.
echo   [A] ALL services (monerod, wallet-rpc, pool, lottery, haproxy)
echo   [C] CORE only (monerod, monero-wallet-rpc, monero-pool)
echo   [P] POOL only (monero-pool)
echo   [L] LOTTERY only (lottery-cron)
echo   [Q] QUIT
echo.
choice /C ACPLQ /N /M "Choose option [A/C/P/L/Q]: "

if %ERRORLEVEL%==5 (
    echo Cancelled.
    exit /b 0
)
if %ERRORLEVEL%==4 goto :LOTTERY_ONLY
if %ERRORLEVEL%==3 goto :POOL_ONLY
if %ERRORLEVEL%==2 goto :CORE_ONLY
if %ERRORLEVEL%==1 goto :ALL_SERVICES

:ALL_SERVICES
echo.
echo [2/6] Rebuilding ALL Docker images...
echo    This may take several minutes...

REM Build core services
docker-compose build --no-cache monerod monero-wallet-rpc monero-pool
if errorlevel 1 (
    echo    ERROR: Core build failed!
    pause
    exit /b 1
)

REM Build lottery service (profile)
docker-compose --profile lottery build --no-cache lottery-cron
if errorlevel 1 (
    echo    WARNING: Lottery build failed. Skipping...
)

REM Build haproxy service (profile)
docker-compose --profile ssl build --no-cache haproxy
if errorlevel 1 (
    echo    WARNING: HAProxy build skipped (may not have SSL cert configured)
)

echo    All builds complete!
echo.

echo [3/6] Restarting ALL services...
docker-compose up -d monerod monero-wallet-rpc monero-pool
docker-compose --profile lottery up -d lottery-cron
REM Note: haproxy only started if SSL is configured
docker-compose --profile ssl up -d haproxy 2>nul
echo    All services restarted!
goto :VERIFY

:CORE_ONLY
echo.
echo [2/6] Rebuilding CORE Docker images (daemon, wallet, pool)...
docker-compose build --no-cache monerod monero-wallet-rpc monero-pool
if errorlevel 1 (
    echo    ERROR: Build failed!
    pause
    exit /b 1
)
echo    Build complete!
echo.

echo [3/6] Restarting CORE services...
docker-compose up -d monerod monero-wallet-rpc monero-pool
if errorlevel 1 (
    echo    ERROR: Failed to restart!
    pause
    exit /b 1
)
echo    Core services restarted!
goto :VERIFY

:POOL_ONLY
echo.
echo [2/6] Rebuilding monero-pool...
docker-compose build --no-cache monero-pool
if errorlevel 1 (
    echo    ERROR: Build failed!
    pause
    exit /b 1
)
echo    Build complete!
echo.

echo [3/6] Restarting monero-pool...
docker-compose up -d monero-pool
if errorlevel 1 (
    echo    ERROR: Failed to restart!
    pause
    exit /b 1
)
echo    Pool restarted!
goto :VERIFY

:LOTTERY_ONLY
echo.
echo [2/6] Rebuilding lottery-cron...
docker-compose --profile lottery build --no-cache lottery-cron
if errorlevel 1 (
    echo    ERROR: Build failed!
    pause
    exit /b 1
)
echo    Build complete!
echo.

echo [3/6] Restarting lottery-cron...
docker-compose --profile lottery up -d lottery-cron
if errorlevel 1 (
    echo    ERROR: Failed to restart!
    pause
    exit /b 1
)
echo    Lottery service restarted!
goto :VERIFY

:VERIFY
echo.
echo [4/6] Waiting for services to start...
timeout /t 5 /nobreak >nul

echo.
echo [5/6] Service Status:
echo ============================================
docker-compose ps
docker-compose --profile lottery ps 2>nul
echo.

echo [6/6] Health Check:
echo ============================================
docker-compose ps --format "{{.Name}}: {{.Status}}"
echo.

echo ============================================
echo   Update Complete!
echo ============================================
echo.
echo Useful commands:
echo   View pool logs:     docker-compose logs -f monero-pool
echo   View lottery logs:  docker-compose logs -f lottery-cron
echo   View all logs:      docker-compose logs -f
echo.
pause
