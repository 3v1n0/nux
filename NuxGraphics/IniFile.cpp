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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "NuxCore/NKernel.h"
#include "GLResource.h"
#include "IniFile.h"

NAMESPACE_BEGIN_OGL

struct RecordSectionIs : std::unary_function<CIniFile::Record, bool>
{
    std::string section_;

    RecordSectionIs(const std::string& section): section_(section){}

    bool operator()( const CIniFile::Record& rec ) const
    {
        return rec.Section == section_;
    }
};

struct RecordSectionKeyIs : std::unary_function<CIniFile::Record, bool>
{
    std::string section_;
    std::string key_;

    RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

    bool operator()( const CIniFile::Record& rec ) const
    {
        return ((rec.Section == section_)&&(rec.Key == key_));
    }
};

struct AscendingSectionSort
{
    bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
    {
        return Start.Section < End.Section;
    }
};

struct DescendingSectionSort
{
    bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
    {
        return Start.Section > End.Section;
    }
};

struct AscendingRecordSort
{
    bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
    {
        return Start.Key < End.Key;
    }
};

struct DescendingRecordSort
{
    bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
    {
        return Start.Key > End.Key;
    }
};

CIniFile::CIniFile(void)													// Default constructor
{
}

CIniFile::~CIniFile(void)
{
}

// A function to trim whitespace from both sides of a given string
void Trim(std::string& str, const std::string & ChrsToTrim = " \t\n\r", int TrimDir = 0)
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == std::string::npos){str.erase(); return;}
    if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
    if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
}

//inline void TrimRight(std::string& str, const std::string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 2);
//}

//inline void TrimLeft(std::string& str, const std::string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 1);
//}

// A function to transform a string to uppercase if neccessary
//void UCase(string& str, bool ucase)
//{
//	if(ucase) transform(str.begin(), str.end(), str.begin(), toupper);
//}

bool CIniFile::Load(std::string FileName, std::vector<Record>& content)
{
	std::string s;																// Holds the current line from the ini file
	std::string CurrentSection;													// Holds the current section name

	std::ifstream inFile (FileName.c_str());										// Create an input filestream
	if (!inFile.is_open()) return false;									// If the input file doesn't open, then return
	content.clear();														// Clear the content vector

	std::string comments = "";													// A string to store comments in

	while(!inFile.eof() /*!std::getline(inFile, s).eof()*/)									// Read until the end of the file
	{
        std::getline(inFile, s);
		Trim(s);															// Trim whitespace from the ends
		if(!s.empty())														// Make sure its not a blank line
		{
			Record r;														// Define a new record

			if((s[0]=='#')||(s[0]==';'))									// Is this a commented line?
			{
				if ((s.find('[')==std::string::npos)&&							// If there is no [ or =
					(s.find('=')==std::string::npos))							// Then it's a comment
				{
					comments += s + '\n';									// Add the comment to the current comments string
				} else {
					r.Commented = s[0];										// Save the comment character
					s.erase(s.begin());										// Remove the comment for further processing
					Trim(s);
				}// Remove any more whitespace
			} else r.Commented = ' ';										// else mark it as not being a comment

			if(s.find('[')!=std::string::npos)									// Is this line a section?
			{		
				s.erase(s.begin());											// Erase the leading bracket
				s.erase(s.find(']'));										// Erase the trailing bracket
                Trim(s);
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = s;												// Set the Section value
				r.Key = "";													// Set the Key value
				r.Value = "";												// Set the Value value
				CurrentSection = s;
			}

			if(s.find('=')!=std::string::npos)									// Is this line a Key/Value?
			{
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = CurrentSection;									// Set the section to the current Section
				r.Key = s.substr(0,s.find('='));							// Set the Key value to everything before the = sign
                Trim(r.Key);
				r.Value = s.substr(s.find('=')+1);							// Set the Value to everything after the = sign
                Trim(r.Value);
			}
			if(comments == "")												// Don't add a record yet if its a comment line
				content.push_back(r);										// Add the record to content
		}
	}
	
	inFile.close();															// Close the file
	return true;
}

