/*
 * Copyright (c) 2018, The Monero Project
 * Copyright (c) 2026, Euro XMR Pool - Tari Merge Mining Extension
 *
 * All rights reserved.
 */

/*
 * Merge Mining Implementation
 * ============================
 * Implements Tari RFC-0132: Merge Mining Monero
 * https://rfc.tari.com/RFC-0132_MergeMiningMonero.html
 *
 * This module handles:
 * - Injection of merge mining tags into Monero coinbase tx_extra
 * - Calculation of Tari block hashes
 * - Serialization of MoneroPowData for Tari verification
 * - Merkle tree calculations for coinbase proofs
 */

#include <stdlib.h>
#include <string.h>
#include "merge_mining.h"
#include "util.h"
#include "log.h"

/* External Keccak function from Monero crypto library */
extern void keccak(const uint8_t *in, size_t inlen, uint8_t *md, int mdlen);

/*
 * Write varint to buffer (Monero consensus encoding)
 * Returns bytes written
 */
static int write_varint_internal(unsigned char *buf, uint64_t val)
{
    int i = 0;
    while (val >= 0x80) {
        buf[i++] = (val & 0x7F) | 0x80;
        val >>= 7;
    }
    buf[i++] = val & 0x7F;
    return i;
}

/*
 * Read varint from buffer
 * Returns bytes read, or -1 on error
 */
static int read_varint_internal(const unsigned char *buf, size_t max, uint64_t *val)
{
    *val = 0;
    int shift = 0;
    size_t i = 0;
    
    while (i < max && i < 10) {
        uint64_t b = buf[i];
        *val |= (b & 0x7F) << shift;
        if ((b & 0x80) == 0)
            return i + 1;
        shift += 7;
        i++;
    }
    return -1;
}

/*
 * Calculate the length a varint will take
 */
static int varint_length(uint64_t val)
{
    int len = 1;
    while (val >= 0x80) {
        len++;
        val >>= 7;
    }
    return len;
}

/*
 * Find the offset of tx_extra in a Monero block blob.
 * Returns offset to tx_extra length varint, or 0 on error.
 *
 * Block structure:
 * - Major version (varint)
 * - Minor version (varint)
 * - Timestamp (varint)
 * - Prev hash (32 bytes)
 * - Nonce (4 bytes)
 * - Miner TX:
 *   - Version (varint)
 *   - Unlock time (varint)
 *   - Input count (varint) = 1
 *   - Input type = 0xFF (txin_gen)
 *   - Height (varint)
 *   - Output count (varint) = 1
 *   - Amount (varint) = 0 for RCT
 *   - Output type tag
 *   - Output key (32 bytes)
 *   - View tag (1 byte, optional)
 *   - tx_extra length (varint) <-- We want this offset
 */
static size_t find_tx_extra_offset(const unsigned char *blob, size_t len)
{
    size_t pos = 0;
    uint64_t val;
    int vlen;
    
    if (len < 80)
        return 0;
    
    /* Major version */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Minor version */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Timestamp */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Prev hash (32 bytes) */
    pos += 32;
    if (pos > len) return 0;
    
    /* Nonce (4 bytes) */
    pos += 4;
    if (pos > len) return 0;
    
    /* Now in miner TX */
    /* TX version */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Unlock time */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Input count (should be 1) */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Input type (0xFF for txin_gen) */
    if (pos >= len || blob[pos] != 0xFF)
        return 0;
    pos++;
    
    /* Height */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    pos += vlen;
    
    /* Output count */
    vlen = read_varint_internal(blob + pos, len - pos, &val);
    if (vlen < 0) return 0;
    uint64_t output_count = val;
    pos += vlen;
    
    /* Skip outputs */
    for (uint64_t i = 0; i < output_count; i++) {
        /* Amount */
        vlen = read_varint_internal(blob + pos, len - pos, &val);
        if (vlen < 0) return 0;
        pos += vlen;
        
        /* Output type tag */
        if (pos >= len) return 0;
        uint8_t out_type = blob[pos++];
        
        /* Output key (32 bytes) */
        pos += 32;
        if (pos > len) return 0;
        
        /* View tag (1 byte if type is txout_to_tagged_key = 0x03) */
        if (out_type == 0x03) {
            pos++;
            if (pos > len) return 0;
        }
    }
    
    /* Now at tx_extra length */
    return pos;
}

