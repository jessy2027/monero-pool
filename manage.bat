@echo off
setlocal EnableDelayedExpansion

REM =============================================================================
REM Monero Pool - Unified Management Script (Windows)
REM =============================================================================

REM Configuration
set "DEFAULT_DATA_DIR=C:\MoneroPool"
set "COMPOSE_FILE=docker-compose.windows.yml"
set "ENV_FILE=.env"
set "BACKUP_RETENTION_DAYS=7"

REM Colors (Simulated)
set "HEADER_PREFIX=[***]"
set "INFO_PREFIX=[INFO]"
set "WARN_PREFIX=[WARN]"
set "ERROR_PREFIX=[ERROR]"

REM -----------------------------------------------------------------------------
REM Main Logic
REM -----------------------------------------------------------------------------

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
REM Helper Functions
REM -----------------------------------------------------------------------------

:check_admin
    net session >nul 2>&1
    if %errorLevel% neq 0 (
        echo %ERROR_PREFIX% This script requires Administrator privileges!
        echo %ERROR_PREFIX% Please right-click and select "Run as Administrator".
        exit /b 1
    )
    exit /b 0

:load_env
    if exist "%ENV_FILE%" (
        for /f "usebackq tokens=1* delims==" %%a in ("%ENV_FILE%") do (
            REM Simple parsing, might need refinement for quotes
            set "VAR_NAME=%%a"
            set "VAR_VALUE=%%b"
            if not "!VAR_NAME:~0,1!"=="#" (
                set "!VAR_NAME!=!VAR_VALUE!"
            )
        )
    )
    if "%DATA_DIR%"=="" set "DATA_DIR=%DEFAULT_DATA_DIR%"
    exit /b 0

:check_docker
    docker --version >nul 2>&1
    if %errorLevel% neq 0 (
        echo %ERROR_PREFIX% Docker is not installed or not running!
        echo %ERROR_PREFIX% Please install Docker Desktop.
        exit /b 1
    )
    exit /b 0

REM -----------------------------------------------------------------------------
REM Commands
REM -----------------------------------------------------------------------------

:setup
    echo.
    echo %HEADER_PREFIX% Setup Monero Pool
    call :check_admin || exit /b 1
    call :check_docker || exit /b 1

    set /p "USER_DIR=Install location [%DEFAULT_DATA_DIR%]: "
    if "%USER_DIR%"=="" set "USER_DIR=%DEFAULT_DATA_DIR%"
    set "DATA_DIR=%USER_DIR%"

    REM Save to .env
    echo DATA_DIR=%DATA_DIR%> "%ENV_FILE%"
    echo COMPOSE_FILE=%COMPOSE_FILE%>> "%ENV_FILE%"
    echo %INFO_PREFIX% Configuration saved to %ENV_FILE%

    REM Create Directories
    echo %INFO_PREFIX% Creating directories in %DATA_DIR%...
    if not exist "%DATA_DIR%\xmr-data" mkdir "%DATA_DIR%\xmr-data"
    if not exist "%DATA_DIR%\xmr-wallet" mkdir "%DATA_DIR%\xmr-wallet"
    if not exist "%DATA_DIR%\xmr-pool-data" mkdir "%DATA_DIR%\xmr-pool-data"
    if not exist "%DATA_DIR%\config\certs" mkdir "%DATA_DIR%\config\certs"
    if not exist "%DATA_DIR%\backups" mkdir "%DATA_DIR%\backups"
    if not exist "%DATA_DIR%\lottery-data" mkdir "%DATA_DIR%\lottery-data"
    if not exist "%DATA_DIR%\lottery-output" mkdir "%DATA_DIR%\lottery-output"
    if not exist "%DATA_DIR%\tari-data" mkdir "%DATA_DIR%\tari-data"
    if not exist "%DATA_DIR%\tari-wallet" mkdir "%DATA_DIR%\tari-wallet"

    REM Copy Configs
    echo %INFO_PREFIX% Copying configuration files...
    if not exist "%DATA_DIR%\config\pool.conf" (
        if exist "pool.conf" (
            copy "pool.conf" "%DATA_DIR%\config\pool.conf" >nul
            echo %INFO_PREFIX% Copied pool.conf
        ) else (
            echo %WARN_PREFIX% pool.conf not found in current directory!
        )
    ) else (
        echo %INFO_PREFIX% pool.conf already exists, skipping...
    )

    if not exist "%DATA_DIR%\config\haproxy.cfg" (
        if exist "haproxy.cfg" (
            copy "haproxy.cfg" "%DATA_DIR%\config\haproxy.cfg" >nul
            echo %INFO_PREFIX% Copied haproxy.cfg
        )
    )

    if not exist "%DATA_DIR%\config\wallet-password.txt" (
        echo CHANGE_THIS_PASSWORD> "%DATA_DIR%\config\wallet-password.txt"
        echo %INFO_PREFIX% Created wallet-password.txt placeholder
    )

    REM Wallet Address Check
    if exist "%DATA_DIR%\config\pool.conf" (
        findstr "46hcZEDqKfbEzEcchonDsbHpSKK2AkQ3X2ozg3Je78r7Xx5X4dVnaakVDjKgLU2qiHggadYM9fWcce95uCPNjz1MAJ5CyHU" "%DATA_DIR%\config\pool.conf" >nul
        if !errorlevel! equ 0 (
            echo %WARN_PREFIX% You are using the default wallet address in pool.conf!
            echo %WARN_PREFIX% Please edit %DATA_DIR%\config\pool.conf immediately.
        )
    )

    REM Build Images
    echo %INFO_PREFIX% Building Docker images...
    docker compose -f "%COMPOSE_FILE%" build

    echo.
    echo %INFO_PREFIX% Setup complete!
    echo %INFO_PREFIX% Next steps:
    echo   1. Edit config: notepad %DATA_DIR%\config\pool.conf
    echo   2. Create wallet: manage.bat create-wallet
    echo   3. Start pool: manage.bat start
    echo   4. Start Tari (Optional): manage.bat start-tari
    exit /b 0

