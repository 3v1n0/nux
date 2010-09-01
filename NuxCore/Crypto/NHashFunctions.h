#ifndef NHASHFUNCTIONS_H
#define NHASHFUNCTIONS_H

NAMESPACE_BEGIN

unsigned int RSHash  (const NString& str);
unsigned int JSHash  (const NString& str);
unsigned int PJWHash (const NString& str);
unsigned int ELFHash (const NString& str);
unsigned int BKDRHash(const NString& str);
unsigned int SDBMHash(const NString& str);
unsigned int DJBHash (const NString& str);
unsigned int DEKHash (const NString& str);
unsigned int BPHash  (const NString& str);
unsigned int FNVHash (const NString& str);
unsigned int APHash  (const NString& str);

NAMESPACE_END

#endif // NHASHFUNCTIONS_H

