#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
🎰 EuroXMR Hashrate Lottery Bot
================================

Système de loterie hebdomadaire pour la pool EuroXMR.
Règles:
  - 1 Share valide = 1 Ticket
  - Tirage tous les dimanches à 20h00
  - Prix: 0.05 XMR (ou % des frais de pool)

Copyright (c) 2025, EuroXMR Pool
License: MIT

Usage:
  python lottery_bot.py <database_path> [options]
  
Options:
  --dry-run     Simule le tirage sans sauvegarder les résultats
  --period      Nombre de jours à considérer (défaut: 7)
  --prize       Montant du prix en XMR (défaut: 0.05)
'''

import argparse
import lmdb
import random
import json
import os
from ctypes import *
from datetime import datetime, timedelta
from collections import defaultdict

# ============================================================================
# STRUCTURES DE DONNÉES (compatibles avec la pool monero-pool)
# ============================================================================

class share_t(Structure):
    """Structure d'une share dans la base LMDB"""
    _fields_ = [
        ('height', c_longlong),
        ('difficulty', c_longlong),
        ('address', c_char * 128),
        ('timestamp', c_longlong)
    ]


# ============================================================================
# FONCTIONS UTILITAIRES
# ============================================================================

def format_address(address):
    """Formate une adresse pour l'affichage (tronquée)"""
    return '{}...{}'.format(address[:8], address[-8:])


def format_address_full(address):
    """Retourne l'adresse complète sans les caractères nuls"""
    return address.rstrip('\0')


def address_from_key(key):
    """Extrait l'adresse depuis une clé LMDB"""
    return key.decode('utf-8').rstrip('\0')


def format_timestamp(timestamp):
    """Formate un timestamp en date lisible"""
    dt = datetime.fromtimestamp(timestamp)
    return dt.strftime('%Y-%m-%d %H:%M:%S')


def format_amount(amount_piconero):
    """Convertit piconero en XMR"""
    return '{0:.6f}'.format(amount_piconero / 1e12)


# ============================================================================
# CLASSE PRINCIPALE: LOTTERY
# ============================================================================

class HashrateLottery:
    """
    Système de loterie basé sur les shares des mineurs.
    
    Algorithme:
    1. Récupère toutes les shares de la période (ex: 7 derniers jours)
    2. Compte les shares par adresse de mineur
    3. Sélection pondérée: plus de shares = plus de chances
    4. Désigne un gagnant unique
    5. Sauvegarde le résultat pour l'affichage
    """
    
    def __init__(self, db_path, period_days=7, prize_xmr=0.05, output_dir=None):
        self.db_path = db_path
        self.period_days = period_days
        self.prize_xmr = prize_xmr
        # Output directory for results (defaults to db_path parent)
        if output_dir:
            self.results_file = os.path.join(output_dir, 'lottery_results.json')
        else:
            self.results_file = os.path.join(os.path.dirname(db_path), 'lottery_results.json')
        
    def get_shares_in_period(self):
        """
        Récupère toutes les shares des N derniers jours.
        
        Returns:
            dict: {adresse_wallet: nombre_de_shares}
        """
        participants = defaultdict(int)
        cutoff_timestamp = int((datetime.now() - timedelta(days=self.period_days)).timestamp())
        total_shares = 0
        
        try:
            env = lmdb.open(self.db_path, readonly=True, max_dbs=1, create=False)
            shares_db = env.open_db('shares'.encode(), dupsort=True)
            
            with env.begin(db=shares_db) as txn:
                with txn.cursor() as curs:
                    # Parcourt toutes les shares
                    for key, value in curs:
                        share = share_t.from_buffer_copy(value)
                        
                        # Vérifie si la share est dans la période
                        if share.timestamp >= cutoff_timestamp:
                            # Récupère l'adresse propre
                            wallet_address = share.address.decode('utf-8').rstrip('\0')
                            
                            # Incrémente le compteur de shares pour ce mineur
                            participants[wallet_address] += 1
                            total_shares += 1
                            
            env.close()
            
        except Exception as e:
            print(f"❌ Erreur lors de la lecture de la base: {e}")
            return {}, 0
            
        return participants, total_shares
    
    def weighted_random_selection(self, participants, total_tickets):
        """
        Algorithme de sélection pondérée (Weighted Random).
        
        Plus un mineur a de shares, plus il a de chances de gagner,
        mais même un petit mineur a une chance non nulle.
        
        Args:
            participants: dict {wallet: share_count}
            total_tickets: int nombre total de shares/tickets
            
        Returns:
            str: adresse du gagnant
        """
        if total_tickets == 0 or not participants:
            return None
            
        # Choisit un numéro de ticket gagnant au hasard
        winning_ticket = random.randint(1, total_tickets)
        
        current_count = 0
        winner_wallet = None
        
        for wallet, shares in participants.items():
            current_count += shares
            # Si le compteur atteint ou dépasse le ticket gagnant, c'est ce wallet qui gagne
            if current_count >= winning_ticket:
                winner_wallet = wallet
                break
                
        return winner_wallet
    
    def calculate_probability(self, participant_shares, total_shares):
        """Calcule la probabilité de gain pour un participant"""
        if total_shares == 0:
            return 0.0
        return (participant_shares / total_shares) * 100
    
    def run_lottery(self, dry_run=False):
        """
        Exécute le tirage de la loterie.
        
        Args:
            dry_run: Si True, ne sauvegarde pas les résultats
            
        Returns:
            dict: Résultats du tirage
        """
        print("=" * 60)
        print("🎰 LOTERIE HEBDOMADAIRE EUROXMR")
        print("=" * 60)
        print(f"📅 Date du tirage: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"📆 Période analysée: {self.period_days} derniers jours")
        print(f"💰 Cagnotte: {self.prize_xmr} XMR")
        print("-" * 60)
        
        # Récupère les participants
        print("🔍 Récupération des shares...")
        participants, total_tickets = self.get_shares_in_period()
        
        if not participants:
            print("❌ Aucun participant trouvé pour cette période!")
            return None
            
        print(f"🎟️  Total Tickets (Shares): {total_tickets:,}")
        print(f"👥 Nombre de participants: {len(participants)}")
        print("-" * 60)
        
        # Affiche le top 10 des mineurs
        print("\n📊 TOP 10 des mineurs (par nombre de shares):")
        sorted_participants = sorted(participants.items(), key=lambda x: x[1], reverse=True)
        for i, (wallet, shares) in enumerate(sorted_participants[:10], 1):
            probability = self.calculate_probability(shares, total_tickets)
            print(f"  {i:2}. {format_address(wallet)} - {shares:,} shares ({probability:.2f}%)")
        
        print("\n" + "=" * 60)
        print("🎲 TIRAGE EN COURS...")
        print("=" * 60)
        
        # Sélection du gagnant
        winner_wallet = self.weighted_random_selection(participants, total_tickets)
        
        if not winner_wallet:
            print("❌ Erreur lors de la sélection du gagnant!")
            return None
            
        winner_shares = participants[winner_wallet]
        winner_probability = self.calculate_probability(winner_shares, total_tickets)
        
        print("\n" + "🏆" * 20)
        print(f"\n🎉 LE GAGNANT EST:")
        print(f"\n    📬 Adresse: {winner_wallet}")
        print(f"    📊 Shares: {winner_shares:,}")
        print(f"    🎯 Probabilité: {winner_probability:.4f}%")
        print(f"    💰 Gain: {self.prize_xmr} XMR")
        print("\n" + "🏆" * 20)
        
        # Prépare les résultats
        result = {
            'draw_date': datetime.now().isoformat(),
            'period_days': self.period_days,
            'total_tickets': total_tickets,
            'total_participants': len(participants),
            'winner': {
                'address': winner_wallet,
                'address_short': format_address(winner_wallet),
                'shares': winner_shares,
                'probability': winner_probability
            },
            'prize_xmr': self.prize_xmr,
            'txid': None,  # À remplir manuellement après le paiement
            'top_participants': [
                {
                    'address': wallet,
                    'address_short': format_address(wallet),
                    'shares': shares,
                    'probability': self.calculate_probability(shares, total_tickets)
                }
                for wallet, shares in sorted_participants[:20]
            ]
        }
        
        # Sauvegarde les résultats
        if not dry_run:
            self.save_results(result)
            print(f"\n💾 Résultats sauvegardés dans: {self.results_file}")
        else:
            print("\n⚠️  Mode DRY-RUN: résultats non sauvegardés")
            
        # Instructions pour le paiement
        print("\n" + "-" * 60)
        print("📝 PROCHAINES ÉTAPES:")
        print(f"  1. Envoyer {self.prize_xmr} XMR à: {winner_wallet}")
        print("  2. Noter le TxID de la transaction")
        print("  3. Mettre à jour lottery_results.json avec le TxID")
        print("  4. Annoncer le gagnant sur Discord/Twitter")
        print("-" * 60)
        
        return result
    
    def save_results(self, result):
        """Sauvegarde les résultats dans un fichier JSON"""
        history = []
        
        # Charge l'historique existant
        if os.path.exists(self.results_file):
            try:
                with open(self.results_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    history = data.get('history', [])
            except:
                pass
                
        # Ajoute le nouveau résultat
        history.insert(0, result)
        
        # Garde les 52 derniers tirages (1 an)
        history = history[:52]
        
        # Sauvegarde
        data = {
            'last_draw': result,
            'next_draw': self.get_next_draw_date(),
            'history': history
        }
        
        with open(self.results_file, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
            
    def get_next_draw_date(self):
        """Calcule la date du prochain tirage (dimanche 20h)"""
        now = datetime.now()
        days_until_sunday = (6 - now.weekday()) % 7
        if days_until_sunday == 0 and now.hour >= 20:
            days_until_sunday = 7
        next_sunday = now + timedelta(days=days_until_sunday)
        next_draw = next_sunday.replace(hour=20, minute=0, second=0, microsecond=0)
        return next_draw.isoformat()
    
    def show_stats(self):
        """Affiche les statistiques actuelles (sans tirage)"""
        print("=" * 60)
        print("📊 STATISTIQUES DE LA LOTERIE EUROXMR")
        print("=" * 60)
        
        participants, total_tickets = self.get_shares_in_period()
        
        if not participants:
            print("❌ Aucun participant trouvé pour cette période!")
            return
            
        print(f"📆 Période: {self.period_days} derniers jours")
        print(f"🎟️  Total Tickets: {total_tickets:,}")
        print(f"👥 Participants: {len(participants)}")
        print(f"💰 Cagnotte: {self.prize_xmr} XMR")
        print(f"📅 Prochain tirage: {self.get_next_draw_date()}")
        print("-" * 60)
        
        print("\n📊 CLASSEMENT DES PARTICIPANTS:")
        sorted_participants = sorted(participants.items(), key=lambda x: x[1], reverse=True)
        for i, (wallet, shares) in enumerate(sorted_participants[:20], 1):
            probability = self.calculate_probability(shares, total_tickets)
            print(f"  {i:2}. {format_address(wallet)} - {shares:,} shares ({probability:.2f}%)")


# ============================================================================
# POINT D'ENTRÉE
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description='🎰 EuroXMR Hashrate Lottery Bot',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Exemples:
  python lottery_bot.py /path/to/pooldb --run
  python lottery_bot.py /path/to/pooldb --run --dry-run
  python lottery_bot.py /path/to/pooldb --stats
  python lottery_bot.py /path/to/pooldb --run --period 7 --prize 0.1
        '''
    )
    
    parser.add_argument('database', help='Chemin vers la base de données LMDB de la pool')
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--run', action='store_true', 
                      help='Lance le tirage de la loterie')
    group.add_argument('--stats', action='store_true',
                      help='Affiche les statistiques sans faire de tirage')
    
    parser.add_argument('--dry-run', action='store_true',
                       help='Simule le tirage sans sauvegarder les résultats')
    parser.add_argument('--period', type=int, default=7,
                       help='Nombre de jours à considérer (défaut: 7)')
    parser.add_argument('--prize', type=float, default=0.05,
                       help='Montant du prix en XMR (défaut: 0.05)')
    parser.add_argument('--output', type=str, default=None,
                       help='Répertoire de sortie pour les résultats')
    
    args = parser.parse_args()
    
    # Crée l'instance de la loterie
    lottery = HashrateLottery(
        db_path=args.database,
        period_days=args.period,
        prize_xmr=args.prize,
        output_dir=args.output
    )
    
    # Exécute l'action demandée
    if args.run:
        lottery.run_lottery(dry_run=args.dry_run)
    elif args.stats:
        lottery.show_stats()


if __name__ == '__main__':
    main()
