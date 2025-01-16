#include "CommandProcessor.h"
#include "Directory.h"
#include"Mini_FAT.h"
#include <algorithm>
#include <cstring>
#include <cctype>
#include <sstream> // For istringstream
#include "File_Entry.h"
#include<iostream>
#include <iomanip>
#include <ios>
#include "Parser.h"
#include <fstream>    // For file I/O
#include <filesystem>
namespace fs = std::filesystem;


// Constructor implementation
CommandProcessor::CommandProcessor(Directory** currentDirPtr)
    : currentDirectoryPtr(currentDirPtr)
{
    commandHelp["help"] = {
    "Provides help information for commands.",
    "Usage:\n"
    "  help\n"
    "  help [command]\n\n"
    "Syntax:\n"
    "  - General Help: `help`\n"
    "  - Command-Specific Help: `help [command_name]`\n\n"
    "Description:\n"
    "  - Displays a list of all available commands with brief descriptions.\n"
    "  - For a specific command, provides detailed information, including its usage and syntax."
    };

    commandHelp["rd"] = {
        "Removes one or more directories.",
        "Usage:\n"
        "  rd [directory_name]+\n\n"
        "Syntax:\n"
        "  - Remove a single directory: `rd [directory_name]`\n"
        "  - Remove multiple directories: `rd [directory1] [directory2] ...`\n\n"
        "Description:\n"
        "  - Deletes the specified directory or directories.\n"
        "  - Each directory must be empty before it can be deleted."
    };

    commandHelp["md"] = {
        "Creates a new directory.",
        "Usage:\n"
        "  md [path]\n\n"
        "Syntax:\n"
        "  - Create a directory: `md [directory_name]`\n"
        "  - Create a directory with a specific path: `md [path/to/directory]`\n\n"
        "Description:\n"
        "  - Creates a new directory in the specified path or current directory."
    };

    commandHelp["echo"] = {
        "Creates a new empty file.",
        "Usage:\n"
        "  echo [path]\n\n"
        "Syntax:\n"
        "  - Create a file: `echo [file_name]`\n"
        "  - Create a file in a specific path: `echo [path/to/file]`\n\n"
        "Description:\n"
        "  - Creates a new empty file at the specified path or current directory."
    };

    commandHelp["write"] = {
        "Writes content to an existing file.",
        "Usage:\n"
        "  write [file_path]\n\n"
        "Syntax:\n"
        "  - Write to a file: `write [file_name]`\n"
        "  - Write to a file in a specific path: `write [path/to/file]`\n\n"
        "Description:\n"
        "  - Opens the specified file for writing.\n"
        "  - Allows input of multiple lines of text until a specific termination input is given."
    };

    commandHelp["dir"] = {
        "Lists the contents of a directory.",
        "Usage:\n"
        "  dir\n"
        "  dir [directory_path]\n\n"
        "Syntax:\n"
        "  - List current directory: `dir`\n"
        "  - List contents of a specific directory: `dir [directory_path]`\n\n"
        "Description:\n"
        "  - Displays files and subdirectories in the specified directory.\n"
        "  - Includes detailed statistics like file count, directory count, total used space, and free space."
    };

    commandHelp["del"] = {
        "Deletes one or more files.",
        "Usage:\n"
        "  del [file|directory]+\n\n"
        "Syntax:\n"
        "  - Delete a file: `del [file_name]`\n"
        "  - Delete multiple files: `del [file1] [file2] ...`\n\n"
        "Description:\n"
        "  - Deletes the specified file(s).\n"
        "  - Does not delete subdirectories or their contents."
    };

    commandHelp["cd"] = {
        "Changes the current directory.",
        "Usage:\n"
        "  cd\n"
        "  cd [directory]\n\n"
        "Syntax:\n"
        "  - Show current directory: `cd`\n"
        "  - Change to a specific directory: `cd [directory_name]`\n"
        "  - Move up one level: `cd ..`\n"
        "  - Move up multiple levels: `cd ../../..`\n\n"
        "Description:\n"
        "  - Changes the current working directory to the specified one.\n"
        "  - Accepts relative or absolute paths."
    };

    commandHelp["pwd"] = {
        "Displays the full path of the current directory.",
        "Usage:\n"
        "  pwd\n\n"
        "Syntax:\n"
        "  - Display the current directory: `pwd`\n\n"
        "Description:\n"
        "  - Prints the absolute path of the current working directory."
    };

    commandHelp["type"] = {
        "Displays the content of a file.",
        "Usage:\n"
        "  type [file_path]\n\n"
        "Syntax:\n"
        "  - Display file content: `type [file_name]`\n"
        "  - Display content of a file in a specific path: `type [path/to/file]`\n\n"
        "Description:\n"
        "  - Reads and displays the content of the specified file.\n"
        "  - Displays an error if the file is not found or is a directory."
    };

    commandHelp["history"] = {
        "Displays the history of executed commands.",
        "Usage:\n"
        "  history\n\n"
        "Syntax:\n"
        "  - Show command history: `history`\n\n"
        "Description:\n"
        "  - Lists all the commands entered in the current session."
    };

    commandHelp["import"] = {
        "Imports text file(s) from your computer into the virtual disk.",
        "Usage:\n"
        "  import [source]\n"
        "  import [source] [destination]\n\n"
        "Syntax:\n"
        "  - Import a file: `import [file_path]`\n"
        "  - Import a file to a specific location: `import [file_path] [destination]`\n\n"
        "Description:\n"
        "  - Transfers files from your physical disk to the virtual disk."
    };
    commandHelp["export"] = {
    "Exports text file(s) from the virtual disk to your computer.",
    "Usage:\n"
    "  export [source]\n"
    "  export [source] [destination]\n\n"
    "Syntax:\n"
    "  - Export a file: `export [file_path]`\n"
    "  - Export a file to a specific location: `export [file_path] [destination]`\n\n"
    "Description:\n"
    "  - Transfers files from the virtual disk to your physical disk.\n"
    "  - [source] can be a file name or full path of a file in the virtual disk.\n"
    "  - [destination] specifies the location on your physical disk where the file will be exported.\n"
    "  - If no destination is provided, the file is exported to the current working directory on the physical disk.\n"
    "  - The command overwrites the file in the destination if it already exists."
    };


    commandHelp["rename"] = {
        "Renames a file.",
        "Usage:\n"
        "  rename [fileName] [new fileName]\n\n"
        "Syntax:\n"
        "  - Rename a file: `rename [current_name] [new_name]`\n\n"
        "Description:\n"
        "  - Renames a file in the current directory or at a full path.\n"
        "  - The new file name must not already exist."
    };
    commandHelp["quit"] = {
    "Exits the application.",
    "Usage:\n"
    "  quit\n\n"
    "Syntax:\n"
    "  - Exit the application: quit\n\n"
    "Description:\n"
    "  - Terminates the current session and closes the application gracefully."
    };
    commandHelp["copy"] = {
    "Copies one or more files or directories to another location.",
    "Usage:\n"
    "  copy [source]\n"
    "  copy [source] [destination]\n\n"
    "Description:\n"
    "  - Copies files or directories from the source to the destination.\n"
    "  - [source] can be a file name, full path to a file, directory name, or full path to a directory.\n"
    "  - [destination] can be a file name, full path to a file, directory name, or full path to a directory.\n"
    "  - If the destination is not provided, the file or directory is copied to the current directory.\n"
    "  - The command confirms overwriting if the destination file or directory already exists.\n"
    "  - Supports recursive copying of directory contents (bonus feature).\n\n"
    "Syntax:\n"
    "  copy [source]\n"
    "  copy [source] [destination]\n"
    };

    commandHelp["cls"] = {
        "Clears the screen.",
        "Usage:\n"
        "  cls\n\n"
        "Syntax:\n"
        "  - Clear the screen: `cls`\n\n"
        "Description:\n"
        "  - Removes all previous outputs and displays a clean prompt."
    };
}

