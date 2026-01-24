# Monero Mining Pool

![Monero Pool](./src/og-image.png)

Pool de minage Monero haute performance basée sur [monero-pool](https://github.com/jtgrassie/monero-pool) de jtgrassie, optimisée pour le déploiement Docker.

> [!CAUTION]
> **USAGE RESTRICTIONS / RESTRICTIONS D'UTILISATION**
>
> 1.  **LOTTERY SYSTEM / SYSTÈME DE LOTERIE**: Strictly reserved for the owner's use. / Strictement réservé à l'usage du propriétaire.
> 2.  **PRIVATE USE ONLY / USAGE PRIVÉ UNIQUEMENT**: This pool is for private, non-commercial use only. / Ce pool est destiné à un usage privé et non commercial uniquement.


## ✨ Caractéristiques

- **Performance** : Architecture légère en C avec libevent et LMDB
- **RandomX** : Support complet du mode fast/full-memory RandomX
- **PPLNS** : Système de paiement équitable favorisant les mineurs loyaux
- **Self-Select** : Option permettant aux mineurs de choisir leur propre template de bloc
- **Docker** : Déploiement complet containerisé (monerod, wallet-rpc, pool)
- **SSL/TLS** : Support HAProxy pour les connexions sécurisées (port 4343)
- **CLI Unifié** : Outils de gestion simplifiés pour Linux et Windows
- **Merge Mining** : Support du minage dual XMR + Tari (XTM)

## 🚀 Démarrage Rapide

### Prérequis

- Docker et Docker Compose
- ~80 Go d'espace disque (blockchain pruned)
- 4 Go de RAM minimum

### Installation et Gestion

Nous fournissons un script de gestion unifié pour simplifier toutes les opérations (`manage.sh` pour Linux, `manage.bat` pour Windows).

#### Linux
```bash
# 1. Configuration initiale (création des dossiers, configs)
sudo ./manage.sh setup

# 2. Editer la configuration
nano /opt/monero-pool/config/pool.conf

# 3. Créer le wallet
sudo ./manage.sh create-wallet

# 4. Démarrer le pool
sudo ./manage.sh start
```

#### Windows (PowerShell / CMD en Admin)
```batch
REM 1. Configuration initiale
manage.bat setup

REM 2. Editer la configuration
notepad C:\MoneroPool\config\pool.conf

REM 3. Créer le wallet
manage.bat create-wallet

REM 4. Démarrer le pool
manage.bat start
```

## 📁 Structure des Données

Par défaut, les données sont stockées dans `/opt/monero-pool` (Linux) ou `C:\MoneroPool` (Windows).
Ce chemin peut être modifié lors du setup (enregistré dans le fichier `.env`).

| Répertoire | Description |
|------------|-------------|
| `blockchain` | Blockchain Monero (~70 Go pruned) |
| `wallet` | Fichiers du wallet pool |
| `pool-data` | Base de données pool (shares, paiements) |
| `config` | Fichiers de configuration |
| `backups` | Sauvegardes automatiques |
| `tari-data` | Données du nœud Tari (si activé) |
| `tari-wallet` | Wallet Tari (si activé) |

## 🔧 Commandes de Gestion (CLI)

Utilisez `./manage.sh <commande>` (Linux) ou `manage.bat <commande>` (Windows).

| Commande | Description |
|----------|-------------|
| `setup` | Installe les dépendances et crée la structure de dossiers |
| `start` | Démarre tous les services (Daemon, Wallet RPC, Pool) |
| `start-tari` | Démarre les services Tari pour le merge mining |
| `stop` | Arrête tous les services |
| `restart` | Redémarre les services |
| `logs` | Affiche les logs en temps réel |
| `create-wallet` | Assistant de création de wallet sécurisé |
| `update` | Met à jour le code, rebuild les images et redémarre |
| `backup` | Crée une sauvegarde immédiate |
| `restore <file>` | Restaure à partir d'une sauvegarde |
| `schedule-backups`| Configure la sauvegarde automatique quotidienne (3h00) |

## ⛏️ Connexion des Mineurs

| Type | Adresse | Port |
|------|---------|------|
| **Stratum** | `votre-serveur.com` | `4242` |
| **Stratum SSL** | `votre-serveur.com` | `4343` |

### Exemple avec XMRig

```json
{
  "pools": [{
    "url": "votre-serveur.com:4242",
    "user": "VOTRE_ADRESSE_MONERO",
    "pass": "x"
  }]
}
```

### Minage Dual XMR + XTM (Tari)

Pour miner simultanément XMR et XTM, utilisez le format d'adresse dual :

```json
{
  "pools": [{
    "url": "votre-serveur.com:4242",
    "user": "ADRESSE_MONERO.ADRESSE_TARI",
    "pass": "x"
  }]
}
```

Activez le merge mining dans `pool.conf` :
```ini
tari-enabled = 1
tari-base-node-host = tari-base-node
tari-base-node-grpc-port = 18142
```

Puis démarrez les services Tari :
```bash
./manage.sh start-tari
```

## 🌐 Interface Web

Accessible sur le port `80` : `http://votre-serveur.com`

## ⚙️ Configuration Avancée

### pool.conf

Fichier principal de configuration situé dans `config/pool.conf` :

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `pool-port` | Port Stratum | `4242` |
| `pool-ssl-port` | Port Stratum SSL | `4343` |
| `webui-port` | Port interface web | `80` |
| `pool-fee` | Frais du pool (%) | `0` |
| `payment-threshold` | Seuil de paiement (XMR) | `0.005` |

### SSL avec HAProxy

Pour activer les connexions SSL :
1. Placez votre certificat dans `config/certs/euroxmr.pem`
2. Modifiez `docker-compose` pour activer le profil SSL ou démarrez manuellement HAProxy.

## 💾 Sauvegarde et Restauration

### Sauvegarde Automatique
Pour activer les sauvegardes quotidiennes (à 3h00 du matin) :
```bash
./manage.sh schedule-backups
# ou
manage.bat schedule-backups
```

### Restauration
Pour restaurer une sauvegarde :
```bash
./manage.sh restore backup_2023-10-27_10-00.tar.gz
```

## 📚 Documentation

- [ARCHITECTURE.md](./ARCHITECTURE.md) - Architecture technique du pool
- [DOCKER-GUIDE.md](./DOCKER-GUIDE.md) - Guide complet de déploiement Docker
- [WALLET-SETUP.md](./WALLET-SETUP.md) - Configuration du wallet
- [EUROXMR-GUIDE.md](./EUROXMR-GUIDE.md) - Guide spécifique EuroXMR
- [sss.md](./sss.md) - Mode Stratum Self-Select

## 🙏 Crédits

Ce pool est basé sur [monero-pool](https://github.com/jtgrassie/monero-pool) par **jtgrassie**.

Donations au développeur original :
```
451ytzQg1vUVkuAW73VsQ72G96FUjASi4WNQse3v8ALfjiR5vLzGQ2hMUdYhG38Fi15eJ5FJ1ZL4EV1SFVi228muGX4f3SV
```

## 📄 Licence

[LICENSE](./LICENSE)
