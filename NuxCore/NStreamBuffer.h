#ifndef NSTREAMBUFFER_H
#define NSTREAMBUFFER_H

#include <streambuf>

NAMESPACE_BEGIN
class NStreamBuffer : public std::streambuf
{
public:
    NStreamBuffer(const BYTE *begin, const BYTE *end);
    explicit NStreamBuffer(const BYTE *str, int size);

    ~NStreamBuffer();

private:
    int_type uflow();
    int_type underflow();
    int_type pbackfail(int_type ch);
    std::streamsize showmanyc();

    // copy ctor and assignment not implemented;
    // copying not allowed
    NStreamBuffer(const NStreamBuffer &);
    NStreamBuffer &operator= (const NStreamBuffer &);

private:
    const BYTE * const begin_;
    const BYTE * const end_;
    const BYTE * current_;
};

NAMESPACE_END

#endif // NSTREAMBUFFER_H