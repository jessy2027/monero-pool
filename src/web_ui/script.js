// ========================================
// I18n & Translations
// ========================================
const translations = {
    en: {
        tagline: "Global Monero Mining Pool",
        nav_dashboard: "Dashboard",
        nav_miner: "Miner Stats",
        nav_start: "Get Started",
        nav_privacy: "Privacy Policy",
        footer_cookies: "(No cookies used)",
        live: "Live",
        pool_hashrate: "Pool Hashrate",
        pool_online: "Pool Online",
        miners_connected: "miners connected",
        miners_connected_cap: "Miners Connected",
        network_hashrate: "Network Hashrate",
        block_height: "Block Height",
        blocks_found: "Blocks Found",
        last_block_found: "Last Block Found",
        pool_fee: "Pool Fee",
        min_payout: "Min. Payout",
        port_ssl: "Port / SSL",
        chart_title: "Pool Hashrate History",
        round_progress: "Current Round Progress",
        round_hashes: "Round Hashes",
        round_luck: "Round Luck",
        est_time: "Est. Time to Block",
        last_template: "Last Template",
        self_select: "Self-Select",
        yes: "Yes",
        blocks_found_title: "Blocks Found by Pool",
        loading_blocks: "Loading blocks...",
        miner_stats_title: "Miner Statistics",
        enter_wallet: "Enter your wallet address to view your mining stats",
        placeholder_wallet: "Enter your Monero wallet address...",
        lookup: "Lookup",
        your_hashrate: "Your Hashrate",
        balance_due: "Balance Due",
        workers: "Workers",
        est_earnings: "Est. Daily Earnings",
        your_workers: "Your Workers",
        loading_workers: "Loading workers...",
        no_workers: "No active workers found",
        err_workers: "Error loading workers",
        payment_history: "Payment History",
        loading_payments: "Loading payments...",
        no_payments: "No payments yet",
        err_payments: "Error loading payments",
        start_title: "Get Started Mining",
        connect_minutes: "Connect to our pool in minutes",
        step1: "Get a Wallet",
        step1_desc: "Download the official Monero GUI wallet or use a paper wallet.",
        download_wallet: "Download Wallet",
        step2: "Download Miner",
        step2_desc: "We recommend XMRig for the best performance and compatibility.",
        get_xmrig: "Get XMRig",
        step3: "Configure & Start",
        step3_desc: "Use the connection details below to configure your miner.",
        conn_details: "Connection Details",
        region_eu: "Europe",
        region_us: "USA",
        stratum_eu: "Stratum TCP",
        stratum_eu_ssl: "Stratum SSL 🔒",
        stratum_us: "Stratum TCP",
        stratum_us_ssl: "Stratum SSL 🔒",


        powered_by: "Powered by",
        status_locked: "Locked",
        status_unlocked: "Unlocked",
        status_orphaned: "Orphaned",
        status_online: "Online",
        wallet_saved: "Wallet saved! Refreshing...",
        invalid_addr: "Invalid Monero address",
        copied: "Copied to clipboard!",
        never: "Never",
        awaiting_first_block: "Awaiting First Block (New Pool)",
        ago: "ago",
        // Hero - Updated for Quick Connect
        hero_title: "Mine Monero Now",
        hero_subtitle: "0% Fee • Global Servers • SSL • PPLNS",
        security_phrase: "No KYC – No tracking – No JavaScript mining",
        new_to_mining: "New to Monero mining? Start here",
        connect_miner: "Connect your miner:",
        replace_wallet: "Replace YOUR_WALLET with your Monero address",
        generate_config: "Generate config.json with your wallet",
        btn_start_mining: "Start Mining",
        btn_view_stats: "View Stats",
        // New features
        why_title: "Why Choose EuroXMR?",
        why_subtitle: "The Global Monero pool built for miners",
        why_low_latency: "Low Latency",
        why_low_latency_desc: "Global servers (EU & US) for optimal ping and faster share submissions",
        why_pplns: "Fair PPLNS",
        why_pplns_desc: "Pay Per Last N Shares rewards loyal miners fairly",
        why_low_threshold: "Low Payout",
        why_low_threshold_desc: "Only 0.005 XMR minimum payout threshold",
        why_ssl: "SSL Support",
        why_ssl_desc: "Secure stratum connection with TLS encryption",
        why_randomx: "RandomX Full",
        why_randomx_desc: "First pool with full RandomX fast-mode support",
        trust_uptime: "Uptime",
        trust_total_paid: "Total Paid",
        trust_blocks: "Blocks Found",
        trust_miners: "Active Miners",
        calc_title: "💰 Profitability Calculator",
        calc_hashrate: "Your Hashrate (H/s)",
        calc_power: "Power (W)",
        calc_cost: "Electricity (€/kWh)",
        calc_daily: "Daily",
        calc_monthly: "Monthly",
        calc_profit: "Net Profit/Month",
        config_title: "XMRig Config Generator",
        config_wallet: "Wallet Address",
        config_worker: "Worker Name",
        config_ssl: "Use SSL",
        copy: "Copy",
        faq_title: "Frequently Asked Questions",
        faq_q1: "What is PPLNS and how does it work?",
        faq_a1: "PPLNS (Pay Per Last N Shares) is a payout system that rewards miners based on their contribution to the last N shares before a block is found. This system discourages pool hopping and rewards loyal miners who contribute consistently.",
        faq_q2: "When will I receive my payment?",
        faq_a2: "Payments are processed automatically when your balance reaches the minimum threshold of 0.005 XMR. Payments are typically sent within a few hours after meeting the threshold.",
        faq_q3: "How do I configure XMRig?",
        faq_a3: "Use our XMRig Config Generator above! Simply enter your wallet address and worker name, then copy the generated configuration.",
        faq_q4: "What hashrate can I expect from my hardware?",
        faq_a4: "RandomX is CPU-optimized. Typical: Intel i5 = 3-5 KH/s, Intel i7/i9 = 5-10 KH/s, AMD Ryzen 5 = 5-8 KH/s, AMD Ryzen 7/9 = 8-15 KH/s.",
        faq_q5: "Is SSL/TLS connection recommended?",
        faq_a5: "Yes! SSL/TLS encrypts your connection and prevents ISP throttling. Use port 4343 with stratum+ssl:// for encrypted mining.",

        // Premium Trust Badges
        badge_secure: "Secure & Encrypted",
        badge_uptime: "99.9% Uptime",
        badge_verified: "Verified Pool",
        badge_low_fee: "0% Pool Fee",
        footer_tagline: "Global Monero Mining Pool",
        footer_trust_ssl: "SSL Encrypted",
        footer_trust_auto: "Auto Payouts",
        footer_trust_support: "Email Support",
        // Lottery
        nav_lottery: "Lottery",
        lottery_title: "Weekly Hashrate Lottery",
        lottery_subtitle: "Mine with us and win bonuses, even without finding a block!",
        lottery_prize: "Current Prize",
        lottery_next_draw: "Next Draw",
        lottery_your_chances: "Your Chances",
        lottery_total_tickets: "Total Tickets",
        lottery_how_title: "How It Works",
        lottery_rule1: "1 Valid Share = 1 Lottery Ticket",
        lottery_rule2: "Draw every Sunday at 20:00 CET",
        lottery_rule3: "More shares = Higher chances, but everyone can win!",
        lottery_winners_title: "Recent Winners",
        lottery_date: "Date",
        lottery_winner: "Winner",
        lottery_amount: "Amount",
        lottery_no_winners: "No winners yet - Be the first!",
        lottery_transparent: "100% Transparent - All transactions verified on blockchain",
        lottery_view_code: "View Open Source Code",
        // Trust & Contact
        about_title: "Who runs this pool?",
        about_text: "EuroXMR is operated by independent miners dedicated to decentralization. We prioritize transparency, stability, and zero fees to support the Monero network.",
        independent_miner: "Pool operated by independent miners",
        contact_title: "Contact & Support",
        contact_discord: "Discord Community",
        discord_join_action: "Join Server",
        privacy_title: "Privacy & Security",
        privacy_text: "We respect your anonymity. EuroXMR does not require account registration or KYC. We do not store personal data or IP addresses permanently. No tracking cookies are used on this site.",
        // Hero Lottery Teaser
        lottery_teaser_title: "Weekly Lottery",
        lottery_teaser_next: "Next Draw",
        // Social Proof Bar
        proof_total_paid: "XMR Paid",
        proof_blocks: "Blocks Found",
        proof_last_block: "Last Block",
        // Onboarding Stepper
        step_wallet: "Wallet",
        step_miner: "Miner",
        step_config: "Configure",
        use_generator: "Use Config Generator",
        detected: "detected",
        // Validation
        valid_address: "✓ Valid Monero address",
        invalid_address_format: "Invalid address format",
        // Config Actions
        download: "Download",
        config_downloaded: "config.json downloaded!",
        // New redesign translations
        why_decentralization: "Protect Monero",
        why_decentralization_desc: "Support network decentralization - avoid 51% attacks from mega-pools",
        decentralization_title: "Why Choose a Small Pool?",
        decentralization_intro: "When you mine on massive pools, you're handing control of Monero's network to a few entities. If just 2-3 pools control over 51% of hashrate, they could censor transactions or undermine the privacy that makes Monero valuable.",
        benefit1_title: "Protect Decentralization",
        benefit1_desc: "Every miner on a small pool strengthens the network against 51% attacks",
        benefit2_title: "Personal Support",
        benefit2_desc: "Get real help from real humans, not automated ticket systems",
        benefit3_title: "Exclusive Perks",
        benefit3_desc: "Weekly lottery and bonus features unavailable on mega-pools",
        decentralization_note: "💡 Block rewards follow the same math regardless of pool size. The only difference is frequency vs. size of payouts.",
        lottery_terms_title: "Terms & Conditions",
        lottery_term1: "Open to all active miners on EuroXMR.eu",
        lottery_term2: "No purchase necessary - mining activity is the only requirement",
        lottery_term3: "Winner selection uses cryptographically verifiable randomness",
        lottery_term4: "All draws and payments are publicly verifiable on Monero blockchain",
        lottery_term5: "Pool operator reserves the right to modify or discontinue the lottery",
        footer_disclaimer: "Mining involves inherent risks including hardware costs, electricity expenses, and market volatility. EuroXMR.eu operates pool infrastructure on a best-effort basis. We are not responsible for network issues, orphaned blocks, or fluctuations in cryptocurrency value. Users must comply with all applicable laws in their jurisdiction.",
        discord_join: "Join Discord",
        // Merge Mining
        mode_standard: "Standard (XMR)",
        mode_dual: "Dual (XMR + Tari)",
        dual_mining_desc: "Mine both Monero and Tari simultaneously without losing XMR hashrate.",
        dual_wallet_format_desc: "Format: XMR_ADDRESS.TARI_ADDRESS",
        new_feature: "New Feature",

        pool_launch_date: "Pool launched December 2025",
        pool_launch_message: "We're a new pool growing our hashrate. Join our early miners and help decentralize Monero!"
    },

    fr: {
        tagline: "Pool de Minage Monero Global",
        // ... (existing FR strings)
        mode_standard: "Standard (XMR)",
        mode_dual: "Double (XMR + Tari)",
        dual_mining_desc: "Minez Monero et Tari simultanément sans perdre de hashrate XMR.",
        dual_wallet_format_desc: "Format : ADRESSE_XMR.ADRESSE_TARI",
        new_feature: "Nouveauté",

        nav_dashboard: "Tableau de Bord",
        nav_miner: "Statistiques",
        // ...
        pool_launch_date: "Pool lancé en Décembre 2025",
        pool_launch_message: "Nous sommes un nouveau pool en pleine croissance. Rejoignez nos premiers mineurs et aidez à décentraliser Monero !"
    },

    de: {
        tagline: "Monero Mining Pool",
        // ...
        mode_standard: "Standard (XMR)",
        mode_dual: "Dual (XMR + Tari)",
        dual_mining_desc: "Minen Sie Monero und Tari gleichzeitig ohne XMR-Hashrate-Verlust.",
        dual_wallet_format_desc: "Format: XMR_ADRESSE.TARI_ADRESSE",
        new_feature: "Neuheit",

        nav_dashboard: "Dashboard",
        // ...
        pool_launch_date: "Pool gestartet im Dezember 2025",
        pool_launch_message: "Wir sind ein neuer Pool, der seine Hashrate aufbaut. Schließen Sie sich unseren Early Miners an und helfen Sie, Monero zu dezentralisieren!"
    },
    es: {
        tagline: "Pool de Minería Monero",
        // ...
        mode_standard: "Estándar (XMR)",
        mode_dual: "Doble (XMR + Tari)",
        dual_mining_desc: "Mina Monero y Tari simultáneamente sin perder hashrate XMR.",
        dual_wallet_format_desc: "Formato: DIRECCION_XMR.DIRECCION_TARI",
        new_feature: "Novedad",

        nav_dashboard: "Panel",
        // ...
        pool_launch_date: "Pool lanzado en Diciembre 2025",
        pool_launch_message: "Somos un nuevo pool que está creciendo. ¡Únete a nuestros primeros mineros y ayuda a descentralizar Monero!"
    },
    ru: {
        tagline: "Пул для майнинга Monero",
        // ...
        mode_standard: "Стандарт (XMR)",
        mode_dual: "Дуал (XMR + Tari)",
        dual_mining_desc: "Майните Monero и Tari одновременно без потери хешрейта XMR.",
        dual_wallet_format_desc: "Формат: XMR_АДРЕС.TARI_АДРЕС",
        new_feature: "Новинка",

        nav_dashboard: "Панель",
        // ...
        pool_launch_date: "Пул запущен в Декабре 2025",
        pool_launch_message: "Мы новый пул, наращивающий хешрейт. Присоединяйтесь к нашим первым майнерам и помогите децентрализовать Monero!"
    }
};

