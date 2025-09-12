////////////////////////////////////////////////////////////////////
//
// RAWDiskImage.h - Header file for CRAWDiskImage, a helper class
//                  that load and saves raw floppy image files.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#ifndef __RAW_DISK_IMAGE_H__
#define __RAW_DISK_IMAGE_H__

#include <string.h>
#include "DiskImageInterface.h"

class CRAWDiskImage:public IDiskImageInterface
{
public:
	CRAWDiskImage() { mSidesNum = 0; mTracksNum = 0; mSectorsNum = 0; mSectorSize = 0; mDataBlock = 0; mDataSize = 0; }
	~CRAWDiskImage(){ if(0 != mDataBlock) delete[] mDataBlock; }

	// IDiskImageInterface //////////////////////////////////////////////////////////////////////////////////
	bool					Load( const std::string& _filename ) override;
	bool					Save( const std::string& _filename ) override;
	unsigned int 			New ( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize ) override;

	int 					GetSidesNum	 ()	const override{ return (int)mSidesNum;   };
	int 					GetTracksNum () const override{ return (int)mTracksNum;  };
	int 					GetSectorsNum() const override{ return (int)mSectorsNum; };
	int 					GetSectorsNum (size_t _side, size_t _track) const override { return (int)mSectorsNum; };

	STrackInfo				GetTrackInfo ( unsigned int _track, unsigned int _side ) const override;
	SSectorInfo				GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const override;

	const 	unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
			unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) override;
			unsigned int	GetSectorID  ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
	const 	unsigned char*	GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;

	std::string 			GetFileSpec() override;
	std::string 			GetDiskInfo() override;

	size_t					GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) override { return mSectorSize; };
	size_t					GetSectorSize() override { return mSectorSize; };

	void					SetSidesNum  ( size_t _sides   ) override { mSidesNum = _sides;     }
	void					SetTracksNum ( size_t _tracks  ) override { mTracksNum = _tracks;   }
	void					SetSectorsNum( size_t _sectors ) override { mSectorsNum = _sectors; }
	void					SetSectorSize( size_t _size    ) override { mSectorSize = _size;    }

	size_t					GetDataSize() override { return mDataSize; };

	bool 					NeedManualSetup() const override { return true; }

	IDiskImageInterface*	NewImage() const override { return new CRAWDiskImage; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	std::string     mFileName;
	size_t          mSidesNum;
	size_t          mTracksNum;
	size_t          mSectorsNum;
	size_t          mSectorSize;
	unsigned char*  mDataBlock;
	size_t          mDataSize;
};

#endif