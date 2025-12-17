@echo off
REM =============================================================================
REM Monero Pool - Backup Script
REM =============================================================================
REM Creates a timestamped backup of all critical pool data
REM Schedule this script with Windows Task Scheduler for automatic backups
REM =============================================================================

setlocal enabledelayedexpansion

REM Configuration
set BACKUP_DIR=C:\MoneroPool\backups
set POOL_DATA=C:\MoneroPool\pool-data
set WALLET_DATA=C:\MoneroPool\wallet
set CONFIG_DATA=C:\MoneroPool\config
set LOTTERY_DATA=C:\MoneroPool\lottery-data
set LOTTERY_OUTPUT=C:\MoneroPool\lottery-output
set KEEP_DAYS=7


REM Create timestamp
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set TIMESTAMP=%datetime:~0,4%-%datetime:~4,2%-%datetime:~6,2%_%datetime:~8,2%-%datetime:~10,2%

echo.
echo ============================================
echo   Monero Pool Backup - %TIMESTAMP%
echo ============================================
echo.

REM Create backup directory
if not exist "%BACKUP_DIR%" mkdir "%BACKUP_DIR%"
set CURRENT_BACKUP=%BACKUP_DIR%\backup_%TIMESTAMP%
mkdir "%CURRENT_BACKUP%"

REM Stop pool for consistent backup (optional - uncomment if needed)
REM echo Stopping pool for backup...
REM docker-compose -f "%~dp0docker-compose.yml" stop monero-pool

REM Backup pool data
echo [1/3] Backing up pool database...
if exist "%POOL_DATA%" (
    xcopy "%POOL_DATA%\*" "%CURRENT_BACKUP%\pool-data\" /E /I /H /Y >nul
    echo    Pool data backed up successfully.
) else (
    echo    WARNING: Pool data directory not found!
)

REM Backup wallet
echo [2/3] Backing up wallet...
if exist "%WALLET_DATA%" (
    xcopy "%WALLET_DATA%\*" "%CURRENT_BACKUP%\wallet\" /E /I /H /Y >nul
    echo    Wallet backed up successfully.
) else (
    echo    WARNING: Wallet directory not found!
)

REM Backup config
echo [3/4] Backing up configuration...
if exist "%CONFIG_DATA%" (
    xcopy "%CONFIG_DATA%\*" "%CURRENT_BACKUP%\config\" /E /I /H /Y >nul
    echo    Configuration backed up successfully.
) else (
    echo    WARNING: Config directory not found!
)

REM Backup lottery data
echo [4/4] Backing up lottery data...
if exist "%LOTTERY_DATA%" (
    xcopy "%LOTTERY_DATA%\*" "%CURRENT_BACKUP%\lottery-data\" /E /I /H /Y >nul
    echo    Lottery data backed up successfully.
) else (
    echo    INFO: Lottery data directory not found (lottery may not be enabled).
)
if exist "%LOTTERY_OUTPUT%" (
    xcopy "%LOTTERY_OUTPUT%\*" "%CURRENT_BACKUP%\lottery-output\" /E /I /H /Y >nul
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
echo   2. Extract backup to C:\MoneroPool\
echo   3. Start the pool: docker-compose up -d
echo.

endlocal
