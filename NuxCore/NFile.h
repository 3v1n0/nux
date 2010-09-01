#ifndef NFILE_H
#define NFILE_H

NAMESPACE_BEGIN

bool LoadFileToArray( std::vector<BYTE>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadTextFileToAnsiArray( std::vector<ANSICHAR>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadTextFileToUnicodeArray( std::vector<UNICHAR>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadFileToString( NString& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool SaveArrayToFile( const std::vector<BYTE>& Array, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool SaveStringToFile( const NString& String, const TCHAR* Filename, NFileManager& FileManager = GFileManager );

NAMESPACE_END


#endif // NFILE_H

