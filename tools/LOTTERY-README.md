# 🎰 EuroXMR Hashrate Lottery System

## Concept

La **Loterie de Hashrate** est un système de gamification pour motiver les mineurs à rester connectés à la pool, même pendant les périodes sans découverte de blocs.

### Règles
- **1 Share valide = 1 Ticket de loterie**
- **Tirage** : Chaque dimanche à 20h00 CET
- **Prix** : 0.05 XMR (configurable)
- **Sélection** : Pondérée (plus de shares = plus de chances, mais tout le monde peut gagner)

---

## Scripts

### `lottery_bot.py` - Script Principal de Tirage

Le cœur du système. Il lit les shares depuis la base LMDB et effectue un tirage au sort pondéré.

#### Usage

```bash
# Voir les statistiques actuelles (sans tirage)
python lottery_bot.py /chemin/vers/pooldb --stats

# Lancer un tirage test (dry-run, ne sauvegarde pas)
python lottery_bot.py /chemin/vers/pooldb --run --dry-run

# Lancer le tirage réel
python lottery_bot.py /chemin/vers/pooldb --run --prize 0.05

# Options avancées
python lottery_bot.py /chemin/vers/pooldb --run --period 7 --prize 0.1
```

#### Options
| Option | Description | Défaut |
|--------|-------------|--------|
| `--run` | Lance le tirage | - |
| `--stats` | Affiche les stats sans tirage | - |
| `--dry-run` | Simulation (pas de sauvegarde) | false |
| `--period` | Nombre de jours à considérer | 7 |
| `--prize` | Prix en XMR | 0.05 |

---

### `lottery_api.py` - Générateur JSON pour le Frontend

Génère les fichiers JSON utilisés par l'interface web.

#### Usage

```bash
python lottery_api.py /chemin/vers/pooldb /chemin/vers/www --period 7 --prize 0.05
```

Cela crée `lottery_stats.json` dans le dossier de sortie.

---

## Configuration Cron (Linux)

```cron
# Générer les stats toutes les heures
0 * * * * /usr/bin/python3 /path/to/tools/lottery_api.py /path/to/pooldb /var/www/html

# Tirage automatique le dimanche à 20h00
0 20 * * 0 /usr/bin/python3 /path/to/tools/lottery_bot.py /path/to/pooldb --run --prize 0.05
```

---

## Intégration Frontend

Le frontend (`webui-embed.html`) inclut déjà :
- ✅ Section Loterie avec design premium
- ✅ Countdown en temps réel jusqu'au prochain tirage
- ✅ Affichage des chances de l'utilisateur
- ✅ Tableau des gagnants récents
- ✅ Traductions EN/FR

Le frontend charge automatiquement `/lottery_stats.json` toutes les 5 minutes.

---

## Format des Données

### `lottery_stats.json` (généré par lottery_api.py)

```json
{
  "generated_at": "2024-12-14T20:00:00",
  "period_days": 7,
  "prize_xmr": 0.05,
  "next_draw": {
    "iso": "2024-12-15T20:00:00",
    "timestamp": 1734285600,
    "formatted": "Dimanche 15 Décembre à 20h00"
  },
  "stats": {
    "total_tickets": 125000,
    "total_participants": 15
  },
  "participants": [
    {
      "address": "4AwT...9zK",
      "address_short": "4AwT...9zK",
      "shares": 45000,
      "probability": 36.0
    }
  ]
}
```

### `lottery_results.json` (généré par lottery_bot.py)

```json
{
  "last_draw": {
    "draw_date": "2024-12-14T20:00:00",
    "winner": {
      "address": "4AwT...9zK",
      "shares": 12500,
      "probability": 15.3
    },
    "prize_xmr": 0.05,
    "txid": null
  },
  "history": [...]
}
```

---

## Transparence

Pour garantir la confiance des mineurs :

1. **Code Open Source** : Le script est public sur GitHub
2. **TxID Vérifiable** : Après paiement, le TxID est affiché et vérifiable sur la blockchain
3. **Tirage Reproductible** : L'algorithme de sélection pondérée est documenté

---

## Workflow Manuel (Phase de Lancement)

Pour les premiers tirages, vous pouvez procéder manuellement :

1. Exporter les stats : `python lottery_bot.py /db --stats > stats.txt`
2. Faire le tirage : `python lottery_bot.py /db --run --dry-run`
3. Enregistrer une vidéo du terminal pendant le tirage
4. Publier la vidéo sur Discord/Twitter
5. Effectuer le paiement manuellement
6. Mettre à jour `lottery_results.json` avec le TxID

---

## Dépendances

```bash
pip install lmdb
```

---

## Licence

MIT License - Partie du projet EuroXMR Pool
