#include "NKernel.h"
#include "NPrintf.h"

NAMESPACE_BEGIN

NString inlPrintf(const TCHAR *Format, ...)
{
    TCHAR Result[4096];
	GET_VARARGS(Result, 4096, INL_ARRAY_COUNT(Result) - 1, Format);
	return NString(Result);
}

NAMESPACE_END

// see also: http://cvs.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/cmd/csh/printf.c

