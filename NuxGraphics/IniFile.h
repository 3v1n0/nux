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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef INIFILE_H
#define INIFILE_H

namespace nux
{

  class CIniFile
  {
  public:
    struct Record
    {
      std::string Comments;
      char Commented;
      std::string Section;
      std::string Key;
      std::string Value;
    };

    enum CommentChar
    {
      Pound = '#',
      SemiColon = ';'
    };

    CIniFile (void);
    virtual ~CIniFile (void);

    static bool AddSection (std::string SectionName, std::string FileName);
    static bool CommentRecord (CommentChar cc, std::string KeyName, std::string SectionName, std::string FileName);
    static bool CommentSection (char CommentChar, std::string SectionName, std::string FileName);
    static std::string Content (std::string FileName);
    static bool Create (std::string FileName);
    static bool DeleteRecord (std::string KeyName, std::string SectionName, std::string FileName);
    static bool DeleteSection (std::string SectionName, std::string FileName);
    static std::vector<Record> GetRecord (std::string KeyName, std::string SectionName, std::string FileName);
    static std::vector<Record> GetSection (std::string SectionName, std::string FileName);
    static std::vector<std::string> GetSectionNames (std::string FileName);
    static std::string GetValue (std::string KeyName, std::string SectionName, std::string FileName);
    static bool RecordExists (std::string KeyName, std::string SectionName, std::string FileName);
    static bool RenameSection (std::string OldSectionName, std::string NewSectionName, std::string FileName);
    static bool SectionExists (std::string SectionName, std::string FileName);
    static bool SetRecordComments (std::string Comments, std::string KeyName, std::string SectionName, std::string FileName);
    static bool SetSectionComments (std::string Comments, std::string SectionName, std::string FileName);
    static bool SetValue (std::string KeyName, std::string Value, std::string SectionName, std::string FileName);
    static bool Sort (std::string FileName, bool Descending);
    static bool UnCommentRecord (std::string KeyName, std::string SectionName, std::string FileName);
    static bool UnCommentSection (std::string SectionName, std::string FileName);

  private:
    static std::vector<Record> GetSections (std::string FileName);
    static bool Load (std::string FileName, std::vector<Record>& content);
    static bool Save (std::string FileName, std::vector<Record>& content);
  };

}

#endif // INIFILE_H