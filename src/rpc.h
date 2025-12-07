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

#ifndef RPC_H
#define RPC_H

#include <stdint.h>
#include <event2/event.h>

/*
 * RPC Module
 * ==========
 * Handles communication with monerod and monero-wallet-rpc.
 *
 * This module provides:
 * - Block template fetching
 * - Block submission
 * - Block header queries
 * - Wallet operations (transfers, balance queries)
 *
 * All RPC calls are asynchronous using libevent.
 */

#define RPC_BODY_MAX 65536
#define RPC_PATH "/json_rpc"

/* RPC callback function types */
typedef struct rpc_callback_t rpc_callback_t;
typedef void (*rpc_callback_fun)(const char *data, rpc_callback_t *callback);
typedef void (*rpc_datafree_fun)(void *data);

/* RPC callback structure */
struct rpc_callback_t {
    rpc_callback_fun cf;    /* Callback function */
    void *data;             /* User data */
    rpc_datafree_fun df;    /* Data cleanup function */
};

/* RPC configuration */
typedef struct rpc_config_t {
    char daemon_host[256];
    uint16_t daemon_port;
    char wallet_host[256];
    uint16_t wallet_port;
    uint32_t timeout;
} rpc_config_t;

/*
 * Initialize the RPC module.
 *
 * @param base    Event base for async operations
 * @param config  RPC configuration
 * @return        0 on success, error code otherwise
 */
int rpc_init(struct event_base *base, const rpc_config_t *config);

/*
 * Create a new RPC callback structure.
 *
 * @param cf    Callback function
 * @param data  User data (ownership transferred)
 * @param df    Data cleanup function (can be NULL for free())
 * @return      Allocated callback structure
 */
rpc_callback_t *rpc_callback_new(rpc_callback_fun cf, void *data, 
                                  rpc_datafree_fun df);

/*
 * Free an RPC callback structure.
 *
 * @param callback  Callback to free
 */
void rpc_callback_free(rpc_callback_t *callback);

/*
 * Make an RPC request to the daemon.
 *
 * @param base      Event base
 * @param body      JSON-RPC request body
 * @param callback  Callback for response handling
 */
void rpc_request(struct event_base *base, const char *body,
                 rpc_callback_t *callback);

/*
 * Make an RPC request to the wallet.
 *
 * @param base      Event base
 * @param body      JSON-RPC request body
 * @param callback  Callback for response handling
 */
void rpc_wallet_request(struct event_base *base, const char *body,
                        rpc_callback_t *callback);

/*
 * Build an RPC request body.
 *
 * Format string uses 's' for strings and 'd' for uint64 integers.
 * Parameters are passed in pairs: key, value, key, value, ...
 *
 * Example:
 *   rpc_get_request_body(body, "get_block_header_by_height", "sd", 
 *                        "height", (uint64_t)1000);
 *
 * @param body    Buffer for the request (at least RPC_BODY_MAX bytes)
 * @param method  RPC method name
 * @param fmt     Format string for parameters (or NULL if no params)
 * @param ...     Parameters as specified by format string
 */
void rpc_get_request_body(char *body, const char *method, char *fmt, ...);

/*
 * Request a new block template from the daemon.
 *
 * @param callback  Callback for template processing
 */
void rpc_fetch_block_template(rpc_callback_t *callback);

/*
 * Request the last block header from the daemon.
 *
 * @param callback  Callback for header processing
 */
void rpc_fetch_last_block_header(rpc_callback_t *callback);

/*
 * Request a range of block headers from the daemon.
 *
 * @param start_height  Starting block height
 * @param end_height    Ending block height
 * @param callback      Callback for headers processing
 */
void rpc_fetch_block_headers_range(uint64_t start_height, uint64_t end_height,
                                    rpc_callback_t *callback);

/*
 * Submit a mined block to the network.
 *
 * @param block_hex  Block blob in hexadecimal
 * @param callback   Callback for result processing
 */
void rpc_submit_block(const char *block_hex, rpc_callback_t *callback);

/*
 * Query the wallet for the view key.
 *
 * @param callback  Callback for key processing
 */
void rpc_fetch_view_key(rpc_callback_t *callback);

/*
 * Send a transfer from the wallet.
 *
 * @param destinations  JSON array of destinations
 * @param callback      Callback for transfer result
 */
void rpc_transfer(const char *destinations, rpc_callback_t *callback);

#endif /* RPC_H */

