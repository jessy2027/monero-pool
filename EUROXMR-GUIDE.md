# EuroXMR Pool - Guide de Configuration

Ce guide documente les personnalisations et améliorations apportées à EuroXMR, la pool de minage Monero européenne.

## 🌟 Fonctionnalités Ajoutées

### Interface Web Améliorée

L'interface web (`src/webui-embed.html`) a été enrichie avec :

1. **Section "Pourquoi EuroXMR ?"** - 5 cartes animées présentant les avantages :
   - ⚡ Faible latence (serveurs européens)
   - 🎯 PPLNS équitable
   - 💎 Seuil de paiement bas (0.005 XMR)
   - 🔒 Support SSL/TLS
   - 🚀 RandomX mode complet

2. **Statistiques de Confiance** - Affichage en temps réel :
   - Uptime
   - Total payé
   - Blocs trouvés
   - Mineurs actifs

3. **Calculateur de Rentabilité** - Interactive :
   - Entrée hashrate, puissance, coût électricité
   - Affichage gains journaliers/mensuels en XMR et €
   - Calcul du profit net après coût électricité
   - Prix XMR récupéré automatiquement via CoinGecko

4. **Générateur de Config XMRig** - Automatique :
   - Entrée adresse wallet et nom worker
   - Option SSL/non-SSL
   - Configuration JSON prête à copier

5. **FAQ Accordion** - 5 questions fréquentes :
   - PPLNS expliqué
   - Timing des paiements
   - Configuration XMRig
   - Hashrates attendus par hardware
   - Avantages SSL

6. **Multi-langues** - 5 langues supportées :
   - 🇬🇧 Anglais (EN)
   - 🇫🇷 Français (FR)
   - 🇩🇪 Allemand (DE)
   - 🇪🇸 Espagnol (ES)
   - 🇷🇺 Russe (RU)

7. **🎰 Loterie Hebdomadaire** - Gamification du minage :
   - 1 Share valide = 1 Ticket de loterie
   - Tirage automatique chaque dimanche à 20h CET
   - Affichage en temps réel des chances de gain
   - Historique des gagnants avec TxID vérifiable
   - 100% transparent (code open source)


### SEO Optimisé

Balises meta ajoutées dans `<head>` :
- Open Graph pour Facebook/LinkedIn
- Twitter Cards
- Keywords ciblés Monero mining
- URL canonique
- Theme colors mobile

---

## 🔐 Configuration SSL (HAProxy)

### Prérequis

1. Un certificat SSL valide pour votre domaine
2. Les fichiers de configuration en place

### Structure des fichiers

```
C:\MoneroPool\config\
├── pool.conf              # Configuration du pool
├── wallet-password.txt    # Mot de passe wallet
├── haproxy.cfg           # Configuration HAProxy
└── certs\
    └── euroxmr.pem       # Certificat SSL (cert + key combinés)
```

### Création du certificat

#### Option A : Let's Encrypt (gratuit)

```bash
# Sur le serveur Linux
sudo apt install certbot
certbot certonly --standalone -d euroxmr.eu

# Combiner certificat et clé
cat /etc/letsencrypt/live/euroxmr.eu/fullchain.pem \
    /etc/letsencrypt/live/euroxmr.eu/privkey.pem > euroxmr.pem

# Copier vers le dossier config
cp euroxmr.pem /path/to/MoneroPool/config/certs/
```

#### Option B : Certificat auto-signé (test uniquement)

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout euroxmr.key -out euroxmr.crt \
  -subj "/CN=euroxmr.eu"

cat euroxmr.crt euroxmr.key > euroxmr.pem
```

### Démarrer HAProxy

```bash
# Copier la configuration
copy haproxy.cfg C:\MoneroPool\config\haproxy.cfg

# Démarrer avec le profil SSL
docker-compose --profile ssl up -d haproxy

# Vérifier le statut
docker logs haproxy-ssl
```

### Configuration HAProxy (`haproxy.cfg`)

```
frontend stratum_ssl
    bind *:4343 ssl crt /etc/ssl/certs/euroxmr.pem
    mode tcp
    default_backend stratum_pool

backend stratum_pool
    mode tcp
    server pool1 monero-pool:4242 check
```

---

## 📁 Fichiers Modifiés

| Fichier | Description |
|---------|-------------|
| `src/webui-embed.html` | Interface web complète avec nouvelles fonctionnalités |
| `docker-compose.yml` | Ajout du service HAProxy pour SSL |
| `haproxy.cfg` | Configuration HAProxy pour terminaison TLS |
| `pool.conf` | Port SSL configuré (4343) |
| `og-image.png` | Image pour partage réseaux sociaux |

---

## 🚀 Démarrage Rapide

### Sans SSL

```bash
docker-compose up -d
```

### Avec SSL

```bash
# 1. Placer le certificat
mkdir -p C:\MoneroPool\config\certs
copy euroxmr.pem C:\MoneroPool\config\certs\

# 2. Copier la config HAProxy
copy haproxy.cfg C:\MoneroPool\config\

# 3. Démarrer tout
docker-compose --profile ssl up -d
```

---

## 🌐 URLs de Connexion

| Type | URL |
|------|-----|
| Web UI | http://euroxmr.eu |
| Stratum | `stratum+tcp://euroxmr.eu:4242` |
| Stratum SSL | `stratum+ssl://euroxmr.eu:4343` |

---

## 📊 Annuaires de Pools

Inscrivez votre pool sur :
- [MiningPoolStats](https://miningpoolstats.stream/monero)
- [XMR Wiki Pools](https://pools.xmr.wiki)
- [MoneroPools](https://moneropools.com)

---

## 🔧 Recompilation

Après modification de `webui-embed.html`, recompiler le pool :

```bash
make release
docker-compose build --no-cache monero-pool
docker-compose up -d monero-pool
```
