#include "Directory_Entry.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include<iostream>

using namespace std; // Using std namespace for convenience
Directory_Entry::Directory_Entry()
    : dir_attr(0x00), dir_firstCluster(0), dir_fileSize(0), subDirectory(nullptr)
{
    // Initialize with empty name
    fill(begin(dir_name), end(dir_name), ' ');
    fill(begin(dir_empty), end(dir_empty), ' ');
}

// Constructor to initialize a Directory_Entry object
Directory_Entry::Directory_Entry(string name, char attr, int firstCluster)
    : dir_attr(attr), dir_firstCluster(firstCluster), dir_fileSize(0), subDirectory(nullptr)
{
    // Assign name based on attribute
    if (attr == 0x10) // Directory
    {
        assignDir_Name(name);
    }
    else // File
    {
        // Split name and extension
        size_t dotPos = name.find_last_of('.');
        if (dotPos != string::npos)
        {
            string fileName = name.substr(0, dotPos);
            string extension = name.substr(dotPos + 1);
            assignFileName(fileName, extension);
        }
        else
        {
            assignFileName(name, "");
        }
    }

    // Initialize dir_empty with blanks
    fill(begin(dir_empty), end(dir_empty), ' ');
}

// Cleans the file/directory name to include only alphanumeric characters and underscores
string Directory_Entry::cleanTheName(string name) {
    // Trim leading and trailing spaces
    size_t first = name.find_first_not_of(' ');
    size_t last = name.find_last_not_of(' ');
    if (first == string::npos || last == string::npos) {
        return ""; // Return empty string if only spaces are present
    }
    name = name.substr(first, last - first + 1);

    // Define a list of invalid characters
    const string invalidChars = R"(/\*?"<>|)";

    // Check for invalid characters
    for (char c : name) {
        if (invalidChars.find(c) != string::npos) {
            return ""; // Return empty string if any invalid character is found
        }
    }

    // Enforce length constraints (max 11 characters total)
    if (name.length() > 11) {
        return ""; // Reject names exceeding length constraints
    }

    return name; // Valid name
}
// Assigns a file name and extension to the dir_name array (8 characters for name, 3 for extension)
void Directory_Entry::assignFileName( string name,  string extension)
{
    string cleanName = cleanTheName(name);  // Clean the file name
    string cleanExt = cleanTheName(extension);  // Clean the extension

    // Truncate or pad the name to ensure it's 8 characters long
    string fname = cleanName.substr(0, 8);
    fname.resize(8, ' ');  // Pad with spaces if the name is shorter than 8 characters

    // Truncate or pad the extension to ensure it's 3 characters long
    string fext = cleanExt.substr(0, 3);
    fext.resize(3, ' ');  // Pad with spaces if the extension is shorter than 3 characters

    // Copy the name and extension into dir_name (first 8 for name, next 3 for extension)
    memcpy(dir_name, fname.c_str(), 8);
    memcpy(dir_name + 8, fext.c_str(), 3);
}

// Assigns a directory name to the dir_name array (up to 11 characters)
void Directory_Entry::assignDir_Name( string name)
{
    string cleanName = cleanTheName(name);  // Clean the directory name

    // Truncate or pad the name to ensure it's 11 characters long
    string dname = cleanName.substr(0, 11);
    dname.resize(11, ' ');  // Pad with spaces if the name is shorter than 11 characters

    // Copy the directory name into dir_name
    memcpy(dir_name, dname.c_str(), 11);
}

std::string Directory_Entry::getName() const
{
    // 1. Extract base name (first 8 chars)
    std::string baseName(dir_name, 8);

    // 2. Extract extension (next 3 chars)
    std::string extension(dir_name + 8, 3);

    // 3. Helper lambda to trim trailing spaces from a string
    auto trimRight = [](std::string& s) {
        size_t pos = s.find_last_not_of(' ');
        if (pos != std::string::npos) {
            s = s.substr(0, pos + 1);
        }
        else {
            s.clear();
        }
        };

    // 4. Trim trailing spaces from baseName and extension
    trimRight(baseName);
    trimRight(extension);

    // 5. If we have an extension, join with a dot; otherwise just return baseName
    if (!extension.empty()) {
        return baseName + "." + extension;
    }
    return baseName;
}

bool Directory_Entry::getIsFile() const {
    return isFile;
}

void Directory_Entry::setIsFile(bool isFileFlag) {
    isFile = isFileFlag;
}

string Directory_Entry::getContent() const {
    return content;
}

void Directory_Entry::setContent(const string& newContent) {
    content = newContent;
    dir_fileSize = static_cast<int>(newContent.size()); // Update file size
}


int Directory_Entry::getSize() const
{
    return dir_fileSize;
}
