
//////////////////////////////////////////////////////////////////////////
// This file comes from NVidia SDK 9.5 data_path.h                      //
//////////////////////////////////////////////////////////////////////////

#ifndef FILEPATH_H
#define FILEPATH_H

#ifdef _WIN32
#  pragma warning(disable:4786)   // symbol size limitation ... STL
#endif

#include <stdio.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

NAMESPACE_BEGIN

class FilePath
{
public:
    FilePath();
    ~FilePath();

    void AddSearchPath(const NString& searchpath);
    void AddSearchPath(const std::vector<NString>& searchpath);

    NString GetPathToFile(const TCHAR* filename) const;
    NString GetFile(const TCHAR* filename) const;
	
private:
    std::vector<NString>          m_SearchPath;
};

NAMESPACE_END

#endif // FILEPATH_H
