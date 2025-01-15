#pragma once
#include "Directory.h"
#include"Directory_Entry.h"
#include<string>
using namespace std;

class File_Entry : public Directory_Entry
{
public:
    string content;
    Directory* parent;
    
    File_Entry(string name, char dir_attr, int dir_firstCluster, Directory* pa);

    File_Entry(Directory_Entry d, Directory* pa);

    int getMySizeOnDisk();

    void emptyMyClusters();

    Directory_Entry getDirectory_Entry();

    void writeFileContent();

    void readFileContent();

    void deleteFile();

    void printContent();
};