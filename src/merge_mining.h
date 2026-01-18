/*
 * Copyright (c) 2018, The Monero Project
 * Copyright (c) 2026, Euro XMR Pool - Tari Merge Mining Extension
 *
 * All rights reserved.
 */

#ifndef MERGE_MINING_H
#define MERGE_MINING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Merge Mining Module
 * ===================
 * Implements Tari RFC-0132: Merge Mining Monero
 *
 * This module handles:
 * - Injection of merge mining tags into Monero coinbase tx_extra
 * - Calculation of Tari block hashes
 * - Serialization of MoneroPowData for Tari verification
 */

/* Monero tx_extra tag for merge mining (RFC-0132) */
#define TX_EXTRA_MERGE_MINING_TAG 0x03

/* Hash sizes */
#define MM_HASH_SIZE 32
#define MM_MERKLE_PROOF_MAX 32  /* Max depth of merkle proof */

/* Merge mining tag structure (injected into Monero coinbase tx_extra) */
typedef struct mm_tag_t {
    uint8_t tag;                      /* Always TX_EXTRA_MERGE_MINING_TAG (0x03) */
    uint8_t depth;                    /* Merkle tree depth (0 for single aux chain) */
    unsigned char hash[MM_HASH_SIZE]; /* 32-byte merge mining hash */
} __attribute__((packed)) mm_tag_t;

/*
 * MoneroPowData structure for Tari block verification.
 * This is serialized using Monero consensus encoding.
 */
typedef struct monero_pow_data_t {
    /* Monero block header fields (76-80 bytes depending on version) */
    unsigned char header[80];
    size_t header_size;
    
    /* RandomX seed hash (32 bytes) */
    unsigned char randomx_key[MM_HASH_SIZE];
    
    /* Transaction count in block */
    uint64_t transaction_count;
    
    /* Transaction merkle root */
    unsigned char merkle_root[MM_HASH_SIZE];
    
    /* Coinbase merkle proof hashes */
    unsigned char (*coinbase_merkle_proof)[MM_HASH_SIZE];
    size_t coinbase_merkle_proof_count;
    
    /* Complete coinbase transaction */
    unsigned char *coinbase_tx;
    size_t coinbase_tx_size;
} monero_pow_data_t;

/*
 * Calculate the merge mining hash for a Tari block template.
 * This is the hash that gets injected into the Monero coinbase tx_extra.
 *
 * @param tari_header   Tari block header bytes
 * @param header_size   Size of header
 * @param output        32-byte output buffer for hash
 * @return              0 on success, -1 on error
 */
int mm_calculate_tari_hash(const unsigned char *tari_header, size_t header_size,
                            unsigned char *output);

/*
 * Inject merge mining tag into a Monero block blob.
 * Modifies the coinbase tx_extra field to include the merge mining hash.
 *
 * @param block_blob    Monero block blob buffer (modified in-place)
 * @param blob_size     Pointer to current blob size (updated on return)
 * @param max_size      Maximum buffer capacity
 * @param mm_hash       32-byte merge mining hash to inject
 * @return              0 on success, -1 on error
 */
int mm_inject_merge_mining_tag(unsigned char *block_blob, size_t *blob_size,
                                size_t max_size, const unsigned char *mm_hash);

/*
 * Check if a block blob already contains a merge mining tag.
 *
 * @param block_blob    Monero block blob
 * @param blob_size     Size of block blob
 * @return              true if tag exists, false otherwise
 */
bool mm_has_merge_mining_tag(const unsigned char *block_blob, size_t blob_size);

/*
 * Remove merge mining tag from a block blob if present.
 *
 * @param block_blob    Monero block blob buffer (modified in-place)
 * @param blob_size     Pointer to current blob size (updated on return)
 * @return              0 on success (or no tag present), -1 on error
 */
int mm_remove_merge_mining_tag(unsigned char *block_blob, size_t *blob_size);

/*
 * Serialize MoneroPowData for Tari block submission.
 * Uses Monero consensus encoding (VarInt for integers).
 *
 * @param pow_data      Filled MoneroPowData structure
 * @param output        Output buffer (must be pre-allocated)
 * @param output_size   Pointer to buffer size (in: max, out: actual)
 * @return              0 on success, -1 on error
 */
int mm_serialize_pow_data(const monero_pow_data_t *pow_data,
                           unsigned char *output, size_t *output_size);

/*
 * Extract MoneroPowData from a solved Monero block.
 *
 * @param block_blob    Solved Monero block blob
 * @param blob_size     Size of block blob
 * @param seed_hash     RandomX seed hash used for this block
 * @param pow_data      Output structure (coinbase fields allocated)
 * @return              0 on success, -1 on error
 */
int mm_extract_pow_data(const unsigned char *block_blob, size_t blob_size,
                         const unsigned char *seed_hash,
                         monero_pow_data_t *pow_data);

/*
 * Free allocated fields in a MoneroPowData structure.
 *
 * @param pow_data      Structure to cleanup
 */
void mm_pow_data_free(monero_pow_data_t *pow_data);

/*
 * Calculate Monero merkle root from transaction hashes.
 *
 * @param hashes        Array of transaction hashes (coinbase first)
 * @param count         Number of hashes
 * @param root          32-byte output for merkle root
 * @return              0 on success, -1 on error
 */
int mm_calculate_merkle_root(const unsigned char (*hashes)[MM_HASH_SIZE],
                              size_t count, unsigned char *root);

/*
 * Calculate coinbase merkle proof for inclusion in MoneroPowData.
 *
 * @param hashes        Array of transaction hashes (coinbase first)
 * @param count         Number of hashes
 * @param proof         Output proof hashes (caller allocates)
 * @param proof_count   Output number of proof hashes
 * @return              0 on success, -1 on error
 */
int mm_calculate_coinbase_proof(const unsigned char (*hashes)[MM_HASH_SIZE],
                                 size_t count,
                                 unsigned char (*proof)[MM_HASH_SIZE],
                                 size_t *proof_count);

#endif /* MERGE_MINING_H */
