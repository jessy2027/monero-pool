# Monero Pool Architecture

## Overview

This document describes the architecture of the monero-pool mining pool server.

## Design Philosophy

The pool is designed with a focus on **performance** and **efficiency**:

- Uses only **2 threads** under normal operation
- Event-driven I/O via **libevent** for handling thousands of concurrent miners
- **LMDB** database for fast, persistent storage
- **PPLNS** (Pay Per Last N Shares) for fair reward distribution

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Mining Pool                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌──────────────┐    ┌──────────────┐    ┌──────────────┐     │
│   │   Miners     │    │  Web Clients │    │   Upstream   │     │
│   │  (Stratum)   │    │    (HTTP)    │    │    Pool      │     │
│   └──────┬───────┘    └──────┬───────┘    └──────┬───────┘     │
│          │                   │                   │              │
│          ▼                   ▼                   ▼              │
│   ┌──────────────────────────────────────────────────────┐     │
│   │                    Event Loop                         │     │
│   │                   (libevent)                          │     │
│   └──────────────────────────────────────────────────────┘     │
│          │                   │                   │              │
│          ▼                   ▼                   ▼              │
│   ┌──────────────┐    ┌──────────────┐    ┌──────────────┐     │
│   │   Stratum    │    │   Web UI     │    │   Trusted    │     │
│   │   Handler    │    │   Handler    │    │   Handler    │     │
│   └──────┬───────┘    └──────────────┘    └──────┬───────┘     │
│          │                                       │              │
│          ▼                                       ▼              │
│   ┌─────────────────────────────────────────────────────┐      │
│   │                   LMDB Database                      │      │
│   │   ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐  │      │
│   │   │ Shares  │ │ Blocks  │ │ Balance │ │Payments │  │      │
│   │   └─────────┘ └─────────┘ └─────────┘ └─────────┘  │      │
│   └─────────────────────────────────────────────────────┘      │
│                                                                  │
│   ┌─────────────────────────────────────────────────────┐      │
│   │                   RPC Client                         │      │
│   │   ┌──────────────────┐  ┌──────────────────────┐   │      │
│   │   │     monerod      │  │  monero-wallet-rpc   │   │      │
│   │   │  (Block/Submit)  │  │    (Payments)        │   │      │
│   │   └──────────────────┘  └──────────────────────┘   │      │
│   └─────────────────────────────────────────────────────┘      │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Module Structure

### Core Modules

| Module | File(s) | Description |
|--------|---------|-------------|
| Pool | `pool.c`, `pool.h` | Main pool logic, event handling, configuration |
| Database | `database.h` | LMDB operations for shares, blocks, balances |
| Stratum | `stratum.h` | Mining protocol implementation |
| Payout | `payout.h` | PPLNS reward calculation and distribution |
| RPC | `rpc.h` | Communication with Monero daemon and wallet |
| Web UI | `webui.c`, `webui.h` | HTTP server for monitoring interface |

### Utility Modules

| Module | File(s) | Description |
|--------|---------|-------------|
| BStack | `bstack.c`, `bstack.h` | Bounded stack with automatic recycling |
| GrowBag | `growbag.c`, `growbag.h` | Dynamic container with efficient allocation |
| Util | `util.c`, `util.h` | Hex conversion, varint encoding, etc. |
| XMR | `xmr.cpp`, `xmr.h` | Monero cryptographic operations |
| Compat | `compat.h` | Cross-platform compatibility layer |
| ForkOff | `forkoff.c`, `forkoff.h` | Daemonization support |

### External Libraries

| Library | Purpose |
|---------|---------|
| libevent | Asynchronous event handling |
| LMDB | Key-value database |
| json-c | JSON parsing |
| OpenSSL | BigNum operations for difficulty |
| libuuid | UUID generation for jobs |

## Data Flow

### Share Submission Flow

