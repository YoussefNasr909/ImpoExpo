#include "File_Entry.h"
using namespace std;

File_Entry::File_Entry(string name, char dir_attr, int dir_firstCluster, Directory* pa)
    : Directory_Entry(name, dir_attr, dir_firstCluster) , content("")
{
    if (pa != nullptr)
    {
        parent = pa;
    }
}

File_Entry :: File_Entry(Directory_Entry d,Directory * pa)
    :Directory_Entry (d.dir_name, d.dir_attr, d.dir_firstCluster)
{
    for (size_t i = 0; i < 12; i++)
    {
        dir_empty[i] = d.dir_empty[i];
    }
    dir_fileSize = d.dir_fileSize;
    content = "";
    if (pa != nullptr)
    {
       this -> parent = pa;
    }
}

int File_Entry::getMySizeOnDisk()
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

void File_Entry::emptyMyClusters()
{
    if (dir_firstCluster != 0)
    {
        int cluster = dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        do
        {
            Mini_FAT::setClusterPointer(cluster, 0);
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);
    }
}

Directory_Entry File_Entry::getDirectory_Entry()
{
    Directory_Entry M(dir_name, dir_attr, dir_firstCluster);
    for (size_t i = 0; i < 12; i++)
    {
        M.dir_empty[i] = dir_empty[i];
    }
    M.dir_fileSize = dir_fileSize;
    return M;
}

void File_Entry::writeFileContent()
{
    Directory_Entry A = this->getDirectory_Entry();
    if (!content.empty())
    {
        vector<char> contentBYTES = Converter::StringToBytes(content);
        vector<vector<char>> bytesList = Converter::splitBytes(contentBYTES);
        int clusterFATIndex;
        if (dir_firstCluster != 0)
        {
            emptyMyClusters();
            clusterFATIndex = Mini_FAT::getAvailableCluster();
            dir_firstCluster = clusterFATIndex;
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
    if (content.empty())
    {
        if (dir_firstCluster != 0)
            emptyMyClusters();
        if (parent != nullptr)
            dir_firstCluster = 0;
    }
    Directory_Entry B = getDirectory_Entry();
    if (parent != nullptr)
    {
        parent->updatecontent(A, B);
        parent->writeDirectory();
    }

    Mini_FAT::writeFAT();
}

void File_Entry::readFileContent()
{
    if (dir_firstCluster != 0)
    {
        content = "";
        int cluster = this->dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        vector<char> ls;
        do
        {
            vector<char> clusterData = Virtual_Disk::readCluster(cluster);
            ls.insert(ls.end(), clusterData.begin(), clusterData.end());
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);

        content = Converter::BytesToString(ls);
    }
}

void File_Entry::deleteFile()
{
    emptyMyClusters();
    if (parent != nullptr)
    {
        parent->removeEntry(getDirectory_Entry());
    }
}

void File_Entry::printContent()
{
    cout << "\n" << dir_name << "\n\n" << content << "\n" << endl;
}
