#include "AcornDFS.h"

const size_t DFS_SECTOR0_OFFSET = 0;
const size_t DFS_SECTOR1_OFFSET = 256;

void DFSRead( unsigned char* _data, size_t _size, DFSDisk& _disk )
{
    // Read disk name and parameters
    char diskName[13];
    memcpy( &diskName[0], &_data[DFS_SECTOR0_OFFSET], 8 );
    memcpy( &diskName[8], &_data[DFS_SECTOR1_OFFSET], 4 );
    diskName[12] = 0;
    _disk.name = diskName;
    _disk.sequence = _data[DFS_SECTOR1_OFFSET+4];
    unsigned int entriesNum = ((unsigned int)_data[DFS_SECTOR1_OFFSET+5] / 8);
    _disk.sectorsNum = (unsigned short int)((_data[DFS_SECTOR1_OFFSET+6] & 3) << 8);
    _disk.sectorsNum |= (unsigned short int) _data[DFS_SECTOR1_OFFSET+7];
    _disk.bootOption = ((_data[DFS_SECTOR1_OFFSET+6] & 0x30) >> 4);

    // Read files' info
    char fileName[8];
    size_t sector_0_offset = 8 + DFS_SECTOR0_OFFSET;
    size_t sector_1_offset = 8 + DFS_SECTOR1_OFFSET;
    for( size_t fileNum = 0; fileNum < entriesNum; ++fileNum )
    {
        DFSEntry tmpEntry;

        // Sector 0
        memcpy( &fileName[0], &_data[sector_0_offset], 7);
        tmpEntry.name = fileName;
        sector_0_offset += 7;

        tmpEntry.directory = _data[sector_0_offset++];

        // Sector 1
        tmpEntry.loadAddress =   _data[sector_1_offset++];
        tmpEntry.loadAddress |= (_data[sector_1_offset++] << 8);
        tmpEntry.execAddress =   _data[sector_1_offset++];
        tmpEntry.execAddress |= (_data[sector_1_offset++] << 8);
        tmpEntry.fileSize    =   _data[sector_1_offset++];
        tmpEntry.fileSize    |= (_data[sector_1_offset++] << 8);
        tmpEntry.startSector = ((_data[sector_1_offset] & 3)  << 8 );
        tmpEntry.loadAddress |=((_data[sector_1_offset] & 12 )<< 16);
        tmpEntry.fileSize    |=((_data[sector_1_offset] & 48 )<< 16);
        tmpEntry.execAddress |=((_data[sector_1_offset++] & 192)<< 16);
        tmpEntry.startSector |=  _data[sector_1_offset++];

        _disk.files.push_back( tmpEntry );
    }
}