let currentLang = 'en';

function setLanguage(lang) {
    if (!translations[lang]) return;
    currentLang = lang;
    localStorage.setItem('user_lang', lang);

    // Update buttons
    document.querySelectorAll('.lang-btn').forEach(btn => btn.classList.remove('active'));
    document.getElementById('btn_' + lang).classList.add('active');

    // Apply translations
    document.querySelectorAll('[data-i18n]').forEach(el => {
        const key = el.getAttribute('data-i18n');
        if (translations[lang][key]) {
            if (el.tagName === 'INPUT' && el.getAttribute('placeholder')) {
                el.placeholder = translations[lang][key];
            } else {
                el.textContent = translations[lang][key];
            }
        }
    });

    // Update dynamic content
    if (window.lastStats) updatePage(window.lastStats);
    fetchBlocks(); // Re-render tables
    if (document.getElementById('miner_stats').classList.contains('hidden') === false) {
        fetchWorkers();
        fetchPayments();
    }
}

function t(key) {
    return translations[currentLang][key] || key;
}

// ========================================
// Utility functions
// ========================================
function formatHashrate(h) {
    if (h < 1e3) return h.toFixed(0) + ' H/s';
    if (h < 1e6) return (h / 1e3).toFixed(2) + ' KH/s';
    if (h < 1e9) return (h / 1e6).toFixed(2) + ' MH/s';
    return (h / 1e9).toFixed(2) + ' GH/s';
}

