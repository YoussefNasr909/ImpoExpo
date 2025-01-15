#include "Virtual_Disk.h"
using namespace std;

// Initialize the static file stream object for the virtual disk
fstream Virtual_Disk::Disk;

// Functions
void Virtual_Disk::createOrOpenDisk(const string& path) {
    Disk.open(path, ios::in | ios::out | ios::binary);

    if (!Disk.is_open()) {
        Disk.open(path, ios::in | ios::out | ios::binary | ios::trunc);

        
    }
}




void Virtual_Disk::writeCluster(const vector<char>& cluster, int clusterIndex)
{
    // Move the write pointer to the position of the specified cluster index
    Disk.seekp(clusterIndex * 1024, ios::beg);
   


    // Write the 1024 bytes of data from the vector to the disk at the current position
    Disk.write(cluster.data(), 1024);

    // If the write operation fails, display an error
   

    // Flush the stream to ensure data is written to the disk
    Disk.flush();
    
}

vector<char> Virtual_Disk::readCluster(int clusterIndex)
{
    /*
    Moves the file read pointer to the beginning of the specified cluster.
    The cluster is 1024 bytes, and we move the pointer by multiplying the
    cluster index by 1024 (the size of one cluster).
    */
    Disk.seekg(clusterIndex * 1024, ios::beg);
    

    // Create a vector to hold the 1024 bytes of data we will read from the disk
    vector<char> bytes(1024);

    /*
    Reads the 1024 bytes of data starting from the current position of the read pointer
    and fills the 'bytes' vector with the data.
    */
    Disk.read(bytes.data(), 1024);

    // Check if the read operation was successful
    
    // Return the vector containing the data read from the cluster
    return bytes;
}

bool Virtual_Disk::isNew()
{
    // Move the file pointer to the end of the file to determine its size
    Disk.seekg(0, ios::end);

    // Get the current position of the read pointer, which represents the size of the file
    int size = static_cast<int>(Disk.tellg());

    // If the file size is zero, it means the disk is new (empty)
    return (size == 0);
}

void Virtual_Disk::closeDisk()
{
    if (Disk.is_open()) {
        Disk.close();
    }
}