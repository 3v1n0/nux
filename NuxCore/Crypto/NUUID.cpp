/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "NKernel.h"
#include "NUUID.h"
#include "NMD5.h"
#include "NSHA1.h"
#include "NSHA2.h"

#ifdef _WIN32
    #include "Winsock2.h"
    #pragma comment(lib, "ws2_32")
#endif

#ifdef INL_PS3
    #include <netinet/in.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

NAMESPACE_BEGIN

#define MD5_Final MD5Final
#define MD5_Init MD5Init
#define MD5_Update MD5Update

#define SHA1_Final SHA1Result
#define SHA1_Init SHA1Reset
#define SHA1_Update SHA1Input

////////////////////////////////////////////////////////////////
//// Begin system dependent
//// system dependent call to get IEEE node ID.
//// This sample implementation generates a random node ID.
////////////////////////////////////////////////////////////////

// See: http://xml.resource.org/public/rfc/html/rfc4122.html


/* set the following to the number of 100ns ticks of the actual
resolution of your system's clock */
#define UUIDS_PER_TICK 1024

/* Set the following to a calls to get and release a global lock */
#define LOCK
#define UNLOCK


/* Set this to what your compiler uses for 64-bit data type */
#ifdef _WIN32
    #define unsigned64_t t_u64
    #define I64(C) C
#elif _WIN64
    #define unsigned64_t unsigned long long
    #define I64(C) C##LL
#elif INL_PS3
    #define unsigned64_t t_u64
    #define I64(C) C
#endif

typedef unsigned64_t uuid_time_t;
typedef struct {
    unsigned char nodeID[6];
} uuid_node_t;

void get_ieee_node_identifier(uuid_node_t *node);
void get_system_time(uuid_time_t *uuid_time);
void get_random_info(char seed[16]);


/* uuid_create -- generate a UUID version 1 */
int uuid_create(uuid_t * uuid);

/* 
    uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a
    "name" from a "name space"
*/
void uuid_create_md5_from_name(
                               uuid_t *uuid,         /* resulting UUID */
                               uuid_t nsid,          /* UUID of the namespace */
                               void *name,           /* the name from which to generate a UUID */
                               int namelen           /* the length of the name */
                               );

/*
    uuid_create_sha1_from_name -- create a version 5 (SHA-1) UUID
    using a "name" from a "name space"
*/
void uuid_create_sha1_from_name(

                                uuid_t *uuid,         /* resulting UUID */
                                uuid_t nsid,          /* UUID of the namespace */
                                void *name,           /* the name from which to generate a UUID */
                                int namelen           /* the length of the name */
                                );

/* uuid_compare --  Compare two UUID's "lexically" and return
    -1   u1 is lexically before u2
    0   u1 is equal to u2
    1   u1 is lexically after u2
    Note that lexical ordering is not temporal ordering!
*/
int uuid_compare(uuid_t *u1, uuid_t *u2);


void get_ieee_node_identifier(uuid_node_t *node)
{
    static int inited = 0;
    static uuid_node_t saved_node;
    char seed[16];
    FILE *fp = NULL;

    if (!inited)
    {
#if (defined INL_VISUAL_STUDIO_2005) || (defined INL_VISUAL_STUDIO_2008)
        fopen_s(&fp, "nodeid", "rb");
#else
        fp = fopen("nodeid", "rb");
#endif
        if (fp)
        {
            fread(&saved_node, sizeof(saved_node), 1, fp);
            fclose(fp);
        }
        else
        {
            get_random_info(seed);
            seed[0] |= 0x01;
            memcpy(&saved_node, seed, sizeof(saved_node));
#if (defined INL_VISUAL_STUDIO_2005) || (defined INL_VISUAL_STUDIO_2008)
            fopen_s(&fp, "nodeid", "wb");
#else
            fp = fopen("nodeid", "wb");
#endif
            if (fp)
            {
                fwrite(&saved_node, sizeof(saved_node), 1, fp);
                fclose(fp);
            }
        }
        inited = 1;
    }

    *node = saved_node;
}

/* system dependent call to get the current system time. Returned as
100ns ticks since UUID epoch, but resolution may be less than
100ns. */
#ifdef _WINDOWS_

void get_system_time(uuid_time_t *uuid_time)
{
    ULARGE_INTEGER time;

    /* NT keeps time in FILETIME format which is 100ns ticks since
    Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
    The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
    + 18 years and 5 leap days. */
    GetSystemTimeAsFileTime((FILETIME *)&time);
    time.QuadPart +=

        (unsigned __int64) (1000*1000*10)       // seconds
        * (unsigned __int64) (60 * 60 * 24)       // days
        * (unsigned __int64) (17+30+31+365*18+5); // # of days
    *uuid_time = time.QuadPart;
}

