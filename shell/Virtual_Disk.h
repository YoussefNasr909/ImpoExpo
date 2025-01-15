#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/** Simulates a virtual disk with functions to read/write clusters and handle the disk file. */
class Virtual_Disk
{
public:
    /** Creates or opens a virtual disk file. If not exists, creates it. */
    static void createOrOpenDisk(const string& path);

    /** Writes a 1024-byte cluster to the virtual disk at the specified index. */
    static void writeCluster(const vector<char>& cluster, int clusterIndex);

    /** Reads a 1024-byte cluster from the virtual disk at the specified index. */
    static vector<char> readCluster(int clusterIndex);

    /** Checks if the virtual disk file is new (empty). */
    static bool isNew();

    static void closeDisk();

    

private:
    /** File stream for the virtual disk, opened in read/write binary mode. */
    static fstream Disk;
};
