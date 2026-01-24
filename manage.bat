@echo off
setlocal EnableDelayedExpansion

REM =============================================================================
REM Monero Pool - Unified Management Script (Windows)
REM =============================================================================

set "DEFAULT_DATA_DIR=C:\MoneroPool"
set "COMPOSE_FILE=docker-compose.windows.yml"
set "ENV_FILE=.env"
set "BACKUP_RETENTION_DAYS=7"

REM Check arguments
if "%1"=="" goto help
if "%1"=="setup" goto setup
if "%1"=="start" goto start
if "%1"=="start-lottery" goto start_lottery
if "%1"=="start-tari" goto start_tari
if "%1"=="stop" goto stop
if "%1"=="restart" goto restart
if "%1"=="logs" goto logs
if "%1"=="create-wallet" goto create_wallet
if "%1"=="backup" goto backup
if "%1"=="restore" goto restore
if "%1"=="schedule-backups" goto schedule_backups
if "%1"=="update" goto update
goto help

REM -----------------------------------------------------------------------------
REM Helpers
REM -----------------------------------------------------------------------------

:load_env
if exist "%ENV_FILE%" (
    for /f "usebackq tokens=1* delims==" %%a in ("%ENV_FILE%") do (
        set "%%a=%%b"
    )
)
if not defined DATA_DIR set "DATA_DIR=%DEFAULT_DATA_DIR%"
exit /b

:check_admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] This script requires Administrator privileges.
    echo         Please right-click and select "Run as Administrator".
    exit /b 1
)
exit /b 0

:check_docker
docker --version >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Docker is not installed or not in PATH.
    echo         Please install Docker Desktop.
    exit /b 1
)
exit /b 0

REM -----------------------------------------------------------------------------
REM Commands
REM -----------------------------------------------------------------------------

:setup
echo [INFO] Setup Monero Pool...
call :check_admin || exit /b 1
call :check_docker || exit /b 1

set /p "USER_DIR=Install location [%DEFAULT_DATA_DIR%]: "
if "%USER_DIR%"=="" set "USER_DIR=%DEFAULT_DATA_DIR%"
set "DATA_DIR=%USER_DIR%"

echo DATA_DIR=%DATA_DIR%> "%ENV_FILE%"
echo COMPOSE_FILE=%COMPOSE_FILE%>> "%ENV_FILE%"
echo [INFO] Configuration saved to %ENV_FILE%

echo [INFO] Creating directories in %DATA_DIR%...
if not exist "%DATA_DIR%\blockchain" mkdir "%DATA_DIR%\blockchain"
if not exist "%DATA_DIR%\wallet" mkdir "%DATA_DIR%\wallet"
if not exist "%DATA_DIR%\pool-data" mkdir "%DATA_DIR%\pool-data"
if not exist "%DATA_DIR%\config\certs" mkdir "%DATA_DIR%\config\certs"
if not exist "%DATA_DIR%\backups" mkdir "%DATA_DIR%\backups"
if not exist "%DATA_DIR%\lottery-data" mkdir "%DATA_DIR%\lottery-data"
if not exist "%DATA_DIR%\lottery-output" mkdir "%DATA_DIR%\lottery-output"
if not exist "%DATA_DIR%\tari-data" mkdir "%DATA_DIR%\tari-data"
if not exist "%DATA_DIR%\tari-wallet" mkdir "%DATA_DIR%\tari-wallet"

echo [INFO] Copying configuration files...
if not exist "%DATA_DIR%\config\pool.conf" (
    if exist "pool.conf" (
        copy "pool.conf" "%DATA_DIR%\config\pool.conf" >nul
        echo [INFO] Copied pool.conf
    ) else (
        echo [WARN] pool.conf not found in current directory!
    )
)

if not exist "%DATA_DIR%\config\haproxy.cfg" (
    if exist "haproxy.cfg" (
        copy "haproxy.cfg" "%DATA_DIR%\config\haproxy.cfg" >nul
        echo [INFO] Copied haproxy.cfg
    )
)