/* Sample code, not for use in production; see RFC 1750 */
void get_random_info(char seed[16])
{
    MD5_CTX c;
    struct {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
        TCHAR hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } r;

    MD5_Init(&c);
    GlobalMemoryStatus(&r.m);
    GetSystemInfo(&r.s);
    GetSystemTimeAsFileTime(&r.t);
    QueryPerformanceCounter(&r.pc);
    r.tc = GetTickCount();


    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.hostname, &r.l);
    MD5_Update(&c, (unsigned char*)&r, sizeof r);
    MD5_Final(&c);
    memcpy((void*)&(seed[0]), (void*)&(c.digest[0]), 16);
}

#else

void get_system_time(uuid_time_t *uuid_time)
{
#ifdef _WIN32
    struct timeval tp;

    gettimeofday(&tp, (struct timezone *)0);

    // Offset between UUID formatted times and Unix formatted times.  UUID UTC base time is October 15, 1582.
    // Unix base time is January 1, 1970.
    *uuid_time = ((unsigned64)tp.tv_sec * 10000000) + ((unsigned64)tp.tv_usec * 10) + I64(0x01B21DD213814000LL);
#else
    inlAssert(0);
#endif
}

/* Sample code, not for use in production; see RFC 1750 */
void get_random_info(char seed[16])
{
#ifdef _WIN32
    MD5_CTX c;
    struct {
        struct sysinfo s;
        struct timeval t;
        char hostname[257];
    } r;

    MD5_Init(&c);
    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);
    MD5_Update(&c, &r, sizeof r);
    MD5_Final(seed, &c);

#else
    inlAssert(0);
#endif
}

#endif

////////////////////////////////////////////////////////////////
//// End system dependent
////////////////////////////////////////////////////////////////

/* various forward declarations */
static int read_state(unsigned16 *clockseq, uuid_time_t *timestamp,
                      uuid_node_t *node);
static void write_state(unsigned16 clockseq, uuid_time_t timestamp,
                        uuid_node_t node);
static void format_uuid_v1(uuid_t *uuid, unsigned16 clockseq,
                           uuid_time_t timestamp, uuid_node_t node);

static void format_uuid_v3or5(uuid_t *uuid, unsigned char hash[16],
                              int v);
static void get_current_time(uuid_time_t *timestamp);
static unsigned16 true_random(void);

// uuid_create -- generator a UUID
// Using IEEE 802 MAC addresses as a source of uniqueness.
// UUID version 1 values are computed using the MAC address of the host.
int GenerateUUID1(uuid_t *uuid)
{
    uuid_time_t timestamp, last_time;
    unsigned16 clockseq;
    uuid_node_t node;
    uuid_node_t last_node;
    int f;

    // acquire system-wide lock so we're alone
    LOCK;
    // get time, node ID, saved state from non-volatile storage

    // The timestap is a 60 bit value, representing the number of 100 nanosecond intervals since 15 October 1582 00:00:000000000.
    get_current_time(&timestamp);
    get_ieee_node_identifier(&node);
    // The clock ID is a 14 bit value
    f = read_state(&clockseq, &last_time, &last_node);

    // if no NV (non-volatile) state, or if clock went backwards, or node ID changed (e.g., new network card) change clockseq
    if (!f || memcmp(&node, &last_node, sizeof(node)))
        clockseq = true_random();
    else if (timestamp < last_time)
        clockseq++;

    // save the state for next time
    write_state(clockseq, timestamp, node);

    UNLOCK;

    // stuff fields into the UUID
    format_uuid_v1(uuid, clockseq, timestamp, node);
    return 1;
}

// This class defines a UUID (Universally Unique IDentifier), also      
// known as GUIDs (Globally Unique IDentifier). A UUID is 128 bits      
// long, and if generated according to this algorithm, is either        
// guaranteed to be different from all other UUIDs/GUIDs generated      
// until 3400 A.D. or extremely likely to be different. UUIDs were      
// originally used in the Network Computing System (NCS) and            
// later in the Open Software Foundation's (OSF) Distributed Computing  
// Environment (DCE).                                                   
//                                                                      
// Structure of universal unique IDs (UUIDs).                           
//                                                                      
// Depending on the network data representation, the multi-             
// octet unsigned integer fields are subject to byte swapping           
// when communicated between dissimilar endian machines.                
//                                                                      