bool CIniFile::Save(std::string FileName, std::vector<Record>& content)
{
	std::ofstream outFile (FileName.c_str());									// Create an output filestream
	if (!outFile.is_open()) return false;									// If the output file doesn't open, then return

	for (int i=0;i<(int)content.size();i++)									// Loop through each vector
	{
		outFile << content[i].Comments;										// Write out the comments
		if(content[i].Key == "")											// Is this a section?
			outFile << content[i].Commented << "[" 
			<< content[i].Section << "]" << std::endl;							// Then format the section
		else
			outFile << content[i].Commented << content[i].Key  
			<< "=" << content[i].Value << std::endl;								// Else format a key/value
	}

	outFile.close();														// Close the file
	return true;
}

std::string CIniFile::Content(std::string FileName)
{
	std::string s="";															// Hold our return string
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file loads
	{
		for (int i=0;i<(int)content.size();i++)								// Loop through the content
		{
			if(content[i].Comments != "") s += content[i].Comments;			// Add the comments
			if(content[i].Commented != ' ') s += content[i].Commented;		// If this is commented, then add it
			if((content[i].Key == ""))										// Is this a section?
				s += '[' + content[i].Section + ']';						// Add the section
			else s += content[i].Key + '=' + content[i].Value;				// Or the Key value to the return srting

			if (i != content.size()) s += '\n';								// If this is not the last line, add a CrLf
		}
		return s;															// Return the contents
	}

	return "";
}

std::vector<std::string> CIniFile::GetSectionNames(std::string FileName)
{
	std::vector<std::string> data;													// Holds the return data
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)								// Loop through the content
		{
			if(content[i].Key =="")											// If there is no key value, then its a section
				data.push_back(content[i].Section);							// Add the section to the return data
		}
	}

	return data;															// Return the data
}

std::vector<CIniFile::Record> CIniFile::GetSection(std::string SectionName, std::string FileName)
{
	std::vector<Record> data;													// Holds the return data
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)								// Loop through the content
		{
			if((content[i].Section == SectionName) &&						// If this is the section name we want
				(content[i].Key != ""))										// but not the section name itself
				data.push_back(content[i]);									// Add the record to the return data
		}
	}
	
	return data;															// Return the data
}

bool CIniFile::RecordExists(std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == content.end()) return false;							// The Section/Key was not found
	}
	return true;															// The Section/Key was found
}

bool CIniFile::SectionExists(std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionIs(SectionName));					// Locate the Section

		if (iter == content.end()) return false;							// The Section was not found
	}
	return true;															// The Section was found
}

std::vector<CIniFile::Record> CIniFile::GetRecord(std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> data;													// Holds the return data
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Record

		if (iter == content.end()) return data;								// The Record was not found

		data.push_back (*iter);												// The Record was found
	}
	return data;															// Return the Record
}

std::string CIniFile::GetValue(std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content = GetRecord(KeyName,SectionName, FileName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return content[0].Value;											// And return the value

	return "";																// No value was found
}

bool CIniFile::SetValue(std::string KeyName, std::string Value, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		if(!SectionExists(SectionName,FileName))							// If the Section doesn't exist
		{
            Record s = {"",' ',SectionName,"",""};							// Define a new section
            Record r = {"",' ',SectionName,KeyName,Value};					// Define a new record
			content.push_back(s);											// Add the section
			content.push_back(r);											// Add the record
			return Save(FileName,content);									// Save
		}

		if(!RecordExists(KeyName,SectionName,FileName))						// If the Key doesn't exist
		{
			std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionIs(SectionName));                    // Locate the Section
			iter++;                                                         // Advance just past the section
			Record r = {"",' ',SectionName,KeyName,Value};                  // Define a new record
			content.insert(iter,r);											// Add the record
			return Save(FileName,content);									// Save
		}

		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Record

		iter->Value = Value;												// Insert the correct value
		return Save(FileName,content);										// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::RenameSection(std::string OldSectionName, std::string NewSectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for(std::vector<Record>::iterator iter = content.begin(); 
			iter < content.end(); iter++)									// Loop through the records
		{
			if(iter->Section == OldSectionName)								// Is this the OldSectionName?
				iter->Section = NewSectionName;								// Now its the NewSectionName
		}
		return Save(FileName,content);										// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::CommentRecord(CommentChar cc, std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == content.end()) return false;							// The Section/Key was not found
	
		iter->Commented = cc;										// Change the Comment value
		return Save(FileName,content);										// Save

	}
	return false;															// In the event the file does not load
}