function formatHashes(h) {
    if (h < 1e6) return h.toLocaleString();
    if (h < 1e9) return (h / 1e6).toFixed(2) + ' MH';
    if (h < 1e12) return (h / 1e9).toFixed(2) + ' GH';
    return (h / 1e12).toFixed(2) + ' TH';
}

function formatTime(ts) {
    if (!ts || ts === 0) return t('awaiting_first_block');
    var diff = Date.now() / 1000 - ts;
    var suffix = ' ' + t('ago');
    if (diff < 60) return Math.floor(diff) + 's' + suffix;
    if (diff < 3600) return Math.floor(diff / 60) + 'm' + suffix;
    if (diff < 86400) return Math.floor(diff / 3600) + 'h' + suffix;
    return Math.floor(diff / 86400) + 'd' + suffix;
}

function formatDuration(seconds) {
    if (seconds < 60) return Math.floor(seconds) + 's';
    if (seconds < 3600) return Math.floor(seconds / 60) + 'm';
    if (seconds < 86400) return Math.floor(seconds / 3600) + 'h ' + Math.floor((seconds % 3600) / 60) + 'm';
    return Math.floor(seconds / 86400) + 'd ' + Math.floor((seconds % 86400) / 3600) + 'h';
}

function showToast(msg, type) {
    var t = document.getElementById('toast');
    t.textContent = msg;
    t.className = 'toast show' + (type ? ' ' + type : '');
    setTimeout(function () { t.classList.remove('show'); }, 3000);
}

function getCookie(name) {
    var v = document.cookie.match('(^|;)\\s*' + name + '\\s*=\\s*([^;]+)');
    return v ? v.pop() : '';
}

function setCookie(name, value) {
    var d = new Date();
    d.setTime(d.getTime() + 365 * 24 * 60 * 60 * 1000);
    document.cookie = name + '=' + value + ';expires=' + d.toGMTString() + ';path=/';
}

function copyToClipboard(el) {
    var copiedText = el.textContent;
    navigator.clipboard.writeText(copiedText).then(function () {
        showToast(t('copied'), 'success');
        // GA4: Track stratum URL/port copy
        if (typeof gtag === 'function') {
            gtag('event', 'copy_stratum', {
                'event_category': 'engagement',
                'event_label': copiedText
            });
        }
    });
}

// ========================================
// Quick Connect Box Functions
// ========================================
var currentOS = 'linux';

function copyStratum() {
    var stratum = document.getElementById('main_stratum').textContent;
    var btn = document.getElementById('copy_stratum_btn');

    navigator.clipboard.writeText(stratum).then(function () {
        showToast(t('copied'), 'success');
        btn.classList.add('copied');
        btn.querySelector('span').textContent = '✓';

        setTimeout(function () {
            btn.classList.remove('copied');
            btn.querySelector('span').textContent = t('copy');
        }, 2000);

        // GA4: Track stratum copy from hero
        if (typeof gtag === 'function') {
            gtag('event', 'copy_stratum_hero', {
                'event_category': 'conversion',
                'event_label': stratum
            });
        }
    });
}

function switchOS(os) {
    currentOS = os;

    // Update tabs
    document.querySelectorAll('.os-tab').forEach(function (tab) {
        tab.classList.remove('active');
    });
    document.getElementById('os_' + os).classList.add('active');

    // Update command
    updateQuickCommand();
}

function switchMiningMode(mode) {
    // Update buttons
    document.querySelectorAll('.mode-btn').forEach(btn => btn.classList.remove('active'));
    document.getElementById('mode_' + mode).classList.add('active');

    // Update content
    document.querySelectorAll('.conn-content').forEach(content => content.style.display = 'none');
    document.getElementById('conn_' + mode).style.display = 'block';

    // Analytics
    if (typeof gtag === 'function') {
        gtag('event', 'switch_mining_mode', {
            'event_category': 'engagement',
            'event_label': mode
        });
    }
}

function initHeroOS() {
    // Simple auto-detect for Hero tabs
    if (navigator.userAgent.toLowerCase().includes('win')) {
        switchOS('windows');
    } else {
        switchOS('linux');
    }
}

function updateQuickCommand() {
    var prefix = currentOS === 'linux' ? './' : '';
    var command = prefix + 'xmrig -o euroxmr.eu:4343 --tls -u YOUR_WALLET -p rig1';
    document.getElementById('quick_command').textContent = command;
}

function copyQuickCommand() {
    var command = document.getElementById('quick_command').textContent;
    var btn = document.getElementById('copy_cmd_btn');

    navigator.clipboard.writeText(command).then(function () {
        showToast(t('copied'), 'success');
        btn.classList.add('copied');
        btn.textContent = '✓';

        setTimeout(function () {
            btn.classList.remove('copied');
            btn.textContent = t('copy');
        }, 2000);

        // GA4: Track command copy
        if (typeof gtag === 'function') {
            gtag('event', 'copy_quick_command', {
                'event_category': 'conversion',
                'event_label': currentOS
            });
        }
    });
}

