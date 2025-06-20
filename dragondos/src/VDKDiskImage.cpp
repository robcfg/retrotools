/////////////////////////////////////////////////////////////////////
//
// VDKDiskImage.cpp - Implementation of CVDKDiskImage, a helper class
//                    that handles .vdk Dragon and CoCo floppy image
//                    files.
//
// For info on .vdk files go to:
//             http://www.burgins.com/emulators.html
//             and look for the PC-Dragon source files.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 10/11/2023
//
/////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include "VDKDiskImage.h"
#include <stdio.h>
#include <string.h>
#include <sstream>

// Load the contents of a image file.
bool CVDKDiskImage::Load( const std::string& _filename )
{
    // Open file
    FILE* pIn = fopen( _filename.c_str(), "rb" );
    if( 0 == pIn )
    {
        return false;
    }

    // Get file size
    unsigned int uFileSize = 0;
    fseek( pIn, 0, SEEK_END );
    uFileSize = (unsigned int)ftell( pIn );
    fseek( pIn, 0, SEEK_SET );

    // Read header
    fread( &vdkHead, 1, sizeof(vdkHead), pIn );

    if( vdkHead.id1 != VDK_ID1 || vdkHead.id2 != VDK_ID2 )
    {
        fclose(pIn);
        return false;
    }

    // Get Name (if any)
    memset( name, 0, VDK_MAXNAME + 1 );
    unsigned char uNameLen = (vdkHead.compression >> VDK_COMPBITS);
    if( 0 != uNameLen )
    {
        fread( name, 1, uNameLen, pIn );
    }

    // Check data size
    unsigned int uDataSize = ((vdkHead.tracks * VDK_SECTORSPERTRACK * VDK_SECTORSIZE) * vdkHead.sides);
    unsigned int uRightSize = vdkHead.header_len + uDataSize /*+ uNameLen*/;

    if( uRightSize != uFileSize )
    {
        fclose(pIn);
        return false;
    }

    // Read Data
    dataBlockSize = uDataSize;
    dataBlock = new unsigned char[dataBlockSize];

    if( 0 == dataBlock )
    {
        fclose(pIn);
        return false;
    }

    fseek(pIn,vdkHead.header_len,SEEK_SET);
    fread( dataBlock, 1, dataBlockSize, pIn );

    // Close file
    fclose( pIn );

    fileName = _filename;

    return true;
}

// Saves current image to a file
bool CVDKDiskImage::Save( const std::string& _filename )
{
    FILE* pOut = fopen( _filename.c_str(), "wb" );
    if( 0 == pOut )
    {
        return false;
    }

    // Save Header
    fwrite( &vdkHead, 1, sizeof(vdkHead), pOut );

    // Save Name
    unsigned char uNameLen = (unsigned char)strlen(name);
    if( 0 != uNameLen )
    {
        fwrite( name, 1, uNameLen, pOut );
    }

    // Write padding if header size exceeds fixed header portion plus name length
    unsigned int paddingNeeded = vdkHead.header_len - (sizeof(vdkHead) + uNameLen);
    while (paddingNeeded--)
    {
        fputc(0, pOut);
    }

    // Save Data
    fwrite( dataBlock, 1, dataBlockSize, pOut );

    // Close file
    fclose(pOut);

    return true;
}

// Creates a blank image with the specified parameters.
unsigned int CVDKDiskImage::New( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack )
{
    vdkHead.id1         = VDK_ID1;
    vdkHead.id2         = VDK_ID2;
    vdkHead.header_len  = sizeof(vdkHead);
    vdkHead.ver_actual  = VDK_VEROUT;    
    vdkHead.ver_compat  = VDK_VERIN;
    vdkHead.source_id   = VDK_SRCIDOUT;
    vdkHead.source_ver  = VDK_SRCVEROUT;
    vdkHead.tracks      = (unsigned char)uTracks;
    vdkHead.sides       = (unsigned char)uSides;
    vdkHead.flags       = 0;
    vdkHead.compression = VDK_COMPOUT; 

    dataBlockSize = ((vdkHead.tracks * VDK_SECTORSPERTRACK * VDK_SECTORSIZE) * vdkHead.sides);

    if(0 != dataBlock)
    {
        delete dataBlock;
    }

    dataBlock = new unsigned char[dataBlockSize];
    if( 0 == dataBlock )
    {
        memset( &vdkHead, 0, sizeof(vdkHead) );
        return 0;
    }

    memset( dataBlock, VDK_EMPTYSECTORFILL, dataBlockSize   );
    memset( name     , 0                  , VDK_MAXNAME + 1 );

    return dataBlockSize;
}

// Give the disk a new name. Maximum 31 chars in size.
void CVDKDiskImage::SetName( const char* newName )
{
    if( 0 == newName )
    {
        return;
    }

    memset( name, 0, VDK_MAXNAME + 1 );

    strncpy( name, newName, VDK_MAXNAME );

    // Update header
    unsigned char uCompressFlags = (vdkHead.compression & VDK_COMPMASK);
    unsigned char uNameLen       = (((unsigned char)strlen(name)) << VDK_COMPBITS);

    vdkHead.header_len  = (unsigned char)(sizeof(vdkHead) + strlen(name));
    vdkHead.compression = (uNameLen | uCompressFlags);
}

