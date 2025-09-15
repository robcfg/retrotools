///////////////////////////////////////////////////////////////////
//
// EDSKDiskImage.h - Header file for the CEDSKDiskImage class.
//                   It's purpose it's to read and write
//                   disk image files in the format defined
//                   by Marco Vieth, Ulrich Doewich and 
//                   Kevin Thacker.
//
// Created 10/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// TODO: 
//       - Add creation of new empty disk images.
//       - This is an idea. Create a base log class so that the
//         whole operation history can be reviewed.
//       - Add support for Simon Owen's extension Offset-Info.
//
///////////////////////////////////////////////////////////////////

#ifndef __FIDO_DSKFILE_H__
#define __FIDO_DSKFILE_H__

#include <vector>

#include "DiskImageInterface.h"

typedef std::vector<unsigned char> uint8vector;

#define CEDSKFile_Side0			false
#define CEDSKFile_Side1			true
#define CEDSKFile_MAX_SIDES_NUM 2
// Notes:
// - Format information can be found at:
//   http://www.cpcwiki.eu/index.php/Format:DSK_disk_image_file_format
//
// - This image format stores data and registers from the 765 FDC controller.
//
// - Track size table is number of tracks*number of sides bytes long
//
// - Depending on the information in the disk information block, the table contains:
//     track lengths for a single sided floppy disc
//     track lengths for a double sided floppy disc
//
// - Track lengths are stored in the same order as the tracks in the image 
//   e.g. In the case of a double sided disk: Track 0 side 0, Track 0 side 1, Track 1 side 0 etc...
//
// - A size of "0" indicates an unformatted track. In this case there is no data, 
//   and no track information block for this track in the image file!
//
// - Actual length of track data = (high byte of track length) * 256
//
// - Track length includes the size of the TRACK INFORMATION BLOCK (256 bytes)
//
// - The location of a Track Information Block for a chosen track is found by summing the sizes of 
//   all tracks up to the chosen track plus the size of the Disc Information Block (&100 bytes).
//   The first track is at offset &100 in the disc image.
//
// - 765FDC Status Register 1
//   b0     MA  Missing Address Mark (Sector_ID or DAM not found)
//   b1     NW  Not Writeable (tried to write/format disc with wprot_tab=on)
//   b2     ND  No Data (Sector_ID not found, CRC fail in ID_field)
//   b3,6   0   Not used
//   b4     OR  Over Run (CPU too slow in execution-phase (ca. 26us/Byte))
//   b5     DE  Data Error (CRC-fail in ID- or Data-Field)
//   b7     EN  End of Track (set past most read/write commands) (see IC)
//
// - 765FDC Status Register 2
//   b0     MD  Missing Address Mark in Data Field (DAM not found)
//   b1     BC  Bad Cylinder (read/programmed track-ID different and read-ID = FF)
//   b2     SN  Scan Not Satisfied (no fitting sector found)
//   b3     SH  Scan Equal Hit (equal)
//   b4     WC  Wrong Cylinder (read/programmed track-ID different) (see b1)
//   b5     DD  Data Error in Data Field (CRC-fail in data-field)
//   b6     CM  Control Mark (read/scan command found sector with deleted DAM)
//   b7     0   Not Used

struct CEDSKFile_DiskInfoBlock
{
	std::string header;							// Header string is 34 bytes long
	std::string creator;						// Creator string is 14 bytes long
	unsigned char tracksNum;					// Number of tracks
	unsigned char sidesNum;						// Number of sides
	unsigned short int trackSize;				// Track size. Unused in EXTENDED DSK files
	std::vector<unsigned char> trackSizeTable;	// Track size table. See notes above.
};

struct CEDSKFile_SectorInfo
{
	unsigned char track;           // Equivalent to C parameter in NEC765 commands
	unsigned char side;            // Equivalent to H parameter in NEC765 commands
	unsigned char sectorID;        // Equivalent to R parameter in NEC765 commands
	unsigned char sectorSize;      // Equivalent to N parameter in NEC765 commands
	unsigned char FDCStatus1;      // FDC Status register 1 (equivalent to NEC765 ST1 status register)
	unsigned char FDCStatus2;      // FDC status register 2 (equivalent to NEC765 ST2 status register)
	unsigned short int dataLength; // Actual data length in bytes
};

struct CEDSKFile_TrackInfoBlock
{
	std::string header;									// Header string is 13 bytes long
	unsigned char unused[3];                    		// 3 unused bytes
	unsigned char trackNumber;                  		// Track number
	unsigned char sideNumber;			        		// Side number
	unsigned char dataRate;                     		// Previously unused. Now Data Rate (John Elliott extension)
	unsigned char recordingMode;                		// Previously unused. Now Recording Mode (John Elliott extension)
	unsigned char sectorSize;                   		// Sector size
	unsigned char sectorsNum;                   		// Number of sectors
	unsigned char gap3Length;                   		// GAP#3 length
	unsigned char fillerByte;                   		// Filler byte value
	std::vector<CEDSKFile_SectorInfo> sectorInfoList;	// Sector information list
	std::vector<uint8vector> sectorData;          		// Actual sector data
	bool isUnformatted;                         		// Is this an unformatted track?
};

class CEDSKDiskImage:public IDiskImageInterface
{
public:
	CEDSKDiskImage();
	~CEDSKDiskImage();

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

	IDiskImageInterface*	NewImage() const override;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	const 	std::string&					GetLastError()		const {return lastError;}
	const 	CEDSKFile_DiskInfoBlock&		GetDiskInfoBlock()	const {return diskInfoBlock;}
			CEDSKFile_DiskInfoBlock&		GetDiskInfoBlock()		  {return diskInfoBlock;}
	bool									IsExtendedDSK()			  {return isExtendedDSK;}
	std::vector<CEDSKFile_TrackInfoBlock>&	GetSide(bool _sideB)	  {return _sideB ? sides[1]:sides[0];}

private:
	// Read functions
	void ReadDiskInformationBlock		(FILE* _pIn);
	bool ReadTrackInformationBlock		(FILE* _pIn);
	void ReadSectorInformationBlock		(FILE* _pIn, CEDSKFile_TrackInfoBlock& _tib);

	// Write functions
	void   WriteDiskInformationBlock	(FILE* _pOut) const;
	void   WriteTrackInformationBlock	(FILE* _pOut, const CEDSKFile_TrackInfoBlock& _tib) const;
	size_t WriteSectorInformationBlock	(FILE* _pOut, const CEDSKFile_SectorInfo& _si     ) const;

	// Variables
	std::string								lastError;
	CEDSKFile_DiskInfoBlock					diskInfoBlock;
	bool									isExtendedDSK;
	std::vector<CEDSKFile_TrackInfoBlock>	sides[2];
	std::string								fileName;
};

#endif