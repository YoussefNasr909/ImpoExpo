#pragma once
#include "Virtual_Disk.h"
#include <vector>
#include <string>
using namespace std;
class Mini_FAT
{
public:
    /** FAT array representing cluster states: -1 for EOF, 0 for free, and positive values for next cluster in chain. */
    static int FAT[1024];

    /** Initializes the FAT, marking reserved clusters as -1 and others as free (0). */
    static void initialize_FAT();

    /** Creates the superblock as a byte vector by serializing the FAT. */
    static vector<char> createSuperBlock();

    /** Writes the FAT to the virtual disk by splitting into clusters. */
    static void writeFAT();

    /** Reads the FAT from the virtual disk and reconstructs it. */
    static void readFAT();

    /** Prints the FAT contents for debugging purposes. */
    static void printFAT();

    /** Sets the FAT array with the provided data. */
    static void setFAT(const int fat_arr[1024]);

    /** Initializes or opens the file system, creating or reading from the virtual disk. */
    static void initialize_Or_Open_FileSystem( string name);

    /** Returns the number of free clusters in the FAT. */
    static int getAvailableClusters();

    /** Returns the index of the first available (free) cluster. */
    static int getAvailableCluster();

    /** Sets the pointer for a cluster in the FAT (next cluster, EOF, or free). */
    static void setClusterPointer(int clusterIndex, int pointer);

    /** Gets the pointer value for a specific cluster in the FAT. */
    static int getClusterPointer(int clusterIndex);

    /** Returns the total free space on the disk in bytes. */
    static int getFreeSize();

    static void CloseTheSystem();

    static long long getTotalClusters();

    static long long getFreeClusters();

    static long long getClusterSize();


private:
};