void CommandProcessor::processCommand(const string& input, bool& isRunning)
{
    // Add command to history
    if (!input.empty())
    {
        commandHistory.push_back(input);
    }

    // Tokenize the input
    vector<string> tokens = Tokenizer::tokenize(input);

    if (tokens.empty())
    {
        return; // No command entered
    }

    // Parse tokens into command and arguments
    Command cmd = Parser::parse(tokens);

    // Convert command name to lowercase for case-insensitive comparison
    transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(),
        [](unsigned char c) { return tolower(c); });

    // Now, use cmd.name and cmd.arguments as before
    if (cmd.name == "help")
    {
        if (cmd.arguments.empty())
        {
            // Test Case (1): Type 'help'
            showGeneralHelp();
        }
        else if (cmd.arguments.size() == 1)
        {
            // Test Case (2) and (3): Type 'help [command]'
            showCommandHelp(cmd.arguments[0]);
        }
        else
        {
            // Invalid syntax
            cout << "Error: Invalid syntax for help command.\n";
            cout << "Usage:\n"
                << "  help\n"
                << "  help [command]\n";
        }
    }
   
    else if (cmd.name == "del")
    {
        if (cmd.arguments.empty())
        {
            // Case (1): Invalid syntax
            cout << "Error: Invalid syntax for del command.\n";
            cout << "Usage: del [file|directory]+ (e.g., del file1.txt dir1 file2.txt)\n";
        }
        else
        {
            // Handle 'del' with one or more file/directory arguments
            handleDel(cmd.arguments);
        }
    }
    else if (cmd.name == "copy")
    {
        if (!cmd.arguments.empty() && cmd.arguments.size() <= 2) {
            handleCopy(cmd.arguments);
        }
        else {
            cout << "Error: Invalid syntax for copy command.\n";
            cout << "Usage:\n";
            cout << "  copy [source]\n";
            cout << "  copy [source] [destination]\n";
        }
    }

    
    else if (cmd.name == "rename") // Handling 'rename' command
    {
        if (cmd.arguments.size() == 2)
        {
            handleRename(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for rename command.\n";
            cout << "Usage: rename [fileName] [new fileName]\n";
        }
    }
    else if (cmd.name == "type") // Handling 'type' command
    {
        if (!cmd.arguments.empty())
        {
            handleType(cmd.arguments); // Pass the entire list of arguments
        }
        else
        {
            cout << "Error: Invalid syntax for type command.\n";
            cout << "Usage: type [file_path]+ (one or more file paths)\n";
        }
    }

    else if (cmd.name == "write") // Handling 'write' command
    {
        if (cmd.arguments.size() == 1)
        {
            handleWrite(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for write command.\n";
            cout << "Usage: write [file_path] or [file_name]\n";
        }
    }
    else if (cmd.name == "echo")
    {
        if (cmd.arguments.size() == 1)
        {
            handleEcho(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for echo command.\n";
            cout << "Usage: echo [file_path]\n";
        }
    }
    else if (cmd.name == "dir")
    {
        if (cmd.arguments.empty())
        {
            // Type 'dir' without arguments
            handleDir("");
        }
        else if (cmd.arguments.size() == 1)
        {
            // Type 'dir [path]'
            handleDir(cmd.arguments[0]);
        }
        else
        {
            // Invalid syntax
            cout << "Error: Invalid syntax for dir command.\n";
            cout << "Usage:\n"
                << "  dir\n"
                << "  dir [path]\n";
        }
    }
    else if (cmd.name == "import") {
        if (!cmd.arguments.empty()) {
            handleImport(cmd.arguments);
        }
        else {
            cout << "Error: Invalid syntax for import command.\n";
            cout << "Usage:\n  import [source]\n  import [source] [destination]\n";
        }
        }
    else if (cmd.name == "md")
    {
        if (cmd.arguments.size() == 1)
        {
            handleMd(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for md command.\n";
            cout << "Usage: md [directory_name]\n";
        }
    }
   
    else if (cmd.name == "cd")
    {
        if (cmd.arguments.size() <= 1)
        {
            string path = "";
            if (cmd.arguments.size() == 1)
            {
                path = cmd.arguments[0];
            }
            handleCd(path);
        }
        else
        {
            cout << "Error: Invalid syntax for cd command.\n";
            cout << "Usage:\n"
                << "  cd\n"
                << "  cd [directory]\n";
        }
    }
    else if (cmd.name == "pwd")
    {
        handlePwd();
    }
    else if (cmd.name == "history")
    {
        if (cmd.arguments.empty())
        {
            handleHistory();
        }
        else
        {
            // Invalid syntax
            cout << "Error: Invalid syntax for 'history' command.\n";
            cout << "Usage: history\n";
        }
    }
    else if (cmd.name == "cls")
    {
        if (cmd.arguments.empty())
        {
            handleCls();
        }
        else
        {
            // Concatenate the entire input to display the unknown command
            cout << "Error: Unknown command '" << input << "'. Type 'help' to see available commands.\n";
        }
    }

    else if (cmd.name == "export") {

        handleExport(cmd.arguments);
    }

    else if (cmd.name == "rd")
    {
        if (cmd.arguments.empty())
        {
            // Test Case (1): Type 'rd' without arguments
            cout << "Error: Invalid syntax for rd command.\n";
            cout << "Usage: rd [directory]+\n";
        }
        else
        {
            // Handle 'rd' with one or more directory arguments
            handleRd(cmd.arguments);
        }
    }
    else if (cmd.name == "quit")
    {
        if (cmd.arguments.empty())
        {
            handleQuit(isRunning);
        }
        else
        {
            // Concatenate the entire input to display the unknown command
            cout << "Error: Incorrect syntax for 'quit' command.\n";
            cout << "Usage: quit\n";
        }
    }
    else
    {
        cout << "Error: Unknown command '" << cmd.name << "'. Type 'help' to see available commands.\n";
    }
}
void CommandProcessor::showGeneralHelp()
{
    cout << "Available Commands:\n";
    int count = 1;
    for (const auto& cmd : commandHelp)
    {
        cout << "  " <<count<<" " << cmd.first << "            -" << cmd.second.first << "\n";
        count++;
    }
}
void CommandProcessor::showCommandHelp(const string& command)
{
    // Convert command to lowercase to make help case-insensitive
    string cmdLower = command;
    transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(),
        [](unsigned char c) { return tolower(c); });

    auto it = commandHelp.find(cmdLower);
    if (it != commandHelp.end())
    {
        cout << it->second.second << "\n";
    }
    else
    {
        cout << "Error: Command '" << command << "' is not supported.\n";
    }
}
std::string CommandProcessor::toLower(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Convert string to uppercase
std::string CommandProcessor::toUpper(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}
void CommandProcessor::handleCls()
{
    // On Windows, use system("cls") to clear the screen
    system("cls");
}
void CommandProcessor::handleMd(const string& dirPath)
{
    // 1. Parse the path to separate parent path and directory name
    string parentPath;
    string dirName;

    size_t lastSlash = dirPath.find_last_of("/\\");
    if (lastSlash == string::npos)
    {
        // Directory to be created in the current directory
        parentPath = "";
        dirName = dirPath;
    }
    else
    {
        parentPath = dirPath.substr(0, lastSlash);
        dirName = dirPath.substr(lastSlash + 1);
    }

    // 2. Navigate to the parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty())
    {
        parentDir = *currentDirectoryPtr;
    }
    else
    {
        parentDir = MoveToDir(parentPath);
    }

    if (parentDir == nullptr)
    {
        cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
        return;
    }

    // 3. Check if a file or directory with the same name already exists
    for (const auto& entry : parentDir->DirOrFiles)
    {
        if (!entry.getIsFile()) // Ensure we're comparing directories
        {
            std::string existingName = entry.getName();
            std::string newName = dirName;

            // Convert both names to lowercase for comparison
            std::transform(existingName.begin(), existingName.end(), existingName.begin(),
                [](unsigned char c) { return std::tolower(c); });
            std::transform(newName.begin(), newName.end(), newName.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (existingName == newName)
            {
                std::cout << "Error: Directory '" << dirName << "' already exists.\n";
                return;
            }
        }
    }

    // 4. Allocate a new cluster for the directory
    int newCluster = Mini_FAT::getAvailableCluster();
    if (newCluster == -1)
    {
        cout << "Error: No available clusters to create directory.\n";
        return;
    }

    // 5. Initialize the new directory's FAT pointer
    Mini_FAT::setClusterPointer(newCluster, -1); // -1 indicates EOF

    // 6. Clean the directory name without altering case
    string cleanedName = Directory_Entry::cleanTheName(dirName);
    if (cleanedName.empty())
    {
        cout << "Error: Invalid directory name.\n";
        return;
    }

    // 7. Create a new Directory object
    Directory* newDir = new Directory(cleanedName, 0x10, newCluster, parentDir);
    newDir->readDirectory(); // Initialize directory entries (e.g., add '.' and '..')

    // 8. Create a Directory_Entry using the existing constructor
    Directory_Entry newDirEntry(cleanedName, 0x10, newCluster);

    // 9. Manually assign the subDirectory pointer
    newDirEntry.subDirectory = newDir;

    // 10. Add the new directory entry to the parent directory's DirOrFiles
    parentDir->DirOrFiles.push_back(newDirEntry);

    // 11. Write the updated parent directory to the virtual disk (if applicable)
    parentDir->writeDirectory();

    cout << "Directory '" << cleanedName << "' created successfully.\n";
}


void CommandProcessor::handleRd(const vector<string>& directories)
{
    if (directories.empty()) {
        cout << "Error: Invalid syntax for rd command.\n";
        cout << "Usage: rd [directory]+\n";
        return;
    }

    // Iterate over each directory argument
    for (const auto& dirPath : directories) {
        // Confirm deletion
        cout << "Are you sure you want to delete directory '" << dirPath << "'? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore remaining input

        if (choice != 'y' && choice != 'Y') {
            cout << "Skipped deleting directory '" << dirPath << "'.\n";
            continue;
        }

        // Parse the path to separate parent path and directory name
        string parentPath;
        string dirName;

        size_t lastSlash = dirPath.find_last_of("/\\");
        if (lastSlash == string::npos) {
            // Directory in current directory
            parentPath = "";
            dirName = dirPath;
        }
        else {
            parentPath = dirPath.substr(0, lastSlash);
            dirName = dirPath.substr(lastSlash + 1);
        }

        // Navigate to the parent directory
        Directory* parentDir = nullptr;
        if (parentPath.empty()) {
            parentDir = *currentDirectoryPtr;
        }
        else {
            parentDir = MoveToDir(parentPath);
        }

        if (parentDir == nullptr) {
            cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            continue;
        }

        // Search for the directory within the parent directory
        int dirIndex = parentDir->searchDirectory(dirName);
        if (dirIndex == -1) {
            cout << "Error: Directory '" << dirName << "' does not exist.\n";
            continue;
        }

        Directory_Entry dirEntry = parentDir->DirOrFiles[dirIndex];
        if (dirEntry.dir_attr != 0x10) { // 0x10 signifies a directory
            cout << "Error: '" << dirName << "' is not a directory.\n";
            continue;
        }

        // Check if the directory is empty
        if (!dirEntry.subDirectory->isEmpty()) {
            cout << "Error: Directory '" << dirPath << "' is not empty.\n";
            continue;
        }

        // Proceed to delete the directory
        delete dirEntry.subDirectory; // Free memory
        parentDir->DirOrFiles.erase(parentDir->DirOrFiles.begin() + dirIndex);
        parentDir->writeDirectory();

        cout << "Directory '" << dirPath << "' deleted successfully.\n";
    }
}

string toUpper(const string& s)
{
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return toupper(c); });
    return result;
}
void CommandProcessor::handleCd(const string& path)
{
    if (path.empty())
    {
        // Display current directory
        cout << "Current Directory: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        return;
    }

    // Handle special directories '.' and '..'
    if (path == ".")
    {
        // Do nothing
        cout << "Navigating to current directory (no change).\n";
        return;
    }
    else if (path == "..")
    {
        if ((*currentDirectoryPtr)->parent != nullptr)
        {
            *currentDirectoryPtr = (*currentDirectoryPtr)->parent;
            cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        }
        else
        {
            cout << "Error: Already at the root directory.\n";
        }
        return;
    }

    // Determine if the path is absolute
    bool isAbsolute = false;
    Directory* traversalDir = *currentDirectoryPtr;
    string drive = "";
    size_t startIndex = 0;

    // Check if the path starts with a drive letter, e.g., "C:\"
    if (path.length() >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == '\\')
    {
        isAbsolute = true;
        drive = path.substr(0, 2); // e.g., "C:"

        // Convert drive to uppercase for case-insensitive comparison
        drive = toUpper(drive);

        // Traverse up to the root directory
        while (traversalDir->parent != nullptr)
        {
            traversalDir = traversalDir->parent;
        }

        // Debugging Statements

        // Verify the drive letter matches (case-insensitive)
        string traversalDrive = toUpper(traversalDir->name.substr(0, 2));
        if (traversalDrive != drive)
        {
            cout << "Error: Drive '" << drive << "' not found.\n";
            return;
        }

        // Update the path to remove the drive part
        // Example: "C:\omar\omar1" becomes "omar\omar1"
        string updatedPath = path.substr(3); // Skip "C:\"
        // Split the updated path
        vector<string> pathComponents;
        string component;
        stringstream ss(updatedPath);
        while (getline(ss, component, '\\'))
        {
            if (!component.empty())
                pathComponents.push_back(component);
        }

        // Start traversing from the root
        for (const auto& dirName : pathComponents)
        {
            if (dirName == ".")
            {
                // Current directory: do nothing
                continue;
            }
            else if (dirName == "..")
            {
                if (traversalDir->parent != nullptr)
                {
                    traversalDir = traversalDir->parent;
                }
                else
                {
                    cout << "Error: Already at the root directory.\n";
                    return;
                }
            }
            else
            {
                // Search for the specified subdirectory
                int dirIndex = traversalDir->searchDirectory(dirName);
                if (dirIndex == -1)
                {
                    cout << "Error: System cannot find the specified folder '" << dirName << "'.\n";
                    return;
                }

                // Get the Directory_Entry object
                Directory_Entry* subDirEntry = &traversalDir->DirOrFiles[dirIndex];
                if (subDirEntry->dir_attr != 0x10) // 0x10 indicates a directory
                {
                    cout << "Error: '" << dirName << "' is not a directory.\n";
                    return;
                }

                // Assign traversalDir to the existing subDirectory
                traversalDir = subDirEntry->subDirectory;
            }
        }

        // Update the current directory pointer to traversalDir
        *currentDirectoryPtr = traversalDir;
        cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        return;
    }

    // If the path is not absolute, split it normally
    vector<string> pathComponents;
    string componentNonAbsolute;
    stringstream ssNonAbsolute(path);
    while (getline(ssNonAbsolute, componentNonAbsolute, '\\'))
    {
        if (!componentNonAbsolute.empty())
            pathComponents.push_back(componentNonAbsolute);
    }

    // Traverse the path components
    bool errorOccurred = false;
    for (const auto& dirName : pathComponents)
    {
        if (dirName == ".")
        {
            // Current directory: do nothing
            continue;
        }
        else if (dirName == "..")
        {
            // Move to parent directory
            if (traversalDir->parent != nullptr)
            {
                traversalDir = traversalDir->parent;
            }
            else
            {
                cout << "Error: Already at the root directory.\n";
                errorOccurred = true;
                break;
            }
        }
        else
        {
            // Move to specified subdirectory
            int dirIndex = traversalDir->searchDirectory(dirName);
            if (dirIndex == -1)
            {
                cout << "Error: System cannot find the specified folder '" << dirName << "'.\n";
                errorOccurred = true;
                break;
            }

            // Get the Directory_Entry object
            Directory_Entry* subDirEntry = &traversalDir->DirOrFiles[dirIndex];
            if (subDirEntry->dir_attr != 0x10) // 0x10 indicates a directory
            {
                cout << "Error: '" << dirName << "' is not a directory.\n";
                errorOccurred = true;
                break;
            }

            // Assign traversalDir to the existing subDirectory
            traversalDir = subDirEntry->subDirectory;
        }
    }

    if (!errorOccurred)
    {
        // Update the current directory pointer to traversalDir
        *currentDirectoryPtr = traversalDir;
        cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
    }
}
void CommandProcessor::handlePwd()
{

    Directory* currentDir = *currentDirectoryPtr;
    cout << "Current Directory: " << currentDir->getFullPath() << "\n";
}
void CommandProcessor::handleHistory()
{
    if (commandHistory.empty())
    {
        cout << "No commands in history.\n";
        return;
    }

    cout << "Command History:\n";
    for (size_t i = 0; i < commandHistory.size(); ++i)
    {
        cout << i + 1 << ": " << commandHistory[i] << "\n";
    }
}
void CommandProcessor::handleQuit(bool& isRunning)
{
    cout << "************************************************************************************************************************" << endl;
    cout << "                                                    Quit the Shell.                                         " << endl;
    cout << "************************************************************************************************************************" << endl;
    isRunning = false; // Set the flag to false to terminate the shell loop
}
File_Entry* CommandProcessor::MoveToFile(string& path)
{
    // Split the path to get directory path and file name
    size_t lastBackslash = path.find_last_of('\\');
    if (lastBackslash == string::npos) {
        cout << "Error: Invalid file path format.\n";
        return nullptr;
    }

    string dirPath = path.substr(0, lastBackslash);
    string fileName = path.substr(lastBackslash + 1);

    if (fileName.empty()) {
        cout << "Error: File name is empty.\n";
        return nullptr;
    }

    // Navigate to the directory
    Directory* targetDir = MoveToDir(dirPath);
    if (targetDir == nullptr) {
        // Error message already printed in MoveToDir
        return nullptr;
    }

    // Search for the file
    int fileIndex = targetDir->searchDirectory(fileName);
    if (fileIndex == -1) {
        cout << "Error: File '" << fileName << "' not found in '" << targetDir->getFullPath() << "'.\n";
        return nullptr;
    }

    Directory_Entry& fileEntry = targetDir->DirOrFiles[fileIndex];
    if (fileEntry.dir_attr == 0x10) { // 0x10 indicates a directory
        cout << "Error: '" << fileName << "' is a directory.\n";
        return nullptr;
    }

    // Assuming you have a File_Entry class that handles file content
    File_Entry* file = new File_Entry(fileEntry, targetDir);
    file->readFileContent();

    return file;
}
Directory* CommandProcessor::MoveToDir(const std::string& path) {
    // Make a local modifiable copy of the input path
    std::string normalizedPath = path;

    // Normalize path separators to '\\'
    std::replace(normalizedPath.begin(), normalizedPath.end(), '/', '\\');

    // Split the path by '\\'
    std::vector<std::string> dirs;
    std::stringstream ss(normalizedPath);
    std::string token;
    while (std::getline(ss, token, '\\')) {
        if (!token.empty()) {
            dirs.push_back(token);
        }
    }

    // Check if the path is empty
    if (dirs.empty()) {
        std::cout << "Error: Path is empty.\n";
        return nullptr;
    }

    // Start at the current directory
    Directory* current = *currentDirectoryPtr;

    // Handle root navigation (e.g., "C:")
    std::string rootDrive = toUpper(current->getDrive()) + ":";
    if (toUpper(dirs[0]) == toUpper(rootDrive)) {
        while (current->parent != nullptr) {
            current = current->parent; // Move to root
        }
        dirs.erase(dirs.begin()); // Remove root from path
    }

    // Traverse the path
    for (const auto& dirName : dirs) {
        int dirIndex = current->searchDirectory(dirName);
        if (dirIndex == -1) {
            std::cout << "Error: Directory '" << dirName << "' not found in '" << current->getFullPath() << "'.\n";
            return nullptr;
        }

        Directory_Entry& entry = current->DirOrFiles[dirIndex];
        if (entry.dir_attr != 0x10) { // 0x10 represents directory attribute
            std::cout << "Error: '" << dirName << "' is not a directory.\n";
            return nullptr;
        }

        // Move to the subdirectory
        current = entry.subDirectory;
        if (!current) {
            std::cout << "Error: Subdirectory '" << dirName << "' is not accessible.\n";
            return nullptr;
        }

        std::cout << "Navigated to directory: " << current->getFullPath() << "\n";
    }

    return current;
}
void CommandProcessor::handleDir(const std::string& path)
{
    // 1. Determine which directory to list
    Directory* targetDir = *currentDirectoryPtr;

    // Handle special cases for "." and ".."
    if (path == ".") {
        // stay in current directory
    }
    else if (path == "..") {
        if (targetDir->parent != nullptr) {
            targetDir = targetDir->parent;
        }
        else {
            std::cout << "Error: Already at the root directory.\n";
            return;
        }
    }
    else if (!path.empty()) {
        Directory* resolvedDir = MoveToDir(path);
        if (!resolvedDir) {
            // MoveToDir prints an error already
            return;
        }
        targetDir = resolvedDir;
    }

    // 2. Print header
    std::cout << "Directory of " << targetDir->getFullPath() << "\n\n";

    int fileCount = 0;
    int dirCount = 0;
    long long totalSize = 0; // Sum of file sizes

    std::vector<Directory_Entry> directories;
    std::vector<Directory_Entry> files;

    // 3. Separate directories and files
    for (const auto& entry : targetDir->DirOrFiles) {
        if (entry.dir_attr == 0x10) {
            // Directory
            // Optionally skip "." or ".." if stored
            if (entry.getName() == "." || entry.getName() == "..") continue;
            directories.push_back(entry);
        }
        else {
            // File
            files.push_back(entry);
        }
    }

    // 4. Sort alphabetically by name
    auto alphaSort = [](const Directory_Entry& a, const Directory_Entry& b) {
        return a.getName() < b.getName();
        };
    std::sort(directories.begin(), directories.end(), alphaSort);
    std::sort(files.begin(), files.end(), alphaSort);

    // 5. Define column widths for formatting
    const int nameWidth = 40;
    const int sizeWidth = 15;

    // 6. Print column headers
    std::cout << std::left << std::setw(nameWidth) << "Name"
        << std::right << std::setw(sizeWidth) << "Size\n";
    std::cout << std::left << std::setw(nameWidth) << "----"
        << std::right << std::setw(sizeWidth) << "----\n";

    // 7. Display directories first
    for (const auto& d : directories) {
        std::string name = d.getName();
        if (name.empty()) {
            name = "<No Directory Name>";
        }
        std::string attr = "<DIR>";

        // Print directory entry
        std::cout << std::left << std::setw(nameWidth) << (name + " " + attr)
            << std::right << std::setw(sizeWidth) << "-" << "\n";
        dirCount++;
    }

    // 8. Display files
    for (const auto& f : files) {
        std::string name = f.getName();
        if (name.empty()) {
            name = "<No Name>.txt"; // Assuming it's a .txt file
        }
        int size = f.getSize(); // Now valid since getSize() exists

        // Print file entry
        std::cout << std::left << std::setw(nameWidth) << name
            << std::right << std::setw(sizeWidth) << size << " bytes\n";
        fileCount++;
        totalSize += size;
    }

    // 9. Calculate free space (using your Mini_FAT methods)
    long long totalClusters = Mini_FAT::getTotalClusters();
    long long freeClusters = Mini_FAT::getFreeClusters();
    long long clusterSize = Mini_FAT::getClusterSize();
    long long freeSpace = freeClusters * clusterSize;

    // 10. Print summary on two separate lines
    std::cout << "\n"
        << std::left << std::setw(nameWidth) << ""
        << std::right << std::setw(sizeWidth) << fileCount << " File(s)     " << totalSize << " bytes\n"
        << std::left << std::setw(nameWidth) << ""
        << std::right << std::setw(sizeWidth) << dirCount << " Dir(s)      " << freeSpace << " bytes free\n";
}



void CommandProcessor::handleEcho(const std::string& filePath)
{
    // 1. Make a modifiable copy of filePath
    std::string trimmed = filePath;

    // 2. Trim leading spaces
    size_t firstPos = trimmed.find_first_not_of(' ');
    if (firstPos != std::string::npos) {
        trimmed.erase(0, firstPos);
    }
    else {
        trimmed.clear();
    }

    // 3. Trim trailing spaces
    size_t lastPos = trimmed.find_last_not_of(' ');
    if (lastPos != std::string::npos) {
        trimmed.erase(lastPos + 1);
    }
    else {
        trimmed.clear();
    }

    // 4. Check if trimmed is empty
    if (trimmed.empty()) {
        std::cout << "Error: Invalid syntax for echo command.\n"
            << "Usage: echo [file_path]\n";
        return;
    }

    // 5. Parse the path to get parent directory and file name
    std::string parentPath;
    std::string fileName;
    size_t lastSlash = trimmed.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        parentPath = "";
        fileName = trimmed;
    }
    else {
        parentPath = trimmed.substr(0, lastSlash);
        fileName = trimmed.substr(lastSlash + 1);
    }

    // 6. Validate file name
    if (!isValidFileName(fileName)) {
        std::cout << "Error: Invalid file name '" << fileName << "'.\n";
        return;
    }

    // 7. Check for extension and append .txt if missing
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos == fileName.length() - 1) {
        // No extension found or dot is at the start/end => append .txt
        fileName += ".txt";
    }

    // 8. Navigate to parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty()) {
        parentDir = *currentDirectoryPtr;
    }
    else {
        parentDir = MoveToDir(parentPath);
        if (parentDir == nullptr) {
            std::cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            return;
        }
    }

    // 9. Check for duplicates (case-insensitive comparison)
    for (const auto& entry : parentDir->DirOrFiles)
    {
        std::string existingName = entry.getName();
        std::string newName = fileName;

        // Convert both names to lowercase for comparison
        std::transform(existingName.begin(), existingName.end(), existingName.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(newName.begin(), newName.end(), newName.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (existingName == newName)
        {
            std::cout << "Error: File '" << fileName << "' already exists.\n";
            return;
        }
    }

    // 10. Create the file entry with fileName, dir_attr = 0x00 (file)
    Directory_Entry newFileEntry(fileName, 0x00, /*firstCluster=*/0);
    newFileEntry.setIsFile(true); // Mark as a file

    // 11. Add the new file to the parent directory
    parentDir->DirOrFiles.push_back(newFileEntry);

    // 12. Persist changes
    parentDir->writeDirectory();

    std::cout << "File '" << newFileEntry.getName() << "' created successfully.\n";
}





void CommandProcessor::handleWrite(const string& filePath)
{
    // 1. Parse the path to separate parent path and file name
    string parentPath;
    string fileName;

    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash == string::npos)
    {
        // File to be written in the current directory
        parentPath = "";
        fileName = filePath;
    }
    else
    {
        parentPath = filePath.substr(0, lastSlash);
        fileName = filePath.substr(lastSlash + 1);
    }

    // 2. Validate file name
    if (!isValidFileName(fileName))
    {
        cout << "Error: Invalid file name '" << fileName << "'.\n";
        return;
    }

    // 3. Navigate to the parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty())
    {
        parentDir = *currentDirectoryPtr;
    }
    else
    {
        parentDir = MoveToDir(parentPath);
    }

    if (parentDir == nullptr)
    {
        cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
        return;
    }

    // 4. Search for the file in the parent directory
    bool fileFound = false;
    for (auto& entry : parentDir->DirOrFiles) // Iterate by reference
    {
        if (entry.getName() == fileName)
        {

            if (!entry.getIsFile())
            {
                cout << "Error: '" << fileName << "' is a directory, not a file.\n";
                return;
            }

            // 5. Prompt user for input
            cout << "Enter text to write to '" << fileName << "'. Type 'END' on a new line to finish.\n";

            string line;
            string newContent;
            while (true)
            {
                getline(cin, line);
                if (line == "END")
                    break;
                newContent += line + "\n";
            }

            // 6. Update the content
            entry.setContent(newContent);

            // 7. Persist changes
            parentDir->writeDirectory(); // Assuming this writes directory metadata to disk

            cout << "Content written to '" << fileName << "' successfully.\n";
            fileFound = true;
            break;
        }
    }

    if (!fileFound)
    {
        cout << "Error: File '" << fileName << "' does not exist.\n";
    }
}
bool CommandProcessor::isValidFileName(const std::string& name) {
    if (name.empty() || name.length() > 11) {
        return false;
    }

    // Check for invalid characters
    const std::string invalidChars = R"(/\*?"<>|)";
    for (char c : name) {
        if (invalidChars.find(c) != std::string::npos) {
            return false;
        }
    }

    // Prevent names starting with a dot
    if (name[0] == '.') {
        return false;
    }

    // Ensure it follows the 8.3 format for files
    size_t dotPos = name.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string base = name.substr(0, dotPos);
        std::string ext = name.substr(dotPos + 1);
        if (base.empty() || ext.empty() || base.length() > 8 || ext.length() > 3) {
            return false;
        }
    }
    else {
        // For directories, allow up to 11 characters without extension
        if (name.length() > 11) {
            return false;
        }
    }

    return true;
}
void CommandProcessor::handleType(const vector<string>& filePaths) {
    if (filePaths.empty()) {
        cout << "Error: Invalid syntax for type command.\n";
        cout << "Usage: type [file_path]+ (one or more file paths)\n";
        return;
    }

    for (const string& filePath : filePaths) {
        // Parse the path to separate parent path and file name
        string parentPath;
        string fileName;

        size_t lastSlash = filePath.find_last_of("/\\");
        if (lastSlash == string::npos) {
            // File is in the current directory
            parentPath = "";
            fileName = filePath;
        }
        else {
            parentPath = filePath.substr(0, lastSlash);
            fileName = filePath.substr(lastSlash + 1);
        }

        // Navigate to the parent directory
        Directory* parentDir = nullptr;
        if (parentPath.empty()) {
            parentDir = *currentDirectoryPtr;
        }
        else {
            parentDir = MoveToDir(parentPath);
        }

        if (parentDir == nullptr) {
            cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            continue; // Proceed to the next file
        }

        // Search for the file in the parent directory
        bool fileFound = false;
        for (const auto& entry : parentDir->DirOrFiles) {
            if (entry.getName() == fileName) {
                if (!entry.getIsFile()) {
                    cout << "Error: '" << fileName << "' is a directory, not a file.\n";
                    fileFound = true; // Mark as found to avoid general not found message
                    break;
                }

                // File found, display its content
                cout << "Content of '" << fileName << "':\n";
                cout << entry.getContent() << "\n";
                fileFound = true;
                break;
            }
        }

        if (!fileFound) {
            cout << "Error: File '" << fileName << "' does not exist.\n";
        }
    }
}