// ========================================
// Chart setup
// ========================================
var hashrateHistory = [];
var maxHistoryPoints = 30;
var hashrateChart = null;

function initChart() {
    var chartCanvas = document.getElementById('hashrateChart');
    if (!chartCanvas) return;

    var observer = new IntersectionObserver(function (entries) {
        if (entries[0].isIntersecting) {
            loadChartLibrary();
            observer.disconnect();
        }
    });
    observer.observe(chartCanvas);
}

function loadChartLibrary() {
    var script = document.createElement('script');
    script.src = 'https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js';
    script.onload = createChartInternal;
    document.body.appendChild(script);
}

function createChartInternal() {
    var ctx = document.getElementById('hashrateChart').getContext('2d');

    // Create gradient
    var gradient = ctx.createLinearGradient(0, 0, 0, 400);
    gradient.addColorStop(0, 'rgba(242, 104, 34, 0.5)'); // Brand Orange
    gradient.addColorStop(1, 'rgba(242, 104, 34, 0.0)');

    hashrateChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Pool Hashrate',
                data: [],
                borderColor: '#F26822',
                backgroundColor: gradient,
                borderWidth: 2,
                fill: true,
                tension: 0.4,
                pointRadius: 0,
                pointHoverRadius: 6
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { display: false },
                tooltip: {
                    mode: 'index',
                    intersect: false,
                    backgroundColor: 'rgba(20, 20, 20, 0.9)',
                    titleColor: '#fff',
                    bodyColor: '#ccc',
                    borderColor: 'rgba(255,255,255,0.1)',
                    borderWidth: 1,
                    padding: 10,
                    callbacks: {
                        label: function (context) {
                            return 'Hashrate: ' + formatHashrate(context.parsed.y);
                        }
                    }
                }
            },
            scales: {
                x: {
                    display: false,
                    grid: { display: false }
                },
                y: {
                    display: true,
                    position: 'right',
                    grid: {
                        color: 'rgba(255, 255, 255, 0.05)',
                        drawBorder: false
                    },
                    ticks: {
                        color: '#666',
                        font: { size: 10 },
                        callback: function (value) {
                            return formatHashrate(value);
                        }
                    }
                }
            },
            interaction: {
                mode: 'nearest',
                axis: 'x',
                intersect: false
            }
        }
    });
}

function updateChart(hashrate) {
    if (!hashrateChart) return;
    var now = new Date();
    var timeLabel = now.getHours().toString().padStart(2, '0') + ':' +
        now.getMinutes().toString().padStart(2, '0') + ':' +
        now.getSeconds().toString().padStart(2, '0');

    hashrateHistory.push({ time: timeLabel, value: hashrate });

    if (hashrateHistory.length > maxHistoryPoints) {
        hashrateHistory.shift();
    }

    hashrateChart.data.labels = hashrateHistory.map(function (d) { return d.time; });
    hashrateChart.data.datasets[0].data = hashrateHistory.map(function (d) { return d.value; });
    hashrateChart.update('none');
}

// ========================================
// Stats fetching
// ========================================
var lastStats = null;
var refreshInterval = 10000; // 10 seconds
var countdown = 10;

function fetchStats() {
    var indicator = document.getElementById('refresh_indicator');
    indicator.classList.add('active');

    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        indicator.classList.remove('active');

        try {
            var s = JSON.parse(xhr.responseText);
            lastStats = s;
            updatePage(s);
        } catch (e) {
            console.error('Error parsing stats:', e);
        }
    };

    xhr.onerror = function () {
        indicator.classList.remove('active');
        console.error('Error fetching stats');
    };

    xhr.open('GET', '/stats');
    xhr.send();
}

function safelyUpdateText(id, text, isHTML) {
    var el = document.getElementById(id);
    if (!el) return;
    el.classList.remove('skeleton', 'skeleton-text');
    el.style.width = ''; // Remove fixed width from skeleton
    el.style.height = '';
    if (isHTML) {
        el.innerHTML = text;
    } else {
        el.textContent = text;
    }
}

function updatePage(s) {
    // Pool hashrate - hide if empty or very low
    var hashrateContainer = document.getElementById('pool_hashrate_container');
    if (hashrateContainer) {
        if (s.pool_hashrate < 100) {
            hashrateContainer.classList.add('hidden');
        } else {
            hashrateContainer.classList.remove('hidden');
        }
    }
    safelyUpdateText('pool_hashrate', formatHashrate(s.pool_hashrate), true);


    // Network stats
    safelyUpdateText('network_hashrate', formatHashrate(s.network_hashrate), true);
    safelyUpdateText('network_height', s.network_height.toLocaleString(), true);
    safelyUpdateText('connected_miners', s.connected_miners, true);
    safelyUpdateText('connected_miners_card', s.connected_miners, true);
    safelyUpdateText('pool_blocks_found', s.pool_blocks_found, true);
    safelyUpdateText('last_block_found', formatTime(s.last_block_found), true);
    safelyUpdateText('pool_fee', (s.pool_fee * 100).toFixed(1) + '%', true);
    safelyUpdateText('payment_threshold', s.payment_threshold + ' XMR', true);
    safelyUpdateText('pool_port', s.pool_port, true);
    safelyUpdateText('pool_ssl_port', s.pool_ssl_port || '--', true);
    safelyUpdateText('last_template_fetched', formatTime(s.last_template_fetched), true);

    // Round progress & luck
    var pct = (s.round_hashes / s.network_difficulty) * 100;
    safelyUpdateText('round_hashes', formatHashes(s.round_hashes) + ' (' + pct.toFixed(2) + '%)', true);
    document.getElementById('round_progress').style.width = Math.min(pct, 100) + '%';

    // Luck indicator
    var luckEl = document.getElementById('round_luck');
    var luckText = pct.toFixed(1) + '%';
    luckEl.textContent = luckText;
    luckEl.className = 'luck-indicator';
    if (pct < 80) {
        luckEl.classList.add('luck-good');
    } else if (pct < 150) {
        luckEl.classList.add('luck-normal');
    } else {
        luckEl.classList.add('luck-bad');
    }

    // Estimated time to block
    if (s.pool_hashrate > 0) {
        var remainingHashes = s.network_difficulty - s.round_hashes;
        if (remainingHashes < 0) remainingHashes = s.network_difficulty;
        var estSeconds = remainingHashes / s.pool_hashrate;
        safelyUpdateText('est_block_time', formatDuration(estSeconds), true);
    } else {
        safelyUpdateText('est_block_time', '--', true);
    }

    // Self-select
    if (s.allow_self_select) {
        document.getElementById('self_select_row').style.display = 'block';
        safelyUpdateText('allow_self_select', t('yes'), true);
    }

    // Stratum URL
    var host = window.location.hostname || 'euroxmr.eu';
    safelyUpdateText('stratum_url', 'stratum+tcp://' + host + ':' + s.pool_port, true);

    // Miner stats - also show if worker_count > 0 (for downstream workers)
    if (s.miner_hashrate > 0 || s.miner_balance > 0 || s.worker_count > 0) {
        document.getElementById('miner_stats').classList.remove('hidden');
        safelyUpdateText('miner_hashrate', formatHashrate(s.miner_hashrate), true);
        safelyUpdateText('miner_balance', s.miner_balance.toFixed(8) + ' XMR', true);
        safelyUpdateText('worker_count', s.worker_count || 0, true);

        // Estimated daily earnings (correct calculation based on network share)
        if (s.miner_hashrate > 0 && s.network_hashrate > 0) {
            var blocksPerDay = 720; // ~120s per block = 720 blocks/day
            var blockReward = 0.6; // approximate current block reward
            var networkShare = s.miner_hashrate / s.network_hashrate;
            var dailyXMR = networkShare * blocksPerDay * blockReward * (1 - s.pool_fee);
            safelyUpdateText('est_earnings', dailyXMR.toFixed(6) + ' XMR', true);
        } else {
            safelyUpdateText('est_earnings', '--', true);
        }

        // Fetch workers
        fetchWorkers();
    }

    // Update chart
    updateChart(s.pool_hashrate);

    // Update trust stats section
    updateTrustStats({
        pool_stats: {
            connected_miners: s.connected_miners,
            pool_blocks_found: s.pool_blocks_found
        },
        network_stats: {
            difficulty: s.network_difficulty
        }
    });

    // Update Social Proof bar in Hero section
    safelyUpdateText('blocks_found_hero', s.pool_blocks_found, true);
    safelyUpdateText('last_block_hero', formatTime(s.last_block_found), true);
    if (s.total_paid !== undefined) {
        safelyUpdateText('total_paid_hero', s.total_paid.toFixed(2), true);
    }
}

