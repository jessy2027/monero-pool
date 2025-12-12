@echo off
REM =============================================================================
REM Monero Pool - Update Script
REM =============================================================================
REM Use this script after pulling new code from git to rebuild and redeploy
REM =============================================================================

echo.
echo ============================================
echo   Monero Pool - Update / Redeploy
echo ============================================
echo.

cd /d "%~dp0"

echo [1/4] Pulling latest code from git...
git pull
if errorlevel 1 (
    echo    WARNING: Git pull failed. Continuing with local code...
)
echo.

echo [2/4] Rebuilding Docker image...
echo    This may take a few minutes...
docker-compose build --no-cache monero-pool
if errorlevel 1 (
    echo    ERROR: Build failed!
    pause
    exit /b 1
)
echo    Build complete!
echo.

echo [3/4] Restarting pool with new image...
docker-compose up -d monero-pool
if errorlevel 1 (
    echo    ERROR: Failed to restart!
    pause
    exit /b 1
)
echo    Pool restarted!
echo.

echo [4/4] Verifying...
timeout /t 5 /nobreak >nul
docker-compose ps
echo.

echo ============================================
echo   Update Complete!
echo ============================================
echo.
echo View logs: docker-compose logs -f monero-pool
echo.
pause
