#ifndef NWIN32CLIPBOARD_H
#define NWIN32CLIPBOARD_H

NAMESPACE_BEGIN

void inlCopyTextToClipboard(const TCHAR* text);
NString inlReadTextToClipboard();

NAMESPACE_END

#endif // NWIN32CLIPBOARD_H
