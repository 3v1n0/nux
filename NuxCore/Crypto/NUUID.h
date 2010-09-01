#ifndef NUUID_H
#define NUUID_H

NAMESPACE_BEGIN

#undef uuid_t

typedef t_ulong     unsigned32;
typedef t_u16    unsigned16;
typedef t_u8        unsigned8;
typedef t_u8        byte;

typedef struct {
    unsigned32  time_low;
    unsigned16  time_mid;
    unsigned16  time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} uuid_t;

INL_DECLSPEC_DLL int GenerateUUID1(uuid_t *uuid);
INL_DECLSPEC_DLL void GenerateUUID3(uuid_t *uuid, uuid_t nsid, void *name, int namelen);
INL_DECLSPEC_DLL void GenerateUUID5(uuid_t *uuid, uuid_t nsid, void *name, int namelen);

NAMESPACE_END

#endif // NUUID_H