int mm_calculate_tari_hash(const unsigned char *tari_header, size_t header_size,
                            unsigned char *output)
{
    if (!tari_header || !output || header_size == 0) {
        log_error("mm_calculate_tari_hash: invalid parameters");
        return -1;
    }
    
    /* Tari uses Keccak-256 for merge mining hash */
    keccak(tari_header, header_size, output, 32);
    
    return 0;
}

int mm_inject_merge_mining_tag(unsigned char *block_blob, size_t *blob_size,
                                size_t max_size, const unsigned char *mm_hash)
{
    if (!block_blob || !blob_size || !mm_hash) {
        log_error("mm_inject_merge_mining_tag: invalid parameters");
        return -1;
    }
    
    /* Find tx_extra location */
    size_t extra_offset = find_tx_extra_offset(block_blob, *blob_size);
    if (extra_offset == 0) {
        log_error("Failed to find tx_extra offset in block blob");
        return -1;
    }
    
    /* Read current tx_extra length */
    uint64_t current_extra_len;
    int old_vlen = read_varint_internal(block_blob + extra_offset, *blob_size - extra_offset,
                                &current_extra_len);
    if (old_vlen < 0) {
        log_error("Failed to read tx_extra length");
        return -1;
    }
    
    /*
     * Merge mining tag format:
     * [0x03]           - 1 byte tag
     * [depth=0]        - 1 byte (varint, always 0 for single aux chain)
     * [hash]           - 32 bytes
     * Total: 34 bytes
     */
    size_t mm_tag_size = 34;
    uint64_t new_extra_len = current_extra_len + mm_tag_size;
    int new_vlen = varint_length(new_extra_len);
    
    /* Size difference due to varint length change */
    int size_diff = new_vlen - old_vlen;
    size_t total_growth = mm_tag_size + size_diff;
    
    /* Check capacity */
    if (*blob_size + total_growth > max_size) {
        log_error("Block blob buffer too small for merge mining tag");
        return -1;
    }
    
    /* 
     * Shift data:
     * 1. Everything after tx_extra content needs to move by mm_tag_size
     * 2. If varint size changed, everything after the varint needs to shift
     */
    size_t data_after_extra = extra_offset + old_vlen + current_extra_len;
    size_t remaining = *blob_size - data_after_extra;
    
    /* Move data after tx_extra to make room for tag */
    memmove(block_blob + data_after_extra + total_growth,
            block_blob + data_after_extra,
            remaining);
    
    /* If varint size changed, shift the tx_extra content */
    if (size_diff != 0) {
        memmove(block_blob + extra_offset + new_vlen,
                block_blob + extra_offset + old_vlen,
                current_extra_len);
    }
    
    /* Write new tx_extra length */
    write_varint_internal(block_blob + extra_offset, new_extra_len);
    
    /* Write merge mining tag at end of tx_extra */
    unsigned char *tag_pos = block_blob + extra_offset + new_vlen + current_extra_len;
    *tag_pos++ = TX_EXTRA_MERGE_MINING_TAG;  /* 0x03 */
    *tag_pos++ = 0x00;                        /* Depth = 0 */
    memcpy(tag_pos, mm_hash, MM_HASH_SIZE);  /* 32-byte hash */
    
    *blob_size += total_growth;
    
    log_debug("Injected merge mining tag at offset %zu (extra len: %lu -> %lu)",
              extra_offset, current_extra_len, new_extra_len);
    
    return 0;
}