bool CIniFile::UnCommentRecord(std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == content.end()) return false;							// The Section/Key was not found
	
		iter->Commented = ' ';												// Remove the Comment value
		return Save(FileName,content);										// Save

	}
	return false;															// In the event the file does not load
}

bool CIniFile::CommentSection(char CommentChar, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for(std::vector<Record>::iterator iter = content.begin(); iter < content.end(); iter++)
		{
			if(iter->Section == SectionName)								// Is this the right section?
				iter->Commented = CommentChar;								// Change the comment value
		}
		return Save(FileName,content);										// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::UnCommentSection(std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for(std::vector<Record>::iterator iter = content.begin(); iter < content.end(); iter++)
		{
			if(iter->Section == SectionName)								// Is this the right section?
				iter->Commented = ' ';										// Remove the comment value
		}																	
		return Save(FileName,content);										// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::DeleteRecord(std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == content.end()) return false;							// The Section/Key was not found
	
		content.erase(iter);												// Remove the Record
		return Save(FileName,content);										// Save

	}
	
	return false;															// In the event the file does not load
}

bool CIniFile::DeleteSection(std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for(int i=(int)content.size()-1;i>-1;i--)								// Iterate backwards through the content
		{							
			if(content[i].Section == SectionName)							// Is this related to the Section?
				content.erase (content.begin()+i);							// Then erase it
		}

		return Save(FileName,content);										// Save
	}
	return false;															// In the event the file does not load
}

bool CIniFile::SetSectionComments(std::string Comments, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for(std::vector<Record>::iterator iter = content.begin(); iter < content.end(); iter++)									// Loop through the records
		{
			if((iter->Section == SectionName) &&							// Is this the Section?
				(iter->Key == ""))											// And not a record
			{	
				if (Comments.size() >= 2)									// Is there a comment?
				{
					if (Comments.substr(Comments.size()-2) != "\n")		// Does the string end in a newline?
						Comments += "\n";								// If not, add one
				}
				iter->Comments = Comments;								// Set the comments
					
				return Save(FileName,content);							// Save
			}
		}
	}
	return false;															// In the event the file does not load
}

bool CIniFile::SetRecordComments(std::string Comments, std::string KeyName, std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		std::vector<Record>::iterator iter = std::find_if(content.begin(), 
				content.end(), 
				RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == content.end()) return false;							// The Section/Key was not found
	
		if (Comments.size() >= 2)											// Is there a comment?
		{
			if (Comments.substr(Comments.size()-2) != "\n")					// Does the string end in a newline?
				Comments += "\n";											// If not, add one
		}
		iter->Comments = Comments;											// Set the comments
		return Save(FileName,content);										// Save

	}
	
	return false;															// In the event the file does not load
}

std::vector<CIniFile::Record> CIniFile::GetSections(std::string FileName)
{
	std::vector<Record> data;													// Holds the return data
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		for (int i=0;i<(int)content.size();i++)								// Loop through the content
		{
			if(content[i].Key == "")										// If this is a section 
				data.push_back(content[i]);									// Add the record to the return data
		}
	}
	
	return data;															// Return the data
}


