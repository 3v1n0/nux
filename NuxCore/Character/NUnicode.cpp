#include "NKernel.h"
#include "NUnicode.h"


NAMESPACE_BEGIN

ANSICHAR* UnicharToAnsicharConvertion::Convert(const UNICHAR* Source)
{
    std::wstring utf16string(Source);
    size_t utf16size = utf16string.length();
    size_t utf8size = 6 * utf16size;
    ANSICHAR *utf8string = new ANSICHAR[utf8size+1];

    const t_UTF16 *source_start = utf16string.c_str();
    const t_UTF16 *source_end = source_start + utf16size;
    t_UTF8* target_start = reinterpret_cast<t_UTF8*>(utf8string);
    t_UTF8* target_end = target_start + utf8size;

    ConversionResult res = ConvertUTF16toUTF8(&source_start, source_end, &target_start, target_end, lenientConversion);
    if (res != conversionOK)
    {
        delete utf8string;
        utf8string = 0;
    }
    // mark end of string
    *target_start = 0;
    return utf8string;
}

UNICHAR* AnsicharToUnicharConvertion::Convert(const ANSICHAR* Source)
{
    std::string utf8string(Source);
    size_t utf8size = utf8string.length();
    size_t utf16size = utf8size;
    UNICHAR *utf16string = new UNICHAR[utf16size+1];

    const t_UTF8 *source_start = reinterpret_cast<const t_UTF8*>(utf8string.c_str());
    const t_UTF8 *source_end = source_start + utf8size;
    t_UTF16* target_start = reinterpret_cast<t_UTF16*>(utf16string);
    t_UTF16* target_end = target_start + utf16size;

    ConversionResult res = ConvertUTF8toUTF16(&source_start, source_end, &target_start, target_end, lenientConversion);
    if (res != conversionOK)
    {
        delete utf16string;
        utf16string = 0;
    }
    // mark end of string
    *target_start = 0;
    return utf16string;
}

NAMESPACE_END