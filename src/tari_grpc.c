/*
 * Copyright (c) 2018, The Monero Project
 * Copyright (c) 2026, Euro XMR Pool - Tari Merge Mining Extension
 *
 * All rights reserved.
 */

/*
 * Tari gRPC Client Implementation
 * ================================
 * Connects to tari_base_node via gRPC (HTTP/2) to:
 * 1. Fetch new block templates with Monero PoW algorithm
 * 2. Submit merge-mined blocks
 * 3. Coordinate with Tari wallet for coinbase transactions
 *
 * Uses libcurl for HTTP/2 transport with manual protobuf handling.
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <ctype.h>

#include "tari_grpc.h"
#include "util.h"
#include "log.h"

/* gRPC service paths */
#define GRPC_GET_NEW_BLOCK_TEMPLATE "/tari.rpc.BaseNode/GetNewBlockTemplate"
#define GRPC_GET_NEW_BLOCK          "/tari.rpc.BaseNode/GetNewBlock"
#define GRPC_SUBMIT_BLOCK           "/tari.rpc.BaseNode/SubmitBlock"
#define GRPC_GET_TIP_INFO           "/tari.rpc.BaseNode/GetTipInfo"

/* Wallet gRPC paths */
#define GRPC_GET_COINBASE           "/tari.rpc.Wallet/GetCoinbase"

/* gRPC message frame header size */
#define GRPC_HEADER_SIZE 5

/* Module state */
static tari_config_t g_config;
static struct event_base *g_base = NULL;
static struct event *poll_timer = NULL;
static tari_block_template_t *current_template = NULL;
static pthread_rwlock_t template_lock = PTHREAD_RWLOCK_INITIALIZER;
static tari_status_t g_status = TARI_STATUS_DISCONNECTED;
static bool g_initialized = false;

/* Response buffer for curl */
typedef struct {
    unsigned char *data;
    size_t size;
    size_t capacity;
} response_buffer_t;

/* Forward declarations */
static void poll_timer_cb(evutil_socket_t fd, short what, void *arg);
static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp);
static int parse_block_template_response(const unsigned char *data, size_t len,
                                          tari_block_template_t *tmpl);
static unsigned char *build_grpc_request(const unsigned char *payload, size_t payload_len,
                                          size_t *out_len);

/*
 * Write varint to buffer (Monero/protobuf style)
 * Returns bytes written
 */
static int write_varint_local(unsigned char *buf, uint64_t val)
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
static int read_varint_local(const unsigned char *buf, size_t max, uint64_t *val)
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

int tari_init(struct event_base *base, const tari_config_t *config)
{
    if (!config || !base) {
        log_error("tari_init: invalid parameters");
        return -1;
    }

    if (!config->enabled) {
        log_info("Tari merge mining: disabled");
        g_status = TARI_STATUS_DISCONNECTED;
        return 0;
    }

    memcpy(&g_config, config, sizeof(tari_config_t));
    g_base = base;

    /* Initialize CURL */
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        log_error("Failed to initialize CURL");
        return -1;
    }

    /* Start polling timer */
    struct timeval tv;
    tv.tv_sec = g_config.poll_interval_ms / 1000;
    tv.tv_usec = (g_config.poll_interval_ms % 1000) * 1000;

    poll_timer = event_new(base, -1, EV_PERSIST, poll_timer_cb, NULL);
    if (!poll_timer) {
        log_error("Failed to create poll timer");
        curl_global_cleanup();
        return -1;
    }
    event_add(poll_timer, &tv);

    g_status = TARI_STATUS_CONNECTING;
    g_initialized = true;

    log_info("Tari gRPC client initialized: %s:%d (poll %dms)",
             g_config.base_node_host, g_config.base_node_grpc_port,
             g_config.poll_interval_ms);

    /* Fetch initial template */
    tari_fetch_block_template(NULL, NULL);

    return 0;
}

void tari_cleanup(void)
{
    if (!g_initialized)
        return;

    log_info("Shutting down Tari gRPC client");

    if (poll_timer) {
        event_del(poll_timer);
        event_free(poll_timer);
        poll_timer = NULL;
    }

    pthread_rwlock_wrlock(&template_lock);
    if (current_template) {
        tari_template_free(current_template);
        current_template = NULL;
    }
    pthread_rwlock_unlock(&template_lock);

    curl_global_cleanup();

    g_status = TARI_STATUS_DISCONNECTED;
    g_initialized = false;
}

bool tari_is_enabled(void)
{
    return g_initialized && g_config.enabled && g_status == TARI_STATUS_CONNECTED;
}

