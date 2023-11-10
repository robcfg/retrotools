////////////////////////////////////////////////////////////////////
//
// RAWDiskImage.h - Header file for CRAWDiskImage, a helper class
//                  that load and saves raw floppy image files.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 08/06/2022 00:12
//
////////////////////////////////////////////////////////////////////

#ifndef __RAW_DISK_IMAGE__H__
#define __RAW_DISK_IMAGE__H__

#include <string.h>
#include "DiskImageInterface.h"

class CRAWDiskImage:public IDiskImageInterface
{
public:
    CRAWDiskImage() { mSidesNum = 0; mTracksNum = 0; mSectorsNum = 0; mSectorSize = 0; mDataBlock = 0; mDataSize = 0; }
    ~CRAWDiskImage(){ if(0 != mDataBlock) delete[] mDataBlock; }

    // IDiskImageInterface interface ///////////////////////
    bool Load(const string& _filename);
    bool Save(const string& _filename); // Save isn't const because it can update the lastError string.
    //NewDisk(tracks,sides,optional sector size)

    int GetSidesNum()   const { return (int)mSidesNum;   }
    int GetTracksNum()  const { return (int)mTracksNum;  }
    int GetSectorsNum() const { return (int)mSectorsNum; }
    int GetSectorsNum(size_t _side, size_t _track) const { return (int)mSectorsNum; }

    string GetFileSpec();
    string GetDiskInfo(); 

    STrackInfo  GetTrackInfo ( unsigned int _track, unsigned int _side ) const;
    SSectorInfo GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const;

    const unsigned char* GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const;
          unsigned char* GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector );
    //const unsigned char* GetSector    ( unsigned short int LSN ) const;
    const unsigned char* GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const;

    bool NeedManualSetup() const { return true; }

    void SetSidesNum  ( size_t _sides   ) { mSidesNum = _sides;     }
    void SetTracksNum ( size_t _tracks  ) { mTracksNum = _tracks;   }
    void SetSectorsNum( size_t _sectors ) { mSectorsNum = _sectors; }
    void SetSectorSize( size_t _size    ) { mSectorSize = _size;    }
    
    size_t GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) { return mSectorSize; }

    /////////////////////////////////////////////////

private:
    string mFileName;
    size_t mSidesNum;
    size_t mTracksNum;
    size_t mSectorsNum;
    size_t mSectorSize;
    unsigned char* mDataBlock;
    size_t mDataSize;
};

#endif