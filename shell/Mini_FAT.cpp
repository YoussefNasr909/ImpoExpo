#include "Mini_FAT.h"
#include "Converter.h"
#include "virtual_Disk.h"
#include <cstring>
using namespace std;

int Mini_FAT::FAT[1024];  // FAT array representing cluster state

// Initializes the FAT array; sets reserved clusters to -1, free clusters to 0
void Mini_FAT::initialize_FAT() {
    for (int i = 0; i < 1024; i++)
    {
        if (i == 0 || i == 4)
        {
            FAT[i] = -1;
        }
        else if (i > 0 && i <= 3)
        {
            FAT[i] = i + 1;
        }
        else
        {
            FAT[i] = 0;
        }
    }
}



// Prints the current state of the FAT array
void Mini_FAT::printFAT()
{
    cout << "FAT has the following: ";
    for (int i = 0; i < 1024; i++)
        cout << "FAT[" << i << "] = " << Mini_FAT::FAT[i] << endl;
}

// Creates a superblock (vector) by copying the FAT array into bytes
vector<char> Mini_FAT::createSuperBlock()
{
    vector<char> superBlock(1024, 0);  // Create vector with space for 1024 FAT entries
    return superBlock;
}

// Writes the FAT array to the virtual disk by splitting it into clusters
void Mini_FAT::writeFAT()
{
    vector<char> FATBYTES = Converter::intArrayToByteArray(Mini_FAT::FAT, 1024);
    vector<vector<char>> ls = Converter::splitBytes(FATBYTES);
    for (int i = 0; i < ls.size(); i++)
    {
        Virtual_Disk::writeCluster(ls[i], i + 1);
    }
}
// Reads the FAT array from the virtual disk (clusters 1-4) and reconstructs it
void Mini_FAT::readFAT()
{
    vector<char> ls;
    for (int i = 1; i <= 4; i++)
    {
        vector<char> b = Virtual_Disk::readCluster(i);
        ls.insert(ls.end(), b.begin(), b.end());
    }
    Converter::byteArrayToIntArray(Mini_FAT::FAT, ls);
}

// Sets the FAT array with a provided array of integers
void Mini_FAT::setFAT(const int fat_array[1024]) {
    memcpy(FAT, fat_array, 1024 * sizeof(int));  // Copy input FAT array to the FAT array
}

// Initializes or opens the file system. If the disk file doesn't exist, it creates it
void Mini_FAT::initialize_Or_Open_FileSystem( string name) {
    Virtual_Disk::createOrOpenDisk(name);
    if (Virtual_Disk::isNew())
    {
        vector<char> superBlock = Mini_FAT::createSuperBlock();
        Virtual_Disk::writeCluster(superBlock, 0);
        Mini_FAT::initialize_FAT();
        Mini_FAT::writeFAT();
    }
    else
    {
        Mini_FAT::readFAT();
    }
}

// Returns the number of free clusters in the FAT array
int Mini_FAT::getAvailableCluster()
{
    for (int i = 0; i < 1024; i++)
    {
        if (Mini_FAT::FAT[i] == 0)
            return i;
    }
    return -1;//our disk is full
}

//Returns the index of the first free cluster in the FAT array
int Mini_FAT::getAvailableClusters()
{
    int counter = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (Mini_FAT::FAT[i] == 0)
            counter++;
    }
    return counter;
}


// Sets the pointer (next cluster) for a given cluster index in the FAT
void Mini_FAT::setClusterPointer(int clusterIndex, int status)
{
    if (clusterIndex >= 0 && clusterIndex < 1024 && status >= 0 && status < 1024)
        Mini_FAT::FAT[clusterIndex] = status;
}

// Retrieves the pointer (next cluster) for a given cluster index in the FAT
int Mini_FAT::getClusterPointer(int clusterIndex)
{
    if (clusterIndex >= 0 && clusterIndex < 1024)
        return Mini_FAT::FAT[clusterIndex];
    else
        return -1;
}

// Returns the total free space available on the disk (in bytes)
int Mini_FAT::getFreeSize()
{
    return Mini_FAT::getAvailableClusters() * 1024;
}

void Mini_FAT::CloseTheSystem()
{
    Mini_FAT::writeFAT();
    Virtual_Disk::closeDisk();
}


long long Mini_FAT::getTotalClusters() {
    return  1024;
}

long long Mini_FAT::getFreeClusters() {
    long long count = 0;
    for (long long i = 0; i < getTotalClusters(); ++i) {
        if (FAT[i] == 0) { // Assuming 0 indicates a free cluster
            count++;
        }
    }
    return count;
}

long long Mini_FAT::getClusterSize() {
    return 1024; // Example cluster size: 4KB
}