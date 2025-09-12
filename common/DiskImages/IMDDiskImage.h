////////////////////////////////////////////////////////////////////
//
// IMDDiskImage.h - Header file for CIMDDiskImage, a helper class
//                  that loads and saves ImageDisk .imd floppy 
//                  image files.
//
// IMD Version 1.18 Revised 8-Mar-2012
//
// For info on .imd files go to:
// 		https://oldcomputers-ddns.org/public/pub/manuals/imd.pdf
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#ifndef __IMD_DISK_IMAGE_H__
#define __IMD_DISK_IMAGE_H__

#include <string.h>
#include <vector>

#include "DiskImageInterface.h"

#define IMD_FORMAT_SIGNATURE		"IMD"
#define IMD_VERSION					"1.18"
#define IMD_ASCII_HEADER_LEN		31
#define IMD_COMMENT_TERMINATOR		0x1A
#define IMD_USE_HEAD_MAP_MASK		0x40
#define IMD_USE_CYLINDER_MAP_MASK	0x80
#define IMD_SIDE_MASK				0x3F
#define IMD_MAX_SIDE_NUM			2
#define IMD_MAX_TRACK_NUM			256
#define IMD_MAX_SECTOR_NUM			255
#define IMD_SECTOR_SIZE_FACTOR_BASE 128
#define IMD_MAX_SECTOR_SIZE_FACTOR	6
#define IMD_MAX_SECTOR_SIZE			(IMD_SECTOR_SIZE_FACTOR_BASE << IMD_MAX_SECTOR_SIZE_FACTOR)
#define IMD_MODE_500KBPS_FM			0
#define IMD_MODE_300KBPS_FM			1
#define IMD_MODE_250KBPS_FM			2
#define IMD_MODE_500KBPS_MFM		3
#define IMD_MODE_300KBPS_MFM		4
#define IMD_MODE_250KBPS_MFM		5
#define IMD_FILLER_BYTE 			0xE5

enum EIMDSectorType
{
	UNAVAILABLE = 0,
	NORMAL_DATA,
	COMPRESSED_DATA,
	NORMAL_DATA_DELETED_ADDRESS_MARK,
	COMPRESSED_DATA_DELETED_ADDRESS_MARK,
	NORMAL_DATA_READ_ERROR,
	COMPRESSED_DATA_READ_ERROR,
	DELETED_DATA_READ_ERROR,
	COMPRESSED_DELETED_READ_ERROR,
	COUNT
};

struct SIMDSector
{
	EIMDSectorType type;
	std::vector<uint8_t> data;
};

// Mode byte explained:
// 00 = 500 kbps FM  \  Note: kbps indicates transfer rate,
// 01 = 300 kbps FM   > not the data rate, which is
// 02 = 250 kbps FM  /  1/2 for FM encoding.
// 03 = 500 kbps MFM
// 04 = 300 kbps MFM
// 05 = 250 kbps MFM
struct SIMDTrack
{
	uint8_t mode;  				// (0-5)
	uint8_t track; 				// (0-n)
	uint8_t side;  				// (0-1) (Bits 6 and 7 signal use of Sector Head map and Sector Cylinder map.
	uint8_t sectorsPerTrack;	// (1-n)
	uint8_t sectorSizeFactor;	// (0-6)
	
	std::vector<uint8_t> 	sectorNumberingMap;
	std::vector<uint8_t> 	sectorCylinderMap;
	std::vector<uint8_t> 	sectorHeadMap;
	std::vector<SIMDSector> sectors;
};

class CIMDDiskImage:public IDiskImageInterface
{
public:
	CIMDDiskImage() {}
	~CIMDDiskImage(){}

	// IDiskImageInterface //////////////////////////////////////////////////////////////////////////////////
	bool					Load( const std::string& _filename ) override;
	bool					Save( const std::string& _filename ) override;
	unsigned int 			New ( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize ) override;

	int 					GetSidesNum() const override;
	int 					GetTracksNum() const override;
	int 					GetSectorsNum() const override;
	int 					GetSectorsNum(size_t _side, size_t _track) const override;

	STrackInfo				GetTrackInfo ( unsigned int _track, unsigned int _side ) const override;
	SSectorInfo				GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const override;

	const 	unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
			unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) override;
			unsigned int	GetSectorID  ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
	const 	unsigned char*	GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;

	std::string 			GetFileSpec() override;
	std::string 			GetDiskInfo() override;

	size_t					GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) override;
	size_t					GetSectorSize() override;

	// void					SetSidesNum  ( size_t _sides    ) override;
	// void					SetTracksNum ( size_t _tracks   ) override;
	// void					SetSectorsNum( size_t _sectors  ) override;
	// void					SetSectorSize( size_t _size     ) override;

	size_t					GetDataSize() override;

	bool 					NeedManualSetup() const override { return false; }

	IDiskImageInterface*	NewImage() const override { return new CIMDDiskImage; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	void            SetComment	 ( const char* newComment ) { comment = newComment; }
	const char*     GetComment	 () const { return comment.c_str(); }

private:
	char 		asciiHeader[IMD_ASCII_HEADER_LEN+1];
	std::string comment;
	std::string fileName;

	std::vector<SIMDTrack> sides[IMD_MAX_SIDE_NUM];

	EIMDSectorType 	ByteToSectorType( uint8_t _byte 			 ) const;
	uint8_t 		SectorTypeToByte( EIMDSectorType _sectorType ) const;
};

#endif