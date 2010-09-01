#include "NKernel.h"

#include "FilePath.h"

NAMESPACE_BEGIN

FilePath::FilePath()
{
    if(std::find(m_SearchPath.begin(), m_SearchPath.end(), TEXT("")) != m_SearchPath.end())
        return;
    m_SearchPath.push_back(TEXT("")); // for fully qualified path
}

FilePath::~FilePath()
{

}

void FilePath::AddSearchPath(const NString& searchpath)
{
    if(std::find(m_SearchPath.begin(), m_SearchPath.end(), searchpath) != m_SearchPath.end())
        return;
    m_SearchPath.push_back(searchpath);
}

void FilePath::AddSearchPath(const std::vector<NString>& searchpath)
{
    for(t_u32 i = 0; i < searchpath.size(); i++)
    {
        if(std::find(m_SearchPath.begin(), m_SearchPath.end(), searchpath[i]) == m_SearchPath.end())
            m_SearchPath.push_back(searchpath[i]);
    }
}

NString FilePath::GetPathToFile(const TCHAR* filename) const
{
    NString FileName = GetFile(filename);
    
    int loc = (int)FileName.FindLastOccurence(TEXT('\\'));
    if (loc == -1)
    {
        loc = (int)FileName.FindLastOccurence(TEXT('/'));
    }

    if (loc != -1)
    	FileName = FileName.GetSubString(0, loc);
    else
        FileName = NString(TEXT("."));
    return FileName;
}

NString FilePath::GetFile(const TCHAR* filename) const
{
    INL_RETURN_VALUE_IF_NULL(filename, NString(TEXT("")));
    if(filename == TEXT(""))
        return NString(TEXT(""));

    NString FileName = filename;

    if(GFileManager.FileExist(FileName.GetTCharPtr()))
        return FileName;

    for(t_u32 i = 0; i < m_SearchPath.size(); i++)
    {
        if(m_SearchPath[i].Size() == 0)
            continue;

        NString FilePath = m_SearchPath[i] + INL_PATH_SEPARATOR_STRING + filename;
        if(GFileManager.FileExist(FilePath.GetTCharPtr()))
            return FilePath;
    }
    
    // Still not found. Then peel off the root of the filename and append our custom search path.
    //      filename = "MediaProg/UI3DGraphics/MyFile.txt"
    //      search for: 
    //            CustomPath0/UI3DGraphics/MyFile.txt
    //            CustomPath1/UI3DGraphics/MyFile.txt
    //            CustomPath2/UI3DGraphics/MyFile.txt
    //            CustomPath3/UI3DGraphics/MyFile.txt
    //            CustomPath0/MyFile.txt
    //            CustomPath1/MyFile.txt
    //            CustomPath2/MyFile.txt
    //            CustomPath3/MyFile.txt
    //          
        
    FileName = filename;
	for(t_size i = 0; i < m_SearchPath.size(); i++)
	{
		t_size pos; 
        NString PathName;
		while(FileName.FindFirstOccurenceOf(TEXT("\\/")) != std::string::npos)
		{
			pos = FileName.FindFirstOccurenceOf(TEXT("\\/")) + 1;
			
			FileName = FileName.GetSubString(pos, FileName.Length()- pos);
			PathName = m_SearchPath[i] + INL_PATH_SEPARATOR_STRING + FileName;
            if(GFileManager.FileExist(PathName.GetTCharPtr()))
                return PathName;
		}
	}

    nuxDebugMsg(TEXT("FilePath::GetFile] Cannot find file: %s"), filename);
    return NString(TEXT(""));
}

NAMESPACE_END
