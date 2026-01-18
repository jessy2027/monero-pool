@echo off
REM =============================================================================
REM Monero Pool - Backup Script
REM =============================================================================
REM Creates a timestamped backup of all critical pool data
REM Schedule this script with Windows Task Scheduler for automatic backups
REM =============================================================================

setlocal enabledelayedexpansion

REM Hardcoded Paths
set DATA_ROOT=C:\MoneroPool

echo.
echo ============================================
echo   Monero Pool - Backup
echo ============================================
echo   Data Root: %DATA_ROOT%
echo ============================================

REM Configuration
set BACKUP_DIR=%DATA_ROOT%\backups
set POOL_DATA=%DATA_ROOT%\pool-data
set WALLET_DATA=%DATA_ROOT%\wallet
set CONFIG_DATA=%DATA_ROOT%\config
set LOTTERY_DATA=%DATA_ROOT%\lottery-data
set LOTTERY_OUTPUT=%DATA_ROOT%\lottery-output
set KEEP_DAYS=7


REM Create timestamp (using PowerShell for reliability)
for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format 'yyyy-MM-dd_HH-mm'"') do set TIMESTAMP=%%I

echo.
echo   Backup Timestamp: %TIMESTAMP%
echo.

REM Create backup directory
if not exist "%BACKUP_DIR%" mkdir "%BACKUP_DIR%"
set CURRENT_BACKUP=%BACKUP_DIR%\backup_%TIMESTAMP%
mkdir "%CURRENT_BACKUP%"

REM Stop pool for consistent backup (optional - uncomment if needed)
REM echo Stopping pool for backup...
REM docker-compose -f "%~dp0docker-compose.yml" stop monero-pool

REM Backup pool data
echo [1/4] Backing up pool database...
if exist "%POOL_DATA%" (
    echo    Copying: %POOL_DATA%
    xcopy "%POOL_DATA%\*" "%CURRENT_BACKUP%\pool-data\" /E /I /H /Y >nul
    echo    Pool data backed up.
) else (
    echo    WARNING: Pool data directory not found at %POOL_DATA%
)

REM Backup wallet
echo [2/4] Backing up wallet...
if exist "%WALLET_DATA%" (
    echo    Copying: %WALLET_DATA%
    xcopy "%WALLET_DATA%\*" "%CURRENT_BACKUP%\wallet\" /E /I /H /Y >nul
    echo    Wallet backed up.
) else (
    echo    WARNING: Wallet directory not found at %WALLET_DATA%
)

REM Backup config
echo [3/4] Backing up configuration...
if exist "%CONFIG_DATA%" (
    echo    Copying: %CONFIG_DATA%
    xcopy "%CONFIG_DATA%\*" "%CURRENT_BACKUP%\config\" /E /I /H /Y >nul
    echo    Configuration backed up.
) else (
    echo    WARNING: Config directory not found at %CONFIG_DATA%
)

REM Backup lottery data
echo [4/6] Backing up lottery data...
if exist "%LOTTERY_DATA%" (
    xcopy "%LOTTERY_DATA%\*" "%CURRENT_BACKUP%\lottery-data\" /E /I /H /Y >nul
    echo    Lottery data backed up.
) else (
    echo    INFO: Lottery data directory not found (lottery may not be enabled).
)
if exist "%LOTTERY_OUTPUT%" (
    xcopy "%LOTTERY_OUTPUT%\*" "%CURRENT_BACKUP%\lottery-output\" /E /I /H /Y >nul
)

REM Backup Tari data
echo [5/6] Backing up Tari node data...
set TARI_DATA=%DATA_ROOT%\tari-data
if exist "%TARI_DATA%" (
    xcopy "%TARI_DATA%\*" "%CURRENT_BACKUP%\tari-data\" /E /I /H /Y >nul
    echo    Tari node data backed up.
) else (
    echo    INFO: Tari data not found (merge mining may not be enabled).
)

echo [6/6] Backing up Tari wallet...
set TARI_WALLET=%DATA_ROOT%\tari-wallet
if exist "%TARI_WALLET%" (
    xcopy "%TARI_WALLET%\*" "%CURRENT_BACKUP%\tari-wallet\" /E /I /H /Y >nul
    echo    Tari wallet backed up.
) else (
    echo    INFO: Tari wallet not found.
)

REM Restart pool if it was stopped
REM echo Restarting pool...
REM docker-compose -f "%~dp0docker-compose.yml" start monero-pool

REM Create compressed archive (requires 7-Zip or PowerShell)
echo.
echo Creating compressed archive...
powershell -Command "Compress-Archive -Path '%CURRENT_BACKUP%\*' -DestinationPath '%CURRENT_BACKUP%.zip' -Force"
if exist "%CURRENT_BACKUP%.zip" (
    rmdir /S /Q "%CURRENT_BACKUP%"
    echo    Archive created: %CURRENT_BACKUP%.zip
)

REM Clean old backups
echo.
echo Cleaning backups older than %KEEP_DAYS% days...
forfiles /P "%BACKUP_DIR%" /M "backup_*.zip" /D -%KEEP_DAYS% /C "cmd /c del @path" 2>nul
echo    Cleanup complete.

REM Summary
echo.
echo ============================================
echo   Backup Complete!
echo ============================================
echo.
echo Backup location: %CURRENT_BACKUP%.zip
echo.
echo To restore from backup:
echo   1. Stop the pool: docker-compose down
echo   2. Extract backup to %DATA_ROOT%
echo   3. Start the pool: docker-compose up -d
echo.

endlocal