:start
    echo.
    echo %HEADER_PREFIX% Starting Monero Pool (All Services)
    call :check_docker || exit /b 1
    call :load_env

    REM Check for wallet password (shared with Monero)
    set "PASSWORD_FILE=%DATA_DIR%\config\wallet-password.txt"
    if not exist "%PASSWORD_FILE%" (
        echo %WARN_PREFIX% Wallet password file not found!
        echo Tari Wallet requires a password ^(same as Monero^).
        set /p "WALLET_PASS=Enter wallet password: "
        echo.
        set /p "WALLET_PASS_CONFIRM=Confirm password: "
        echo.

        if not "!WALLET_PASS!"=="!WALLET_PASS_CONFIRM!" (
            echo %ERROR_PREFIX% Passwords do not match!
            exit /b 1
        )

        echo !WALLET_PASS!> "%PASSWORD_FILE%"
        echo %INFO_PREFIX% Password saved to %PASSWORD_FILE%
    )

    REM Centralized config path
    set "TARI_CONFIG_DIR=%DATA_DIR%\config\tari"
    set "TARI_CONFIG_FILE=%TARI_CONFIG_DIR%\config.toml"

    if not exist "%TARI_CONFIG_DIR%" mkdir "%TARI_CONFIG_DIR%"

    if not exist "%TARI_CONFIG_FILE%" (
        echo %INFO_PREFIX% Generating default Tari configuration...
        (
            echo [wallet]
            echo grpc_enabled = true
            echo grpc_address = "/ip4/0.0.0.0/tcp/18143"
            echo base_node_service_peers = ["/dns4/tari-base-node/tcp/18142"]
            echo db_file = "/var/tari/wallet/console-wallet.sqlite"
            echo data_dir = "/var/tari/wallet"
            echo network = "esmeralda"
        ) > "%TARI_CONFIG_FILE%"
        echo %INFO_PREFIX% Created %TARI_CONFIG_FILE%
    )

    REM Patch obsolete config
    if exist "%TARI_CONFIG_FILE%" (
        echo %INFO_PREFIX% Checking for obsolete configuration in %TARI_CONFIG_FILE%...
        findstr /C:"grpc =" "%TARI_CONFIG_FILE%" >nul
        if !errorlevel! equ 0 (
            echo %WARN_PREFIX% Obsolete 'grpc' field detected. Patching...
            copy "%TARI_CONFIG_FILE%" "%TARI_CONFIG_FILE%.bak" >nul
            powershell -Command "(Get-Content '%TARI_CONFIG_FILE%') | Where-Object { $_ -notmatch '^\s*grpc\s*=' } | Set-Content '%TARI_CONFIG_FILE%'"
            echo %INFO_PREFIX% Tari configuration patched.
        )
    )

    echo %INFO_PREFIX% Starting Monero Pool, Tari, Lottery, and SSL Proxy...
    docker compose -f "%COMPOSE_FILE%" --profile tari --profile lottery --profile ssl up -d
    echo %INFO_PREFIX% All services started.
    docker compose -f "%COMPOSE_FILE%" ps
    exit /b 0