tari_status_t tari_get_status(void)
{
    return g_status;
}

static size_t header_cb(char *buffer, size_t size, size_t nitems, void *userdata)
{
    size_t numbytes = size * nitems;
    char *line = malloc(numbytes + 1);
    if (!line) return numbytes;
    memcpy(line, buffer, numbytes);
    line[numbytes] = '\0';

    if (strncasecmp(line, "grpc-status:", 12) == 0) {
        log_warn("Tari gRPC Status: %s", trim(line + 12));
    } else if (strncasecmp(line, "grpc-message:", 13) == 0) {
        log_warn("Tari gRPC Message: %s", trim(line + 13));
    }

    free(line);
    return numbytes;
}

static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    response_buffer_t *buf = (response_buffer_t *)userp;

    if (buf->size + realsize > buf->capacity) {
        size_t new_cap = buf->capacity * 2;
        if (new_cap < buf->size + realsize)
            new_cap = buf->size + realsize + 1024;
        unsigned char *tmp = realloc(buf->data, new_cap);
        if (!tmp)
            return 0;
        buf->data = tmp;
        buf->capacity = new_cap;
    }

    memcpy(buf->data + buf->size, contents, realsize);
    buf->size += realsize;
    return realsize;
}

static unsigned char *build_grpc_request(const unsigned char *payload, size_t payload_len,
                                          size_t *out_len)
{
    /* gRPC message frame: [compressed:1][length:4][payload] */
    size_t total = GRPC_HEADER_SIZE + payload_len;
    unsigned char *frame = malloc(total);
    if (!frame)
        return NULL;

    frame[0] = 0x00;  /* Not compressed */
    frame[1] = (payload_len >> 24) & 0xFF;
    frame[2] = (payload_len >> 16) & 0xFF;
    frame[3] = (payload_len >> 8) & 0xFF;
    frame[4] = payload_len & 0xFF;

    if (payload_len > 0)
        memcpy(frame + GRPC_HEADER_SIZE, payload, payload_len);

    *out_len = total;
    return frame;
}

static void *tari_fetch_thread_func(void *arg)
{
    pthread_detach(pthread_self());

    CURL *curl = curl_easy_init();
    if (!curl) {
        log_error("Failed to create thread-local CURL handle");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    char url[512];
    snprintf(url, sizeof(url), "http://%s:%d%s",
             g_config.base_node_host, g_config.base_node_grpc_port,
             GRPC_GET_NEW_BLOCK_TEMPLATE);

    /*
     * Build protobuf request for GetNewBlockTemplate
     * Minotari v0.52: algo is a message PowAlgo (Tag 1, Type 2)
     */
    unsigned char payload[] = {0x0A, 0x02, 0x08, 0x00}; /* Tag 1 (algo), Len 2, Val {Tag 1: 0 (Monero)} */
    size_t frame_len;
    unsigned char *frame = build_grpc_request(payload, sizeof(payload), &frame_len);
    if (!frame) {
        curl_easy_cleanup(curl);
        return NULL;
    }

    response_buffer_t resp = {0};
    resp.data = malloc(4096);
    resp.capacity = 4096;
    resp.size = 0;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/grpc");
    headers = curl_slist_append(headers, "TE: trailers");
    headers = curl_slist_append(headers, "Accept: application/grpc");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, frame);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, frame_len);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    free(frame);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        log_warn("Tari gRPC request failed: %s", curl_easy_strerror(res));
        g_status = TARI_STATUS_ERROR;
        free(resp.data);
        return NULL;
    }

    if (http_code != 200) {
        log_warn("Tari gRPC returned HTTP %ld", http_code);
        g_status = TARI_STATUS_ERROR;
        free(resp.data);
        return NULL;
    }

    if (resp.size < GRPC_HEADER_SIZE) {
        log_warn("Tari gRPC response too short");
        free(resp.data);
        return NULL;
    }

    size_t payload_start = GRPC_HEADER_SIZE;
    size_t payload_len = resp.size - GRPC_HEADER_SIZE;

    tari_block_template_t *tmpl = calloc(1, sizeof(tari_block_template_t));
    if (!tmpl) {
        free(resp.data);
        return NULL;
    }

    if (parse_block_template_response(resp.data + payload_start, payload_len, tmpl) != 0) {
        log_warn("Failed to parse Tari block template");
        tari_template_free(tmpl);
        free(resp.data);
        return NULL;
    }

    /* Update current template */
    pthread_rwlock_wrlock(&template_lock);
    if (current_template)
        tari_template_free(current_template);
    current_template = tmpl;
    pthread_rwlock_unlock(&template_lock);

    g_status = TARI_STATUS_CONNECTED;

    log_debug("Tari template updated: height=%lu, diff=%lu",
              tmpl->height, tmpl->difficulty);

    free(resp.data);
    return NULL;
}

