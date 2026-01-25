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
- **Gestion Unifiée** : Scripts `manage.sh` (Linux) et `manage.bat` (Windows) pour toutes les opérations.

## 🚀 Démarrage Rapide

### Prérequis

- Docker Desktop (Windows/Linux) ou Docker Engine (Linux)
- ~80 Go d'espace disque (blockchain pruned)
- 4 Go de RAM minimum

### Installation

1. **Initialisation** :
   *   **Linux** :
       ```bash
       ./manage.sh setup
       ```
   *   **Windows** :
       ```batch
       manage.bat setup
       ```

2. **Configurer le wallet** :
   Suivez les instructions interactives pour créer le wallet ou placez votre mot de passe dans `config/wallet-password.txt`.

   Pour créer un wallet via le script :
   ```bash
   # Linux
   ./manage.sh create-wallet

   # Windows
   manage.bat create-wallet
   ```

3. **Démarrer les services** :
   ```bash
   # Linux
   ./manage.sh start

   # Windows
   manage.bat start
   ```

4. **Vérifier les logs** :
   ```bash
   # Linux
   ./manage.sh logs

   # Windows
   manage.bat logs
   ```

## 📁 Structure des Données

Les données sont stockées par défaut dans `/opt/monero-pool` (Linux) ou `C:\MoneroPool` (Windows).

| Répertoire | Description |
|------------|-------------|
| `xmr-data` | Blockchain Monero (~70 Go pruned) |
| `xmr-wallet` | Fichiers du wallet pool |
| `xmr-pool-data` | Base de données pool (shares, paiements) |
| `config` | Fichiers de configuration |
| `tari-data` | Données du nœud Tari (Merge Mining) |
| `tari-wallet` | Wallet Tari |

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
# Linux
./manage.sh start-tari

# Windows
manage.bat start-tari
```

## 🌐 Interface Web

Accessible sur le port `80` : `http://votre-serveur.com`

L'interface fournit :
- Statistiques du pool (hashrate, blocs trouvés)
- Statistiques par mineur (balance, paiements)
- Historique des paiements

## ⚙️ Configuration

### pool.conf

Fichier principal de configuration situé dans `config/pool.conf` :

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `pool-port` | Port Stratum | `4242` |
| `pool-ssl-port` | Port Stratum SSL | `4343` |
| `webui-port` | Port interface web | `80` |
| `pool-fee` | Frais du pool (%) | `0` |
| `payment-threshold` | Seuil de paiement (XMR) | `0.005` |
| `pool-start-diff` | Difficulté initiale | `1000` |

## 🔧 Commandes Utiles

Les scripts `manage.sh` (Linux) et `manage.bat` (Windows) centralisent toutes les commandes :

```bash
# Démarrer tous les services
./manage.sh start

# Arrêter tous les services
./manage.sh stop

# Redémarrer
./manage.sh restart

# Voir les logs
./manage.sh logs
./manage.sh logs monerod

# Créer un backup immédiat
./manage.sh backup

# Restaurer un backup
./manage.sh restore backup_2023-10-27_10-00.tar.gz

# Mettre à jour (git pull + rebuild)
./manage.sh update
```

## 📚 Documentation

- [ARCHITECTURE.md](./ARCHITECTURE.md) - Architecture technique du pool
- [DOCKER-GUIDE.md](./DOCKER-GUIDE.md) - Guide complet de déploiement Docker
- [WALLET-SETUP.md](./WALLET-SETUP.md) - Configuration du wallet
- [EUROXMR-GUIDE.md](./EUROXMR-GUIDE.md) - Guide spécifique EuroXMR
- [sss.md](./sss.md) - Mode Stratum Self-Select

## 💾 Sauvegarde

Exécuter régulièrement :
```bash
# Linux
./manage.sh backup

# Windows
manage.bat backup
```

Vous pouvez planifier des backups quotidiens automatiquement :
```bash
# Linux (via cron)
./manage.sh schedule-backups

# Windows (via Task Scheduler)
manage.bat schedule-backups
```

## 🙏 Crédits

Ce pool est basé sur [monero-pool](https://github.com/jtgrassie/monero-pool) par **jtgrassie**.

Donations au développeur original :
```
451ytzQg1vUVkuAW73VsQ72G96FUjASi4WNQse3v8ALfjiR5vLzGQ2hMUdYhG38Fi15eJ5FJ1ZL4EV1SFVi228muGX4f3SV
```

## 📄 Licence

[LICENSE](./LICENSE)
