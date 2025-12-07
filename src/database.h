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

#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include <lmdb.h>
#include <pthread.h>

/*
 * Database Module
 * ===============
 * Handles all LMDB database operations for the pool including:
 * - Share storage and retrieval
 * - Block storage and management
 * - Balance tracking
 * - Payment history
 * - Property storage
 *
 * Thread Safety:
 * All operations are thread-safe using read-write locks.
 * The database automatically resizes when needed.
 */

/* Database size constants */
#define DB_INIT_SIZE 0x140000000ULL  /* 5GB initial size */
#define DB_GROW_SIZE 0xA0000000ULL   /* 2.5GB growth increment */
#define DB_COUNT_MAX 10              /* Maximum number of databases */
#define ADDRESS_MAX 128              /* Maximum address length */

/* Block status flags */
typedef enum {
    BLOCK_LOCKED   = 0,
    BLOCK_UNLOCKED = 1,
    BLOCK_ORPHANED = 2
} block_status_t;

/* Share record structure */
typedef struct share_t {
    uint64_t height;
    uint64_t difficulty;
    char address[ADDRESS_MAX];
    time_t timestamp;
} share_t;

/* Block record structure */
typedef struct block_t {
    uint64_t height;
    char hash[64] __attribute__((nonstring));
    char prev_hash[64] __attribute__((nonstring));
    uint64_t difficulty;
    uint32_t status;
    uint64_t reward;
    time_t timestamp;
} block_t;

/* Payment record structure */
typedef struct payment_t {
    uint64_t amount;
    time_t timestamp;
    char address[ADDRESS_MAX];
} payment_t;

/* Database context - opaque structure */
typedef struct db_context_t db_context_t;

/*
 * Initialize the database.
 *
 * @param data_dir  Path to the data directory
 * @return          0 on success, error code otherwise
 */
int db_init(const char *data_dir);

/*
 * Close the database and free resources.
 */
void db_close(void);

/*
 * Check and resize database if needed.
 *
 * @return  0 on success, error code otherwise
 */
int db_resize(void);

/*
 * Store a share in the database.
 *
 * @param height  Block height for the share
 * @param share   Pointer to share data
 * @return        0 on success, error code otherwise
 */
int db_store_share(uint64_t height, const share_t *share);

/*
 * Store a block in the database.
 *
 * @param height  Block height
 * @param block   Pointer to block data
 * @return        0 on success, error code otherwise
 */
int db_store_block(uint64_t height, const block_t *block);

/*
 * Add amount to an address balance.
 *
 * @param address  Wallet address
 * @param amount   Amount to add (in atomic units)
 * @param parent   Parent transaction (can be NULL)
 * @return         0 on success, error code otherwise
 */
int db_balance_add(const char *address, uint64_t amount, MDB_txn *parent);

/*
 * Get balance for an address.
 *
 * @param address  Wallet address
 * @return         Balance in atomic units
 */
uint64_t db_balance_get(const char *address);

/*
 * Get a property value from the database.
 *
 * @param name      Property name
 * @param value     Buffer to store the value
 * @param max_size  Maximum size of the buffer
 * @return          0 on success, MDB_NOTFOUND if not exists, error code otherwise
 */
int db_property_get(const char *name, void *value, size_t max_size);

/*
 * Set a property value in the database.
 *
 * @param name   Property name
 * @param value  Pointer to the value
 * @param size   Size of the value
 * @return       0 on success, error code otherwise
 */
int db_property_set(const char *name, const void *value, size_t size);

/*
 * Begin a database transaction.
 *
 * @param parent  Parent transaction (can be NULL)
 * @param flags   Transaction flags
 * @param txn     Pointer to store the transaction handle
 * @return        0 on success, error code otherwise
 */
int db_txn_begin(MDB_txn *parent, unsigned int flags, MDB_txn **txn);

/*
 * Get the database environment handle.
 * Use with caution - prefer higher-level functions.
 *
 * @return  The MDB environment handle
 */
MDB_env *db_get_env(void);

/*
 * Get database handles for direct access.
 * Use with caution - prefer higher-level functions.
 */
MDB_dbi db_get_shares_dbi(void);
MDB_dbi db_get_blocks_dbi(void);
MDB_dbi db_get_balance_dbi(void);
MDB_dbi db_get_payments_dbi(void);
MDB_dbi db_get_properties_dbi(void);

/*
 * Get synchronization primitives for thread-safe access.
 */
pthread_rwlock_t *db_get_tx_lock(void);

#endif /* DATABASE_H */

