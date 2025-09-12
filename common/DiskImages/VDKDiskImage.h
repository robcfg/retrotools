////////////////////////////////////////////////////////////////////
//
// VDKDiskImage.h - Header file for CVDKDiskImage, a helper class
//                  that loads and saves .vdk Dragon and CoCo floppy 
//                  image files.
//
// For info on .vdk files go to:
//             http://www.burgins.com/emulators.html
//             and look for the PC-Dragon source files.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Note: The data in the disk image is organized as Track, Side and
//       Sector. So you'll have to access the data like this:
//
//       for (track = 0; track < num_tracks; track++) 
//       {
//         for (side = 0; side < num_sides; side++) 
//         {
//           for (sector = 0; sector < num_sectors; sector++) 
//           {
//             // Read sector size
//           }
//         }
//       }
//
//       Info taken from XRoar 0.22 source file vdisk.c
//       http://www.6809.org.uk/dragon/xroar.shtml#source
//
// Info on Logical Sector Numbers (LSNs) : 
//        
//          LSN = track * HEADS * SECTORS + head * SECTORS + sector
//
//          track  = LSN / (SECTORS * HEADS)
//          head   = LSN % (SECTORS * HEADS) / SECTORS
//          sector = LSN % (SECTORS * HEADS) % SECTORS
//
//          Thanks to Rolf Michelsen for the LSNs info!
//
////////////////////////////////////////////////////////////////////

#ifndef __VDK_DISK_IMAGE_H__
#define __VDK_DISK_IMAGE_H__

#include <string.h>
#include "DiskImageInterface.h"

// Defines taken from PC-Dragon source file DOSCART.C
#define VDK_VER_MAJOR       2
#define VDK_VER_MINOR       6
#define VDK_ID1             'd'
#define VDK_ID2             'k'
#define VDK_VEROUT          0x10
#define VDK_VERIN           0x10
#define VDK_SRCIDOUT        ('P')
#define VDK_SRCVEROUT       ((VDK_VER_MAJOR << 4) | (VDK_VER_MINOR))
#define VDKFLAGS_WP         0x01
#define VDKFLAGS_ALOCK      0x02
#define VDKFLAGS_FLOCK      0x04
#define VDKFLAGS_DISKSET    0x08
#define VDK_COMPBITS        3
#define VDK_COMPMASK        0x07
#define VDK_COMPOUT         0x00
#define VDK_MAXNAME         31
#define VDK_EMPTYSECTORFILL 0xE5

#define VDK_SECTORSPERTRACK 18
#define VDK_SECTORSIZE      256
#define VDK_TRACKSIZE       (VDK_SECTORSPERTRACK * VDK_SECTORSIZE)

// Header structure defined in PC-Dragon source file DOSCART.C
struct SVDKHeader
{
	/* v1.0 */
	unsigned char  id1;           /* signature byte 1 */
	unsigned char  id2;           /* signature byte 2 */
	unsigned short header_len;    /* total header length (offset to data) */
	unsigned char  ver_actual;    /* version of VDK format */
	unsigned char  ver_compat;    /* backwards compatibility version */
	unsigned char  source_id;     /* identity of file source */
	unsigned char  source_ver;    /* version of file source */
	unsigned char  tracks;        /* number of tracks (40 or 80) */
	unsigned char  sides;         /* number of sides (1 or 2) */
	unsigned char  flags;         /* various flags */
	unsigned char  compression;   /* compression flags and name length */

	SVDKHeader() { memset(this,0, sizeof(SVDKHeader)); }
};

class CVDKDiskImage:public IDiskImageInterface
{
public:
	CVDKDiskImage() { dataBlock = 0; dataBlockSize = 0; name[0] = 0; }
	~CVDKDiskImage(){ if(0 != dataBlock) delete dataBlock; }

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
			// unsigned int	GetSectorID  ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
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

	IDiskImageInterface*	NewImage() const override { return new CVDKDiskImage; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	const SVDKHeader&		GetHeader       () const { return vdkHead;			}
	unsigned char*			GetDataBlock    ()    	 { return dataBlock;		}
	unsigned int			GetDataBlockSize()		 { return dataBlockSize;	}
	const char*				GetName         () const { return name;				}

	void                 	SetName			( const char* newName );

private:
	SVDKHeader           	vdkHead;
	unsigned char*       	dataBlock;
	unsigned int         	dataBlockSize;
	char                 	name[VDK_MAXNAME + 1];
	std::string          	fileName;
};

#endif