bool mm_has_merge_mining_tag(const unsigned char *block_blob, size_t blob_size)
{
    if (!block_blob || blob_size < 80)
        return false;
    
    size_t extra_offset = find_tx_extra_offset(block_blob, blob_size);
    if (extra_offset == 0)
        return false;
    
    uint64_t extra_len;
    int vlen = read_varint_internal(block_blob + extra_offset, blob_size - extra_offset, &extra_len);
    if (vlen < 0)
        return false;
    
    /* Search tx_extra for merge mining tag */
    const unsigned char *extra = block_blob + extra_offset + vlen;
    size_t pos = 0;
    
    while (pos < extra_len) {
        uint8_t tag = extra[pos];
        
        if (tag == TX_EXTRA_MERGE_MINING_TAG) {
            return true;
        }
        
        /* Skip other tags (simplified - production should handle all tag types) */
        switch (tag) {
            case 0x00:  /* Padding */
                pos++;
                break;
            case 0x01:  /* Tx pubkey */
                pos += 33;  /* 1 + 32 */
                break;
            case 0x02:  /* Extra nonce */
                if (pos + 1 < extra_len) {
                    uint8_t nonce_len = extra[pos + 1];
                    pos += 2 + nonce_len;
                } else {
                    return false;
                }
                break;
            case 0x03:  /* Merge mining (shouldn't reach here, but handle it) */
                return true;
            case 0x04:  /* Additional pubkeys */
                if (pos + 1 < extra_len) {
                    uint8_t count = extra[pos + 1];
                    pos += 2 + (count * 32);
                } else {
                    return false;
                }
                break;
            default:
                /* Unknown tag - can't continue safely */
                return false;
        }
    }
    
    return false;
}

int mm_remove_merge_mining_tag(unsigned char *block_blob, size_t *blob_size)
{
    if (!block_blob || !blob_size)
        return -1;
    
    if (!mm_has_merge_mining_tag(block_blob, *blob_size))
        return 0;  /* Nothing to remove */
    
    /* Find and remove the tag - implementation similar to injection but in reverse */
    /* This is left as a stub for now - needed if re-injecting with new hash */
    
    log_warn("mm_remove_merge_mining_tag: not fully implemented");
    return -1;
}

int mm_serialize_pow_data(const monero_pow_data_t *pow_data,
                           unsigned char *output, size_t *output_size)
{
    if (!pow_data || !output || !output_size) {
        log_error("mm_serialize_pow_data: invalid parameters");
        return -1;
    }
    
    size_t max_size = *output_size;
    unsigned char *p = output;
    
    /*
     * MoneroPowData serialization per RFC-0132:
     * - Header bytes (variable, typically 76-80 bytes)
     * - RandomX key (32 bytes)
     * - Transaction count (varint)
     * - Merkle root (32 bytes)
     * - Coinbase merkle proof count (varint)
     * - Coinbase merkle proof hashes (32 bytes each)
     * - Coinbase transaction length (varint)
     * - Coinbase transaction bytes
     */
    
    size_t total_needed = pow_data->header_size + 32 + 10 + 32 +
                          10 + (pow_data->coinbase_merkle_proof_count * 32) +
                          10 + pow_data->coinbase_tx_size;
    
    if (total_needed > max_size) {
        log_error("Output buffer too small for MoneroPowData");
        return -1;
    }
    
    /* Header */
    memcpy(p, pow_data->header, pow_data->header_size);
    p += pow_data->header_size;
    
    /* RandomX key */
    memcpy(p, pow_data->randomx_key, MM_HASH_SIZE);
    p += MM_HASH_SIZE;
    
    /* Transaction count */
    p += write_varint_internal(p, pow_data->transaction_count);
    
    /* Merkle root */
    memcpy(p, pow_data->merkle_root, MM_HASH_SIZE);
    p += MM_HASH_SIZE;
    
    /* Coinbase merkle proof */
    p += write_varint_internal(p, pow_data->coinbase_merkle_proof_count);
    for (size_t i = 0; i < pow_data->coinbase_merkle_proof_count; i++) {
        memcpy(p, pow_data->coinbase_merkle_proof[i], MM_HASH_SIZE);
        p += MM_HASH_SIZE;
    }
    
    /* Coinbase transaction */
    p += write_varint_internal(p, pow_data->coinbase_tx_size);
    memcpy(p, pow_data->coinbase_tx, pow_data->coinbase_tx_size);
    p += pow_data->coinbase_tx_size;
    
    *output_size = (size_t)(p - output);
    
    log_debug("Serialized MoneroPowData: %zu bytes", *output_size);
    
    return 0;
}