:start_lottery
    echo.
    echo %HEADER_PREFIX% Starting Lottery Service
    call :check_docker || exit /b 1
    call :load_env
    docker compose -f "%COMPOSE_FILE%" --profile lottery up -d
    echo %INFO_PREFIX% Lottery service started.
    exit /b 0

:start_tari
    echo.
    echo %HEADER_PREFIX% Starting Tari Merge Mining Services
    call :check_docker || exit /b 1
    call :load_env
    set "TARI_CONFIG_DIR=%DATA_DIR%\config\tari"
    set "TARI_CONFIG_FILE=%TARI_CONFIG_DIR%\config.toml"

    REM Patch obsolete config
    if exist "%TARI_CONFIG_FILE%" (
        echo %INFO_PREFIX% Checking for obsolete configuration in %TARI_CONFIG_FILE%...
        findstr /C:"grpc =" "%TARI_CONFIG_FILE%" >nul
        if !errorlevel! equ 0 (
            echo %WARN_PREFIX% Obsolete 'grpc' field detected. Patching...
            copy "%TARI_CONFIG_FILE%" "%TARI_CONFIG_FILE%.bak" >nul
            powershell -Command "(Get-Content '%TARI_CONFIG_FILE%') | Where-Object { $_ -notmatch '^\s*grpc\s*=' } | Set-Content '%TARI_CONFIG_FILE%'"
            echo %INFO_PREFIX% Tari configuration patched.
        )
    )

    docker compose -f "%COMPOSE_FILE%" --profile tari up -d
    echo %INFO_PREFIX% Tari services started.
    exit /b 0

:stop
    echo.
    echo %HEADER_PREFIX% Stopping Monero Pool
    call :check_docker || exit /b 1
    call :load_env
    docker compose -f "%COMPOSE_FILE%" down
    echo %INFO_PREFIX% Services stopped.
    exit /b 0

:restart
    call :stop
    call :start
    exit /b 0

:logs
    call :check_docker || exit /b 1
    call :load_env
    if "%2"=="" (
        docker compose -f "%COMPOSE_FILE%" logs -f --tail=100
    ) else (
        docker compose -f "%COMPOSE_FILE%" logs -f --tail=100 %2
    )
    exit /b 0

:create_wallet
    echo.
    echo %HEADER_PREFIX% Create Pool Wallet
    call :check_docker || exit /b 1
    call :load_env

    if exist "%DATA_DIR%\xmr-wallet\pool-wallet" (
        echo %ERROR_PREFIX% Wallet already exists at %DATA_DIR%\xmr-wallet\pool-wallet
        set /p "CONFIRM=Do you want to overwrite it? (y/N) "
        if /i not "!CONFIRM!"=="y" exit /b 0
    )

    set /p "WALLET_PASS=Enter new wallet password: "
    echo.
    set /p "WALLET_PASS_CONFIRM=Confirm password: "
    echo.

    if not "!WALLET_PASS!"=="!WALLET_PASS_CONFIRM!" (
        echo %ERROR_PREFIX% Passwords do not match!
        exit /b 1
    )

    echo !WALLET_PASS!> "%DATA_DIR%\config\wallet-password.txt"

    echo %INFO_PREFIX% Generating wallet...
    docker compose -f "%COMPOSE_FILE%" run --rm monero-wallet-rpc ^
        --daemon-host=monerod ^
        --generate-new-wallet=/home/monero/wallet/pool-wallet ^
        --password="!WALLET_PASS!" ^
        --command "exit"

    echo %INFO_PREFIX% Wallet created successfully!
    exit /b 0

