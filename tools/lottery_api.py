#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
🎰 EuroXMR Lottery API Generator
=================================

Génère les fichiers JSON pour l'affichage de la loterie sur le site web.
Ce script doit être lancé périodiquement (cron) pour mettre à jour les stats.

Usage:
  python lottery_api.py <database_path> <output_dir>
  
Fichiers générés:
  - lottery_stats.json    : Statistiques actuelles
  - lottery_results.json  : Historique des tirages
'''

import argparse
import lmdb
import json
import os
from ctypes import *
from datetime import datetime, timedelta
from collections import defaultdict

# Structure compatible avec la pool
class share_t(Structure):
    _fields_ = [
        ('height', c_longlong),
        ('difficulty', c_longlong),
        ('address', c_char * 128),
        ('timestamp', c_longlong)
    ]


def format_address(address):
    """Formate une adresse pour l'affichage"""
    return '{}...{}'.format(address[:4], address[-4:])


def get_next_draw_date():
    """Calcule la date du prochain tirage (dimanche 20h)"""
    now = datetime.now()
    days_until_sunday = (6 - now.weekday()) % 7
    if days_until_sunday == 0 and now.hour >= 20:
        days_until_sunday = 7
    next_sunday = now + timedelta(days=days_until_sunday)
    next_draw = next_sunday.replace(hour=20, minute=0, second=0, microsecond=0)
    return {
        'iso': next_draw.isoformat(),
        'timestamp': int(next_draw.timestamp()),
        'formatted': next_draw.strftime('%A %d %B à %Hh%M')
    }


def get_lottery_stats(db_path, period_days=7, prize_xmr=0.05):
    """
    Génère les statistiques de la loterie actuelle.
    
    Returns:
        dict: Statistiques pour l'affichage web
    """
    participants = defaultdict(int)
    cutoff_timestamp = int((datetime.now() - timedelta(days=period_days)).timestamp())
    total_shares = 0
    
    try:
        env = lmdb.open(db_path, readonly=True, max_dbs=1, create=False)
        shares_db = env.open_db('shares'.encode(), dupsort=True)
        
        with env.begin(db=shares_db) as txn:
            with txn.cursor() as curs:
                for key, value in curs:
                    share = share_t.from_buffer_copy(value)
                    if share.timestamp >= cutoff_timestamp:
                        wallet_address = share.address.decode('utf-8').rstrip('\0')
                        participants[wallet_address] += 1
                        total_shares += 1
                        
        env.close()
        
    except Exception as e:
        print(f"Erreur: {e}")
        return None
    
    # Calcul des probabilités
    sorted_participants = sorted(participants.items(), key=lambda x: x[1], reverse=True)
    
    participants_data = []
    for wallet, shares in sorted_participants[:50]:  # Top 50
        probability = (shares / total_shares * 100) if total_shares > 0 else 0
        participants_data.append({
            'address': wallet,
            'address_short': format_address(wallet),
            'shares': shares,
            'probability': round(probability, 4)
        })
    
    return {
        'generated_at': datetime.now().isoformat(),
        'period_days': period_days,
        'prize_xmr': prize_xmr,
        'next_draw': get_next_draw_date(),
        'stats': {
            'total_tickets': total_shares,
            'total_participants': len(participants)
        },
        'participants': participants_data
    }


def get_user_probability(participants_data, wallet_address):
    """
    Trouve la probabilité d'un utilisateur spécifique.
    Utilisé par l'API pour afficher "Vos Chances".
    """
    for p in participants_data:
        if p['address'] == wallet_address:
            return p
    return {
        'address': wallet_address,
        'address_short': format_address(wallet_address),
        'shares': 0,
        'probability': 0
    }


def main():
    parser = argparse.ArgumentParser(description='Génère les fichiers JSON pour la loterie')
    parser.add_argument('database', help='Chemin vers la base LMDB')
    parser.add_argument('output_dir', help='Dossier de sortie pour les fichiers JSON')
    parser.add_argument('--period', type=int, default=7, help='Période en jours')
    parser.add_argument('--prize', type=float, default=0.05, help='Prix en XMR')
    
    args = parser.parse_args()
    
    # Génère les stats
    print("📊 Génération des statistiques...")
    stats = get_lottery_stats(args.database, args.period, args.prize)
    
    if stats:
        # Sauvegarde
        os.makedirs(args.output_dir, exist_ok=True)
        output_file = os.path.join(args.output_dir, 'lottery_stats.json')
        
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(stats, f, indent=2, ensure_ascii=False)
            
        print(f"✅ Fichier généré: {output_file}")
        print(f"   - {stats['stats']['total_participants']} participants")
        print(f"   - {stats['stats']['total_tickets']:,} tickets")
    else:
        print("❌ Erreur lors de la génération")


if __name__ == '__main__':
    main()