int tari_fetch_block_template(tari_template_callback_t callback, void *data)
{
    if (!g_initialized || !g_config.enabled)
        return -1;

    pthread_t thread;
    if (pthread_create(&thread, NULL, tari_fetch_thread_func, NULL) != 0) {
        log_error("Failed to create Tari fetch thread");
        return -1;
    }
    /* Thread detaches itself in the function */
    return 0;
}

static int parse_block_template_response(const unsigned char *data, size_t len,
                                          tari_block_template_t *tmpl)
{
    if (len < 10)
        return -1;

    size_t pos = 0;

    while (pos < len) {
        if (pos >= len)
            break;

        uint64_t key;
        int vlen = read_varint_local(data + pos, len - pos, &key);
        if (vlen < 0)
            break;
        pos += vlen;

        uint32_t field_num = key >> 3;
        uint32_t wire_type = key & 0x07;

        if (wire_type == 0) {  /* Varint */
            uint64_t val;
            vlen = read_varint_local(data + pos, len - pos, &val);
            if (vlen < 0)
                break;
            pos += vlen;

            if (field_num == 2) {  /* height or difficulty depending on context */
                if (tmpl->height == 0)
                    tmpl->height = val;
                else
                    tmpl->difficulty = val;
            }
        }
        else if (wire_type == 2) {  /* Length-delimited */
            uint64_t field_len;
            vlen = read_varint_local(data + pos, len - pos, &field_len);
            if (vlen < 0)
                break;
            pos += vlen;

            if (pos + field_len > len)
                break;

            if (field_num == 1 && !tmpl->header) {  /* Header */
                tmpl->header = malloc(field_len);
                if (tmpl->header) {
                    memcpy(tmpl->header, data + pos, field_len);
                    tmpl->header_size = field_len;
                }
            }
            else if (field_num == 2 && !tmpl->body) {  /* Body */
                tmpl->body = malloc(field_len);
                if (tmpl->body) {
                    memcpy(tmpl->body, data + pos, field_len);
                    tmpl->body_size = field_len;
                }
            }

            pos += field_len;
        }
        else {
            break;
        }
    }

    if (tmpl->header && tmpl->header_size > 0) {
        extern void keccak(const uint8_t *in, size_t inlen, uint8_t *md, int mdlen);
        keccak(tmpl->header, tmpl->header_size, tmpl->merge_mining_hash, 32);
    }

    tmpl->timestamp = time(NULL);

    return (tmpl->header && tmpl->body) ? 0 : -1;
}

const tari_block_template_t *tari_get_current_template(void)
{
    const tari_block_template_t *t = NULL;
    pthread_rwlock_rdlock(&template_lock);
    t = current_template;
    pthread_rwlock_unlock(&template_lock);
    return t;
}

int tari_get_coinbase(uint64_t height, uint64_t reward, uint64_t fees,
                       unsigned char **coinbase, size_t *coinbase_len)
{
    if (!g_initialized || !g_config.enabled)
        return -1;

    CURL *curl = curl_easy_init();
    if (!curl) return -1;

    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    char url[512];
    snprintf(url, sizeof(url), "http://%s:%d%s",
             g_config.wallet_host, g_config.wallet_grpc_port,
             GRPC_GET_COINBASE);

    unsigned char payload[64];
    size_t plen = 0;

    payload[plen++] = 0x08;
    plen += write_varint_local(payload + plen, reward);

    payload[plen++] = 0x10;
    plen += write_varint_local(payload + plen, fees);

    payload[plen++] = 0x18;
    plen += write_varint_local(payload + plen, height);

    size_t frame_len;
    unsigned char *frame = build_grpc_request(payload, plen, &frame_len);
    if (!frame) {
        curl_easy_cleanup(curl);
        return -1;
    }

    response_buffer_t resp = {0};
    resp.data = malloc(4096);
    resp.capacity = 4096;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/grpc");
    headers = curl_slist_append(headers, "TE: trailers");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, frame);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, frame_len);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    free(frame);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || resp.size < GRPC_HEADER_SIZE) {
        free(resp.data);
        return -1;
    }

    *coinbase_len = resp.size - GRPC_HEADER_SIZE;
    *coinbase = malloc(*coinbase_len);
    if (!*coinbase) {
        free(resp.data);
        return -1;
    }
    memcpy(*coinbase, resp.data + GRPC_HEADER_SIZE, *coinbase_len);

    free(resp.data);
    return 0;
}

