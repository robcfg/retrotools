#ifndef __JVC_DISK_IMAGE_H__
#define __JVC_DISK_IMAGE_H__

#include <string>
#include "DiskImageInterface.h"

// JVC Header structure with default values
struct JVCHeader
{
	unsigned char sectorsPerTrack;    	// Default: 18
	unsigned char sideCount;          	// Default: 1
	unsigned char sectorSizeCode;     	// Default: 1
	unsigned char firstSectorID;      	// Default: 1
	unsigned char sectorAttributeFlag;	// Default: 0
	
	JVCHeader() : sectorsPerTrack(18), sideCount(1), sectorSizeCode(1), firstSectorID(1), sectorAttributeFlag(0) {}
};

class CJVCDiskImage : public IDiskImageInterface
{
public:
	CJVCDiskImage();
	~CJVCDiskImage();

	void 					SetName			(const char* newName);
	void 					SetFileName		(const std::string& _filename) { fileName = _filename; }
	void 					SetHeaderSize	(unsigned int size) { headerSize = size; }
	unsigned int 			GetHeaderSize	() const { return headerSize; }

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

	size_t					GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) override { return GetSectorSize(); }
	size_t					GetSectorSize() override { return 128 << header.sectorSizeCode; }

	// void					SetSidesNum  ( size_t _sides    ) override;
	// void					SetTracksNum ( size_t _tracks   ) override;
	// void					SetSectorsNum( size_t _sectors  ) override;
	// void					SetSectorSize( size_t _size     ) override;

	size_t					GetDataSize() override { return dataBlockSize; };

	bool 					NeedManualSetup() const override;

	IDiskImageInterface*	NewImage() const override { return new CJVCDiskImage; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	unsigned char* dataBlock;
	unsigned int dataBlockSize;
	std::string fileName;
	JVCHeader header;
	unsigned char tracks;
	unsigned int headerSize;  // -1 = not yet determined, 0 = headerless, >0 = header with this size
};

#endif // JVC_DISK_IMAGE_H 