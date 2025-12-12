# 🔐 Configuration du Wallet pour l'Auto-Ouverture

Ce guide explique comment configurer le wallet pour qu'il s'ouvre automatiquement au démarrage du pool.

---

## 📁 Fichier de Mot de Passe

### Emplacement
```
C:\MoneroPool\config\wallet-password.txt
```

### Contenu
Le fichier doit contenir **uniquement** votre mot de passe wallet, sur une seule ligne, **sans espace ni retour à la ligne à la fin**.

**Exemple :**
```
MonMotDePasseTresSecurise123!
```

### Création du fichier (PowerShell)
```powershell
# Créer le fichier avec le mot de passe (remplacez par votre mot de passe)
Set-Content -Path "C:\MoneroPool\config\wallet-password.txt" -Value "VOTRE_MOT_DE_PASSE_ICI" -NoNewline
```

### Création du fichier (Notepad)
1. Ouvrir Notepad
2. Taper votre mot de passe (PAS de retour à la ligne!)
3. Fichier → Enregistrer sous
4. Emplacement : `C:\MoneroPool\config\`
5. Nom du fichier : `wallet-password.txt`
6. Type : `Tous les fichiers (*.*)`
7. Encodage : `UTF-8`

---

## 🔑 Création du Wallet (Première fois uniquement)

Avant de démarrer le pool, vous devez créer un wallet :

### Option 1 : Nouveau wallet (recommandé pour un nouveau pool)

```cmd
docker-compose run --rm monero-wallet-rpc ^
  --daemon-host=monerod ^
  --daemon-port=18081 ^
  --generate-new-wallet=/home/monero/wallet/pool-wallet ^
  --password=VOTRE_MOT_DE_PASSE_ICI
```

> ⚠️ **IMPORTANT** : Notez et sauvegardez la **seed phrase** (25 mots) affichée ! C'est le seul moyen de récupérer vos fonds si le wallet est perdu !

### Option 2 : Restaurer un wallet existant

```cmd
docker-compose run --rm monero-wallet-rpc ^
  --daemon-host=monerod ^
  --daemon-port=18081 ^
  --restore-from-seed ^
  --generate-new-wallet=/home/monero/wallet/pool-wallet ^
  --password=VOTRE_MOT_DE_PASSE_ICI
```

Le système vous demandera d'entrer votre seed phrase.

---

## 📂 Structure des fichiers Wallet

Après création, vous aurez ces fichiers dans `C:\MoneroPool\wallet\` :

```
C:\MoneroPool\wallet\
├── pool-wallet           ← Fichier wallet principal
├── pool-wallet.keys      ← Clés du wallet (CRITIQUE!)
└── pool-wallet.address.txt ← Adresse publique du wallet
```

---

## ✅ Vérification de la configuration

### 1. Vérifiez que le fichier password existe :
```cmd
type C:\MoneroPool\config\wallet-password.txt
```

### 2. Vérifiez que le wallet existe :
```cmd
dir C:\MoneroPool\wallet\
```
Vous devez voir `pool-wallet` et `pool-wallet.keys`.

### 3. Vérifiez dans docker-compose.yml :
```yaml
monero-wallet-rpc:
  command:
    - "--wallet-file=/home/monero/wallet/pool-wallet"
    - "--password-file=/config/wallet-password.txt"
```

---

## 🚀 Démarrage

Une fois configuré :

```cmd
docker-compose up -d
```

Le wallet s'ouvrira automatiquement !

### Vérifier que ça fonctionne :
```cmd
docker-compose logs monero-wallet-rpc
```

Vous devez voir quelque chose comme :
```
Opened wallet: 4...
Starting wallet RPC server
```

---

## ⚠️ Sécurité

1. **Ne partagez JAMAIS** le fichier `wallet-password.txt`
2. **Sauvegardez** votre seed phrase (25 mots) en lieu sûr, **hors ligne**
3. **Sauvegardez** les fichiers wallet régulièrement
4. Considérez utiliser un **view-only wallet** pour le pool et garder le wallet complet ailleurs

---

## 🔧 Dépannage

### Erreur "wallet file not found"
Le wallet n'a pas encore été créé. Suivez la section "Création du Wallet".

### Erreur "invalid password"
Le mot de passe dans `wallet-password.txt` ne correspond pas à celui utilisé lors de la création du wallet.

### Erreur "wallet locked"
Un autre processus utilise le wallet. Arrêtez tout et redémarrez :
```cmd
docker-compose down
docker-compose up -d
```

### Le wallet ne se synchronise pas
Attendez que monerod soit synchronisé à 100% :
```cmd
docker-compose logs monerod | findstr "Synced"
```
