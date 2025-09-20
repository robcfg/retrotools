////////////////////////////////////////////////////////////////////
//
// FS_UTils.h - Header file for auxiliary functions.
//
////////////////////////////////////////////////////////////////////

#pragma once

#include "DiskImages/DiskImageInterface.h"
#include "FileSystems/FileSystemInterface.h"

unsigned short int LSN      ( const IDiskImageInterface& _disk, unsigned short int _track, unsigned short int _side, unsigned short int _sector );
unsigned char      LSNTrack ( const IDiskImageInterface& _disk, unsigned short int LSN );
unsigned char      LSNHead  ( const IDiskImageInterface& _disk, unsigned short int LSN );
unsigned char      LSNSector( const IDiskImageInterface& _disk, unsigned short int LSN );

const CDirectoryEntryWrapper* FindDirectoryEntry( const CDirectoryEntryWrapper* _parent, std::vector<std::string>& _tokens, size_t curToken );