int mm_extract_pow_data(const unsigned char *block_blob, size_t blob_size,
                         const unsigned char *seed_hash,
                         monero_pow_data_t *pow_data)
{
    if (!block_blob || !seed_hash || !pow_data) {
        log_error("mm_extract_pow_data: invalid parameters");
        return -1;
    }
    
    memset(pow_data, 0, sizeof(monero_pow_data_t));
    
    /*
     * Extract Monero block header fields (first ~76-80 bytes depending on version)
     * This is a simplified extraction - production code should properly parse
     */
    
    size_t pos = 0;
    uint64_t val;
    int vlen;
    
    /* Count header bytes through nonce */
    size_t header_start = 0;
    
    /* Major version */
    vlen = read_varint_internal(block_blob + pos, blob_size - pos, &val);
    if (vlen < 0) return -1;
    pos += vlen;
    
    /* Minor version */
    vlen = read_varint_internal(block_blob + pos, blob_size - pos, &val);
    if (vlen < 0) return -1;
    pos += vlen;
    
    /* Timestamp */
    vlen = read_varint_internal(block_blob + pos, blob_size - pos, &val);
    if (vlen < 0) return -1;
    pos += vlen;
    
    /* Prev hash (32 bytes) */
    pos += 32;
    
    /* Nonce (4 bytes) */
    pos += 4;
    
    /* Header is from start to end of nonce */
    pow_data->header_size = pos;
    if (pow_data->header_size > 80) {
        log_error("Header too large: %zu bytes", pow_data->header_size);
        return -1;
    }
    memcpy(pow_data->header, block_blob + header_start, pow_data->header_size);
    
    /* RandomX key (seed hash) */
    memcpy(pow_data->randomx_key, seed_hash, MM_HASH_SIZE);
    
    /* For a proper implementation, we would need to:
     * 1. Parse the full block to get transaction count
     * 2. Calculate transaction hashes
     * 3. Build merkle tree and extract coinbase proof
     * 4. Extract raw coinbase transaction bytes
     *
     * This is a placeholder - the full implementation requires access
     * to the parsed block structure from Monero's cryptonote_basic
     */
    
    pow_data->transaction_count = 1;  /* At minimum, coinbase */
    
    /* For blocks with only coinbase, merkle root = coinbase hash */
    /* and proof is empty */
    pow_data->coinbase_merkle_proof_count = 0;
    pow_data->coinbase_merkle_proof = NULL;
    
    /* Extract coinbase transaction - find it after block header */
    size_t extra_offset = find_tx_extra_offset(block_blob, blob_size);
    if (extra_offset == 0) {
        log_error("Failed to find coinbase transaction");
        return -1;
    }
    
    /* Coinbase starts after nonce */
    size_t coinbase_start = pow_data->header_size;
    
    /* Find end of coinbase (need to parse through tx_extra and RCT) */
    uint64_t extra_len;
    vlen = read_varint_internal(block_blob + extra_offset, blob_size - extra_offset, &extra_len);
    if (vlen < 0) return -1;
    
    size_t coinbase_end = extra_offset + vlen + extra_len;
    
    /* Add RCT signature type (1 byte for coinbase = 0x00) */
    if (coinbase_end < blob_size) {
        coinbase_end++;
    }
    
    pow_data->coinbase_tx_size = coinbase_end - coinbase_start;
    pow_data->coinbase_tx = malloc(pow_data->coinbase_tx_size);
    if (!pow_data->coinbase_tx) {
        return -1;
    }
    memcpy(pow_data->coinbase_tx, block_blob + coinbase_start, pow_data->coinbase_tx_size);
    
    /* Calculate merkle root (for single tx, it's just the tx hash) */
    keccak(pow_data->coinbase_tx, pow_data->coinbase_tx_size, pow_data->merkle_root, 32);
    
    return 0;
}

