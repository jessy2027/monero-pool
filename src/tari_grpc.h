/*
 * Copyright (c) 2018, The Monero Project
 * Copyright (c) 2026, Euro XMR Pool - Tari Merge Mining Extension
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.
 */

#ifndef TARI_GRPC_H
#define TARI_GRPC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <event2/event.h>

/*
 * Tari gRPC Client Module
 * =======================
 * Handles communication with tari_base_node and tari_console_wallet
 * for merged mining support.
 *
 * Features:
 * - Fetch block templates via gRPC
 * - Submit merge-mined blocks
 * - Coordinate coinbase transactions with wallet
 * - Automatic template polling synchronized with Monero
 */

/* Address and buffer constants */
#define TARI_ADDRESS_MAX 128
#define TARI_HASH_SIZE 32
#define TARI_HEADER_MAX 1024
#define TARI_BODY_MAX 65536

/* gRPC connection status */
typedef enum {
    TARI_STATUS_DISCONNECTED = 0,
    TARI_STATUS_CONNECTING,
    TARI_STATUS_CONNECTED,
    TARI_STATUS_ERROR
} tari_status_t;

/* Tari block template structure */
typedef struct tari_block_template_t {
    unsigned char *header;
    size_t header_size;
    unsigned char *body;
    size_t body_size;
    uint64_t height;
    uint64_t difficulty;
    unsigned char merge_mining_hash[TARI_HASH_SIZE];
    uint64_t total_fees;
    uint64_t reward;
    time_t timestamp;
} tari_block_template_t;

/* Tari gRPC configuration */
typedef struct tari_config_t {
    char base_node_host[256];
    uint16_t base_node_grpc_port;
    char wallet_host[256];
    uint16_t wallet_grpc_port;
    bool enabled;
    uint32_t poll_interval_ms;
    uint32_t timeout_ms;
} tari_config_t;

/* Callback types */
typedef void (*tari_template_callback_t)(const tari_block_template_t *tmpl, void *data);
typedef void (*tari_submit_callback_t)(bool success, const char *error, void *data);
typedef void (*tari_connect_callback_t)(tari_status_t status, void *data);

/*
 * Initialize the Tari gRPC module.
 *
 * @param base    Event base for async operations
 * @param config  Tari configuration
 * @return        0 on success, -1 on error
 */
int tari_init(struct event_base *base, const tari_config_t *config);

/*
 * Cleanup and shutdown the Tari gRPC module.
 */
void tari_cleanup(void);

/*
 * Check if Tari merge mining is enabled.
 *
 * @return  true if enabled and connected
 */
bool tari_is_enabled(void);

/*
 * Get current connection status.
 *
 * @return  Current status
 */
tari_status_t tari_get_status(void);

/*
 * Fetch a new block template from the Tari base node.
 * The template will be returned via the callback.
 *
 * @param callback  Function to call with the template
 * @param data      User data passed to callback
 * @return          0 on success, -1 on error
 */
int tari_fetch_block_template(tari_template_callback_t callback, void *data);

/*
 * Get the current cached Tari block template.
 * This is the most recent template fetched from the node.
 * Thread-safe.
 *
 * @return  Pointer to current template, or NULL if none
 */
const tari_block_template_t *tari_get_current_template(void);

/*
 * Request a coinbase transaction from the Tari wallet.
 *
 * @param height       Block height
 * @param reward       Block reward in atomic units
 * @param fees         Total transaction fees
 * @param coinbase     Output: allocated coinbase data
 * @param coinbase_len Output: length of coinbase data
 * @return             0 on success, -1 on error
 */
int tari_get_coinbase(uint64_t height, uint64_t reward, uint64_t fees,
                       unsigned char **coinbase, size_t *coinbase_len);

/*
 * Submit a solved block to the Tari network.
 *
 * @param header        Tari block header
 * @param header_size   Size of header
 * @param body          Tari block body
 * @param body_size     Size of body
 * @param pow_data      Serialized MoneroPowData
 * @param pow_data_size Size of pow_data
 * @param callback      Function to call with result
 * @param data          User data passed to callback
 * @return              0 on success, -1 on error
 */
int tari_submit_block(const unsigned char *header, size_t header_size,
                       const unsigned char *body, size_t body_size,
                       const unsigned char *pow_data, size_t pow_data_size,
                       tari_submit_callback_t callback, void *data);

/*
 * Validate a Tari address format.
 *
 * @param address  Address string to validate
 * @return         true if valid, false otherwise
 */
bool tari_validate_address(const char *address);

/*
 * Free a block template structure.
 *
 * @param tmpl  Template to free
 */
void tari_template_free(tari_block_template_t *tmpl);

/*
 * Copy a block template structure (deep copy).
 *
 * @param src  Source template
 * @return     Newly allocated copy, or NULL on error
 */
tari_block_template_t *tari_template_copy(const tari_block_template_t *src);

/*
 * Get Tari network difficulty.
 *
 * @return  Current network difficulty
 */
uint64_t tari_get_difficulty(void);

/*
 * Get Tari network height.
 *
 * @return  Current block height
 */
uint64_t tari_get_height(void);

#endif /* TARI_GRPC_H */
