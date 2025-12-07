/*
Copyright (c) 2018, The Monero Project

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef PAYOUT_H
#define PAYOUT_H

#include <stdint.h>
#include <stdbool.h>
#include <lmdb.h>

#include "database.h"

/*
 * Payout Module
 * =============
 * Implements PPLNS (Pay Per Last N Shares) reward distribution.
 *
 * How PPLNS Works:
 * ----------------
 * When a block is found, rewards are distributed proportionally to miners
 * based on their shares submitted during the round. The "N" in PPLNS refers
 * to the window of shares considered for payout.
 *
 * Calculation:
 *   miner_reward = (miner_shares / total_shares) * block_reward
 *
 * This system rewards loyal miners and protects against pool-hopping attacks.
 */

/* Payout configuration */
typedef struct payout_config_t {
    double pool_fee;           /* Pool fee as a fraction (e.g., 0.01 = 1%) */
    double share_mul;          /* Share multiplier for PPLNS window */
    double payment_threshold;  /* Minimum balance for payout (in XMR) */
    char pool_fee_wallet[128]; /* Wallet address for pool fees */
    bool disable_payouts;      /* If true, payouts are disabled */
    bool is_upstream;          /* If true, this is an upstream pool */
} payout_config_t;

/*
 * Initialize the payout module.
 *
 * @param config  Payout configuration
 * @return        0 on success, error code otherwise
 */
int payout_init(const payout_config_t *config);

/*
 * Process a mined block and distribute rewards.
 *
 * Uses PPLNS algorithm to calculate each miner's share of the block reward.
 * The pool fee is deducted and sent to the fee wallet if configured.
 *
 * @param block   Block that was mined
 * @param parent  Parent database transaction (can be NULL)
 * @return        0 on success, error code otherwise
 */
int payout_block(const block_t *block, MDB_txn *parent);

/*
 * Process pending blocks (check for confirmation/orphan status).
 *
 * For each locked block:
 * - If confirmed: unlock and process payout
 * - If orphaned: mark as orphaned
 *
 * @param blocks  Array of block headers from chain
 * @param count   Number of blocks in array
 * @return        0 on success, error code otherwise
 */
int payout_process_blocks(const block_t *blocks, size_t count);

/*
 * Send pending payments to miners.
 *
 * Iterates through all balances and sends payments to miners
 * who have reached the payment threshold.
 *
 * @return  0 on success, error code otherwise
 */
int payout_send_payments(void);

/*
 * Calculate reward for a single share.
 *
 * @param share_diff   Difficulty of the share
 * @param block_diff   Difficulty of the block
 * @param block_reward Total block reward
 * @param share_mul    PPLNS share multiplier
 * @return             Calculated reward in atomic units
 */
uint64_t payout_calculate_share_reward(uint64_t share_diff, 
                                        uint64_t block_diff,
                                        uint64_t block_reward,
                                        double share_mul);

/*
 * Apply pool fee to an amount.
 *
 * @param amount    Original amount
 * @param fee_rate  Fee rate as a fraction
 * @param fee_out   If not NULL, stores the fee amount
 * @return          Amount after fee deduction
 */
uint64_t payout_apply_fee(uint64_t amount, double fee_rate, uint64_t *fee_out);

#endif /* PAYOUT_H */

