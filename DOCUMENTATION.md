# 📚 Monero Pool - Documentation Complète

Bienvenue dans la documentation complète du projet Monero Pool. Ce document regroupe toutes les informations nécessaires pour installer, configurer, administrer et maintenir votre pool de minage Monero.

## 📑 Table des Matières

1.  [Introduction](#1-introduction)
2.  [Architecture](#2-architecture)
3.  [Installation et Démarrage](#3-installation-et-démarrage)
    *   [Prérequis](#prérequis)
    *   [Windows](#installation-windows)
    *   [Linux](#installation-linux)
4.  [Configuration](#4-configuration)
    *   [Pool (pool.conf)](#pool-configuration-poolconf)
    *   [Wallet](#wallet)
    *   [SSL / TLS (HAProxy)](#ssl--tls-haproxy)
5.  [Administration](#5-administration)
    *   [Commandes Utiles](#commandes-utiles)
    *   [Mises à Jour](#mises-à-jour)
    *   [Sauvegardes (Backup)](#sauvegardes-backup)
6.  [Fonctionnalités Avancées](#6-fonctionnalités-avancées)
    *   [Loterie](#loterie)
    *   [Stratum Self-Select](#stratum-self-select)
    *   [Multi-Region / Remote Nodes](#multi-region--remote-nodes)
7.  [Dépannage](#7-dépannage)

---

## 1. Introduction

Ce projet est une solution complète de pool de minage Monero prête à l'emploi, basée sur Docker. Elle combine la performance du moteur C `monero-pool` de jtgrassie avec la facilité de gestion de Docker Compose.

**Fonctionnalités principales :**
*   **Performance** : Moteur en C avec libevent et LMDB.
*   **PPLNS** : Système de paiement équitable.
*   **Interface Web** : Monitoring complet, multilingue, stats temps réel.
*   **Sécurité** : Wallet RPC isolé, support SSL, backups automatisés.
*   **Cross-Platform** : Scripts d'installation et de maintenance pour Windows et Linux.

---

## 2. Architecture

Le système est composé de plusieurs conteneurs Docker orchestrés ensemble :

| Conteneur | Rôle |
|-----------|------|
| `monerod` | Nœud Monero complet (Blockchain). Synchronise le réseau. |
| `monero-wallet-rpc` | Gère les paiements et le solde du pool. |
| `monero-pool` | Le serveur de minage (Stratum). Reçoit les shares des mineurs. |
| `haproxy` | (Optionnel) Gère le SSL/TLS pour les connexions sécurisées. |
| `lottery-cron` | (Optionnel) Gère le système de loterie hebdomadaire. |

---

## 3. Installation et Démarrage

### Prérequis
*   **Docker Desktop** (Windows) ou **Docker Engine** + **Compose** (Linux).
*   ~100 Go d'espace disque libre (pour la blockchain pruned).
*   4 Go de RAM minimum.
*   Une adresse de wallet Monero personnelle (pour recevoir les frais du pool).

### Installation Windows

1.  **Lancer le script d'installation** (Clic droit -> "Exécuter en tant qu'administrateur") :
    ```batch
    setup-windows.bat
    ```
    *Ce script crée les dossiers (`C:\MoneroPool`), configure l'environnement et construit les images.*

2.  **Configurer** : Editez `C:\MoneroPool\config\pool.conf` et mettez votre adresse wallet dans `pool-wallet`.

3.  **Démarrer** :
    ```batch
    docker-compose up -d
    ```

### Installation Linux

1.  **Lancer le script d'installation** :
    ```bash
    chmod +x setup-linux.sh
    ./setup-linux.sh
    ```
    *Ce script crée l'arborescence dans `./data`, génère le fichier `.env` et prépare tout.*

2.  **Configurer** : Editez `./data/config/pool.conf`.

3.  **Démarrer** :
    ```bash
    docker compose up -d
    ```

---

## 4. Configuration

### Pool Configuration (`pool.conf`)

Fichier : `C:\MoneroPool\config\pool.conf` (Win) ou `./data/config/pool.conf` (Linux).

Paramètres clés :
*   `pool-wallet` : **CRITIQUE**. L'adresse où sont minés les blocs. C'est VOTRE adresse.
*   `pool-fee` : Frais du pool (0.0 = 0%).
*   `payment-threshold` : Seuil de paiement automatique (ex: 0.005 XMR).
*   `pool-start-diff` : Difficulté de départ pour les mineurs (ex: 1000).

### Wallet

Le `monero-wallet-rpc` a besoin d'un mot de passe pour chiffrer le fichier wallet.
*   Editez `config/wallet-password.txt` et mettez un mot de passe fort (une seule ligne, sans espaces).

Lors du premier démarrage, le wallet sera créé automatiquement si vous suivez la procédure du guide `WALLET-SETUP.md` (via la commande `docker-compose run ...`).

### SSL / TLS (HAProxy)

Pour activer le Stratum SSL (port 4343) :

1.  Placez votre certificat (`euroxmr.pem`) dans `config/certs/`. *Le .pem doit contenir la clé privée ET le certificat.*
2.  Copiez `haproxy.cfg` dans `config/`.
3.  Démarrez le service SSL :
    ```bash
    docker-compose --profile ssl up -d
    ```

---

## 5. Administration

### Commandes Utiles

| Action | Commande |
|--------|----------|
| **Démarrer** | `docker-compose up -d` |
| **Arrêter** | `docker-compose down` |
| **Logs** | `docker-compose logs -f monero-pool` |
| **Status** | `docker-compose ps` |

### Mises à Jour

Utilisez le script de mise à jour interactif pour reconstruire les services après une modification ou un `git pull`.

*   **Windows** : Lancez `update.bat`.
*   **Linux** : Lancez `./update-linux.sh`.

Le menu vous permet de choisir de tout mettre à jour (`[A]`) ou seulement certains composants (`[P]` pour Pool seul).

### Sauvegardes (Backup)

Il est CRITIQUE de sauvegarder le dossier `pool-data` (qui contient les shares et les soldes des mineurs) et le dossier `wallet`.

*   **Windows** : Lancez `backup.bat`.
    *   Crée une archive `.zip` datée dans `backups\`.
    *   Peut être planifié avec le "Planificateur de tâches Windows".
*   **Linux** : Lancez `./backup-linux.sh`.
    *   Crée une archive `.tar.gz` datée dans `backups/`.
    *   Garde automatiquement les 7 derniers jours.

---

## 6. Fonctionnalités Avancées

### Loterie

Un système de loterie hebdomadaire est inclus.
*   **Activation** : `docker-compose --profile lottery up -d`.
*   **Fonctionnement** : 1 share valide = 1 ticket. Tirage le dimanche à 20h.
*   **Données** : Les résultats sont stockés dans `lottery-data`.

### Stratum Self-Select

Ce pool supporte le mode "Self-Select", permettant aux mineurs de choisir leur propre template de bloc (réduisant les risques de censure et de centralisation).
*   Le mineur se connecte avec `"mode": "self-select"`.
*   Voir `sss.md` pour les spécifications techniques complètes.

### Multi-Region / Remote Nodes

Pour étendre le pool (ex: un serveur aux USA connecté au serveur EU) :

1.  **Serveur Principal (EU)** :
    *   Configurer `trusted-listen` et `trusted-port` (ex: 4244) dans `pool.conf`.
    *   Ouvrir ce port dans `docker-compose.yml`.

2.  **Serveur Distant (US)** :
    *   Installer le pool normalement.
    *   Dans `pool.conf`, configurer `upstream-host` (IP du serveur EU) et `upstream-port`.
    *   Le serveur US relaiera alors le travail vers l'EU.

---

## 7. Dépannage

*   **Le pool ne démarre pas ?** Vérifiez les logs : `docker-compose logs monero-pool`. Souvent une erreur de config dans `pool.conf`.
*   **Pas de connexion wallet ?** Vérifiez que `monero-wallet-rpc` est démarré et que le mot de passe dans `config/wallet-password.txt` est correct.
*   **Erreur "Address already in use" ?** Un autre service utilise le port 80, 4242 ou 18081.

---
*Documentation générée le 2025-12-27 pour Monero Pool.*