function fetchWorkers() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        var container = document.getElementById('workers_container');
        try {
            var workers = JSON.parse(xhr.responseText);

            if (workers.length === 0) {
                container.innerHTML = '<div class="no-workers">' + t('no_workers') + '</div>';
                return;
            }

            var html = '<table class="workers-table"><thead><tr>' +
                '<th>' + t('rig_id') + '</th>' +
                '<th>' + t('hashrate_10m') + '</th>' +
                '<th>' + t('status') + '</th>' +
                '</tr></thead><tbody>';

            for (var i = 0; i < workers.length; i += 2) {
                var rigId = workers[i] || 'default';
                var hashrate = workers[i + 1] || 0;
                html += '<tr>' +
                    '<td>' + rigId + '</td>' +
                    '<td>' + formatHashrate(hashrate) + '</td>' +
                    '<td><span class="worker-status">' + t('status_online') + '</span></td>' +
                    '</tr>';
            }

            html += '</tbody></table>';
            container.innerHTML = html;

        } catch (e) {
            container.innerHTML = '<div class="no-workers">' + t('err_workers') + '</div>';
        }
    };
    xhr.open('GET', '/workers');
    xhr.send();
}

// ========================================
// Countdown timer
// ========================================
function updateCountdown() {
    countdown--;

    // Animate ring
    var ring = document.getElementById('refresh_timer_ring');
    if (ring) {
        var offset = ((10 - countdown) / 10) * 100;
        ring.style.strokeDashoffset = offset;
    }

    if (countdown <= 0) {
        countdown = 10;
        if (ring) ring.style.strokeDashoffset = 0;
        fetchStats();
    }
    // document.getElementById('refresh_countdown').textContent = countdown; // Removed text countdown
}

// ========================================
// Fetch blocks history
// ========================================
function fetchBlocks() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        var container = document.getElementById('blocks_container');
        try {
            var blocks = JSON.parse(xhr.responseText);

            if (blocks.length === 0) {
                container.innerHTML = '<div class="no-workers">' + t('no_payments') + '</div>';
                return;
            }

            var statusMap = {
                0: { text: t('status_locked'), class: 'locked' },
                1: { text: t('status_unlocked'), class: 'unlocked' },
                2: { text: t('status_orphaned'), class: 'orphaned' }
            };

            var html = '<table class="blocks-table"><thead><tr>' +
                '<th>' + t('block_height') + '</th>' +
                '<th>Hash</th>' +
                '<th>' + t('status') + '</th>' +
                '<th>Reward</th>' +
                '<th>' + t('found') + '</th>' +
                '</tr></thead><tbody>';

            for (var i = 0; i < blocks.length; i++) {
                var block = blocks[i];
                var status = statusMap[block.status] || { text: 'Unknown', class: 'locked' };
                var reward = (block.reward / 1e12).toFixed(4);
                html += '<tr>' +
                    '<td>' + block.height.toLocaleString() + '</td>' +
                    '<td><span class="block-hash" title="' + block.hash + '">' + block.hash.substring(0, 16) + '...</span></td>' +
                    '<td><span class="block-status ' + status.class + '">' + status.text + '</span></td>' +
                    '<td>' + reward + ' XMR</td>' +
                    '<td>' + formatTime(block.timestamp) + '</td>' +
                    '</tr>';
            }

            html += '</tbody></table>';
            container.innerHTML = html;

        } catch (e) {
            container.innerHTML = '<div class="no-workers">' + t('loading_error') + '</div>';
        }
    };
    xhr.open('GET', '/blocks');
    xhr.send();
}

// ========================================
// Fetch payments history
// ========================================
function fetchPayments() {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        var container = document.getElementById('payments_container');
        try {
            var payments = JSON.parse(xhr.responseText);

            if (payments.length === 0) {
                container.innerHTML = '<div class="no-workers">' + t('no_payments') + '</div>';
                return;
            }

            var html = '<table class="payments-table"><thead><tr>' +
                '<th>Amount</th>' +
                '<th>Date</th>' +
                '</tr></thead><tbody>';

            for (var i = 0; i < payments.length; i++) {
                var payment = payments[i];
                var amount = (payment.amount / 1e12).toFixed(8);
                var date = new Date(payment.timestamp * 1000);
                var dateStr = date.toLocaleDateString() + ' ' + date.toLocaleTimeString();
                html += '<tr>' +
                    '<td><span class="payment-amount">' + amount + ' XMR</span></td>' +
                    '<td>' + dateStr + '</td>' +
                    '</tr>';
            }

            html += '</tbody></table>';
            container.innerHTML = html;

        } catch (e) {
            container.innerHTML = '<div class="no-workers">' + t('err_payments') + '</div>';
        }
    };
    xhr.open('GET', '/payments');
    xhr.send();
}