void mm_pow_data_free(monero_pow_data_t *pow_data)
{
    if (!pow_data)
        return;
    
    if (pow_data->coinbase_merkle_proof)
        free(pow_data->coinbase_merkle_proof);
    
    if (pow_data->coinbase_tx)
        free(pow_data->coinbase_tx);
    
    memset(pow_data, 0, sizeof(monero_pow_data_t));
}

int mm_calculate_merkle_root(const unsigned char (*hashes)[MM_HASH_SIZE],
                              size_t count, unsigned char *root)
{
    if (!hashes || !root || count == 0)
        return -1;
    
    if (count == 1) {
        /* Single transaction - root is the hash itself */
        memcpy(root, hashes[0], MM_HASH_SIZE);
        return 0;
    }
    
    /*
     * Monero merkle tree calculation:
     * Uses tree_hash algorithm from crypto/tree-hash.c
     * This is a simplified implementation
     */
    
    size_t level_size = count;
    unsigned char (*level)[MM_HASH_SIZE] = malloc(count * MM_HASH_SIZE);
    if (!level)
        return -1;
    
    memcpy(level, hashes, count * MM_HASH_SIZE);
    
    while (level_size > 1) {
        size_t new_size = (level_size + 1) / 2;
        
        for (size_t i = 0; i < new_size; i++) {
            size_t left = i * 2;
            size_t right = left + 1;
            
            if (right >= level_size) {
                /* Odd number - just copy the last one */
                memcpy(level[i], level[left], MM_HASH_SIZE);
            } else {
                /* Hash pair together */
                unsigned char pair[64];
                memcpy(pair, level[left], MM_HASH_SIZE);
                memcpy(pair + MM_HASH_SIZE, level[right], MM_HASH_SIZE);
                keccak(pair, 64, level[i], MM_HASH_SIZE);
            }
        }
        
        level_size = new_size;
    }
    
    memcpy(root, level[0], MM_HASH_SIZE);
    free(level);
    
    return 0;
}

int mm_calculate_coinbase_proof(const unsigned char (*hashes)[MM_HASH_SIZE],
                                 size_t count,
                                 unsigned char (*proof)[MM_HASH_SIZE],
                                 size_t *proof_count)
{
    if (!hashes || !proof || !proof_count || count == 0)
        return -1;
    
    if (count == 1) {
        /* Single transaction - no proof needed */
        *proof_count = 0;
        return 0;
    }
    
    /*
     * Build merkle proof for coinbase (index 0)
     * Proof consists of sibling hashes at each level
     */
    
    size_t level_size = count;
    unsigned char (*level)[MM_HASH_SIZE] = malloc(count * MM_HASH_SIZE);
    if (!level)
        return -1;
    
    memcpy(level, hashes, count * MM_HASH_SIZE);
    
    size_t proof_idx = 0;
    size_t target_idx = 0;  /* Coinbase is always index 0 */
    
    while (level_size > 1) {
        size_t sibling_idx;
        
        if (target_idx % 2 == 0) {
            sibling_idx = target_idx + 1;
            if (sibling_idx >= level_size) {
                sibling_idx = target_idx;  /* Odd level - use self */
            }
        } else {
            sibling_idx = target_idx - 1;
        }
        
        memcpy(proof[proof_idx++], level[sibling_idx], MM_HASH_SIZE);
        
        /* Build next level */
        size_t new_size = (level_size + 1) / 2;
        for (size_t i = 0; i < new_size; i++) {
            size_t left = i * 2;
            size_t right = left + 1;
            
            if (right >= level_size) {
                memcpy(level[i], level[left], MM_HASH_SIZE);
            } else {
                unsigned char pair[64];
                memcpy(pair, level[left], MM_HASH_SIZE);
                memcpy(pair + MM_HASH_SIZE, level[right], MM_HASH_SIZE);
                keccak(pair, 64, level[i], MM_HASH_SIZE);
            }
        }
        
        target_idx /= 2;
        level_size = new_size;
    }
    
    *proof_count = proof_idx;
    free(level);
    
    return 0;
}
