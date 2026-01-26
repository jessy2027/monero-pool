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

#ifndef STRATUM_H
#define STRATUM_H

#include <stdint.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <event2/bufferevent.h>

#include "bstack.h"
#include "database.h"

/*
 * Stratum Protocol Module
 * =======================
 * Implements the Stratum mining protocol for Monero.
 * Handles miner connections, job distribution, and share submission.
 *
 * Features:
 * - Standard Stratum protocol support
 * - Self-select mode for decentralized mining
 * - NiceHash compatibility
 * - Proxy/XNP support
 * - Dynamic difficulty retargeting
 */

/* Protocol constants */
#define MAX_LINE 8192
#define JOB_BODY_MAX 8192
#define ERROR_BODY_MAX 512
#define STATUS_BODY_MAX 512
#define CLIENT_JOBS_MAX 4
#define MAX_HOST 256
#define MAX_RIG_ID 32
#define MAX_BAD_SHARES 5

/* Stratum modes */
typedef enum {
    MODE_NORMAL = 0,
    MODE_SELF_SELECT = 1
} stratum_mode_t;

/* Hashrate statistics with time-decayed EMA */
typedef struct hr_stats_t {
    time_t last_calc;
    uint64_t diff_since;
    double avg[6];  /* 2m, 10m, 30m, 1h, 1d, 1w */
} hr_stats_t;

/* Block template structure */
typedef struct block_template_t {
    char *hashing_blob;
    size_t hashing_blob_size;
    char *block_blob;
    size_t block_blob_size;
    uint64_t difficulty;
    uint64_t height;
    char prev_hash[65];
    uint32_t reserved_offset;
    char seed_hash[65];
    char next_seed_hash[65];
    uint64_t tx_count;
} block_template_t;

/* Job structure */
typedef struct job_t {
    uuid_t id;
    char *blob;
    block_template_t *block_template;
    uint32_t extra_nonce;
    uint64_t target;
    __uint128_t *submissions;
    size_t submissions_count;
    block_template_t *miner_template;
} job_t;

/* Client structure */
typedef struct client_t {
    int fd;
    char host[MAX_HOST];
    uint16_t port;
    int json_id;
    struct bufferevent *bev;
    char address[ADDRESS_MAX];
    char tari_address[128];
    char worker_id[64];
    char client_id[32];
    char rig_id[MAX_RIG_ID];
    char agent[256];
    bstack_t *active_jobs;
    uint64_t hashes;
    hr_stats_t hr_stats;
    time_t connected_since;
    bool is_xnp;
    bool is_nicehash;
    uint32_t mode;
    uint8_t bad_shares;
    bool downstream;
    uint32_t downstream_accounts;
    uint64_t req_diff;
    /* Hash handle for uthash */
    void *hh_placeholder[3];
} client_t;

/* Account structure */
typedef struct account_t {
    char address[ADDRESS_MAX];
    size_t worker_count;
    time_t connected_since;
    uint64_t hashes;
    hr_stats_t hr_stats;
    /* Hash handle for uthash */
    void *hh_placeholder[3];
} account_t;

/*
 * Update hashrate statistics using exponential moving average.
 *
 * @param stats  Pointer to hashrate stats structure
 */
void hr_update(hr_stats_t *stats);

/*
 * Generate error response body.
 *
 * @param body     Buffer to write to
 * @param json_id  JSON-RPC request ID
 * @param error    Error message
 */
void stratum_get_error_body(char *body, int json_id, const char *error);

/*
 * Generate status response body.
 *
 * @param body     Buffer to write to
 * @param json_id  JSON-RPC request ID
 * @param status   Status message
 */
void stratum_get_status_body(char *body, int json_id, const char *status);

/*
 * Generate job body for normal mode.
 *
 * @param body      Buffer to write to
 * @param client    Client to send job to
 * @param response  Whether this is a response to a request
 */
void stratum_get_job_body(char *body, const client_t *client, bool response);

/*
 * Generate job body for self-select mode.
 *
 * @param body      Buffer to write to
 * @param client    Client to send job to
 * @param response  Whether this is a response to a request
 */
void stratum_get_job_body_ss(char *body, const client_t *client, bool response);

/*
 * Generate job body for proxy mode.
 *
 * @param body       Buffer to write to
 * @param client     Client to send job to
 * @param block_hex  Block template in hex
 * @param response   Whether this is a response to a request
 */
void stratum_get_proxy_job_body(char *body, const client_t *client,
                                 const char *block_hex, bool response);

/*
 * Convert target difficulty to hex string.
 *
 * @param target      Target difficulty value
 * @param target_hex  Buffer to write hex string (at least 17 bytes)
 */
void target_to_hex(uint64_t target, char *target_hex);

/*
 * Calculate appropriate target difficulty for a client.
 *
 * @param client  Client to calculate target for
 * @param job     Current job
 * @return        Calculated target difficulty
 */
uint64_t client_target(const client_t *client, const job_t *job);

/*
 * Check if retargeting is required.
 *
 * @param client  Client to check
 * @param job     Current job
 * @return        true if retarget needed
 */
bool retarget_required(const client_t *client, const job_t *job);

/*
 * Apply new target to job.
 *
 * @param client  Client to retarget
 * @param job     Job to update
 */
void retarget(client_t *client, job_t *job);

/*
 * Recycle job resources.
 *
 * @param item  Pointer to job structure
 */
void job_recycle(void *item);

/*
 * Recycle block template resources.
 *
 * @param item  Pointer to block template structure
 */
void template_recycle(void *item);

#endif /* STRATUM_H */

