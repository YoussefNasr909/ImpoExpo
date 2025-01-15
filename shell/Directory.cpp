#include "Directory.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>
using namespace std;

Directory::Directory(string name, char dir_attr, int dir_firstCluster, Directory* pa)
    : Directory_Entry(name, dir_attr, dir_firstCluster)  
{
    this-> parent = pa;
}


Directory_Entry Directory::GetDirectory_Entry()
{
    Directory_Entry M(this->dir_name, this->dir_attr, this->dir_firstCluster);
    for (int i = 0; i < 12; i++)
    {
        M.dir_empty[i] = this->dir_empty[i];
    }
    M.dir_fileSize = this->dir_fileSize;
    return M;
}

int Directory::getmySizeOnDisk()
{
    int size = 0;
    if (dir_firstCluster != 0)
    {
        int cluster = dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        do
        {
            size++;
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);
    }
    return size;
}

bool Directory::canAddEntry(Directory_Entry d)
{
    bool can = false;
    int neededSize = (DirOrFiles.size() + 1) * 32;
    int neededCluster = neededSize / 1024;
    int rem = neededSize % 1024;
    if (rem > 0) neededCluster++;
    neededCluster += d.dir_fileSize / 1024;
    int rem1 = d.dir_fileSize % 1024;
    if (rem1 > 0) neededCluster++;
    if (getmySizeOnDisk() + Mini_FAT::getAvailableClusters() >= neededCluster)
        can = true;
    return can;
}

void Directory::emptymyClusters()
{
    if (this->dir_firstCluster != 0)
    {
        int cluster = this->dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        if (cluster == 5 && next == 0)
            return;
        do
        {
            Mini_FAT::setClusterPointer(cluster, 0);
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);
    }
}

void Directory::updatecontent(Directory_Entry OLD, Directory_Entry New)
{
    readDirectory();
    int index = searchDirectory(OLD.dir_name);
    if (index != -1)
    {
        DirOrFiles[index] = New;
        writeDirectory();
    }
}

void Directory::removeEntry(Directory_Entry d)
{
    auto it = find_if(DirOrFiles.begin(), DirOrFiles.end(), [&](const Directory_Entry& entry) {
        return entry.getName() == d.getName();
        });
    if (it != DirOrFiles.end()) {
        DirOrFiles.erase(it);
        writeDirectory();
    }
    
}

void Directory::addEntry(Directory_Entry d)
{
    DirOrFiles.push_back(d);
    writeDirectory();
}

void Directory::deletDirectory()
{
    emptymyClusters();
    if (this->parent != nullptr)
    {
        this->parent->removeEntry(GetDirectory_Entry());
    }
}

int Directory::searchDirectory( string name)
{
    for (int i = 0; i < DirOrFiles.size(); i++)
    {
        string entryName = DirOrFiles[i].getName();
        if (entryName == name) // Case-sensitive comparison
            return i;
    }
    return -1;
}


void Directory::readDirectory() {
    if (this->dir_firstCluster != 0)
    {
        DirOrFiles.clear();
        int cluster = this->dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        if (cluster == 5 && next == 0)
            return;
        vector<char> ls;
        do
        {
            vector<char> clusterData = Virtual_Disk::readCluster(cluster);
            ls.insert(ls.end(), clusterData.begin(), clusterData.end());
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);

        DirOrFiles = Converter::BytesToDirectory_Entries(ls);
    }

}