```
1. Miner submits share via Stratum
   │
2. Validate job exists for client
   │
3. Check for duplicate submission
   │
4. Build block from template + nonce
   │
5. Hash with RandomX (or CN variant)
   │
6. Compare hash to target difficulty
   │
   ├─► If hash >= block difficulty:
   │   │
   │   └─► Submit block to network
   │
7. Store share in database
   │
8. Update client/account hashrate stats
   │
9. Check if retargeting needed
```

### Block Found Flow

```
1. Block submitted to network
   │
2. Block stored as LOCKED in database
   │
3. After 60 confirmations:
   │
   ├─► Block UNLOCKED → Process PPLNS payout
   │   │
   │   └─► Credit balances
   │
   └─► Block ORPHANED → Mark as orphaned
```

### Payment Flow

```
1. Timer triggers payment check (every 10 minutes)
   │
2. Query balances exceeding threshold
   │
3. Build transfer_split RPC request
   │
4. Submit to monero-wallet-rpc
   │
5. On success:
   │
   ├─► Deduct balances
   │
   └─► Store payment records
```

## Thread Safety

### Locks

| Lock | Purpose |
|------|---------|
| `rwlock_tx` | Database transaction serialization |
| `rwlock_acc` | Account hash table access |
| `rwlock_cfd` | Client-by-fd hash table access |
| `mutex_clients` | Client reading counter |
| `mutex_log` | Thread-safe logging |

### Lock Order

To prevent deadlocks, always acquire locks in this order:
1. `rwlock_tx`
2. `mutex_clients`
3. `rwlock_acc`
4. `rwlock_cfd`

## Configuration

Key configuration options in `pool.conf`:

```ini
# Network
pool-listen = 0.0.0.0
pool-port = 4242
pool-ssl-port = 4343

# Difficulty
pool-start-diff = 1000      # Starting difficulty
pool-fixed-diff = 0         # 0 = dynamic
pool-nicehash-diff = 280000 # For NiceHash clients

# Retargeting
retarget-time = 30          # Seconds between retargets
retarget-ratio = 0.55       # Target utilization ratio

# Payouts
pool-fee = 0.01             # 1% fee
payment-threshold = 0.005   # Minimum XMR for payout
share-mul = 2.0             # PPLNS window multiplier
```

## Performance Considerations

1. **Event-Driven I/O**: libevent handles all network I/O without blocking
2. **Bounded Structures**: `bstack` and `growbag` prevent unbounded memory growth
3. **Database Design**: LMDB provides zero-copy reads and efficient transactions
4. **RandomX**: Full-memory mode available via `MONERO_RANDOMX_FULL_MEM` env var

## Security Considerations

1. **Share Validation**: All shares are cryptographically verified
2. **Duplicate Prevention**: Per-job submission tracking
3. **Bad Share Limit**: Clients disconnected after `MAX_BAD_SHARES`
4. **Trusted Connections**: Separate listener for pool-to-pool communication

## Extending the Pool

### Adding a New API Endpoint

1. Add handler in `webui.c`:
```c
static void send_json_new_endpoint(struct evhttp_request *req, void *arg) {
    // Implementation
}
```

2. Register in `process_request()`:
```c
if (strstr(url, "/new_endpoint") != NULL) {
    send_json_new_endpoint(req, arg);
    return;
}
```

### Adding a New Stratum Method

1. Add handler in `pool.c`:
```c
static void miner_on_new_method(json_object *message, client_t *client) {
    // Implementation
}
```

2. Register in `miner_on_read()`:
```c
else if (strcmp(method_name, "new_method") == 0) {
    miner_on_new_method(message, client);
}
```

## Building

```bash
# Set Monero source path
export MONERO_ROOT=/path/to/monero

# Debug build
make

# Release build
make release
```

## Testing

For local testing, use testnet:
```bash
monerod --testnet --rpc-bind-port 28081
monero-wallet-rpc --testnet --rpc-bind-port 28084 --wallet-file test_wallet
./monero-pool --config-file pool_testnet.conf
```