if not exist "%DATA_DIR%\config\wallet-password.txt" (
    echo CHANGE_THIS_PASSWORD> "%DATA_DIR%\config\wallet-password.txt"
    echo [INFO] Created wallet-password.txt placeholder
)

REM Wallet check
findstr "46hcZEDqKfbEzEcchonDsbHpSKK2AkQ3X2ozg3Je78r7Xx5X4dVnaakVDjKgLU2qiHggadYM9fWcce95uCPNjz1MAJ5CyHU" "%DATA_DIR%\config\pool.conf" >nul
if %errorLevel% equ 0 (
    echo [WARN] You are using the default wallet address in pool.conf!
    echo        Please edit %DATA_DIR%\config\pool.conf immediately.
)

echo [INFO] Building Docker images...
docker compose -f "%COMPOSE_FILE%" build

echo [INFO] Setup complete!
echo Next steps:
echo   1. Edit config: notepad "%DATA_DIR%\config\pool.conf"
echo   2. Create wallet: manage.bat create-wallet
echo   3. Start pool: manage.bat start
goto :eof

:start
call :check_docker || exit /b 1
call :load_env
echo [INFO] Starting services...
docker compose -f "%COMPOSE_FILE%" up -d
docker compose -f "%COMPOSE_FILE%" ps
goto :eof

:start_lottery
call :check_docker || exit /b 1
call :load_env
echo [INFO] Starting lottery service...
docker compose -f "%COMPOSE_FILE%" --profile lottery up -d
goto :eof

:start_tari
call :check_docker || exit /b 1
call :load_env
echo [INFO] Starting Tari services...
docker compose -f "%COMPOSE_FILE%" --profile tari up -d
goto :eof

:stop
call :check_docker || exit /b 1
call :load_env
echo [INFO] Stopping services...
docker compose -f "%COMPOSE_FILE%" down
goto :eof

:restart
call :stop
call :start
goto :eof

:logs
call :check_docker || exit /b 1
call :load_env
if "%2"=="" (
    docker compose -f "%COMPOSE_FILE%" logs -f --tail=100
) else (
    docker compose -f "%COMPOSE_FILE%" logs -f --tail=100 %2
)
goto :eof

:create_wallet
echo [INFO] Create Pool Wallet
call :check_docker || exit /b 1
call :load_env

if exist "%DATA_DIR%\wallet\pool-wallet" (
    echo [ERROR] Wallet already exists at %DATA_DIR%\wallet\pool-wallet
    echo         Delete it manually if you want to recreate it.
    exit /b 1
)

echo.
echo Enter new wallet password:
set /p "WALLET_PASS="
echo.

if "%WALLET_PASS%"=="" (
    echo [ERROR] Password cannot be empty.
    exit /b 1
)

echo %WALLET_PASS%> "%DATA_DIR%\config\wallet-password.txt"

echo [INFO] Generating wallet...
docker compose -f "%COMPOSE_FILE%" run --rm monero-wallet-rpc ^
    --daemon-host=monerod ^
    --generate-new-wallet=/home/monero/wallet/pool-wallet ^
    --password="%WALLET_PASS%" ^
    --command "exit"

echo [INFO] Wallet created successfully!
goto :eof

:backup
call :check_admin || exit /b 1
call :load_env

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format 'yyyy-MM-dd_HH-mm'"') do set TIMESTAMP=%%I
set "BACKUP_DIR=%DATA_DIR%\backups"
set "TEMP_DIR=%BACKUP_DIR%\backup_%TIMESTAMP%"

echo [INFO] Backup started: %TIMESTAMP%
if not exist "%BACKUP_DIR%" mkdir "%BACKUP_DIR%"
mkdir "%TEMP_DIR%"