int tari_submit_block(const unsigned char *header, size_t header_size,
                       const unsigned char *body, size_t body_size,
                       const unsigned char *pow_data, size_t pow_data_size,
                       tari_submit_callback_t callback, void *data)
{
    if (!g_initialized || !g_config.enabled)
        return -1;

    CURL *curl = curl_easy_init();
    if (!curl) return -1;

    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, g_config.timeout_ms);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    char url[512];
    snprintf(url, sizeof(url), "http://%s:%d%s",
             g_config.base_node_host, g_config.base_node_grpc_port,
             GRPC_SUBMIT_BLOCK);

    size_t total_payload = header_size + body_size + pow_data_size + 64;
    unsigned char *payload = malloc(total_payload);
    if (!payload) {
        curl_easy_cleanup(curl);
        return -1;
    }

    size_t plen = 0;

    payload[plen++] = 0x0A;
    plen += write_varint_local(payload + plen, header_size + body_size);
    memcpy(payload + plen, header, header_size);
    plen += header_size;
    memcpy(payload + plen, body, body_size);
    plen += body_size;

    payload[plen++] = 0x12;
    plen += write_varint_local(payload + plen, pow_data_size);
    memcpy(payload + plen, pow_data, pow_data_size);
    plen += pow_data_size;

    size_t frame_len;
    unsigned char *frame = build_grpc_request(payload, plen, &frame_len);
    free(payload);
    if (!frame) {
        curl_easy_cleanup(curl);
        return -1;
    }

    response_buffer_t resp = {0};
    resp.data = malloc(1024);
    resp.capacity = 1024;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/grpc");
    headers = curl_slist_append(headers, "TE: trailers");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, frame);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, frame_len);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    free(frame);
    curl_easy_cleanup(curl);

    bool success = (res == CURLE_OK);
    const char *error = success ? NULL : curl_easy_strerror(res);

    if (callback)
        callback(success, error, data);

    free(resp.data);
    return success ? 0 : -1;
}

bool tari_validate_address(const char *address)
{
    if (!address)
        return false;

    size_t len = strlen(address);
    if (len < 64 || len > TARI_ADDRESS_MAX)
        return false;

    for (size_t i = 0; i < len; i++) {
        char c = address[i];
        if (!((c >= '0' && c <= '9') ||
              (c >= 'a' && c <= 'f') ||
              (c >= 'A' && c <= 'F')))
            return false;
    }

    return true;
}

void tari_template_free(tari_block_template_t *tmpl)
{
    if (!tmpl)
        return;

    if (tmpl->header)
        free(tmpl->header);
    if (tmpl->body)
        free(tmpl->body);
    free(tmpl);
}

tari_block_template_t *tari_template_copy(const tari_block_template_t *src)
{
    if (!src)
        return NULL;

    tari_block_template_t *dst = calloc(1, sizeof(tari_block_template_t));
    if (!dst)
        return NULL;

    dst->height = src->height;
    dst->difficulty = src->difficulty;
    dst->total_fees = src->total_fees;
    dst->reward = src->reward;
    dst->timestamp = src->timestamp;
    memcpy(dst->merge_mining_hash, src->merge_mining_hash, TARI_HASH_SIZE);

    if (src->header && src->header_size > 0) {
        dst->header = malloc(src->header_size);
        if (!dst->header) {
            free(dst);
            return NULL;
        }
        memcpy(dst->header, src->header, src->header_size);
        dst->header_size = src->header_size;
    }

    if (src->body && src->body_size > 0) {
        dst->body = malloc(src->body_size);
        if (!dst->body) {
            free(dst->header);
            free(dst);
            return NULL;
        }
        memcpy(dst->body, src->body, src->body_size);
        dst->body_size = src->body_size;
    }

    return dst;
}

uint64_t tari_get_difficulty(void)
{
    const tari_block_template_t *t = tari_get_current_template();
    return t ? t->difficulty : 0;
}

uint64_t tari_get_height(void)
{
    const tari_block_template_t *t = tari_get_current_template();
    return t ? t->height : 0;
}

static void poll_timer_cb(evutil_socket_t fd, short what, void *arg)
{
    (void)fd;
    (void)what;
    (void)arg;

    log_trace("Polling Tari for new block template");
    tari_fetch_block_template(NULL, NULL);
}
