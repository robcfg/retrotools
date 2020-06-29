#pragma once

#include <string>
#include <vector>

struct DFSEntry
{
    std::string   name;
    unsigned char directory;
    unsigned int  loadAddress;
    unsigned int  execAddress;
    unsigned int  fileSize;
    unsigned int  startSector;
    bool          locked;
};

struct DFSDisk
{
    std::string name;
    unsigned char sequence;
    unsigned char bootOption;
    unsigned short int sectorsNum;
    std::vector<DFSEntry> files;
};

void        DFSRead           ( unsigned char* _data, size_t _size, DFSDisk& _disk );
std::string BootOptionToString( unsigned char  _bootOption );