// Field                        Data Type                   Octet  Note
//
// time_low                     unsigned 32 bit integer     0-3    The low field of the timestamp
// time_mid                     unsigned 16 bit integer     4-5    The middle field of the timestamp
// time_hi_and_version          unsigned 16 bit integer     6-7    The high field of the timestamp multiplexed with the version number
// clock_seq_hi_and_reserved    unsigned 8 bit integer      8      The high field of the clock sequence multiplexed with the variant
// clock_seq_low                unsigned 8 bit integer      9      The low field of the clock sequence
// node                         unsigned 48 bit integer     10-15  The spatially unique node identifier

//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                          time_low                             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |       time_mid                |         time_hi_and_version   |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                         node (2-5)                            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// The version number is in the most significant 4 bits of the time_hi_and_version field.
//     Msb0  Msb1  Msb2  Msb3   Version Description
//     0     0     0     1        1     The time-based version specified in this document.
//     0     0     1     0        2     DCE Security version, with embedded POSIX UIDs.
//     0     0     1     1        3     The name-based version specified in this document that uses MD5 hashing.
//     0     1     0     0        4     The randomly or pseudo-randomly generated version specified in this document.
//     0     1     0     1        5     The name-based version specified in this document that uses SHA-1 hashing.


// format_uuid_v1 -- make a UUID from the timestamp, clockseq, and node ID
void format_uuid_v1(uuid_t* uuid, unsigned16 clock_seq, uuid_time_t timestamp, uuid_node_t node)
{
    // Construct a version 1 uuid with the information we've gathered plus a few constants.
    uuid->time_low              = (unsigned long)(timestamp & 0xFFFFFFFF);
    uuid->time_mid              = (unsigned short)((timestamp >> 32) & 0xFFFF);
    uuid->time_hi_and_version   = (unsigned short)((timestamp >> 48) & 0x0FFF);


    uuid->time_hi_and_version           |= (1 << 12);   
    uuid->clock_seq_low                 = clock_seq & 0xFF;
    uuid->clock_seq_hi_and_reserved     = (clock_seq & 0x3F00) >> 8;
    uuid->clock_seq_hi_and_reserved     |= 0x80;
    memcpy(&uuid->node, &node, sizeof(uuid->node));
}

/* data type for UUID generator persistent state */
typedef struct
{
    uuid_time_t  ts;       /* saved timestamp */
    uuid_node_t  node;     /* saved node ID */
    unsigned16   cs;       /* saved clock sequence */
} uuid_state;

static uuid_state st;

// read_state -- read UUID generator state from non-volatile store
int read_state(unsigned16 *clockseq, uuid_time_t *timestamp,
               uuid_node_t *node)
{
    static int inited = 0;
    FILE *fp;

    /* only need to read state once per boot */
    if (!inited)
    {
#if (defined INL_VISUAL_STUDIO_2005) || (defined INL_VISUAL_STUDIO_2008)
        fopen_s(&fp, "state", "rb");
#else
        fp = fopen("state", "rb");
#endif
        if (fp == NULL)
            return 0;
        fread(&st, sizeof st, 1, fp);
        fclose(fp);
        inited = 1;
    }
    *clockseq = st.cs;
    *timestamp = st.ts;
    *node = st.node;
    return 1;
}

// write_state -- save UUID generator state back to non-volatile storage
void write_state(unsigned16 clockseq, uuid_time_t timestamp,
                 uuid_node_t node)
{
    static int inited = 0;
    static uuid_time_t next_save;
    FILE* fp = NULL;
    if (!inited) {
        next_save = timestamp;
        inited = 1;
    }

    /* always save state to volatile shared state */
    st.cs = clockseq;
    st.ts = timestamp;
    st.node = node;
    if (timestamp >= next_save)
    {
#if (defined INL_VISUAL_STUDIO_2005) || (defined INL_VISUAL_STUDIO_2008)
        fopen_s(&fp, "state", "wb");
#else
        fp = fopen("state", "wb");
#endif
        fwrite(&st, sizeof st, 1, fp);
        fclose(fp);
        /* schedule next save for 10 seconds from now */
        next_save = timestamp + (10 * 10 * 1000 * 1000);
    }
}

// get-current_time -- get time as 60-bit 100ns ticks since UUID epoch.
// Compensate for the fact that real clock resolution is
// less than 100ns.
void get_current_time(uuid_time_t *timestamp)
{
    static int inited = 0;
    static uuid_time_t time_last;
    static unsigned16 uuids_this_tick;
    uuid_time_t time_now;

    if (!inited) {
        get_system_time(&time_now);
        uuids_this_tick = UUIDS_PER_TICK;
        inited = 1;
    }

    for ( ; ; ) {
        get_system_time(&time_now);

        /* if clock reading changed since last UUID generated, */
        if (time_last != time_now) {
            /* reset count of uuids gen'd with this clock reading */
            uuids_this_tick = 0;
            time_last = time_now;
            break;
        }
        if (uuids_this_tick < UUIDS_PER_TICK) {
            uuids_this_tick++;
            break;
        }
        /* going too fast for our clock; spin */
    }
    /* add the count of uuids to low order bits of the clock reading */
    *timestamp = time_now + uuids_this_tick;
}