:backup
    echo.
    echo %HEADER_PREFIX% Backup Monero Pool
    call :check_admin || exit /b 1
    call :load_env

    for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "datetime=%%I"
    set "TIMESTAMP=!datetime:~0,4!-!datetime:~4,2!-!datetime:~6,2!_!datetime:~8,2!-!datetime:~10,2!"

    set "BACKUP_DIR=%DATA_DIR%\backups"
    set "TEMP_DIR=%BACKUP_DIR%\backup_!TIMESTAMP!"

    mkdir "%TEMP_DIR%"

    echo %INFO_PREFIX% Backing up data...

    if exist "%DATA_DIR%\xmr-pool-data" xcopy "%DATA_DIR%\xmr-pool-data" "%TEMP_DIR%\xmr-pool-data" /E /I /H /Y >nul
    if exist "%DATA_DIR%\xmr-wallet" xcopy "%DATA_DIR%\xmr-wallet" "%TEMP_DIR%\xmr-wallet" /E /I /H /Y >nul
    if exist "%DATA_DIR%\config" xcopy "%DATA_DIR%\config" "%TEMP_DIR%\config" /E /I /H /Y >nul
    if exist "%DATA_DIR%\lottery-data" xcopy "%DATA_DIR%\lottery-data" "%TEMP_DIR%\lottery-data" /E /I /H /Y >nul
    if exist "%DATA_DIR%\lottery-output" xcopy "%DATA_DIR%\lottery-output" "%TEMP_DIR%\lottery-output" /E /I /H /Y >nul
    if exist "%DATA_DIR%\tari-data" xcopy "%DATA_DIR%\tari-data" "%TEMP_DIR%\tari-data" /E /I /H /Y >nul
    if exist "%DATA_DIR%\tari-wallet" xcopy "%DATA_DIR%\tari-wallet" "%TEMP_DIR%\tari-wallet" /E /I /H /Y >nul

    echo %INFO_PREFIX% Compressing...
    powershell -Command "Compress-Archive -Path '%TEMP_DIR%\*' -DestinationPath '%BACKUP_DIR%\backup_!TIMESTAMP!.zip' -Force"

    rmdir /S /Q "%TEMP_DIR%"

    echo %INFO_PREFIX% Backup created: %BACKUP_DIR%\backup_!TIMESTAMP!.zip

    REM Cleanup old backups
    echo %INFO_PREFIX% Cleaning old backups (older than %BACKUP_RETENTION_DAYS% days)...
    forfiles /P "%BACKUP_DIR%" /M "backup_*.zip" /D -%BACKUP_RETENTION_DAYS% /C "cmd /c del @path" 2>nul

    exit /b 0