bool CIniFile::Sort(std::string FileName, bool Descending)
{
	std::vector<CIniFile::Record> content;										// Used to hold the sorted content
	std::vector<CIniFile::Record> sections = GetSections(FileName);				// Get a list of Sections

	if(!sections.empty())													// Is there anything to process?
	{

		if(Descending)														// Descending or Ascending?
            std::sort(sections.begin(), sections.end(), DescendingSectionSort() );
		else																// Sort the Sections
            std::sort(sections.begin(), sections.end(), AscendingSectionSort() );

		for(std::vector<Record>::iterator iter = sections.begin(); iter < sections.end(); iter++) // For each Section
		{																		
			content.push_back(*iter);										// Add the sorted Section to the content

			std::vector<CIniFile::Record> records = GetSection(iter->Section ,FileName); // Get a list of Records for this section

			if(Descending)													// Descending or Ascending?
				std::sort(records.begin(), records.end(), DescendingRecordSort());
			else															// Sort the Records
				std::sort(records.begin(), records.end(), AscendingRecordSort());

			for(std::vector<Record>::iterator it = records.begin(); it < records.end(); it++) // For each Record
				content.push_back(*it);										// Add the sorted Record to the content
		}
		
		return Save(FileName,content);										// Save
		}

	return false;															// There were no sections
}

bool CIniFile::AddSection(std::string SectionName, std::string FileName)
{
	std::vector<Record> content;													// Holds the current record													// Holds the current record

	if (Load(FileName, content))											// Make sure the file is loaded
	{
		Record s = {"",' ',SectionName,"",""};								// Define a new section
		content.push_back(s);												// Add the section
		return Save(FileName,content);										// Save
	}

	return false;															// The file did not open
}

bool CIniFile::Create(std::string FileName)
{
	std::vector<Record> content;													// Create empty content
	return Save(FileName,content);											// Save
}


void Show(std::string FileName)
{
    std::cout << std::endl 
        << "++++++++++++++++++++++++++++++++++++++++"
        << std::endl
        << "+ Contents of the file are below       +"
        << std::endl
        << "++++++++++++++++++++++++++++++++++++++++"
        << std::endl
        << CIniFile::Content(FileName)
        << std::endl
        << "++++++++++++++++++++++++++++++++++++++++"
        << std::endl << std::endl;
}

