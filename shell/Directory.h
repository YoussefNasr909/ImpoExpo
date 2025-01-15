#pragma once
#include<vector>
#include"Directory_Entry.h"
#include "Mini_FAT.h"
#include "Virtual_Disk.h"
#include "Converter.h"
using namespace std;

class Directory : public Directory_Entry {
	
	public:
		vector< Directory_Entry>DirOrFiles;

		Directory* parent;

        Directory_Entry dir_entry;

        Directory(string name, char dir_attr, int dir_firstCluster, Directory* pa);

		Directory_Entry GetDirectory_Entry();

		int getmySizeOnDisk();

		bool canAddEntry(Directory_Entry d);

		void emptymyClusters();

		void writeDirectory();

		void readDirectory ();

		void addEntry(Directory_Entry d);

		void removeEntry(Directory_Entry d);

		void deletDirectory();

		void updatecontent(Directory_Entry OLD, Directory_Entry New);

		int searchDirectory(string name);

        string getFullPath() const ;

        string name;
        Directory_Entry findSubDirectory(const string& dirname);
        Directory* getDirectoryByPath(const string& path);

		string getDrive() const;
        bool isEmpty() const;

	};