// true_random -- generate a crypto-quality random number. This sample doesn't do that.
static unsigned16 true_random(void)
{
    static int inited = 0;
    uuid_time_t time_now;

    if (!inited) {
        get_system_time(&time_now);
        time_now = time_now / UUIDS_PER_TICK;
        srand((unsigned int)
            (((time_now >> 32) ^ time_now) & 0xffffffff));
        inited = 1;
    }

    return rand();
}

// create a version 3 (MD5) UUID using a "name" from a "name space"
void uuid_create_md5_from_name(uuid_t *uuid, uuid_t nsid, void *name, int namelen)
{
    MD5_CTX c;
    unsigned char hash[16];
    uuid_t net_nsid;

    /* put name space ID in network byte order so it hashes the same
    no matter what endian machine we're on */
    net_nsid = nsid;
    net_nsid.time_low = htonl(net_nsid.time_low);
    net_nsid.time_mid = htons(net_nsid.time_mid);
    net_nsid.time_hi_and_version = htons(net_nsid.time_hi_and_version);

    MD5_Init(&c);
    MD5_Update(&c, (unsigned char*)&net_nsid, sizeof net_nsid);
    MD5_Update(&c, (unsigned char*)name, namelen);
    MD5_Final(&c);
    memcpy((void*)&(hash[0]), (void*)&(c.digest[0]), 16);

    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, hash, 3);
}

void GenerateUUID3(uuid_t *uuid, uuid_t nsid, void *name, int namelen)
{
    uuid_create_md5_from_name(uuid, nsid, name, namelen);
}

#define SHA_CTX SHA1Context

//  format_uuid_v3or5 -- make a UUID from a (pseudo)random 128-bit number
void format_uuid_v3or5(uuid_t *uuid, unsigned char hash[16], int version)
{
    /* convert UUID to local byte order */
    memcpy(uuid, hash, sizeof *uuid);
    uuid->time_low = ntohl(uuid->time_low);
    uuid->time_mid = ntohs(uuid->time_mid);
    uuid->time_hi_and_version = ntohs(uuid->time_hi_and_version);

    /* put in the variant and version bits */
    uuid->time_hi_and_version &= 0x0FFF;
    uuid->time_hi_and_version |= (version << 12);
    uuid->clock_seq_hi_and_reserved &= 0x3F;
    uuid->clock_seq_hi_and_reserved |= 0x80;
}

// Create a version 5 (SHA-1) UUID using a "name" from a "name space"
void uuid_create_sha1_from_name(uuid_t *uuid, uuid_t nsid, void *name, int namelen)
{
    SHA_CTX c;
    unsigned char hash[20];
    uuid_t net_nsid;

    /* put name space ID in network byte order so it hashes the same
    no matter what endian machine we're on */
    net_nsid = nsid;
    net_nsid.time_low = htonl(net_nsid.time_low);
    net_nsid.time_mid = htons(net_nsid.time_mid);
    net_nsid.time_hi_and_version = htons(net_nsid.time_hi_and_version);

    SHA1_Init(&c);
    SHA1_Update(&c, (const unsigned char*)&net_nsid, sizeof net_nsid);
    SHA1_Update(&c, (const unsigned char*)name, namelen);
    SHA1_Final(/*hash,*/ &c);
    memcpy((void*)&(hash[0]), (void*)&(c.Message_Digest[0]), 16);

    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, hash, 5);
}

void GenerateUUID5(uuid_t *uuid, uuid_t nsid, void *name, int namelen)
{
    uuid_create_sha1_from_name(uuid, nsid, name, namelen);
}

// uuid_compare --  Compare two UUID's "lexically" and return 0 if they are identical
#define CHECK(f1, f2) if (f1 != f2) return f1 < f2 ? -1 : 1;
int uuid_compare(uuid_t *u1, uuid_t *u2)
{
    int i;

    CHECK(u1->time_low, u2->time_low);
    CHECK(u1->time_mid, u2->time_mid);
    CHECK(u1->time_hi_and_version, u2->time_hi_and_version);
    CHECK(u1->clock_seq_hi_and_reserved, u2->clock_seq_hi_and_reserved);
    CHECK(u1->clock_seq_low, u2->clock_seq_low)
    for (i = 0; i < 6; i++)
    {
        if (u1->node[i] < u2->node[i])
            return -1;
        if (u1->node[i] > u2->node[i])
            return 1;
    }
    return 0;
}
#undef CHECK

NAMESPACE_END