:restore
    echo.
    echo %HEADER_PREFIX% Restore Monero Pool
    call :check_admin || exit /b 1
    call :load_env

    set "BACKUP_FILE=%~2"
    if "%BACKUP_FILE%"=="" (
        echo %ERROR_PREFIX% Usage: manage.bat restore ^<backup_file.zip^>
        echo Available backups:
        dir /B "%DATA_DIR%\backups\*.zip"
        exit /b 1
    )

    if not exist "%BACKUP_FILE%" (
        if exist "%DATA_DIR%\backups\%BACKUP_FILE%" (
            set "BACKUP_FILE=%DATA_DIR%\backups\%BACKUP_FILE%"
        ) else (
            echo %ERROR_PREFIX% Backup file not found: %BACKUP_FILE%
            exit /b 1
        )
    )

    echo %WARN_PREFIX% This will OVERWRITE current data in %DATA_DIR%!
    echo %WARN_PREFIX% Services will be stopped.
    set /p "CONFIRM=Are you sure? (y/N) "
    if /i not "!CONFIRM!"=="y" exit /b 0

    call :stop

    echo %INFO_PREFIX% Restoring from %BACKUP_FILE%...

    set "TEMP_RESTORE=%temp%\monero_restore_!random!"
    mkdir "%TEMP_RESTORE%"

    powershell -Command "Expand-Archive -Path '%BACKUP_FILE%' -DestinationPath '%TEMP_RESTORE%' -Force"

    if not exist "%TEMP_RESTORE%\xmr-pool-data" (
        echo %ERROR_PREFIX% Invalid backup structure (xmr-pool-data not found). Aborting.
        rmdir /S /Q "%TEMP_RESTORE%"
        exit /b 1
    )

    if exist "%TEMP_RESTORE%\xmr-pool-data" xcopy "%TEMP_RESTORE%\xmr-pool-data" "%DATA_DIR%\xmr-pool-data" /E /I /H /Y >nul
    if exist "%TEMP_RESTORE%\xmr-wallet" xcopy "%TEMP_RESTORE%\xmr-wallet" "%DATA_DIR%\xmr-wallet" /E /I /H /Y >nul
    if exist "%TEMP_RESTORE%\config" xcopy "%TEMP_RESTORE%\config" "%DATA_DIR%\config" /E /I /H /Y >nul
    if exist "%TEMP_RESTORE%\lottery-data" xcopy "%TEMP_RESTORE%\lottery-data" "%DATA_DIR%\lottery-data" /E /I /H /Y >nul
    if exist "%TEMP_RESTORE%\tari-data" xcopy "%TEMP_RESTORE%\tari-data" "%DATA_DIR%\tari-data" /E /I /H /Y >nul
    if exist "%TEMP_RESTORE%\tari-wallet" xcopy "%TEMP_RESTORE%\tari-wallet" "%DATA_DIR%\tari-wallet" /E /I /H /Y >nul

    rmdir /S /Q "%TEMP_RESTORE%"
    echo %INFO_PREFIX% Restore complete.

    set /p "START_CONFIRM=Start services now? (y/N) "
    if /i "!START_CONFIRM!"=="y" call :start
    exit /b 0

:schedule_backups
    echo.
    echo %HEADER_PREFIX% Schedule Daily Backups
    call :check_admin || exit /b 1
    call :load_env

    set "TASK_NAME=MoneroPoolBackup"
    set "SCRIPT_PATH=%~f0"

    schtasks /query /TN "%TASK_NAME%" >nul 2>&1
    if %errorlevel% equ 0 (
        echo %WARN_PREFIX% Backup task already scheduled.
    ) else (
        schtasks /create /TN "%TASK_NAME%" /TR "\"%SCRIPT_PATH%\" backup" /SC DAILY /ST 03:00 /RL HIGHEST /F
        echo %INFO_PREFIX% Scheduled daily backup at 03:00.
    )
    exit /b 0

:update
    echo.
    echo %HEADER_PREFIX% Update Monero Pool
    call :check_docker || exit /b 1
    call :load_env

    echo %INFO_PREFIX% Pulling latest changes...
    git pull

    echo %INFO_PREFIX% Rebuilding images...
    docker compose -f "%COMPOSE_FILE%" build

    echo %INFO_PREFIX% Restarting services...
    docker compose -f "%COMPOSE_FILE%" up -d

    echo %INFO_PREFIX% Update complete.
    exit /b 0

:help
    echo Monero Pool Management CLI (Windows)
    echo Usage: manage.bat ^<command^>
    echo.
    echo Commands:
    echo   setup             Initial setup of the pool
    echo   start             Start the pool services
    echo   start-lottery     Start the lottery service
    echo   start-tari        Start the Tari merge mining services
    echo   stop              Stop all services
    echo   restart           Restart services
    echo   logs [service]    View logs (optional: monerod, monero-pool)
    echo   create-wallet     Interactive wallet creation
    echo   backup            Create an immediate backup
    echo   restore ^<file^>    Restore from a backup file (zip)
    echo   schedule-backups  Add daily backup to Windows Task Scheduler
    echo   update            Git pull, rebuild and restart
    exit /b 1
