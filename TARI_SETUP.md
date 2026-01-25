# Guide de Configuration Tari Merge Mining (GCP)

Ce guide explique comment activer le minage combiné (Merged Mining) Monero + Tari sur votre infrastructure Euro XMR existante.

## 1. Infrastructure Google Cloud (GCP)

### A. Disque Persistant (Persistent Disk)

La blockchain Tari nécessite de l'espace disque (similaire à Monero). Il est recommandé d'utiliser un disque séparé ou d'étendre votre disque actuel.

**Si vous ajoutez un nouveau disque dédié à Tari :**

1.  **Créer le disque (via Google Cloud Console ou CLI) :**
    ```bash
    gcloud compute disks create tari-data-disk --size=100GB --type=pd-ssd --zone=VOTRE_ZONE
    ```

2.  **Attacher le disque à votre VM :**
    ```bash
    gcloud compute instances attach-disk VOTRE_VM --disk=tari-data-disk --zone=VOTRE_ZONE
    ```

3.  **Formater et Monter le disque (sur la VM Linux) :**
    ```bash
    # Trouver le disque (souvent /dev/sdb)
    sudo lsblk

    # Formater (ATTENTION : efface tout sur le disque)
    sudo mkfs.ext4 -m 0 -E lazy_itable_init=0,lazy_journal_init=0,discard /dev/sdb

    # Créer le point de montage
    sudo mkdir -p /opt/monero-pool/tari-data

    # Monter
    sudo mount -o discard,defaults /dev/sdb /opt/monero-pool/tari-data

    # Configurer les permissions
    sudo chown -R $USER:$USER /opt/monero-pool/tari-data

    # Ajouter à fstab pour le montage automatique au redémarrage
    echo UUID=`sudo blkid -s UUID -o value /dev/sdb` /opt/monero-pool/tari-data ext4 discard,defaults,nofail 0 2 | sudo tee -a /etc/fstab
    ```

### B. Pare-feu (Firewall Rules)

Vous devez ouvrir **uniquement** le port P2P de Tari pour permettre la synchronisation avec le réseau.

*   **Port TCP 18189** : Allow Ingress (Depuis n'importe où `0.0.0.0/0`).
    *   *Usage :* Communication Peer-to-Peer Tari.

**Important :** Ne PAS ouvrir les ports 18142 ou 18143 sur l'internet public. Ils sont utilisés pour la communication gRPC interne (entre le pool et le nœud) et sont sécurisés au sein du réseau Docker.

Commande gcloud :
```bash
gcloud compute firewall-rules create allow-tari-p2p \
    --allow tcp:18189 \
    --description="Allow Tari P2P network traffic" \
    --direction=INGRESS
```

## 2. Configuration du Pool

1.  **Modifier le fichier de configuration :**
    ```bash
    nano /opt/monero-pool/config/pool.conf
    ```

2.  **Ajouter/Modifier les lignes suivantes :**
    ```ini
    # Configuration Tari Merge Mining
    tari-enabled = 1
    tari-base-node-host = tari-base-node
    tari-base-node-grpc-port = 18142
    tari-wallet-host = tari-wallet
    tari-wallet-grpc-port = 18143
    tari-poll-interval-ms = 1000
    ```

## 3. Démarrage (Deployment)

Le système utilise désormais des conteneurs Docker gérés par `manage.sh`. Le nœud Tari et le Wallet tourneront en tant que services système (via Docker Restart policies), assurant un redémarrage automatique en cas de reboot de la VM.

1.  **Mettre à jour l'infrastructure :**
    ```bash
    ./manage.sh update
    ```

2.  **Démarrer les services Tari :**
    ```bash
    ./manage.sh start-tari
    ```

3.  **Vérifier le fonctionnement :**
    ```bash
    # Logs du nœud Tari (synchronisation)
    ./manage.sh logs tari-base-node

    # Logs du Wallet
    ./manage.sh logs tari-wallet

    # Logs du Pool (vérifier l'initialisation Tari)
    ./manage.sh logs monero-pool
    ```

## 4. Vérification du Minage

Une fois le nœud Tari synchronisé :
1.  Connectez un mineur XMRig avec une adresse au format `ADRESSE_XMR.ADRESSE_TARI` (si supporté par le frontend/proxy) ou configurez votre propre wallet Tari dans `pool.conf` si vous minez pour le pool (selon la politique de paiement).
2.  Surveillez les logs du pool pour voir les messages : `Injected Tari MM tag`.

## 5. Maintenance

*   **Arrêt :** `./manage.sh stop` arrête tout, y compris Tari.
*   **Backup :** `./manage.sh backup` inclut désormais les données Tari (`tari-data` et `tari-wallet`).
