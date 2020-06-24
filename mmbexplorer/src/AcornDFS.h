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
};

struct DFSDisk
{
    std::string name;
    unsigned char sequence;
    unsigned char bootOption;
    unsigned short int sectorsNum;
    std::vector<DFSEntry> files;
};

// Boot options
//   0: "0: *None",
//   1: "1: *Load $.!BOOT",
//   2: "2: *Run $.!BOOT",
//   3: "3: *Exec $.!BOOT"
void DFSRead( unsigned char* _data, size_t _size, DFSDisk& _disk );