int Usage()
{
    //CIniFile IniFile;
    std::string FileName = "test.ini";

    // Create a new file
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to create a new file called \"test.ini\"" << std::endl << std::endl;
    std::cout << "string FileName = \"test.ini\";" << std::endl;
    std::cout << "CIniFile::Create(FileName);" << std::endl << std::endl;
    if (CIniFile::Create(FileName)) std::cout << "File was successfully created" << std::endl << std::endl;
    else std::cout << "Failed to create the file" << std::endl << std::endl;
    Show(FileName);

    // Create a new section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to create a new section called [MySection]" << std::endl << std::endl;
    std::cout << "CIniFile::AddSection(\"MySection\", FileName);" << std::endl << std::endl;
    if (CIniFile::AddSection("MySection",FileName)) std::cout << "Section was successfully created" << std::endl << std::endl;
    else std::cout << "Failed to create the section" << std::endl << std::endl;
    Show(FileName);

    // Add a key to the section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to add a new key/value (MyKey=MyValue) to [MySection]" << std::endl << std::endl;
    std::cout << "CIniFile::SetValue(\"MyKey\",\"MyValue\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetValue("MyKey","MyValue","MySection",FileName)) std::cout << "Record was successfully created" << std::endl << std::endl;
    else std::cout << "Failed to create the record" << std::endl << std::endl;
    Show(FileName);

    // Add a key and create a section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to add a new key/value (TestKey=TestValue)" << std::endl << "and create a new section [TestSection] at the same time" << std::endl << std::endl;
    std::cout << "CIniFile::SetValue(\"TestKey\",\"TestValue\",\"TestSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetValue("TestKey","TestValue","TestSection",FileName)) std::cout << "Record and section were successfully created" << std::endl << std::endl;
    else std::cout << "Failed to create the record and section" << std::endl << std::endl;
    Show(FileName);

    // Change a key value
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to change the key/value for (MyKey=MyValue) to (MyKey=YourValue)" << std::endl << std::endl;
    std::cout << "CIniFile::SetValue(\"MyKey\",\"YourValue\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetValue("MyKey","YourValue","MySection",FileName)) std::cout << "Record was successfully changed" << std::endl << std::endl;
    else std::cout << "Failed to change the record" << std::endl << std::endl;
    Show(FileName);

    // Get a value
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to get the value of MyKey" << std::endl << std::endl;
    std::cout << "CIniFile::GetValue(\"MyKey\",\"MySection\",FileName);" << std::endl << std::endl;
    std::string v = CIniFile::GetValue("MyKey","MySection",FileName);
    std::cout << "The value of MyKey is: " << v.c_str() << std::endl << std::endl;
    Show(FileName);

    // Get a list of Sections
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to get a list of sections" << std::endl << std::endl;
    std::cout << "CIniFile::GetSectionNames(FileName);" << std::endl << std::endl;
    std::vector<std::string> s = CIniFile::GetSectionNames(FileName);
    std::cout << "The sections are returned as a std::vector<std::string>\n\n";
    for(int i=0; i < (int)s.size(); i++)
        std::cout << s[i].c_str() << std::endl;
    Show(FileName);

    // Section Exists
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to verify that [MySection] exists" << std::endl << std::endl;
    std::cout << "CIniFile::SectionExists(\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SectionExists("MySection",FileName)) std::cout << "Section exists" << std::endl << std::endl;
    else std::cout << "Section does not exist" << std::endl << std::endl;
    Show(FileName);

    // Record Exists
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to verify that MyKey exists" << std::endl << std::endl;
    std::cout << "CIniFile::RecordExists(\"MyKey\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::RecordExists("MyKey","MySection",FileName)) std::cout << "Record exists" << std::endl << std::endl;
    else std::cout << "Record does not exist" << std::endl << std::endl;
    Show(FileName);

    // Case Sensitive
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "BE CAREFUL - functions in CIniFile are CASE-SENSITIVE" << std::endl << std::endl;
    std::cout << "CIniFile::RecordExists(\"mykey\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::RecordExists("mykey","MySection",FileName)) std::cout << "Record exists" << std::endl << std::endl;
    else std::cout << "Record does not exist" << std::endl << std::endl;
    Show(FileName);

    // Add a comment to the section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to add comments to [MySection]" << std::endl << std::endl;
    std::cout << "CIniFile::SetSectionComments(\"# This Section was created by CIniFile\\n\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetSectionComments("# This Section was created by CIniFile\n","MySection",FileName)) std::cout << "Comments were successfully added" << std::endl << std::endl;
    else std::cout << "Failed to add the comments" << std::endl << std::endl;
    Show(FileName);

    // Add a comment to the record
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to add comments to MyKey" << std::endl << std::endl;
    std::cout << "CIniFile::SetRecordComments(\"# This Key was created by CIniFile\\n\",\"MyKey\",\"MySection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetRecordComments("# This Key was created by CIniFile\n","MyKey","MySection",FileName)) std::cout << "Comments were successfully added" << std::endl << std::endl;
    else std::cout << "Failed to add the comments" << std::endl << std::endl;
    Show(FileName);

    // Rename Section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to rename [MySection] to [YourSection]" << std::endl << std::endl;
    std::cout << "CIniFile::RenameSection(\"MySection\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::RenameSection("MySection","YourSection",FileName)) std::cout << "Section was successfully changed" << std::endl << std::endl;
    else std::cout << "Failed to change the section" << std::endl << std::endl;
    Show(FileName);

    // Multiple comments
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Multiple comments can be added by putting \\n# in the comments string" << std::endl << std::endl;
    std::cout << "CIniFile::SetSectionComments(\"# This Section was created by CIniFile\\n# Kids, don't try this at home \\n\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetSectionComments("# This Section was created by CIniFile\n# Kids, don't try this at home","YourSection",FileName)) std::cout << "Comments were successfully added" << std::endl << std::endl;
    else std::cout << "Failed to add the comments" << std::endl << std::endl;
    Show(FileName);

    // Remove comments
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Comments are removed by setting them to \"\"" << std::endl << std::endl;
    std::cout << "CIniFile::SetRecordComments(\"\",\"MyKey\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::SetRecordComments("","MyKey","YourSection",FileName)) std::cout << "Comments were successfully removed" << std::endl << std::endl;
    else std::cout << "Failed to remove the comments" << std::endl << std::endl;
    Show(FileName);

    // Comment entire section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to comment the entire section [YourSection]" << std::endl << std::endl;
    std::cout << "CIniFile::CommentSection(\"#\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::CommentSection('#',"YourSection",FileName)) std::cout << "Section was successfully commented" << std::endl << std::endl;
    else std::cout << "Failed to comment the section" << std::endl << std::endl;
    Show(FileName);

    // UnComment entire section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to un-comment the entire section [YourSection]" << std::endl << std::endl;
    std::cout << "CIniFile::UnCommentSection(\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::UnCommentSection("YourSection",FileName)) std::cout << "Section was successfully un-commented" << std::endl << std::endl;
    else std::cout << "Failed to un-comment the section" << std::endl << std::endl;
    Show(FileName);

    // Comment a single record
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to comment the record MyKey" << std::endl << std::endl;
    std::cout << "(Note that both # and ; are recognized as commented lines by CIniFile)" << std::endl << std::endl;
    std::cout << "CIniFile::CommentRecord(CIniFile::CommentChar::Pound,\"MyKey\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::CommentRecord(CIniFile::Pound,"MyKey","YourSection",FileName))
        std::cout << "Record was successfully commented" << std::endl << std::endl;
    else
        std::cout << "Failed to comment the record" << std::endl << std::endl;
    Show(FileName);

    // UnComment a single record
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to un-comment the record MyKey" << std::endl << std::endl;
    std::cout << "CIniFile::UnCommentRecord(\"MyKey\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::UnCommentRecord("MyKey","YourSection",FileName)) std::cout << "Record was successfully un-commented" << std::endl << std::endl;
    else std::cout << "Failed to un-comment the record" << std::endl << std::endl;
    Show(FileName);

    // Sort
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to sort the file - false means ASCENDING, true means DESCENDING" << std::endl << std::endl;
    std::cout << "(Note that the comments will stay with their targets)" << std::endl << std::endl;
    std::cout << "CIniFile::Sort(FileName,false);" << std::endl << std::endl;
    if (CIniFile::Sort(FileName,false)) std::cout << "File was successfully sorted" << std::endl << std::endl;
    else std::cout << "Failed to sort the file" << std::endl << std::endl;
    Show(FileName);

    // Delete entire section
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to delete the entire section [TestSection]" << std::endl << std::endl;
    std::cout << "CIniFile::DeleteSection(\"TestSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::DeleteSection("TestSection",FileName)) std::cout << "Section was successfully deleted" << std::endl << std::endl;
    else std::cout << "Failed to delete the section" << std::endl << std::endl;
    Show(FileName);

    // Delete record
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Attempting to delete the record <yKey" << std::endl << std::endl;
    std::cout << "CIniFile::DeleteRecord(\"MyKey\",\"YourSection\",FileName);" << std::endl << std::endl;
    if (CIniFile::DeleteRecord("MyKey","YourSection",FileName)) std::cout << "Record was successfully deleted" << std::endl << std::endl;
    else std::cout << "Failed to delete the record" << std::endl << std::endl;
    Show(FileName);

    // Content
    std::cout << "TestIniFile - Demo program for the CIniFile Class" << std::endl << std::endl;
    std::cout << "Finally, the content of the file can be retrieved as a std::string" << std::endl << std::endl;
    std::cout << "CIniFile::Content(FileName);" << std::endl << std::endl;
    std::cout << "The contents of the file throughout this demo have used this function to display the contents below" << std::endl;
    Show(FileName);

    return 0;
}
NAMESPACE_END_OGL
