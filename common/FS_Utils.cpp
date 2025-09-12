////////////////////////////////////////////////////////////////////
//
// FS_UTils.cpp - Implementation of auxiliary functions.
//
////////////////////////////////////////////////////////////////////

#include "FS_Utils.h"
#include <string.h> // for strcasecmp
#include <vector>

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

// Translates track, head and sector numbers to Logical Sector Number
unsigned short int LSN( const IDiskImageInterface& _disk, unsigned short int _track, unsigned short int _side, unsigned short int _sector )
{
    return ((_track * _disk.GetSidesNum() * _disk.GetSectorsNum()) + (_side * _disk.GetSectorsNum()) + _sector);
}

// Returns the track number of a given LSN
unsigned char LSNTrack( const IDiskImageInterface& _disk, unsigned short int LSN )
{
    // track = LSN / (SECTORS * HEADS)
    return (unsigned char)(LSN/(_disk.GetSidesNum() * _disk.GetSectorsNum()));
}

// Returns the head number of a given LSN
unsigned char LSNHead( const IDiskImageInterface& _disk, unsigned short int LSN )
{
    // head = LSN % (SECTORS * HEADS) / SECTORS
    return (unsigned char)(LSN % (_disk.GetSectorsNum() * _disk.GetSidesNum()) / _disk.GetSectorsNum());
}

// Returns the sector number of a given LSN
unsigned char LSNSector( const IDiskImageInterface& _disk, unsigned short int LSN )
{
    // sector = LSN % (SECTORS * HEADS) % SECTORS
    return (unsigned char)(LSN % (_disk.GetSectorsNum() * _disk.GetSidesNum()) % _disk.GetSectorsNum());
}

const CDirectoryEntryWrapper* FindDirectoryEntry( const CDirectoryEntryWrapper* _parent, std::vector<std::string>& _tokens, size_t curToken )
{
    for( auto child : _parent->GetChildren() )
    {
        if( 0 == _stricmp(child->GetName().c_str(), _tokens[curToken].c_str() ) )
        {
            ++curToken;

            if( curToken == _tokens.size() )
                return child;
            else
                return FindDirectoryEntry( child, _tokens, curToken );
        }
    }

    return nullptr;
}

