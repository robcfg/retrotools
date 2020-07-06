#pragma once

#include <string>
#include <vector>

struct DFSEntry
{
    std::string                name;
    unsigned char              directory;
    unsigned int               loadAddress;
    unsigned int               execAddress;
    unsigned int               fileSize;
    unsigned int               startSector;
    bool                       locked;
    std::vector<unsigned char> data;
};

struct DFSDisk
{
    std::string           name;
    unsigned char         sequence = 0;
    unsigned char         bootOption = 0;
    unsigned short int    sectorsNum = 800; // 800 sectors of 256 bytes = 204800 bytes
    std::vector<DFSEntry> files;
};

void        DFSRead           ( unsigned char* _data, size_t _size, DFSDisk& _disk );
bool        DFSWrite          ( unsigned char* _data, size_t _size, const DFSDisk& _disk );
void        DFSPackFiles      ( DFSDisk& _disk );
std::string BootOptionToString( unsigned char  _bootOption );