// ========================================
// Wallet lookup
// ========================================
var addressEl = document.getElementById('address');
var lookupBtn = document.getElementById('lookup_btn');

lookupBtn.onclick = function () {
    var wa = addressEl.value.trim();
    if (wa && /^[48][1-9A-HJ-NP-Za-km-z]{94,105}$/.test(wa)) {
        setCookie('wa', wa);
        document.getElementById('miner_stats').classList.remove('hidden');
        showToast(t('wallet_saved'), 'success');
        // GA4: Track wallet lookup (miner checking stats)
        if (typeof gtag === 'function') {
            gtag('event', 'wallet_lookup', {
                'event_category': 'engagement'
            });
        }
        setTimeout(function () {
            fetchStats();
            fetchPayments();
        }, 500);
    } else {
        showToast(t('invalid_addr'));
    }
};

addressEl.onkeypress = function (e) {
    if (e.key === 'Enter') lookupBtn.onclick();
};

// Load saved wallet
var savedWa = getCookie('wa');
if (savedWa) {
    addressEl.value = savedWa;
}

// ========================================
// Initialize
// ========================================
// ========================================
// Initialize
// ========================================
window.onload = function () {
    // Init lang
    var savedLang = localStorage.getItem('user_lang') || 'en';
    setLanguage(savedLang);

    initChart();
    fetchStats();
    fetchBlocks();
    if (getCookie('wa')) {
        fetchPayments();
    }
    setInterval(updateCountdown, 1000);
    // Refresh blocks every 5 minutes
    setInterval(fetchBlocks, 300000);

    // Initialize calculator
    initCalculator();
    // Initialize config generator
    initConfigGenerator();
    // Initialize lottery
    initLottery();

    // Initialize Scroll Reveal
    initScrollReveal();

    // Auto-detect OS for Hero
    initHeroOS();
};

// ========================================
// Scroll Reveal
// ========================================
function initScrollReveal() {
    var observerOptions = {
        root: null,
        rootMargin: '0px',
        threshold: 0.1
    };

    var observer = new IntersectionObserver(function (entries, observer) {
        entries.forEach(function (entry) {
            if (entry.isIntersecting) {
                entry.target.classList.add('is-visible');
                observer.unobserve(entry.target); // Only animate once
            }
        });
    }, observerOptions);

    var elements = document.querySelectorAll('.reveal-on-scroll');
    elements.forEach(function (el) {
        observer.observe(el);
    });
}


// ========================================
// Calculator
// ========================================
let xmrPrice = 150; // Default EUR, will be updated
let xmrPriceUsd = 160; // Default USD, will be updated
let networkDifficulty = 400000000000; // Default

async function fetchXMRPrice() {
    // Try CoinGecko first
    try {
        const res = await fetch('https://api.coingecko.com/api/v3/simple/price?ids=monero&vs_currencies=eur,usd', {
            headers: { 'Accept': 'application/json' },
            cache: 'no-cache'
        });
        if (!res.ok) {
            throw new Error('CoinGecko API error: ' + res.status);
        }
        const data = await res.json();
        if (data.monero && data.monero.eur) {
            xmrPrice = data.monero.eur;
            xmrPriceUsd = data.monero.usd || xmrPrice * 1.07;
            console.log('XMR price updated from CoinGecko: €' + xmrPrice.toFixed(2));
            // Recalculate profitability with new price
            if (typeof calculateProfitability === 'function') {
                calculateProfitability();
            }
            return;
        }
    } catch (e) {
        console.log('CoinGecko failed:', e.message, '- trying fallback');
    }

    // Fallback to CryptoCompare
    try {
        const res = await fetch('https://min-api.cryptocompare.com/data/price?fsym=XMR&tsyms=EUR,USD', {
            headers: { 'Accept': 'application/json' },
            cache: 'no-cache'
        });
        if (res.ok) {
            const data = await res.json();
            if (data.EUR) {
                xmrPrice = data.EUR;
                xmrPriceUsd = data.USD || xmrPrice * 1.07;
                console.log('XMR price updated from CryptoCompare: €' + xmrPrice.toFixed(2));
                if (typeof calculateProfitability === 'function') {
                    calculateProfitability();
                }
                return;
            }
        }
    } catch (e) {
        console.log('CryptoCompare also failed:', e.message);
    }

    console.log('Using default XMR price: €' + xmrPrice);
}

// Refresh price every 5 minutes
setInterval(fetchXMRPrice, 300000);

function calculateProfitability() {
    const hashrate = parseFloat(document.getElementById('calc_hashrate_input').value) || 0;
    const power = parseFloat(document.getElementById('calc_power_input').value) || 0;
    const electricityCost = parseFloat(document.getElementById('calc_cost_input').value) || 0;

    // XMR per day = (hashrate / network_hashrate) * block_reward * blocks_per_day
    // Simplified: XMR per day ≈ (hashrate * 86400) / network_difficulty * block_reward
    const blockReward = 0.6; // Approximate current block reward
    const blocksPerDay = 720; // ~2 min per block

    // Use stored network difficulty if available
    const netDiff = networkDifficulty || 400000000000;
    const dailyXMR = (hashrate * 86400 * blockReward) / netDiff;
    const monthlyXMR = dailyXMR * 30;

    const dailyEUR = dailyXMR * xmrPrice;
    const monthlyEUR = monthlyXMR * xmrPrice;

    // Power cost per month
    const powerCostMonth = (power / 1000) * 24 * 30 * electricityCost;
    const netProfit = monthlyEUR - powerCostMonth;

    // Update UI
    document.getElementById('calc_daily_xmr').textContent = dailyXMR.toFixed(6) + ' XMR';
    document.getElementById('calc_daily_eur').textContent = '≈ €' + dailyEUR.toFixed(2);
    document.getElementById('calc_monthly_xmr').textContent = monthlyXMR.toFixed(4) + ' XMR';
    document.getElementById('calc_monthly_eur').textContent = '≈ €' + monthlyEUR.toFixed(2);
    document.getElementById('calc_profit').textContent = (netProfit >= 0 ? '€' : '-€') + Math.abs(netProfit).toFixed(2);
    document.getElementById('calc_profit').style.color = netProfit >= 0 ? 'var(--success)' : 'var(--danger)';
    document.getElementById('calc_power_cost').textContent = 'Power: €' + powerCostMonth.toFixed(2);
}

function initCalculator() {
    fetchXMRPrice();
    calculateProfitability();

    ['calc_hashrate_input', 'calc_power_input', 'calc_cost_input'].forEach(id => {
        const el = document.getElementById(id);
        if (el) {
            el.addEventListener('input', calculateProfitability);
        }
    });
}