void CommandProcessor::handleDel(const vector<string>& targets)
{
    if (targets.empty())
    {
        cout << "Error: Invalid syntax for del command.\n";
        cout << "Usage: del [dirFile]+\n";
        return;
    }

    for (const auto& target : targets)
    {
        Directory* parentDir = nullptr;
        Directory_Entry* dirEntry = nullptr;
        string entryName;
        string dirPath;

        // Check if target is a full path or relative
        bool isFullPath = (target.find(":\\") != string::npos || target[0] == '\\');

        if (isFullPath)
        {
            size_t lastSlash = target.find_last_of("\\");
            if (lastSlash == string::npos || lastSlash == target.length() - 1)
            {
                cout << "Error: Invalid path '" << target << "'.\n";
                continue;
            }

            dirPath = target.substr(0, lastSlash);
            entryName = target.substr(lastSlash + 1);
            parentDir = MoveToDir(dirPath);

            if (!parentDir)
            {
                cout << "Error: Path '" << dirPath << "' does not exist.\n";
                continue;
            }
        }
        else
        {
            parentDir = *currentDirectoryPtr;
            entryName = target;
        }

        // Search for the entry in the parent directory
        int entryIndex = parentDir->searchDirectory(entryName);
        if (entryIndex == -1)
        {
            cout << "Error: '" << entryName << "' does not exist in '" << parentDir->getFullPath() << "'.\n";
            continue;
        }

        dirEntry = &parentDir->DirOrFiles[entryIndex];

        if (dirEntry->dir_attr == 0x10) // Directory
        {
            cout << "Are you sure you want to delete all files in the directory '"
                << dirEntry->getName() << "'? (y/n): ";
            char confirmation;
            cin >> confirmation;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (tolower(confirmation) == 'y')
            {
                string fullPath = parentDir->getFullPath() + "\\" + dirEntry->getName();
                Directory* targetDir = MoveToDir(fullPath);

                if (!targetDir)
                {
                    cout << "Error: Could not access the directory '" << dirEntry->getName() << "'.\n";
                    continue;
                }

                // Delete all files in the directory
                for (auto it = targetDir->DirOrFiles.begin(); it != targetDir->DirOrFiles.end();)
                {
                    if (it->dir_attr != 0x10) // Skip subdirectories
                    {
                        string fileName = it->getName();
                        cout << "Are you sure you want to delete the file '" << fileName << "'? (y/n): ";
                        cin >> confirmation;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        if (tolower(confirmation) == 'y')
                        {
                            File_Entry file(*it, targetDir);
                            file.deleteFile();
                            cout << "File '" << fileName << "' deleted successfully.\n";
                            it = targetDir->DirOrFiles.erase(it); // Remove from directory list
                        }
                        else
                        {
                            ++it;
                        }
                    }
                    else
                    {
                        ++it; // Skip subdirectories
                    }
                }

                targetDir->writeDirectory(); // Persist changes
                cout << "All files in the directory '" << dirEntry->getName() << "' have been processed.\n";
            }
            else
            {
                cout << "Skipped deletion of files in directory '" << dirEntry->getName() << "'.\n";
            }
        }
        else // File
        {
            string fileName = dirEntry->getName(); // Ensure file name is retained
            cout << "Are you sure you want to delete the file '" << fileName << "'? (y/n): ";
            char confirmation;
            cin >> confirmation;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (tolower(confirmation) == 'y')
            {
                File_Entry file(*dirEntry, parentDir);
                file.deleteFile();
                cout << "File '" << fileName << "' deleted successfully.\n";

                // Remove from DirOrFiles and persist changes
                parentDir->DirOrFiles.erase(parentDir->DirOrFiles.begin() + entryIndex);
                parentDir->writeDirectory();
            }
            else
            {
                cout << "Skipped deletion of '" << fileName << "'.\n";
            }
        }
    }
}



