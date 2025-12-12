# Monero Pool - Docker Deployment Guide

## 🚀 Quick Start

### Prerequisites
1. **Docker Desktop for Windows** - [Download here](https://www.docker.com/products/docker-desktop/)
2. At least **100 GB** of free disk space (for pruned blockchain)
3. A Monero wallet address for receiving mining rewards

### Installation

1. **Run the setup script** (as Administrator):
   ```cmd
   setup-windows.bat
   ```

2. **Edit your configuration**:
   ```cmd
   notepad C:\MoneroPool\config\pool.conf
   ```
   
   **IMPORTANT**: Change `pool-wallet` to your Monero wallet address!

3. **Start the pool**:
   ```cmd
   docker-compose up -d
   ```

4. **Wait for blockchain sync** (can take several hours on first run):
   ```cmd
   docker-compose logs -f monerod
   ```

---

## 📁 Directory Structure

```
C:\MoneroPool\
├── blockchain\     ← Monero blockchain data (~70 GB pruned)
├── wallet\         ← Wallet RPC data
├── pool-data\      ← Pool database (shares, payments) - CRITICAL!
├── config\         ← Configuration files
│   └── pool.conf   ← Main pool configuration
└── backups\        ← Automated backups
```

---

## 🔧 Common Commands

| Action | Command |
|--------|---------|
| Start pool | `docker-compose up -d` |
| Stop pool | `docker-compose down` |
| View logs | `docker-compose logs -f` |
| View pool logs only | `docker-compose logs -f monero-pool` |
| Restart pool | `docker-compose restart monero-pool` |
| Rebuild after update | `docker-compose build --no-cache && docker-compose up -d` |
| Check status | `docker-compose ps` |

---

## 🌐 Access Points

| Service | URL/Address |
|---------|-------------|
| **Web UI** | http://localhost:8080 |
| **Stratum (miners)** | `stratum+tcp://YOUR_IP:4242` |
| **Monerod RPC** | http://localhost:18081 |
| **Wallet RPC** | http://localhost:28084 |

---

## 💾 Backup

### Manual Backup
Run `backup.bat` to create a timestamped backup of:
- Pool database (shares, balances, payments)
- Wallet data
- Configuration

### Automated Backup
Schedule `backup.bat` with Windows Task Scheduler:
1. Open Task Scheduler
2. Create Basic Task → "Monero Pool Backup"
3. Trigger: Daily at 3:00 AM
4. Action: Start `C:\Users\jessy\monero-pool\backup.bat`

### Restore from Backup
```cmd
docker-compose down
powershell -Command "Expand-Archive -Path 'C:\MoneroPool\backups\backup_XXXX.zip' -DestinationPath 'C:\MoneroPool\' -Force"
docker-compose up -d
```

---

## 🔄 Auto-Start with Windows

Docker Desktop automatically starts with Windows by default.
The pool has `restart: always` configured, so it will restart automatically.

To verify:
1. Docker Desktop Settings → General → ✅ "Start Docker Desktop when you log in"
2. The `restart: always` policy ensures containers restart after:
   - Windows reboot
   - Docker restart
   - Container crash

---

## 📊 Monitoring

### Health Check
The pool container has a built-in health check. Check status:
```cmd
docker inspect monero-pool --format='{{.State.Health.Status}}'
```

### Resource Usage
```cmd
docker stats
```

---

## 🔒 Security Recommendations

1. **Firewall**: Only open port 4242 (stratum) to the internet
2. **Wallet Password**: Use a strong password for your wallet
3. **Backups**: Keep encrypted backups offsite
4. **Updates**: Regularly rebuild the pool image for security patches

---

## 🆘 Troubleshooting

### Pool won't start
```cmd
docker-compose logs monero-pool
```

### Blockchain sync issues
```cmd
docker-compose logs monerod
# If corrupted, delete and resync:
docker-compose down
rmdir /S /Q C:\MoneroPool\blockchain
docker-compose up -d
```

### Wallet issues
```cmd
docker-compose logs monero-wallet-rpc
```

### Reset everything
```cmd
docker-compose down -v
rmdir /S /Q C:\MoneroPool
# Then run setup-windows.bat again
```

---

## 📝 Files Created

| File | Description |
|------|-------------|
| `Dockerfile` | Multi-stage build for the pool |
| `docker-compose.yml` | Orchestration configuration |
| `pool.docker.conf` | Docker-optimized pool config template |
| `setup-windows.bat` | Initial setup script |
| `backup.bat` | Backup automation script |

---

## 🔗 Useful Links

- [Monero Pool GitHub](https://github.com/jtgrassie/monero-pool)
- [Docker Desktop](https://www.docker.com/products/docker-desktop/)
- [Monero Project](https://www.getmonero.org/)
