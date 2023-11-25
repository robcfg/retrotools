////////////////////////////////////////////////////////////////////
//
// RAWFile.cpp - Implementation of CRAWDiskImage, a helper class that
//               loads and saves raw floppy image files.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 09/11/2019 17:34
//
////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "RawDiskImage.h"
#include <sstream>

bool CRAWDiskImage::Load(const std::string& _filename)
{
    mFileName = _filename;

    // Open file
    FILE* pIn = fopen( _filename.c_str(), "rb" );
    if( 0 == pIn )
        return false;

    // Get file size
    size_t uFileSize = 0;
    fseek( pIn, 0, SEEK_END );
    uFileSize = ftell( pIn );
    fseek( pIn, 0, SEEK_SET );

    // Allocate memory
    if( mDataBlock )
    {
        delete[] mDataBlock;
        mDataSize = 0;
    }

    mDataBlock = new unsigned char[uFileSize];
    if( !mDataBlock )
    {
        fclose( pIn );
        return false;
    }

    // Read data
    fread( mDataBlock, 1, uFileSize, pIn );
    fclose( pIn );

    mDataSize = uFileSize;

    return true;
}

bool CRAWDiskImage::Save(const std::string& _filename)
{
    // Open file
    FILE* pOut = fopen( _filename.c_str(), "wb" );
    if( 0 == pOut )
    {
        return false;
    }

    size_t bytesWritten = fwrite( mDataBlock, 1, mDataSize, pOut );
    fclose(pOut);

    if( bytesWritten != mDataSize )
    {
        return false;
    }

    return true;
}

std::string CRAWDiskImage::GetFileSpec()
{
    return "Raw image files\t*.*\n"; // Native chooser format.
}

std::string CRAWDiskImage::GetDiskInfo()
{
    std::string retVal;

    if( mFileName.empty() )
        return retVal;

    std::stringstream sstream;

    std::string shortFileName;
    size_t found = mFileName.find_last_of("/\\");
    if( found )
        shortFileName = mFileName.substr(found+1);
    else
        shortFileName = mFileName;

    size_t diskSize = mSidesNum * mTracksNum * mSectorsNum * mSectorSize;
    sstream << shortFileName.c_str() << std::endl << std::endl;
    sstream << "Tracks      : "   << mTracksNum  << std::endl;
    sstream << "Sides       : "   << mSidesNum   << std::endl;
    sstream << "Sectors     : "   << mSectorsNum << std::endl;
    sstream << "Sector size : "   << mSectorSize << " bytes" << std::endl;
    sstream << "Total size  : "   << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;
    retVal = sstream.str();

    return retVal;
}

STrackInfo CRAWDiskImage::GetTrackInfo ( unsigned int _track, unsigned int _side ) const
{
    STrackInfo retVal{};

    return retVal;
}

SSectorInfo CRAWDiskImage::GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const
{
    SSectorInfo retVal{};

    return retVal;
}
 
const unsigned char* CRAWDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
    if( mSidesNum == 0 || mTracksNum == 0 || mSectorsNum == 0 || mSectorSize == 0 )
    {
        return 0;
    }

    size_t trackSize = mSectorsNum * mSectorSize;
    size_t pos = (mSidesNum * uTrack * trackSize ) + (uSide * trackSize ) + (uSector * mSectorSize);
    if( pos < mDataSize - mSectorSize )
    {
        return &mDataBlock[pos];
    }

    return 0;
}

unsigned char* CRAWDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector )
{
    if( mSidesNum == 0 || mTracksNum == 0 || mSectorsNum == 0 || mSectorSize == 0 )
    {
        return 0;
    }

    size_t trackSize = mSectorsNum * mSectorSize;
    size_t pos = (mSidesNum * uTrack * trackSize ) + (uSide * trackSize ) + (uSector * mSectorSize);
    if( pos < mDataSize - mSectorSize )
    {
        return &mDataBlock[pos];
    }

    return 0;
}

const unsigned char* CRAWDiskImage::GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const
{
    return GetSector( uTrack, uSide, uSectorID-1 ); // -1 to convert from 1-based index to 0-based index
}