void CommandProcessor::handleRename(const vector<string>& args)
{
    if (args.size() != 2)
    {
        cout << "Error: Invalid syntax for rename command.\n";
        cout << "Usage: rename [fileName or fullPath] [new fileName]\n";
        return;
    }

    string filePath = args[0];
    string newFileName = args[1];

    // Validate that newFileName does not contain a path
    if (newFileName.find("\\") != string::npos || newFileName.find(":") != string::npos)
    {
        cout << "Error: The new file name should be a file name only. You cannot provide a full path.\n";
        return;
    }

    Directory* targetDir = nullptr;
    string fileName;

    // Determine if the input is a full path or a relative name
    if (filePath.find(":\\") != string::npos || filePath[0] == '\\')
    {
        string path = filePath.substr(0, filePath.find_last_of("\\"));
        targetDir = MoveToDir(path);

        if (!targetDir)
        {
            cout << "Error: File '" << filePath << "' does not exist on your disk.\n";
            return;
        }

        fileName = filePath.substr(filePath.find_last_of("\\") + 1);
    }
    else
    {
        targetDir = *currentDirectoryPtr;
        fileName = filePath;
    }

    // Check if the file exists in the target directory
    int fileIndex = targetDir->searchDirectory(fileName);
    if (fileIndex == -1)
    {
        cout << "Error: File '" << filePath << "' does not exist on your disk.\n";
        return;
    }

    Directory_Entry& fileEntry = targetDir->DirOrFiles[fileIndex];

    // Ensure the entry is a file, not a directory
    if (fileEntry.dir_attr == 0x10)
    {
        cout << "Error: '" << fileName << "' is a directory. Use 'rd' to rename directories.\n";
        return;
    }

    // Check for duplicate file names in the directory
    for (const auto& entry : targetDir->DirOrFiles)
    {
        if (entry.getName() == newFileName)
        {
            cout << "Error: A duplicate file name exists in the directory.\n";
            return;
        }
    }

    // Rename the file
    fileEntry.assignDir_Name(newFileName);
    targetDir->writeDirectory(); // Persist the change to the disk

    cout << "File '" << fileName << "' renamed to '" << newFileName << "' successfully.\n";
}
void CommandProcessor::handleCopy(const vector<string>& args)
{
    // **Case (1): Type copy alone**
    if (args.empty())
    {
        cout << "Error: Invalid syntax for copy command.\n";
        cout << "Usage: copy [source] [destination]\n";
        return;
    }

    string sourcePath = args[0];
    string destinationPath = args.size() > 1 ? args[1] : "";

    // **Parse the Source Path**
    Directory* sourceDir = nullptr;
    string sourceName;
    size_t lastSlash = sourcePath.find_last_of("/\\");

    if (lastSlash == string::npos)
    {
        // Source is in the current directory
        sourceDir = *currentDirectoryPtr;
        sourceName = sourcePath;
    }
    else
    {
        // Source path includes directories
        string sourceParentPath = sourcePath.substr(0, lastSlash);
        sourceName = sourcePath.substr(lastSlash + 1);
        sourceDir = MoveToDir(sourceParentPath);
    }

    // **Check if Source Directory Exists**
    if (!sourceDir)
    {
        // **Case (5): Full path does not exist**
        cout << "Error: Source path '" << sourcePath << "' does not exist.\n";
        return;
    }

    // **Search for the Source Entry**
    int sourceIndex = sourceDir->searchDirectory(sourceName);
    if (sourceIndex == -1)
    {
        // **Case (2): Source file does not exist**
        cout << "Error: Source '" << sourceName << "' does not exist.\n";
        return;
    }

    Directory_Entry& sourceEntry = sourceDir->DirOrFiles[sourceIndex];

    // **Handle File Copying**
    if (sourceEntry.dir_attr == 0x00) // 0x00 indicates a file
    {
        // **Determine Destination Directory and File Name**
        Directory* destinationDir = nullptr;
        string destFileName;

        if (destinationPath.empty())
        {
            // **Default Destination: Current Directory with Same Name**
            destinationDir = *currentDirectoryPtr;
            destFileName = sourceName;
            if (sourceDir == destinationDir && sourceName == destFileName)
            {
                cout << "The file cannot be copied onto itself.\n";
                cout << "0 file(s) copied.\n";
                return;
            }
        }

        else
        {
            // **Check if Destination Path is Absolute**
            bool isAbsolute = false;
            if (destinationPath.size() >= 3 && destinationPath[1] == ':' &&
                (destinationPath[2] == '\\' || destinationPath[2] == '/'))
            {
                isAbsolute = true;
            }

            if (isAbsolute)
            {
                // **Destination Path is Absolute: Extract File Name**
                size_t destLastSlash = destinationPath.find_last_of("/\\");
                if (destLastSlash == string::npos)
                {
                    // **Invalid Absolute Path (No File Name)**
                    cout << "Error: Invalid destination path.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }
                destFileName = destinationPath.substr(destLastSlash + 1);
                destinationDir = *currentDirectoryPtr; // Copy to Current Directory
            }
            else
            {
                // **Destination Path is Relative**
                size_t destLastSlash = destinationPath.find_last_of("/\\");
                if (destLastSlash == string::npos)
                {
                    // **Destination is in Current Directory**
                    destinationDir = *currentDirectoryPtr;
                    destFileName = destinationPath;
                }
                else
                {
                    // **Destination Includes Directory Path**
                    string destParentPath = destinationPath.substr(0, destLastSlash);
                    destFileName = destinationPath.substr(destLastSlash + 1);
                    destinationDir = MoveToDir(destParentPath);
                }
            }
        }

        // **Check if Destination Directory Exists**
        if (!destinationDir)
        {
            // **Case (5): Destination Directory Not Found**
            cout << "Error: Destination directory does not exist.\n";
            cout << "0 file(s) copied.\n";
            return;
        }

        // **Check if Destination is a Directory**
        int destIndex = destinationDir->searchDirectory(destFileName);
        bool destIsDirectory = false;

        if (destIndex != -1)
        {
            // **Destination Exists**
            if (destinationDir->DirOrFiles[destIndex].dir_attr == 0x10)
            {
                // **Destination is a Directory**
                destIsDirectory = true;
                destinationDir = destinationDir->DirOrFiles[destIndex].subDirectory;
                destFileName = sourceName; // Copy with Same Name into Destination Directory
            }
        }

        if (destIsDirectory)
        {
            // **Case (13): Copy File into Destination Directory**
            // **Check if File Already Exists in Destination Directory**
            int existingIndex = destinationDir->searchDirectory(sourceName);
            if (existingIndex != -1)
            {
                // **Case (14): Destination File Exists - Prompt for Overwrite**
                cout << "Error: File with the name '" << sourceName << "' already exists in the destination directory.\n";
                cout << "Do you want to overwrite it? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (tolower(choice) != 'y')
                {
                    cout << "Copy operation canceled for '" << sourceName << "'.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }

                // **Overwrite Existing File**
                Directory_Entry& existingEntry = destinationDir->DirOrFiles[existingIndex];
                existingEntry = sourceEntry; // Assuming Shallow Copy is Sufficient
                cout << "File '" << sourceName << "' overwritten successfully in the destination directory.\n";
                cout << "1 file(s) copied.\n";
                return;
            }

            // **Destination File Does Not Exist - Proceed to Copy**
            Directory_Entry newFileEntry = sourceEntry;
            newFileEntry.assignDir_Name(sourceName); // Assign Same Name

            if (!destinationDir->canAddEntry(newFileEntry))
            {
                // **Case (6): Not Enough Space**
                cout << "Error: Not enough space to copy file '" << sourceName << "'.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            destinationDir->addEntry(newFileEntry);
            cout << "File '" << sourceName << "' copied successfully to the destination directory.\n";
            cout << "1 file(s) copied.\n";
            return;
        }
        else
        {
            // **Destination is a File or Intended to be a File**
            // **Check for Self-Copy**
            if (sourcePath == destinationPath)
            {
                // **Case (3) & (4): Self-Copy Detected**
                cout << "Error: The file cannot be copied onto itself.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            // **Check if Destination File Exists**
            if (destinationDir->searchDirectory(destFileName) != -1)
            {
                // **Case (14): Destination File Exists - Prompt for Overwrite**
                cout << "Error: File with the name '" << destFileName << "' already exists in the destination directory.\n";
                cout << "Do you want to overwrite it? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (tolower(choice) != 'y')
                {
                    cout << "Copy operation canceled for '" << destFileName << "'.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }

                // **Overwrite Existing File**
                Directory_Entry& existingEntry = destinationDir->DirOrFiles[destIndex];
                existingEntry = sourceEntry; // Assuming Shallow Copy is Sufficient
                cout << "File '" << destFileName << "' overwritten successfully.\n";
                cout << "1 file(s) copied.\n";
                return;
            }

            // **Destination File Does Not Exist - Proceed to Copy**
            Directory_Entry newFileEntry = sourceEntry;
            newFileEntry.assignDir_Name(destFileName); // Assign New File Name

            if (!destinationDir->canAddEntry(newFileEntry))
            {
                // **Case (6): Not Enough Space**
                cout << "Error: Not enough space to copy file '" << sourceName << "'.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            destinationDir->addEntry(newFileEntry);
            cout << "File '" << sourceName << "' copied successfully as '" << destFileName << "'.\n";
            cout << "1 file(s) copied.\n";
            return;
        }
    }

    // **Handle Directory Copying**
    if (sourceEntry.dir_attr == 0x10) // 0x10 indicates a directory
    {
        // **Case (7) (BONUS): Copy All Files from Source Directory to Current Directory**
        // **Case (10): Copy All Files from Source Directory to Existing Destination Directory**
        // **Case (14): Handle Overwrite Scenarios**

        Directory* destinationDir = nullptr;
        string destDirName;

        if (destinationPath.empty())
        {
            // **Case (7) (BONUS): No Destination Path Provided - Copy to Current Directory**
            destinationDir = *currentDirectoryPtr;
        }
        else
        {
            // **Destination Path Provided - Determine if Directory Exists**
            size_t destLastSlash = destinationPath.find_last_of("/\\");
            if (destLastSlash == string::npos)
            {
                // **Destination is in Current Directory**
                destinationDir = *currentDirectoryPtr;
                destDirName = destinationPath;
            }
            else
            {
                // **Destination Includes Directory Path**
                string destParentPath = destinationPath.substr(0, destLastSlash);
                destDirName = destinationPath.substr(destLastSlash + 1);
                destinationDir = MoveToDir(destParentPath);
            }
        }

        if (!destinationDir)
        {
            // **Case (9): Destination Directory Not Found**
            cout << "Error: Destination directory '" << destinationPath << "' does not exist.\n";
            return;
        }

        // **Check if Destination Path is an Existing Directory**
        bool destIsDirectory = false;
        if (!destDirName.empty())
        {
            int destIndex = destinationDir->searchDirectory(destDirName);
            if (destIndex != -1 && destinationDir->DirOrFiles[destIndex].dir_attr == 0x10)
            {
                // **Destination is an Existing Directory**
                destIsDirectory = true;
                destinationDir = destinationDir->DirOrFiles[destIndex].subDirectory;
            }
            else if (destIndex != -1 && destinationDir->DirOrFiles[destIndex].dir_attr != 0x10)
            {
                // **Destination Exists but is Not a Directory**
                cout << "Error: Destination path '" << destinationPath << "' is not a directory.\n";
                return;
            }
            else
            {
                // **Destination Directory Does Not Exist**
                cout << "Error: Destination directory '" << destinationPath << "' does not exist.\n";
                return;
            }
        }

        // **Iterate Through Source Directory Entries and Copy Files**
        int filesCopied = 0;
        for (const auto& entry : sourceEntry.subDirectory->DirOrFiles)
        {
            if (entry.dir_attr == 0x00) // Only Copy Files
            {
                string srcFileName = entry.getName();
                int destIndex = destinationDir->searchDirectory(srcFileName);

                if (destIndex != -1)
                {
                    // **Case (14): Destination File Exists - Prompt for Overwrite**
                    cout << "Error: File with the name '" << srcFileName << "' already exists in the destination directory.\n";
                    cout << "Do you want to overwrite it? (y/n): ";
                    char choice;
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (tolower(choice) != 'y')
                    {
                        cout << "Copy operation skipped for '" << srcFileName << "'.\n";
                        continue;
                    }

                    // **Overwrite Existing File**
                    Directory_Entry& existingEntry = destinationDir->DirOrFiles[destIndex];
                    existingEntry = entry; // Assuming Shallow Copy is Sufficient
                    cout << "File '" << srcFileName << "' overwritten successfully in destination directory.\n";
                    filesCopied++;
                    continue;
                }

                // **Destination File Does Not Exist - Proceed to Copy**
                Directory_Entry newFileEntry = entry;
                newFileEntry.assignDir_Name(srcFileName); // Assign Same Name

                if (!destinationDir->canAddEntry(newFileEntry))
                {
                    // **Case (6): Not Enough Space**
                    cout << "Error: Not enough space to copy file '" << srcFileName << "'.\n";
                    continue;
                }

                destinationDir->addEntry(newFileEntry);
                cout << "File '" << srcFileName << "' copied successfully to destination directory.\n";
                filesCopied++;
            }
            // **Note**: Skipping subdirectories as per initial requirements
        }

        // **Output Summary of Copied Files**
        cout << filesCopied << " file(s) copied from directory '" << sourceName << "'.\n";
        return;
    }

    // **Unsupported Entry Type**
    cout << "Error: Unsupported entry type for '" << sourceName << "'.\n";
}


void CommandProcessor::handleImport(const std::vector<std::string>& args) {
    // Check for correct number of arguments
    if (args.empty() || args.size() > 2) {
        // Syntax error
        std::cout << "Error: Invalid syntax for import command.\n";
        std::cout << "Usage:\n  import [source]\n  import [source] [destination]\n";
        return;
    }

    std::string source = args[0];
    std::string destination = (args.size() == 2) ? args[1] : "";

    // Resolve the source path using filesystem library
    fs::path sourcePath(source);
    if (!sourcePath.is_absolute()) {
        // First, check in the current directory of the virtual disk
        fs::path currentDirPath = fs::path((*currentDirectoryPtr)->getFullPath());
        fs::path potentialPath = currentDirPath / sourcePath;
        if (fs::exists(potentialPath)) {
            sourcePath = potentialPath;
        }
        else {
            // If not found, check in the debug folder
            // Adjust the debug folder path as per your project's structure
            fs::path debugFolder = "C:\\Users\\omara\\Desktop\\OSOS-master\\OSOS-master\\shell\\x64\\Debug\\";
            potentialPath = debugFolder / sourcePath;
            if (fs::exists(potentialPath)) {
                sourcePath = potentialPath;
            }
            else {
                // Source file or directory not found in both locations
                std::cout << "Error: Source file or directory '" << source << "' does not exist.\n";
                return;
            }
        }
    }

    // Proceed if the source exists
    if (!fs::exists(sourcePath)) {
        std::cout << "Error: Source file or directory '" << source << "' does not exist.\n";
        return;
    }

    // Initialize import counter
    int importedFileCount = 0;

    // Handle if source is a directory: import all .txt files
    if (fs::is_directory(sourcePath)) {
        Directory* targetDir = *currentDirectoryPtr; // Start with current directory

        // Determine the target directory based on destination
        if (!destination.empty()) {
            fs::path destPath(destination);
            // Normalize destination path by removing trailing slashes
            destPath = destPath.lexically_normal();

            if (destPath.is_absolute()) {
                if (fs::exists(destPath)) {
                    if (fs::is_directory(destPath)) {
                        // Destination is an existing directory: navigate to it
                        Directory* destDir = MoveToDir(destPath.string());
                        if (destDir == nullptr) {
                            std::cout << "Error: Destination directory '" << destination << "' does not exist or is not correctly linked.\n";
                            return;
                        }
                        targetDir = destDir;
                    }
                    else {
                        std::cout << "Error: Destination path '" << destination << "' is not a directory.\n";
                        return;
                    }
                }
                else {
                    // Destination directory does not exist: need to create it
                    // Extract parent path and directory name
                    fs::path parentPath = destPath.parent_path();
                    std::string dirName = destPath.filename().string();
                    if (dirName.empty()) {
                        std::cout << "Error: Destination directory name is empty.\n";
                        return;
                    }

                    Directory* parentDir = nullptr;
                    if (parentPath.empty()) {
                        // Destination is directly under the current directory
                        parentDir = *currentDirectoryPtr;
                    }
                    else {
                        // Navigate to the parent directory
                        parentDir = MoveToDir(parentPath.string());
                        if (parentDir == nullptr) {
                            std::cout << "Error: Parent directory '" << parentPath.string() << "' does not exist.\n";
                            return;
                        }
                    }

                    // Check if directory with the same name already exists in parent
                    bool dirExists = false;
                    Directory* existingDir = nullptr;
                    for (const auto& entry : parentDir->DirOrFiles) {
                        if (!entry.getIsFile() && toLower(entry.getName()) == toLower(dirName)) {
                            dirExists = true;
                            existingDir = entry.subDirectory;
                            break;
                        }
                    }

                    if (dirExists && existingDir != nullptr) {
                        std::cout << "Directory '" << destination << "' already exists.\n";
                        targetDir = existingDir;
                    }
                    else {
                        // Create the directory
                        char dir_attr = 0x10;               // Directory attribute
                        int dir_firstCluster = 0;           // First cluster (use 0 if not applicable)
                        Directory* newDir = new Directory(dirName, dir_attr, dir_firstCluster, parentDir);

                        // Create a new Directory_Entry for the new directory
                        Directory_Entry newDirEntry(dirName, 0x10, 0);
                        newDirEntry.setIsFile(false);          // Mark as directory
                        newDirEntry.subDirectory = newDir;     // Link the Directory object

                        // Add the new directory entry to the parent directory
                        parentDir->addEntry(newDirEntry);
                        parentDir->writeDirectory();           // Persist changes

                        targetDir = newDir;                    // Set the target directory to the newly created directory
                        std::cout << "Directory '" << destination << "' created successfully.\n";
                    }
                }
            }
            else {
                // Destination is a relative directory name (e.g., "omar")
                std::string dirName = destPath.string();
                if (dirName.empty()) {
                    std::cout << "Error: Destination directory name is empty.\n";
                    return;
                }

                // Check if the directory already exists in the current directory
                bool dirExists = false;
                Directory* existingDir = nullptr;
                for (const auto& entry : targetDir->DirOrFiles) {
                    if (!entry.getIsFile() && toLower(entry.getName()) == toLower(dirName)) {
                        dirExists = true;
                        existingDir = entry.subDirectory;
                        break;
                    }
                }

                if (dirExists && existingDir != nullptr) {
                    std::cout << "Directory '" << destination << "' already exists.\n";
                    targetDir = existingDir;
                }
                else {
                    // Create the directory
                    char dir_attr = 0x10;               // Directory attribute
                    int dir_firstCluster = 0;           // First cluster (use 0 if not applicable)
                    Directory* newDir = new Directory(dirName, dir_attr, dir_firstCluster, targetDir);

                    // Create a new Directory_Entry for the new directory
                    Directory_Entry newDirEntry(dirName, 0x10, 0);
                    newDirEntry.setIsFile(false);          // Mark as directory
                    newDirEntry.subDirectory = newDir;     // Link the Directory object

                    // Add the new directory entry to the target directory
                    targetDir->addEntry(newDirEntry);
                    targetDir->writeDirectory();           // Persist changes

                    targetDir = newDir;                    // Set the target directory to the newly created directory
                    std::cout << "Directory '" << destination << "' created successfully.\n";
                }
            }
        }

        // Iterate over the source directory and import .txt files
        for (const auto& entry : fs::directory_iterator(sourcePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string fileName = entry.path().filename().string();

                // Validate file name
                const std::string invalidChars = "\\/:*?\"<>|";
                bool isValid = true;
                for (char c : fileName) {
                    if (invalidChars.find(c) != std::string::npos) {
                        isValid = false;
                        break;
                    }
                }
                if (fileName.empty() || !isValid) {
                    // Assign a default name if necessary
                    std::string extension = entry.path().extension().string();
                    if (extension.empty()) {
                        std::cout << "Error: File '" << entry.path().string() << "' has no valid name or extension. Skipping import.\n";
                        continue;
                    }
                    fileName = "<No Name>" + extension;
                    std::cout << "Warning: File name was empty or invalid. Assigned default name '" << fileName << "'.\n";
                }

                // Check if the file already exists in the target directory
                bool fileExists = false;
                int existingFileIndex = -1;
                for (size_t i = 0; i < targetDir->DirOrFiles.size(); ++i) {
                    // Case-insensitive comparison
                    if (toLower(targetDir->DirOrFiles[i].getName()) == toLower(fileName)) {
                        fileExists = true;
                        existingFileIndex = static_cast<int>(i);
                        break;
                    }
                }

                if (fileExists) {
                    // Prompt to overwrite
                    std::cout << "File '" << fileName << "' already exists. Do you want to overwrite it? (yes/no): ";
                    std::string userChoice;
                    std::getline(std::cin, userChoice);
                    std::transform(userChoice.begin(), userChoice.end(), userChoice.begin(), ::tolower);
                    if (userChoice != "yes") {
                        std::cout << "Skipped importing '" << fileName << "'.\n";
                        continue;
                    }
                }

                // Read the content of the source file
                std::ifstream inputFile(entry.path(), std::ios::binary);
                if (!inputFile.is_open()) {
                    std::cout << "Error: Unable to open source file '" << entry.path().string() << "'. Skipping import.\n";
                    continue;
                }
                std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
                inputFile.close();

                // Debug Statement

                if (fileExists && existingFileIndex != -1) {
                    // Overwrite the existing file's content
                    Directory_Entry& existingEntry = targetDir->DirOrFiles[existingFileIndex];
                    existingEntry.setContent(fileContent);
                    existingEntry.setIsFile(true);      // Ensure it's marked as a file
                    existingEntry.dir_attr = 0x00;      // Ensure dir_attr is set correctly
                    targetDir->writeDirectory();
                    std::cout << "File '" << fileName << "' overwritten and imported successfully.\n";
                }
                else {
                    // Create a new file entry
                    Directory_Entry newFile(fileName, 0x00, static_cast<int>(fileContent.size())); // attr=0x00 for file
                    newFile.setIsFile(true);                      // Mark as file
                    newFile.dir_attr = 0x00;                      // Ensure dir_attr is set correctly
                    newFile.setContent(fileContent);              // Set file content and update size
                    targetDir->addEntry(newFile);                 // Add to directory
                    targetDir->writeDirectory();                   // Write changes
                    std::cout << "File '" << fileName << "' imported successfully.\n";
                    importedFileCount++;
                }
            }
        }

        // Display the total number of files imported
        std::cout << "Total files imported: " << importedFileCount << "\n";
    }
}



void CommandProcessor::handleExport(const std::vector<std::string>& args) {
    if (args.size() < 1 || args.size() > 2) {
        std::cout << "Error: Invalid syntax for export command.\n";
        std::cout << "Usage: export [source_file_or_directory] [destination_file_or_directory]\n";
        return;
    }

    std::string sourcePath = args[0];
    std::string destinationPath = (args.size() == 2) ? args[1] : fs::current_path().string();

    Directory* currentDir = *currentDirectoryPtr;
    Directory_Entry* sourceEntry = nullptr;

    bool isSourceAbsolutePath = (sourcePath.length() >= 3 && isalpha(sourcePath[0]) && sourcePath[1] == ':' && (sourcePath[2] == '\\' || sourcePath[2] == '/'));

    // Resolve source path
    if (isSourceAbsolutePath) {
        std::string dirPath = sourcePath.substr(0, sourcePath.find_last_of("\\/"));
        std::string entryName = sourcePath.substr(sourcePath.find_last_of("\\/") + 1);

        Directory* resolvedDir = MoveToDir(dirPath);
        if (!resolvedDir) {
            std::cout << "Error: Directory '" << dirPath << "' does not exist.\n";
            return;
        }

        int entryIndex = resolvedDir->searchDirectory(entryName);
        if (entryIndex == -1) {
            std::cout << "Error: File or directory '" << entryName << "' does not exist in '" << dirPath << "'.\n";
            return;
        }

        sourceEntry = &resolvedDir->DirOrFiles[entryIndex];
    }
    else {
        int entryIndex = currentDir->searchDirectory(sourcePath);
        if (entryIndex == -1) {
            std::cout << "Error: File or directory '" << sourcePath << "' does not exist in the current directory.\n";
            return;
        }

        sourceEntry = &currentDir->DirOrFiles[entryIndex];
    }

    int exportedFiles = 0; // Counter for exported files

    // Check if source is a directory
    if (sourceEntry->dir_attr == 0x10) { // Directory
        Directory sourceDir(sourceEntry->getName(), sourceEntry->dir_attr, sourceEntry->dir_firstCluster, currentDir);
        sourceDir.readDirectory();

        for (const auto& entry : sourceDir.DirOrFiles) {
            if (entry.dir_attr != 0x10) { // Export files only
                File_Entry file(entry, &sourceDir);
                file.readFileContent();

                std::string destinationFilePath = (fs::path(destinationPath) / entry.getName()).string();

                // Check for overwrite
                if (fs::exists(destinationFilePath)) {
                    std::cout << "File '" << destinationFilePath << "' already exists. Overwrite? (yes/no): ";
                    std::string choice;
                    std::getline(std::cin, choice);
                    std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
                    if (choice != "yes") {
                        std::cout << "Skipping '" << entry.getName() << "'.\n";
                        continue;
                    }
                }

                std::ofstream outFile(destinationFilePath, std::ios::binary);
                if (!outFile.is_open()) {
                    std::cout << "Error: Unable to open destination file '" << destinationFilePath << "'.\n";
                    continue;
                }

                outFile.write(file.content.c_str(), file.content.size());
                outFile.close();

                exportedFiles++;
            }
        }

        std::cout << "Total files exported from '" << sourceDir.getFullPath() << "': " << exportedFiles << "\n";
        return;
    }

    // If source is a single file
    if (sourceEntry->dir_attr != 0x10) {
        File_Entry file(*sourceEntry, currentDir);
        file.readFileContent();

        std::string destinationFilePath = destinationPath;
        if (fs::is_directory(destinationPath)) {
            destinationFilePath = (fs::path(destinationPath) / sourceEntry->getName()).string();
        }

        // Check for overwrite
        if (fs::exists(destinationFilePath)) {
            std::cout << "File '" << destinationFilePath << "' already exists. Overwrite? (yes/no): ";
            std::string choice;
            std::getline(std::cin, choice);
            std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
            if (choice != "yes") {
                std::cout << "Export canceled for '" << sourceEntry->getName() << "'.\n";
                return;
            }
        }

        std::ofstream outFile(destinationFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cout << "Error: Unable to open destination file '" << destinationFilePath << "'.\n";
            return;
        }

        outFile.write(file.content.c_str(), file.content.size());
        outFile.close();

        exportedFiles++;
        std::cout << "File '" << sourceEntry->getName() << "' exported successfully to '" << destinationFilePath << "'.\n";
        std::cout << "Total files exported: " << exportedFiles << "\n";
        return;
    }
}