// Returns pointer to the required sector's data or NULL if parameters are invalid.
const unsigned char* CVDKDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
    // Check values
    if( 0 == dataBlock                 ) return 0;
    if( uTrack  >= vdkHead.tracks      ) return 0;
    if( uSide   >= vdkHead.sides       ) return 0;
    if( uSector >= VDK_SECTORSPERTRACK ) return 0;

    // Do some math
    unsigned int uTrackStart = VDK_TRACKSIZE * vdkHead.sides * uTrack;
    unsigned int uSectorPos = uTrackStart + ( uSide * VDK_TRACKSIZE ) + ( uSector * VDK_SECTORSIZE );

    return &dataBlock[uSectorPos];
}

unsigned char* CVDKDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector )
{
    // Check values
    if( 0 == dataBlock                 ) return 0;
    if( uTrack  >= vdkHead.tracks      ) return 0;
    if( uSide   >= vdkHead.sides       ) return 0;
    if( uSector >= VDK_SECTORSPERTRACK ) return 0;

    // Do some math
    unsigned int uTrackStart = VDK_TRACKSIZE * vdkHead.sides * uTrack;
    unsigned int uSectorPos = uTrackStart + ( uSide * VDK_TRACKSIZE ) + ( uSector * VDK_SECTORSIZE );

    return &dataBlock[uSectorPos];
}

// Returns pointer to the required sector's data or NULL if parameters are invalid.
const unsigned char* CVDKDiskImage::GetSector( unsigned short int LSN ) const
{
    return GetSector( LSNTrack(LSN), LSNHead(LSN), LSNSector(LSN) );
}

// Translates track, head and sector numbers to Logical Sector Number
unsigned short int CVDKDiskImage::LSN( unsigned short int uTrack, unsigned short int uSide, unsigned short int uSector ) const
{
    return ((uTrack * vdkHead.sides * VDK_SECTORSPERTRACK) + (uSide * VDK_SECTORSPERTRACK) + uSector);
}

// Returns the track number of a given LSN
unsigned char CVDKDiskImage::LSNTrack( unsigned short int LSN ) const
{
    // track = LSN / (SECTORS * HEADS)
    return (unsigned char)(LSN/(vdkHead.sides * VDK_SECTORSPERTRACK));
}

// Returns the head number of a given LSN
unsigned char CVDKDiskImage::LSNHead( unsigned short int LSN ) const
{
    // head = LSN % (SECTORS * HEADS) / SECTORS
    return (unsigned char)(LSN % (VDK_SECTORSPERTRACK * vdkHead.sides) / VDK_SECTORSPERTRACK);
}

// Returns the sector number of a given LSN
unsigned char CVDKDiskImage::LSNSector( unsigned short int LSN ) const
{
    // sector = LSN % (SECTORS * HEADS) % SECTORS
    return (unsigned char)(LSN % (VDK_SECTORSPERTRACK * vdkHead.sides) % VDK_SECTORSPERTRACK);
}

// IDiskImageInterface ///////////////////////

int CVDKDiskImage::GetSidesNum() const
{
    return (int)vdkHead.sides;
}

int CVDKDiskImage::GetTracksNum() const
{
    return (int)vdkHead.tracks;
}

int CVDKDiskImage::GetSectorsNum() const
{
    return VDK_SECTORSPERTRACK;
}

int CVDKDiskImage::GetSectorsNum(size_t _side, size_t _track) const
{
    return GetSectorsNum();
}

std::string CVDKDiskImage::GetFileSpec()
{
    return "Dragon VDK files\t*.vdk\n"; // Native chooser format.
}

std::string CVDKDiskImage::GetDiskInfo()
{
    std::string retVal;

    if( fileName.empty() )
    {
        return retVal;
    }
    std::stringstream sstream;

    int diskSize = GetTracksNum() * GetSidesNum() * GetSectorsNum() * VDK_SECTORSIZE;

    std::string shortFileName;
    size_t found = fileName.find_last_of("/\\");
    if( found )
    {
        shortFileName = fileName.substr(found+1);
    }
    else
    {
        shortFileName = fileName;
    }

    sstream << shortFileName.c_str() << std::endl << std::endl;
    sstream << "Tracks      : " << GetTracksNum()  << std::endl;
    sstream << "Sides       : " << GetSidesNum()   << std::endl;
    sstream << "Sectors     : " << GetSectorsNum() << std::endl;
    sstream << "Sector size : " << VDK_SECTORSIZE  << " bytes" << std::endl;
    sstream << "Total size  : " << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;

    retVal = sstream.str();

    return retVal;
}

const unsigned char* CVDKDiskImage::GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const
{
	return GetSector( uTrack, uSide, uSectorID );
}

STrackInfo CVDKDiskImage::GetTrackInfo ( unsigned int _track, unsigned int _side ) const
{
	STrackInfo retVal;

	retVal.isValid     = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum());
	retVal.isFormatted = true;
	retVal.sectorsNum  = GetSectorsNum();
	retVal.dataSize    = retVal.sectorsNum * VDK_SECTORSIZE;

	return retVal;
}

SSectorInfo CVDKDiskImage::GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const
{
	SSectorInfo retVal;

	retVal.isValid   = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum()) && (_sector < (unsigned int)GetSectorsNum());
	retVal.hasErrors = false;
	retVal.isInUse   = true;  // FS should check or update this info
	retVal.isWeak    = false;
	retVal.copiesNum = 1; // Number of copies of the sector stored
	retVal.dataSize  = VDK_SECTORSIZE;

	return retVal;
}