// ========================================
// XMRig Config Generator
// ========================================
function updateConfig() {
    const wallet = document.getElementById('config_wallet').value || 'YOUR_WALLET_ADDRESS';
    const worker = document.getElementById('config_worker').value || 'rig1';
    const useSSL = document.getElementById('config_ssl').value === 'true';

    const config = {
        autosave: true,
        cpu: true,
        opencl: false,
        cuda: false,
        pools: [
            {
                url: useSSL ? 'euroxmr.eu:4343' : 'euroxmr.eu:4242',
                user: wallet,
                pass: worker,
                "rig-id": worker,
                coin: "monero",
                keepalive: true,
                tls: useSSL
            }
        ]
    };

    document.getElementById('config_output').textContent = JSON.stringify(config, null, 2);
}

function copyConfig() {
    const configText = document.getElementById('config_output').textContent;
    navigator.clipboard.writeText(configText).then(() => {
        showToast(t('copied'), 'success');
        // GA4: Track config copy (conversion event)
        if (typeof gtag === 'function') {
            gtag('event', 'copy_config', {
                'event_category': 'conversion'
            });
        }
    });
}

function initConfigGenerator() {
    updateConfig();
    ['config_wallet', 'config_worker', 'config_ssl'].forEach(id => {
        const el = document.getElementById(id);
        if (el) {
            el.addEventListener('input', updateConfig);
            el.addEventListener('change', updateConfig);
        }
    });

    // Add wallet validation
    const walletInput = document.getElementById('config_wallet');
    if (walletInput) {
        walletInput.addEventListener('input', validateWalletAddress);
    }

    // Detect OS and update XMRig download link
    detectAndUpdateOS();
}

// ========================================
// Wallet Address Validation
// ========================================
function validateWalletAddress() {
    const input = document.getElementById('config_wallet');
    const icon = document.getElementById('wallet_validation_icon');
    const msg = document.getElementById('wallet_validation_msg');
    const value = input.value.trim();

    if (!value) {
        input.classList.remove('valid', 'invalid');
        icon.classList.remove('valid', 'invalid');
        msg.textContent = '';
        msg.className = 'validation-message';
        return;
    }

    // Monero address validation: starts with 4 or 8, 95 chars (standard) or 106 chars (integrated)
    const isValid = /^[48][0-9AB][1-9A-HJ-NP-Za-km-z]{93}$/.test(value) ||
        /^[48][0-9AB][1-9A-HJ-NP-Za-km-z]{104}$/.test(value);

    if (isValid) {
        input.classList.add('valid');
        input.classList.remove('invalid');
        icon.classList.add('valid');
        icon.classList.remove('invalid');
        msg.textContent = t('valid_address') || '✓ Valid Monero address';
        msg.className = 'validation-message valid';
    } else {
        input.classList.add('invalid');
        input.classList.remove('valid');
        icon.classList.add('invalid');
        icon.classList.remove('valid');
        msg.textContent = t('invalid_address_format') || 'Invalid address format';
        msg.className = 'validation-message invalid';
    }
}

// ========================================
// OS Detection
// ========================================
function detectAndUpdateOS() {
    const userAgent = navigator.userAgent.toLowerCase();
    const osIcon = document.getElementById('os_icon');
    const osName = document.getElementById('os_name');
    const downloadBtn = document.getElementById('xmrig_download');

    if (!osIcon || !osName) return;

    let os = 'unknown';
    let icon = '💻';

    if (userAgent.includes('win')) {
        os = 'Windows';
        icon = '🪟';
    } else if (userAgent.includes('mac')) {
        os = 'macOS';
        icon = '🍎';
    } else if (userAgent.includes('linux')) {
        os = 'Linux';
        icon = '🐧';
    } else if (userAgent.includes('android')) {
        os = 'Android';
        icon = '🤖';
    }

    osIcon.textContent = icon;
    osName.textContent = os;

    // Update download link based on OS
    if (downloadBtn) {
        const baseUrl = 'https://github.com/xmrig/xmrig/releases/latest';
        downloadBtn.href = baseUrl;
    }
}

// ========================================
// Config Download
// ========================================
function downloadConfig() {
    const config = document.getElementById('config_output').textContent;
    const blob = new Blob([config], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'config.json';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    // GA4: Track config download (conversion event)
    if (typeof gtag === 'function') {
        gtag('event', 'download_config', {
            'event_category': 'conversion'
        });
    }
    showToast(t('config_downloaded') || 'config.json downloaded!', 'success');
}

// ========================================
// FAQ Accordion
// ========================================
function toggleFaq(btn) {
    const item = btn.parentElement;
    const wasActive = item.classList.contains('active');

    // Close all
    document.querySelectorAll('.faq-item').forEach(i => i.classList.remove('active'));

    // Open clicked if it wasn't active
    if (!wasActive) {
        item.classList.add('active');
    }
}

// ========================================
// Update Trust Stats from pool data
// ========================================
function updateTrustStats(stats) {
    if (stats) {
        // Update miners count
        const trustMiners = document.getElementById('trust_miners');
        if (trustMiners && stats.pool_stats && stats.pool_stats.connected_miners !== undefined) {
            trustMiners.textContent = stats.pool_stats.connected_miners;
        }

        // Update blocks found
        const trustBlocks = document.getElementById('trust_blocks');
        if (trustBlocks && stats.pool_stats && stats.pool_stats.pool_blocks_found !== undefined) {
            trustBlocks.textContent = stats.pool_stats.pool_blocks_found;
        }

        // Update network difficulty for calculator
        if (stats.network_stats && stats.network_stats.difficulty) {
            networkDifficulty = stats.network_stats.difficulty;
            calculateProfitability();
        }
    }
}

// ========================================
// Particles.js Initialization
// ========================================
if (typeof particlesJS !== 'undefined') {
    particlesJS('particles-js', {
        particles: {
            number: {
                value: 40,
                density: {
                    enable: true,
                    value_area: 1000
                }
            },
            color: {
                value: ['#F26822', '#ffffff']
            },
            shape: {
                type: 'circle'
            },
            opacity: {
                value: 0.2,
                random: true,
                anim: {
                    enable: true,
                    speed: 0.3,
                    opacity_min: 0.05,
                    sync: false
                }
            },
            size: {
                value: 2,
                random: true,
                anim: {
                    enable: true,
                    speed: 1,
                    size_min: 0.1,
                    sync: false
                }
            },
            line_linked: {
                enable: true,
                distance: 150,
                color: '#F26822',
                opacity: 0.08,
                width: 1
            },
            move: {
                enable: true,
                speed: 1,
                direction: 'none',
                random: true,
                straight: false,
                out_mode: 'out',
                bounce: false,
                attract: {
                    enable: true,
                    rotateX: 600,
                    rotateY: 1200
                }
            }
        },
        interactivity: {
            detect_on: 'canvas',
            events: {
                onhover: {
                    enable: true,
                    mode: 'grab'
                },
                onclick: {
                    enable: true,
                    mode: 'push'
                },
                resize: true
            },
            modes: {
                grab: {
                    distance: 140,
                    line_linked: {
                        opacity: 0.5
                    }
                },
                push: {
                    particles_nb: 3
                }
            }
        },
        retina_detect: true
    });
}

// ========================================
// Lottery Functions
// ========================================
function initLottery() {
    updateLotteryCountdown();
    setInterval(updateLotteryCountdown, 1000);
    fetchLotteryStats();
    // Refresh lottery stats every 5 minutes
    setInterval(fetchLotteryStats, 300000);
}

function getNextSundayDraw() {
    const now = new Date();
    const daysUntilSunday = (7 - now.getDay()) % 7;
    const nextSunday = new Date(now);

    if (daysUntilSunday === 0 && now.getHours() >= 20) {
        // It's Sunday after 20:00, next draw is next week
        nextSunday.setDate(now.getDate() + 7);
    } else if (daysUntilSunday === 0) {
        // It's Sunday before 20:00
        // Keep the same date
    } else {
        nextSunday.setDate(now.getDate() + daysUntilSunday);
    }

    nextSunday.setHours(20, 0, 0, 0);
    return nextSunday;
}

function updateLotteryCountdown() {
    const nextDraw = getNextSundayDraw();
    const now = new Date();
    const diff = nextDraw - now;

    if (diff <= 0) {
        document.getElementById('lottery_countdown').textContent = t('lottery_drawing') || 'Drawing now!';
        return;
    }

    const days = Math.floor(diff / (1000 * 60 * 60 * 24));
    const hours = Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
    const minutes = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60));
    const seconds = Math.floor((diff % (1000 * 60)) / 1000);

    let countdownText = '';
    if (days > 0) countdownText += days + 'd ';
    countdownText += hours.toString().padStart(2, '0') + ':';
    countdownText += minutes.toString().padStart(2, '0') + ':';
    countdownText += seconds.toString().padStart(2, '0');

    const countdownEl = document.getElementById('lottery_countdown');
    if (countdownEl) {
        countdownEl.textContent = countdownText;
    }

    // Also update Hero teaser countdown
    const teaserCountdownEl = document.getElementById('lottery_teaser_countdown');
    if (teaserCountdownEl) {
        teaserCountdownEl.textContent = countdownText;
    }
}

