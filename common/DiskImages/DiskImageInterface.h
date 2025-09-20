//////////////////////////////////////////////////////////////
//
// DiskImageInterface.h - Header file for the Disk Image
//                        interface class.
//
//                        It is a common interface for
//                        different disk image format classes.
//
// Created 13/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// Notes:
//
//////////////////////////////////////////////////////////////

#pragma once

#include <string>

#define DISK_IMAGE_INTERFACE_INVALID 0xFFFFFFFF

struct STrackInfo
{
	bool	isValid;
	bool	isFormatted;
	size_t	sectorsNum;
	size_t  dataSize;
};

struct SSectorInfo
{
	bool	isValid;
	bool    hasErrors;
	bool	isInUse;
	bool	isWeak;
	size_t	copiesNum; // Number of copies of the sector stored
	size_t  dataSize;
};

class IDiskImageInterface
{
public:
	virtual ~IDiskImageInterface() {};

	// IDiskImageInterface functions to be implemented by derived classes. //////////////////////////////////
	virtual bool					Load(const std::string& _filename) = 0;
	virtual bool					Save(const std::string& _filename) = 0;
	virtual unsigned int 			New ( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize ) = 0;

	virtual int 					GetSidesNum() const = 0;
	virtual int 					GetTracksNum() const = 0;
	virtual int 					GetSectorsNum() const = 0;
	virtual int 					GetSectorsNum(size_t _side, size_t _track) const = 0;

	virtual STrackInfo				GetTrackInfo ( unsigned int _track, unsigned int _side ) const = 0;
	virtual SSectorInfo				GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const = 0;
 
	virtual const unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const = 0;
	virtual unsigned char*			GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) = 0;
	virtual unsigned int			GetSectorID  ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const { return (unsigned char)(uSector & 0xFF); }
	virtual const unsigned char*	GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const = 0;

	virtual std::string 			GetFileSpec() = 0;
	virtual std::string 			GetDiskInfo() = 0;

	virtual size_t					GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) { return 0; }
	virtual size_t					GetSectorSize() { return 0; }

	virtual size_t					GetDataSize() = 0;

	virtual bool 					NeedManualSetup() const = 0;

	virtual void					SetSidesNum  ( size_t _sides    ) {}
	virtual void					SetTracksNum ( size_t _tracks   ) {}
	virtual void					SetSectorsNum( size_t _sectors  ) {}
	virtual void					SetSectorSize( size_t _size     ) {}

	virtual IDiskImageInterface*	NewImage() const = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Copy and uncomment the block below into your derived class' header file.

	// IDiskImageInterface //////////////////////////////////////////////////////////////////////////////////
	// bool					Load( const std::string& _filename ) override;
	// bool					Save( const std::string& _filename ) override;
	// unsigned int 			New ( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize ) override;

	// int 					GetSidesNum() const override;
	// int 					GetTracksNum() const override;
	// int 					GetSectorsNum() const override;
	// int 					GetSectorsNum(size_t _side, size_t _track) const override;

	// STrackInfo				GetTrackInfo ( unsigned int _track, unsigned int _side ) const override;
	// SSectorInfo				GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const override;

	// const 	unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
	// 		unsigned char*	GetSector    ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) override;
	// 		unsigned int	GetSectorID  ( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;
	// const 	unsigned char*	GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const override;

	// std::string 			GetFileSpec() override;
	// std::string 			GetDiskInfo() override;

	// size_t					GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector ) override;
	// size_t					GetSectorSize() override;

	// void					SetSidesNum  ( size_t _sides    ) override;
	// void					SetTracksNum ( size_t _tracks   ) override;
	// void					SetSectorsNum( size_t _sectors  ) override;
	// void					SetSectorSize( size_t _size     ) override;

	// size_t					GetDataSize() override;

	// bool 					NeedManualSetup() const override { return false; }

	// IDiskImageInterface*	NewImage() const override { return new ; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
};
