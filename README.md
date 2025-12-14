# Monero Mining Pool

![Monero Pool](./src/og-image.png)

Pool de minage Monero haute performance basée sur [monero-pool](https://github.com/jtgrassie/monero-pool) de jtgrassie, optimisée pour le déploiement Docker.

## ✨ Caractéristiques

- **Performance** : Architecture légère en C avec libevent et LMDB
- **RandomX** : Support complet du mode fast/full-memory RandomX
- **PPLNS** : Système de paiement équitable favorisant les mineurs loyaux
- **Self-Select** : Option permettant aux mineurs de choisir leur propre template de bloc
- **Docker** : Déploiement complet containerisé (monerod, wallet-rpc, pool)
- **SSL/TLS** : Support HAProxy pour les connexions sécurisées (port 4343)

## 🚀 Démarrage Rapide

### Prérequis

- Docker Desktop pour Windows
- ~80 Go d'espace disque (blockchain pruned)
- 4 Go de RAM minimum

### Installation

1. **Créer les répertoires de données** :
   ```batch
   setup-windows.bat
   ```

2. **Configurer le wallet** :  
   Créer le fichier `C:\MoneroPool\config\wallet-password.txt` avec votre mot de passe.

3. **Démarrer les services** :
   ```bash
   docker-compose up -d
   ```

4. **Vérifier les logs** :
   ```bash
   docker-compose logs -f monero-pool
   ```

## 📁 Structure des Données

| Répertoire | Description |
|------------|-------------|
| `C:\MoneroPool\blockchain` | Blockchain Monero (~70 Go pruned) |
| `C:\MoneroPool\wallet` | Fichiers du wallet pool |
| `C:\MoneroPool\pool-data` | Base de données pool (shares, paiements) |
| `C:\MoneroPool\config` | Fichiers de configuration |

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

## 🌐 Interface Web

Accessible sur le port `80` : `http://votre-serveur.com`

L'interface fournit :
- Statistiques du pool (hashrate, blocs trouvés)
- Statistiques par mineur (balance, paiements)
- Historique des paiements

## ⚙️ Configuration

### pool.conf

Fichier principal de configuration situé dans `C:\MoneroPool\config\pool.conf` :

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `pool-port` | Port Stratum | `4242` |
| `pool-ssl-port` | Port Stratum SSL | `4343` |
| `webui-port` | Port interface web | `80` |
| `pool-fee` | Frais du pool (%) | `0` |
| `payment-threshold` | Seuil de paiement (XMR) | `0.005` |
| `pool-start-diff` | Difficulté initiale | `1000` |

### SSL avec HAProxy

Pour activer les connexions SSL :

1. Placer votre certificat dans `C:\MoneroPool\config\certs\euroxmr.pem`
2. Démarrer avec le profil SSL :
   ```bash
   docker-compose --profile ssl up -d
   ```

## 🔧 Commandes Utiles

```bash
# Démarrer tous les services
docker-compose up -d

# Arrêter tous les services
docker-compose down

# Voir les logs du pool
docker-compose logs -f monero-pool

# Voir les logs du daemon
docker-compose logs -f monerod

# Reconstruire le pool après modification
docker-compose build --no-cache monero-pool && docker-compose up -d monero-pool

# Backup des données
backup.bat
```

## 📚 Documentation

- [ARCHITECTURE.md](./ARCHITECTURE.md) - Architecture technique du pool
- [DOCKER-GUIDE.md](./DOCKER-GUIDE.md) - Guide complet de déploiement Docker
- [WALLET-SETUP.md](./WALLET-SETUP.md) - Configuration du wallet
- [EUROXMR-GUIDE.md](./EUROXMR-GUIDE.md) - Guide spécifique EuroXMR
- [sss.md](./sss.md) - Mode Stratum Self-Select

## 💾 Sauvegarde

Exécuter régulièrement :
```batch
backup.bat
```

**Fichiers critiques à sauvegarder** :
- `C:\MoneroPool\pool-data` - Base de données (balances, paiements)
- `C:\MoneroPool\wallet` - Wallet du pool
- `C:\MoneroPool\config\pool.conf` - Configuration

## 🙏 Crédits

Ce pool est basé sur [monero-pool](https://github.com/jtgrassie/monero-pool) par **jtgrassie**.

Donations au développeur original :
```
451ytzQg1vUVkuAW73VsQ72G96FUjASi4WNQse3v8ALfjiR5vLzGQ2hMUdYhG38Fi15eJ5FJ1ZL4EV1SFVi228muGX4f3SV
```

## 📄 Licence

[LICENSE](./LICENSE)
