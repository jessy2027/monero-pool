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

#ifndef COMPAT_H
#define COMPAT_H

/*
 * Cross-Platform Compatibility Layer
 * ===================================
 * Provides platform abstraction for:
 * - Signal handling
 * - Process management
 * - Socket operations
 * - Thread synchronization
 * - System information
 *
 * Currently supports:
 * - Linux
 * - macOS/Darwin
 * - Windows (via MinGW or MSVC) [partial]
 */

#if defined(_WIN32) || defined(_WIN64)
    #define POOL_PLATFORM_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define POOL_PLATFORM_DARWIN 1
#elif defined(__linux__)
    #define POOL_PLATFORM_LINUX 1
#else
    #define POOL_PLATFORM_UNKNOWN 1
#endif

/* ============================================================================
 * Platform-specific includes
 * ========================================================================= */

#ifdef POOL_PLATFORM_WINDOWS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>

/* Windows doesn't have these POSIX headers */
#define SIGUSR1 10
#define SIGPIPE 13

/* Redefine incompatible functions */
#define sleep(x) Sleep((x) * 1000)
#define usleep(x) Sleep((x) / 1000)
#define getpid() _getpid()

/* Socket compatibility */
typedef SOCKET socket_t;
#define INVALID_SOCKET_VAL INVALID_SOCKET
#define close_socket(s) closesocket(s)
#define socket_error() WSAGetLastError()

/* Process compatibility */
typedef HANDLE pid_t;
#define fork() (-1)  /* fork not supported on Windows */

/* UUID compatibility */
#ifndef uuid_t
typedef unsigned char uuid_t[16];
#endif

static inline void uuid_generate(uuid_t uuid) {
    /* Use Windows UUID generation */
    CoCreateGuid((GUID*)uuid);
}

/* sysconf compatibility */
#define _SC_NPROCESSORS_ONLN 1

static inline long sysconf(int name) {
    if (name == _SC_NPROCESSORS_ONLN) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
    }
    return -1;
}

/* Attribute compatibility */
#define __attribute__(x)

#else /* POSIX platforms (Linux, macOS) */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <uuid/uuid.h>

/* Socket compatibility */
typedef int socket_t;
#define INVALID_SOCKET_VAL (-1)
#define close_socket(s) close(s)
#define socket_error() errno

#endif /* Platform selection */

/* ============================================================================
 * Common definitions
 * ========================================================================= */

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* 128-bit integer support */
#if defined(__GNUC__) || defined(__clang__)
    typedef unsigned __int128 uint128_t;
#elif defined(_MSC_VER)
    /* MSVC doesn't have native 128-bit integers */
    typedef struct {
        uint64_t low;
        uint64_t high;
    } uint128_t;
    
    static inline int uint128_eq(uint128_t a, uint128_t b) {
        return a.low == b.low && a.high == b.high;
    }
#endif

/* ============================================================================
 * Thread-safe logging lock
 * ========================================================================= */

#ifdef POOL_PLATFORM_WINDOWS

#include <windows.h>

typedef CRITICAL_SECTION compat_mutex_t;

static inline void compat_mutex_init(compat_mutex_t *mutex) {
    InitializeCriticalSection(mutex);
}

static inline void compat_mutex_lock(compat_mutex_t *mutex) {
    EnterCriticalSection(mutex);
}

static inline void compat_mutex_unlock(compat_mutex_t *mutex) {
    LeaveCriticalSection(mutex);
}

static inline void compat_mutex_destroy(compat_mutex_t *mutex) {
    DeleteCriticalSection(mutex);
}

#else /* POSIX */

#include <pthread.h>

typedef pthread_mutex_t compat_mutex_t;

static inline void compat_mutex_init(compat_mutex_t *mutex) {
    pthread_mutex_init(mutex, NULL);
}

static inline void compat_mutex_lock(compat_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
}

static inline void compat_mutex_unlock(compat_mutex_t *mutex) {
    pthread_mutex_unlock(mutex);
}

static inline void compat_mutex_destroy(compat_mutex_t *mutex) {
    pthread_mutex_destroy(mutex);
}

#endif /* Platform mutex */

/* ============================================================================
 * Signal handling abstraction
 * ========================================================================= */

typedef void (*signal_handler_t)(int);

#ifdef POOL_PLATFORM_WINDOWS

/* Windows signal simulation using console handler */
static signal_handler_t _compat_sigint_handler = NULL;
static signal_handler_t _compat_sigterm_handler = NULL;

static inline BOOL WINAPI _compat_console_handler(DWORD signal) {
    switch (signal) {
        case CTRL_C_EVENT:
            if (_compat_sigint_handler) _compat_sigint_handler(SIGINT);
            return TRUE;
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            if (_compat_sigterm_handler) _compat_sigterm_handler(SIGTERM);
            return TRUE;
    }
    return FALSE;
}

static inline void compat_signal(int sig, signal_handler_t handler) {
    static int initialized = 0;
    if (!initialized) {
        SetConsoleCtrlHandler(_compat_console_handler, TRUE);
        initialized = 1;
    }
    
    switch (sig) {
        case SIGINT:
            _compat_sigint_handler = handler;
            break;
        case SIGTERM:
            _compat_sigterm_handler = handler;
            break;
        case SIGPIPE:
            /* Ignored on Windows - no SIGPIPE equivalent */
            break;
    }
}

#else /* POSIX */

static inline void compat_signal(int sig, signal_handler_t handler) {
    if (handler == (signal_handler_t)1) {  /* SIG_IGN */
        signal(sig, SIG_IGN);
    } else {
        signal(sig, handler);
    }
}

#endif /* Platform signal */

/* ============================================================================
 * Network initialization
 * ========================================================================= */

static inline int compat_network_init(void) {
#ifdef POOL_PLATFORM_WINDOWS
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data);
#else
    return 0;
#endif
}

static inline void compat_network_cleanup(void) {
#ifdef POOL_PLATFORM_WINDOWS
    WSACleanup();
#endif
}

/* ============================================================================
 * Time functions
 * ========================================================================= */

#ifdef POOL_PLATFORM_WINDOWS
#include <time.h>

/* Windows doesn't have a standard gettimeofday */
static inline int gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    if (tv) {
        FILETIME ft;
        ULARGE_INTEGER uli;
        
        GetSystemTimeAsFileTime(&ft);
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        
        /* Convert from 100-nanosecond intervals since 1601 to Unix epoch */
        uli.QuadPart -= 116444736000000000ULL;
        tv->tv_sec = (long)(uli.QuadPart / 10000000);
        tv->tv_usec = (long)((uli.QuadPart % 10000000) / 10);
    }
    return 0;
}

#endif /* Windows time */

/* ============================================================================
 * Path manipulation
 * ========================================================================= */

#ifdef POOL_PLATFORM_WINDOWS
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STR "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STR "/"
#endif

/* ============================================================================
 * Error message compatibility
 * ========================================================================= */

#ifdef POOL_PLATFORM_WINDOWS

static inline const char *compat_strerror(int errnum) {
    static char buffer[256];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errnum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer,
        sizeof(buffer),
        NULL
    );
    return buffer;
}

#else

#include <string.h>
#define compat_strerror(e) strerror(e)

#endif /* Platform strerror */

#endif /* COMPAT_H */

