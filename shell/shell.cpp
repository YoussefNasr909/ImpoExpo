#include "Virtual_Disk.h"
#include "Mini_FAT.h"
#include "Directory.h"
#include "Directory_Entry.h"
#include "File_Entry.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "CommandProcessor.h"
#include "Converter.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int main()
{
    // Path to the virtual disk file
    string diskPath = "virtual_disk.bin";

    // Initialize or open the virtual disk and FAT
    Mini_FAT::initialize_Or_Open_FileSystem(diskPath);

    // Create the root directory "C:\"
    Directory* rootDir = new Directory("C:", 0x10, 0, nullptr);
    rootDir->name = "C:";

    rootDir->readDirectory(); 

    // Initialize the current directory to root
    Directory* currentDir = rootDir;

    // Initialize the command processor with the current directory pointer
    CommandProcessor cmdProcessor(&currentDir);
    bool isRunning = true;
    cout << "************************************************************************************************************************"<<endl;
    cout << "                                                    Welcome To The Shell                                         " << endl;
    cout << "************************************************************************************************************************" << endl;
    // Shell loop
    while (isRunning)
    {
        string input;
        cout << currentDir->getFullPath() << " >> ";
        getline(cin, input);
        cmdProcessor.processCommand(input, isRunning);
    }

    // Cleanup: Delete the root directory (which recursively deletes subdirectories)
    Mini_FAT::CloseTheSystem();
    delete rootDir;

    return 0;
}