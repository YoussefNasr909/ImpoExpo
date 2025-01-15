#pragma once
#include "Directory_Entry.h"
#include "Virtual_Disk.h"
#include "Mini_FAT.h"
#include <vector>
#include <string>
using namespace std;

class Converter
{
public:
    // Converts an integer to a 4-byte vector (little-endian format)
    static vector<char> intToByte(int n);

    // Converts a 4-byte vector to an integer (little-endian format)
    static int byteToInt( vector<char> bytes);

    // Converts an array of integers to a byte array
    static vector<char> intArrayToByteArray( int* ints, int size);

    // Converts a byte array back to an array of integers
    static void byteArrayToIntArray(int* ints,  vector<char> bytes);

    // Splits a byte vector into chunks of 1024 bytes (pads if necessary)
    static vector<vector<char>> splitBytes( vector<char> bytes);

    // Converts a byte vector to a Directory_Entry object
    static Directory_Entry BytesToDirectory_Entry( vector<char> bytes);

    // Converts a Directory_Entry object to a byte vector
    static vector<char> Directory_EntryToBytes( Directory_Entry d);

    static vector<char> Directory_EntriesToBytes(vector<Directory_Entry> d);

    static vector<Directory_Entry> BytesToDirectory_Entries(vector<char> bytes);

    static vector<char> StringToBytes(string s);
    
    static string BytesToString(vector<char> b);
};