void Directory::writeDirectory()
{
    Directory_Entry A = this->GetDirectory_Entry();
    if (!this->DirOrFiles.empty())
    {
        vector<char> dirsOrFilesBytes = Converter::Directory_EntriesToBytes(this->DirOrFiles);
        vector<vector<char>> bytesList = Converter::splitBytes(dirsOrFilesBytes);
        int clusterFATIndex;
        if (this->dir_firstCluster != 0)
        {
            this->emptymyClusters();
            clusterFATIndex = Mini_FAT::getAvailableCluster();
            this->dir_firstCluster = clusterFATIndex;
        }
        else
        {
            clusterFATIndex = Mini_FAT::getAvailableCluster();
            if (clusterFATIndex != 0)
                this->dir_firstCluster = clusterFATIndex;
        }
        int lastCluster = -1;
        for (int i = 0; i < bytesList.size(); i++)
        {
            if (clusterFATIndex != -1)
            {
                Virtual_Disk::writeCluster(bytesList[i], clusterFATIndex);
                Mini_FAT::setClusterPointer(clusterFATIndex, -1);
                if (lastCluster != -1)
                    Mini_FAT::setClusterPointer(lastCluster, clusterFATIndex);
                lastCluster = clusterFATIndex;
                clusterFATIndex = Mini_FAT::getAvailableCluster();
            }
        }
    }
    if (this->DirOrFiles.empty())
    {
        if (dir_firstCluster != 0)
            this->emptymyClusters();
        if (parent != nullptr)
            this->dir_firstCluster = 0;
    }
    Directory_Entry B = this->GetDirectory_Entry();
    if (this->parent != nullptr)
    {
        this->parent->updatecontent(A, B);
    }

    Mini_FAT::writeFAT();
}

string Directory::getFullPath() const
{
    if (parent == nullptr)
    {
        // Root directory using getDrive()
        string drive = getDrive();
        if (drive.empty()) {
            cout << "Error: Drive letter not found for root directory.\n";
            return "";
        }
        return drive + ":\\";
    }
    else
    {
        string parentPath = parent->getFullPath();

        // Ensure parentPath ends with a backslash
        if (!parentPath.empty() && parentPath.back() != '\\')
            parentPath += "\\";

        string currentName = getName();

        // Validate currentName to prevent malformed paths
        if (currentName.empty()) {
            cout << "Error: Current directory name is empty.\n";
            return parentPath; // Return parent path as a fallback
        }

        return parentPath + currentName;
    }
}
Directory_Entry Directory::findSubDirectory(const string& dirname)
{
    // Search for a subdirectory with the given name
    for (const auto& entry : DirOrFiles)
    {
        if (entry.dir_attr == 0x10 && entry.getName() == dirname)
        {
            return entry;
        }
    }
    // Return a default entry if not found
    Directory_Entry emptyEntry;
    return emptyEntry;
}

Directory* Directory::getDirectoryByPath(const string& path)
{
    if (path.empty())
    {
        return this;
    }

    vector<string> pathComponents;
    string component;
    stringstream ss (path);
    while (getline(ss, component, '/'))
    {
        if (!component.empty())
            pathComponents.push_back(component);
    }

    Directory* traversalDir = this;

    for (const auto& dirName : pathComponents)
    {
        if (dirName == ".")
        {
            // Current directory: do nothing
            continue;
        }
        else if (dirName == "..")
        {
            // Parent directory
            if (traversalDir->parent != nullptr)
            {
                traversalDir = traversalDir->parent;
            }
            else
            {
                // Reached root; cannot go up further
                return nullptr;
            }
        }
        else
        {
            // Subdirectory
            int dirIndex = traversalDir->searchDirectory(dirName);
            if (dirIndex == -1)
            {
                // Subdirectory not found
                return nullptr;
            }

            Directory_Entry subDirEntry = traversalDir->DirOrFiles[dirIndex];
            if (subDirEntry.dir_attr != 0x10)
            {
                // Not a directory
                return nullptr;
            }

            // Load the subdirectory
            traversalDir = new Directory(subDirEntry.getName(), subDirEntry.dir_attr, subDirEntry.dir_firstCluster, traversalDir);
            traversalDir->readDirectory();
        }
    }

    return traversalDir;
}
string Directory::getDrive() const
{
    if (getName().length() == 2 && getName()[1] == ':') {
        string drive = getName().substr(0, 1);
        transform(drive.begin(), drive.end(), drive.begin(), ::toupper);
        return drive;
    }
    else {
        // Traverse up to find the root drive
        const Directory* current = this;
        while (current->parent != nullptr) {
            current = current->parent;
            if (current->getName().length() == 2 && current->getName()[1] == ':') {
                string drive = current->getName().substr(0, 1);
                transform(drive.begin(), drive.end(), drive.begin(), ::toupper);
                return drive;
            }
        }
        return ""; // No drive found
    }
}


bool Directory::isEmpty() const {
    return DirOrFiles.empty();
}
