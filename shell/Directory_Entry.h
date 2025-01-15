#pragma once

#include <string>
using namespace std;

class Directory;

class Directory_Entry
{
public:
    Directory_Entry();
    Directory_Entry(string name, char attr, int firstCluster);
    void assignFileName(string name, string extension);
    void assignDir_Name(string name);
    char dir_name[11];
    char dir_attr;
    char dir_empty[12];
    int dir_firstCluster;
    int dir_fileSize;
    Directory* subDirectory;
    static string cleanTheName(string s);
    string getName() const;
    bool getIsFile() const;
    void setIsFile(bool isFile);
    string getContent() const;
    void setContent(const string& content);
    bool isFile;
    string content;
    int getSize() const;

};