function fetchLotteryStats() {
    // Try to fetch lottery stats from the API
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        try {
            var data = JSON.parse(xhr.responseText);
            updateLotteryUI(data);
        } catch (e) {
            console.log('Lottery stats not available yet');
        }
    };
    xhr.onerror = function () {
        console.log('Could not fetch lottery stats');
    };
    // API endpoint for lottery stats (generated by lottery_api.py)
    xhr.open('GET', '/lottery_stats.json', true);
    xhr.send();

    // Also fetch lottery results (winners history)
    var xhrResults = new XMLHttpRequest();
    xhrResults.onreadystatechange = function () {
        if (xhrResults.readyState === 4) {
            if (xhrResults.status === 200) {
                try {
                    var data = JSON.parse(xhrResults.responseText);
                    console.log('Lottery results loaded:', data);
                    updateLotteryWinners(data);
                } catch (e) {
                    console.error('Error parsing lottery results:', e);
                }
            } else {
                console.log('Lottery results HTTP status:', xhrResults.status);
            }
        }
    };
    xhrResults.open('GET', '/lottery_results.json', true);
    xhrResults.send();
}

function updateLotteryUI(data) {
    if (!data) return;

    // Update prize dynamically from lottery_stats.json
    if (data.prize_xmr !== undefined) {
        var prizeText = data.prize_xmr + ' XMR';
        var prizeEl = document.getElementById('lottery_prize');
        if (prizeEl) {
            prizeEl.textContent = prizeText;
        }
        // Also update hero teaser
        var teaserPrize = document.getElementById('lottery_teaser_prize');
        if (teaserPrize) {
            teaserPrize.textContent = prizeText;
        }
        console.log('Lottery prize updated dynamically:', prizeText);
    }

    // Update total tickets
    if (data.stats) {
        document.getElementById('lottery_total_tickets').textContent =
            data.stats.total_tickets ? data.stats.total_tickets.toLocaleString() : '--';

        const participantsEl = document.getElementById('lottery_participants');
        if (participantsEl && data.stats.total_participants) {
            participantsEl.textContent = data.stats.total_participants + ' participants';
        }
    }

    // Update user probability if wallet is saved
    var savedWallet = getCookie('wa');
    if (savedWallet && data.participants) {
        // Generate short address from saved wallet for comparison
        // (server now filters full addresses for privacy)
        var savedWalletShort = savedWallet.substring(0, 4) + '...' + savedWallet.slice(-4);
        var userStats = data.participants.find(p => p.address_short === savedWalletShort);
        if (userStats) {
            document.getElementById('lottery_user_probability').textContent =
                userStats.probability.toFixed(2) + '%';
            document.getElementById('lottery_user_shares').textContent =
                userStats.shares.toLocaleString() + ' shares';
        } else {
            document.getElementById('lottery_user_probability').textContent = '0%';
            document.getElementById('lottery_user_shares').textContent = 'Not participating';
        }
    } else {
        document.getElementById('lottery_user_probability').textContent = '--';
        document.getElementById('lottery_user_shares').textContent = t('enter_wallet') || 'Enter wallet above';
    }
}

function updateLotteryWinners(data) {
    if (!data) {
        console.log('updateLotteryWinners: no data');
        return;
    }

    console.log('updateLotteryWinners called with history length:', data.history ? data.history.length : 0);

    // Update winners history from lottery_results.json
    if (data.history && data.history.length > 0) {
        var tbody = document.getElementById('lottery_winners_body');
        if (!tbody) {
            console.error('lottery_winners_body element not found!');
            return;
        }

        var html = '';

        data.history.slice(0, 5).forEach(function (draw) {
            var date = new Date(draw.draw_date).toLocaleDateString();
            var winner = draw.winner ? draw.winner.address_short : '--';
            var amount = draw.prize_xmr + ' XMR';
            var txid = draw.txid ?
                '<a href="https://xmrchain.net/tx/' + draw.txid + '" target="_blank">' +
                draw.txid.substring(0, 8) + '...</a>' :
                '<span style="color: var(--text-muted)">Pending</span>';

            html += '<tr>' +
                '<td>' + date + '</td>' +
                '<td class="lottery-winner-address">' + winner + '</td>' +
                '<td class="lottery-winner-amount">' + amount + '</td>' +
                '<td class="lottery-txid">' + txid + '</td>' +
                '</tr>';
        });

        console.log('Setting winners HTML:', html);
        tbody.innerHTML = html;
    } else {
        console.log('No history to display');
    }
}