if exist "%DATA_DIR%\pool-data" xcopy "%DATA_DIR%\pool-data\*" "%TEMP_DIR%\pool-data\" /E /I /H /Y >nul
if exist "%DATA_DIR%\wallet" xcopy "%DATA_DIR%\wallet\*" "%TEMP_DIR%\wallet\" /E /I /H /Y >nul
if exist "%DATA_DIR%\config" xcopy "%DATA_DIR%\config\*" "%TEMP_DIR%\config\" /E /I /H /Y >nul
if exist "%DATA_DIR%\lottery-data" xcopy "%DATA_DIR%\lottery-data\*" "%TEMP_DIR%\lottery-data\" /E /I /H /Y >nul
if exist "%DATA_DIR%\lottery-output" xcopy "%DATA_DIR%\lottery-output\*" "%TEMP_DIR%\lottery-output\" /E /I /H /Y >nul
if exist "%DATA_DIR%\tari-data" xcopy "%DATA_DIR%\tari-data\*" "%TEMP_DIR%\tari-data\" /E /I /H /Y >nul
if exist "%DATA_DIR%\tari-wallet" xcopy "%DATA_DIR%\tari-wallet\*" "%TEMP_DIR%\tari-wallet\" /E /I /H /Y >nul

echo [INFO] Compressing...
powershell -Command "Compress-Archive -Path '%TEMP_DIR%\*' -DestinationPath '%BACKUP_DIR%\backup_%TIMESTAMP%.zip' -Force"

if exist "%BACKUP_DIR%\backup_%TIMESTAMP%.zip" (
    rmdir /S /Q "%TEMP_DIR%"
    echo [INFO] Backup created: %BACKUP_DIR%\backup_%TIMESTAMP%.zip
)

echo [INFO] Cleaning old backups...
forfiles /P "%BACKUP_DIR%" /M "backup_*.zip" /D -%BACKUP_RETENTION_DAYS% /C "cmd /c del @path" 2>nul
goto :eof

:restore
call :check_admin || exit /b 1
call :load_env

set "BACKUP_FILE=%~2"
if "%BACKUP_FILE%"=="" (
    echo [ERROR] Usage: manage.bat restore ^<backup_file.zip^>
    echo Available backups:
    dir /b "%DATA_DIR%\backups\*.zip"
    exit /b 1
)

if not exist "%BACKUP_FILE%" (
    if exist "%DATA_DIR%\backups\%BACKUP_FILE%" (
        set "BACKUP_FILE=%DATA_DIR%\backups\%BACKUP_FILE%"
    ) else (
        echo [ERROR] Backup file not found.
        exit /b 1
    )
)

echo [WARN] This will OVERWRITE data in %DATA_DIR%
echo [WARN] Services will be stopped.
echo Press Ctrl+C to cancel or any key to continue...
pause >nul

call :stop

echo [INFO] Restoring from %BACKUP_FILE%...
powershell -Command "Expand-Archive -Path '%BACKUP_FILE%' -DestinationPath '%DATA_DIR%' -Force"

echo [INFO] Restore complete.
echo Start services with: manage.bat start
goto :eof

:schedule_backups
call :check_admin || exit /b 1
call :load_env

echo [INFO] Scheduling daily backup task...
schtasks /create /tn "MoneroPoolBackup" /tr "\"%~dp0manage.bat\" backup" /sc daily /st 03:00 /ru SYSTEM /f
echo [INFO] Task scheduled.
goto :eof

:update
call :check_docker || exit /b 1
call :load_env

echo [INFO] Pulling latest changes...
git pull
echo [INFO] Rebuilding...
docker compose -f "%COMPOSE_FILE%" build
echo [INFO] Restarting...
docker compose -f "%COMPOSE_FILE%" up -d
goto :eof

:help
echo Monero Pool Management CLI (Windows)
echo Usage: manage.bat ^<command^>
echo.
echo Commands:
echo   setup             Initial setup
echo   start             Start services
echo   start-lottery     Start lottery service
echo   start-tari        Start Tari merge mining services
echo   stop              Stop services
echo   restart           Restart services
echo   logs [service]    View logs
echo   create-wallet     Create wallet
echo   backup            Create backup
echo   restore ^<file^>    Restore from backup
echo   schedule-backups  Schedule daily backup task
echo   update            Git pull, rebuild and restart
goto :eof
