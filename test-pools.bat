@echo off
REM =============================================================================
REM Script de test Upstream/Downstream sur la même machine
REM =============================================================================
setlocal enabledelayedexpansion

echo.
echo  ===================================================
echo   Monero Pool - Test Upstream/Downstream
echo  ===================================================
echo.

:menu
echo  Choisissez une action:
echo.
echo  [1] Creer les dossiers de test
echo  [2] Build et demarrer les pools (upstream + downstream)
echo  [3] Voir les logs (tous)
echo  [4] Voir les logs upstream
echo  [5] Voir les logs downstream
echo  [6] Arreter les pools
echo  [7] Supprimer les conteneurs et volumes de test
echo  [8] Status des conteneurs
echo  [0] Quitter
echo.
set /p choice="Votre choix: "

if "%choice%"=="1" goto create_dirs
if "%choice%"=="2" goto start_pools
if "%choice%"=="3" goto logs_all
if "%choice%"=="4" goto logs_upstream
if "%choice%"=="5" goto logs_downstream
if "%choice%"=="6" goto stop_pools
if "%choice%"=="7" goto cleanup
if "%choice%"=="8" goto status
if "%choice%"=="0" goto end
goto menu

:create_dirs
echo.
echo  Creation des dossiers de test...
if not exist "C:\MoneroPool\test-upstream-data" mkdir "C:\MoneroPool\test-upstream-data"
if not exist "C:\MoneroPool\test-downstream-data" mkdir "C:\MoneroPool\test-downstream-data"
echo  Dossiers crees:
echo    - C:\MoneroPool\test-upstream-data
echo    - C:\MoneroPool\test-downstream-data
echo.
pause
goto menu

:start_pools
echo.
echo  Build et demarrage des pools...
docker compose -f docker-compose.test.yml build
docker compose -f docker-compose.test.yml up -d
echo.
echo  ===================================================
echo   Pools demarres !
echo  ===================================================
echo.
echo   Upstream:
echo     - Stratum:  localhost:4242
echo     - WebUI:    http://localhost:8080
echo     - Trusted:  localhost:4244
echo.
echo   Downstream:
echo     - Stratum:  localhost:5242
echo     - WebUI:    http://localhost:8081
echo.
echo   Pour tester:
echo     1. Connectez un mineur a localhost:4242 (upstream)
echo     2. Connectez un mineur a localhost:5242 (downstream)
echo     3. Verifiez les WebUI pour voir les connexions
echo.
pause
goto menu

:logs_all
echo.
echo  Affichage des logs (Ctrl+C pour quitter)...
docker compose -f docker-compose.test.yml logs -f pool-upstream pool-downstream
goto menu

:logs_upstream
echo.
echo  Logs Upstream (Ctrl+C pour quitter)...
docker compose -f docker-compose.test.yml logs -f pool-upstream
goto menu

:logs_downstream
echo.
echo  Logs Downstream (Ctrl+C pour quitter)...
docker compose -f docker-compose.test.yml logs -f pool-downstream
goto menu

:stop_pools
echo.
echo  Arret des pools...
docker compose -f docker-compose.test.yml down
echo  Pools arretes.
echo.
pause
goto menu

:cleanup
echo.
echo  Nettoyage complet...
docker compose -f docker-compose.test.yml down -v
echo.
echo  Suppression des dossiers de test...
if exist "C:\MoneroPool\test-upstream-data" rmdir /s /q "C:\MoneroPool\test-upstream-data"
if exist "C:\MoneroPool\test-downstream-data" rmdir /s /q "C:\MoneroPool\test-downstream-data"
echo  Nettoyage termine.
echo.
pause
goto menu

:status
echo.
echo  Status des conteneurs:
docker compose -f docker-compose.test.yml ps
echo.
pause
goto menu

:end
echo.
echo  Au revoir!
exit /b 0
