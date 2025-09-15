/////////////////////////////////////////////////////////////////////
//
// IMDDiskImage.cpp - Implementation of CIMDDiskImage, a helper
//                    class that handles ImageDisk .imd floppy 
//                    image files.
//
// IMD Version 1.18 Revised 8-Mar-2012
//
// For info on .imd files go to:
// 		https://oldcomputers-ddns.org/public/pub/manuals/imd.pdf
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
/////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "../FS_Utils.h"
#include "IMDDiskImage.h"

// Load the contents of a image file.
bool CIMDDiskImage::Load( const std::string& _filename )
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
	fread( &asciiHeader, 1, IMD_ASCII_HEADER_LEN, pIn );
	asciiHeader[IMD_ASCII_HEADER_LEN] = 0;

	std::string strHeader = asciiHeader;
	std::string strSignature = IMD_FORMAT_SIGNATURE;
	if( strHeader.compare(0, strSignature.length(), strSignature) != 0 )
	{
		return false;
	}

	// Read comment
	comment.clear();
	char charBuf;
	fread( &charBuf, 1, 1, pIn );
	while( charBuf != IMD_COMMENT_TERMINATOR )
	{
		comment.push_back(charBuf);
		fread( &charBuf, 1, 1, pIn );
	}

	// Read tracks
	while( ftell(pIn) < uFileSize )
	{
		SIMDTrack tmpTrack;

		fread( &tmpTrack.mode				, 1, 1, pIn );
		fread( &tmpTrack.track				, 1, 1, pIn );
		fread( &tmpTrack.side				, 1, 1, pIn );
		fread( &tmpTrack.sectorsPerTrack	, 1, 1, pIn );
		fread( &tmpTrack.sectorSizeFactor	, 1, 1, pIn );

		if( ((tmpTrack.side & IMD_SIDE_MASK) >= IMD_MAX_SIDE_NUM) || (tmpTrack.sectorSizeFactor > IMD_MAX_SECTOR_SIZE_FACTOR) )
			return false;

		size_t sectorSize = IMD_SECTOR_SIZE_FACTOR_BASE << tmpTrack.sectorSizeFactor;

		bool hasHeadMap		= (tmpTrack.side & IMD_USE_HEAD_MAP_MASK) != 0;
		bool hasCylinderMap	= (tmpTrack.side & IMD_USE_CYLINDER_MAP_MASK) != 0;

		tmpTrack.sectorNumberingMap.resize(tmpTrack.sectorsPerTrack);
		fread( tmpTrack.sectorNumberingMap.data(), 1, tmpTrack.sectorsPerTrack, pIn );
		if( hasHeadMap )
		{
			tmpTrack.sectorHeadMap.resize(tmpTrack.sectorsPerTrack);
			fread( tmpTrack.sectorHeadMap.data(), 1, tmpTrack.sectorsPerTrack, pIn );
		}
		if( hasCylinderMap )
		{
			tmpTrack.sectorCylinderMap.resize(tmpTrack.sectorsPerTrack);
			fread( tmpTrack.sectorNumberingMap.data(), 1, tmpTrack.sectorsPerTrack, pIn );
		}

		// Read sectors
		uint8_t tmpByte = 0;
		for( uint8_t sector = 0; sector < tmpTrack.sectorsPerTrack; ++sector )
		{
			SIMDSector tmpSector;

			fread( &tmpByte, 1, 1, pIn );
			tmpSector.type = ByteToSectorType( tmpByte );

			// Compressed sectors will be uncompressed so they can actually be written to.
			switch( tmpSector.type )
			{
			case EIMDSectorType::NORMAL_DATA:
			case EIMDSectorType::NORMAL_DATA_READ_ERROR:
			case EIMDSectorType::NORMAL_DATA_DELETED_ADDRESS_MARK:
			case EIMDSectorType::DELETED_DATA_READ_ERROR:
				{
					tmpSector.data.resize( sectorSize );
					fread( tmpSector.data.data(), 1, sectorSize, pIn );
				}
				break;
			case EIMDSectorType::COMPRESSED_DATA:
			case EIMDSectorType::COMPRESSED_DATA_READ_ERROR:
			case EIMDSectorType::COMPRESSED_DATA_DELETED_ADDRESS_MARK:
			case EIMDSectorType::COMPRESSED_DELETED_READ_ERROR:
				{
					fread( &tmpByte, 1, 1, pIn );
					tmpSector.data.insert( tmpSector.data.begin(), sectorSize, tmpByte );
				}
				break;
			case EIMDSectorType::UNAVAILABLE:
				break;
			// If the type is outside the expected range, it's an error!
			default:return false; break;
			}

			tmpTrack.sectors.push_back( tmpSector );
		}

		sides[tmpTrack.side & IMD_SIDE_MASK].push_back( tmpTrack );
	}

	// Close file
	fclose( pIn );

	fileName = _filename;

	return true;
}

// Saves current image to a file
bool CIMDDiskImage::Save( const std::string& _filename )
{
	FILE* pOut = fopen( _filename.c_str(), "wb" );
	if( 0 == pOut )
	{
		return false;
	}

	// Write header
	fwrite( &asciiHeader, 1, IMD_ASCII_HEADER_LEN, pOut );

	// Write comment
	char charBuf = IMD_COMMENT_TERMINATOR;
	fwrite( comment.c_str(), comment.length(), 1, pOut );	
	fwrite( &charBuf, 1, 1, pOut );

	// Write tracks
	for( size_t side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		for( size_t track = 0; track < sides[side].size(); ++track )
		{
			const SIMDTrack& curTrack = sides[side][track];

			fwrite( &curTrack.mode				, 1, 1, pOut );
			fwrite( &curTrack.track				, 1, 1, pOut );
			fwrite( &curTrack.side				, 1, 1, pOut );
			fwrite( &curTrack.sectorsPerTrack	, 1, 1, pOut );
			fwrite( &curTrack.sectorSizeFactor	, 1, 1, pOut );
			
			fwrite(  curTrack.sectorNumberingMap.data(), 1, curTrack.sectorNumberingMap.size(), pOut );

			if( !curTrack.sectorCylinderMap.empty() )
				fwrite(  curTrack.sectorCylinderMap.data(), 1, curTrack.sectorCylinderMap.size(), pOut );
			if( !curTrack.sectorHeadMap.empty() )
				fwrite(  curTrack.sectorHeadMap.data(), 1, curTrack.sectorHeadMap.size(), pOut );

			// Write sectors
			uint8_t tmpByte = 0;
			for( size_t sector = 0; sector < curTrack.sectors.size(); ++sector )
			{
				const SIMDSector& curSector = curTrack.sectors[sector];

				// Adjust type as we decompress sectors on load.
				switch( curSector.type )
				{
				case EIMDSectorType::NORMAL_DATA:
				case EIMDSectorType::NORMAL_DATA_READ_ERROR:
				case EIMDSectorType::NORMAL_DATA_DELETED_ADDRESS_MARK:
				case EIMDSectorType::DELETED_DATA_READ_ERROR:
					{
						tmpByte = SectorTypeToByte( curSector.type );
					}
					break;
				case EIMDSectorType::COMPRESSED_DATA:
				case EIMDSectorType::COMPRESSED_DATA_READ_ERROR:
				case EIMDSectorType::COMPRESSED_DATA_DELETED_ADDRESS_MARK:
				case EIMDSectorType::COMPRESSED_DELETED_READ_ERROR:
					{
						// Taking advantage of the fact that the compressed sector type is always
						// the same as the uncompressed one, plus one.
						// So, as we uncompressed the sector data, just subtract one from the type.
						// Please note that this only works for types 1 or higher.
						tmpByte = SectorTypeToByte( curSector.type ) - 1;
					}
				default:break;
				}

				fwrite( &tmpByte, 1, 1, pOut );

				if( curSector.type != EIMDSectorType::UNAVAILABLE )
				{
					fwrite( curSector.data.data(), 1, curSector.data.size(), pOut );
				}
			}
		}
	}

	// Close file
	fclose( pOut );

	return true;
}

// Creates a blank image with the specified parameters.
unsigned int CIMDDiskImage::New( unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize )
{
	// Compute and check shift factor
	unsigned int quotient = uSectorSize / 128;
	unsigned char shiftFactor = static_cast<unsigned char>(log2(quotient));

	if( uSides > IMD_MAX_SIDE_NUM || uSectorSize > IMD_MAX_SECTOR_SIZE || shiftFactor > IMD_MAX_SECTOR_SIZE_FACTOR )
		return 0;

	// Format ASCII header and comment
	//IMD v.vv: dd/mm/yyyy hh:mm:ss\r\n
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time);

    std::ostringstream ssASCIIHeader;
    ssASCIIHeader << "IMD " << IMD_VERSION << ": " << std::put_time(&local_tm, "%d/%m/%Y %H:%M:%S") << "\r\n";

	strcpy( asciiHeader, ssASCIIHeader.str().c_str() );

	comment = "Created by CIMDDiskImage (https://github.com/robcfg/retrotools)";

	// Prepare track and sector data for the new image.
	SIMDTrack tmpTrack;
	tmpTrack.mode				= IMD_MODE_250KBPS_MFM; // Arbitrary default value
	tmpTrack.track				= 0;
	tmpTrack.side				= 0;
	tmpTrack.sectorsPerTrack	= uSecsPerTrack;
	tmpTrack.sectorSizeFactor	= shiftFactor;
	
	SIMDSector tmpSector;
	tmpSector.type = EIMDSectorType::NORMAL_DATA;
	tmpSector.data.insert( tmpSector.data.begin(), uSectorSize, IMD_FILLER_BYTE );

	for( unsigned char sector = 0; sector < uSecsPerTrack; ++sector )
	{
		tmpTrack.sectors.push_back( tmpSector );
		tmpTrack.sectorNumberingMap.push_back(sector);
	}

	// Insert tracks
	for( unsigned char side = 0; side < uSides; ++side )
	{
		sides[side].clear();
		tmpTrack.side = side;

		for( unsigned char track = 0; track < uTracks; ++track )
		{
			tmpTrack.track = track;
			sides[side].push_back( tmpTrack );
		}
	}

	return GetDataSize();
}

// Returns pointer to the required sector's data or NULL if parameters are invalid.
const unsigned char* CIMDDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
	return GetSector( uTrack, uSide, uSector );
}

unsigned char* CIMDDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector )
{
	if( uSide   >= IMD_MAX_SIDE_NUM ) return nullptr;
	if( uTrack  >= sides[uSide].size() ) return nullptr;

	// Look for the right track as they can be in any order on the IMD file.
	size_t trackIdx = IMD_MAX_TRACK_NUM;
	for( size_t track = 0; track < sides[uSide].size(); ++track )
	{
		if( sides[uSide][track].track == uTrack )
		{
			trackIdx = track;
			break;
		}
	}

	if( trackIdx == IMD_MAX_TRACK_NUM )
	{
		return nullptr;
	}

	SIMDTrack& curTrack = sides[uSide][trackIdx];

	// Determine if sector numbers start from 0 or 1.
	// TODO:Maybe the Filesystem should be using the
	//		GetSectorByID function instead.
	unsigned char minSectorID = 1;
	for( size_t sector = 0; sector < curTrack.sectorNumberingMap.size(); ++sector )
	{
		minSectorID = std::min( curTrack.sectorNumberingMap[sector], minSectorID );
	}

	size_t sectorIdx = IMD_MAX_SECTOR_NUM;
	for( size_t sector = 0; sector < curTrack.sectorNumberingMap.size(); ++sector )
	{
		// IMD physical sector numbers may start from 0 or 1, maybe other numbers...
		if( curTrack.sectorNumberingMap[sector] == uSector+minSectorID )
		{
			sectorIdx = sector;
			break;
		}
	}

	if( sectorIdx == IMD_MAX_SECTOR_NUM )
	{
		return nullptr;
	}

	if( curTrack.sectors[sectorIdx].type != EIMDSectorType::UNAVAILABLE || curTrack.sectors[sectorIdx].type != EIMDSectorType::COUNT )
	{
		return curTrack.sectors[sectorIdx].data.data();
	}

	return nullptr;
}

// Returns pointer to the required sector's data or NULL if parameters are invalid.
// const unsigned char* CIMDDiskImage::GetSector( unsigned short int LSN ) const
// {
// 	return GetSector( LSNTrack(*this,LSN), LSNHead(*this,LSN), LSNSector(*this,LSN) );
// }

// IDiskImageInterface ///////////////////////

int CIMDDiskImage::GetSidesNum() const
{
	int retVal = 0;

	for( int side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		retVal += (sides[side].empty() ? 0 : 1);
	}

	return retVal;
}

int CIMDDiskImage::GetTracksNum() const
{
	int retVal = 0;

	for( int side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		retVal = (sides[side].size() <= retVal ? retVal : sides[side].size());
	}

	return retVal;
}

int CIMDDiskImage::GetSectorsNum() const
{
	int retVal = 0;

	for( int side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		for( int track = 0; track < sides[side].size(); ++track )
		{
			// If no current sector per track value is assigned, assign it.
			// Otherwise, if there are tracks with different sectors per track,
			// return -1 to signify that there's not an uniform number of
			// sectors per track.
			if( retVal != 0 && sides[side][track].sectorsPerTrack != 0 && sides[side][track].sectorsPerTrack != retVal )
			{
				return -1;
			}

			retVal = (sides[side][track].sectorsPerTrack <= retVal ? retVal : sides[side][track].sectorsPerTrack);
		}
	}

	return retVal;
}

size_t CIMDDiskImage::GetSectorSize()
{
	size_t retVal = 0;

	for( int side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		for( int track = 0; track < sides[side].size(); ++track )
		{
			// If no current sector size value is assigned, assign it.
			// Otherwise, if there are sectors with different size,
			// return more than the maximum sector size in bytes to 
			// signify that there's not an uniform sector size.
			if( retVal != 0 && sides[side][track].sectors.size() > 0 && sides[side][track].sectorSizeFactor != retVal )
			{
				return IMD_MAX_SECTOR_SIZE + 1;
			}

			retVal = (sides[side][track].sectorSizeFactor <= retVal ? retVal : sides[side][track].sectorSizeFactor);
		}
	}

	return (IMD_SECTOR_SIZE_FACTOR_BASE << retVal);
}

size_t CIMDDiskImage::GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector )
{
	size_t retVal = 0;

	if( _side < IMD_MAX_SIDE_NUM && _track < sides[_side].size() && _sector < sides[_side][_track].sectors.size() )
	{
		retVal = (IMD_SECTOR_SIZE_FACTOR_BASE << sides[_side][_track].sectorSizeFactor);
	}

	return retVal;
}

size_t CIMDDiskImage::GetDataSize()
{
	size_t retVal = 0;

	for( int side = 0; side < IMD_MAX_SIDE_NUM; ++side )
	{
		for( int track = 0; track < sides[side].size(); ++track )
		{
			retVal += (IMD_SECTOR_SIZE_FACTOR_BASE << sides[side][track].sectorSizeFactor) * sides[side][track].sectors.size();
		}
	}

	return retVal;
}

int CIMDDiskImage::GetSectorsNum(size_t _side, size_t _track) const
{
	if( _side < IMD_MAX_SIDE_NUM && _track < sides[_side].size() )
		return sides[_side][_track].sectors.size();

	return 0;
}

std::string CIMDDiskImage::GetFileSpec()
{
	return "ImageDISK IMD files\t*.{imd}\n"; // FLTK Native chooser format.
}

std::string CIMDDiskImage::GetDiskInfo()
{
	std::string retVal;

	if( fileName.empty() )
	{
		return retVal;
	}
	std::stringstream sstream;

	size_t diskSize = GetDataSize();

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

	int sectorsPerTrack = GetSectorsNum();
	int sectorSize = GetSectorSize();

	sstream << shortFileName.c_str() << std::endl << std::endl;
	sstream << "Header      : " << asciiHeader     << std::endl;
	sstream << "Tracks      : " << GetTracksNum()  << std::endl;
	sstream << "Sides       : " << GetSidesNum()   << std::endl;
	sstream << "Sectors     : " << ((sectorsPerTrack >= 0) ? std::to_string(sectorsPerTrack) : "Variable") << std::endl;
	sstream << "Sector size : " << ((sectorSize <= IMD_MAX_SECTOR_SIZE) ? std::to_string(sectorSize) : "Variable") << " bytes" << std::endl;
	sstream << "Total size  : " << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;
	sstream << "Comment     : " << std::endl << comment << std::endl;

	retVal = sstream.str();

	return retVal;
}

const unsigned char* CIMDDiskImage::GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const
{
	return GetSector( uTrack, uSide, uSectorID );
}

STrackInfo CIMDDiskImage::GetTrackInfo ( unsigned int _track, unsigned int _side ) const
{
	STrackInfo retVal;

	int sectorsNum = GetSectorsNum();

	retVal.isValid     = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum());
	retVal.isFormatted = sides[_side][_track].sectors.size() > 0;
	retVal.sectorsNum  = (sectorsNum == -1 ? DISK_IMAGE_INTERFACE_INVALID : sectorsNum);
	retVal.dataSize    = sides[_side][_track].sectors.size() * (IMD_SECTOR_SIZE_FACTOR_BASE << sides[_side][_track].sectorSizeFactor);

	return retVal;
}

unsigned int CIMDDiskImage::GetSectorID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
	return uSector+1;
}

SSectorInfo CIMDDiskImage::GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const
{
	SSectorInfo retVal;

	retVal.isValid = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum()) && (_sector < (unsigned int)GetSectorsNum());

	if( retVal.isValid )
	{
		const SIMDTrack& curTrack = sides[_side][_track];
		
		size_t sectorIdx = IMD_MAX_SECTOR_NUM;
		for( size_t sector = 0; sector < curTrack.sectorNumberingMap.size(); ++sector )
		{
			// IMD physical sector numbers start from 1...
			if( curTrack.sectorNumberingMap[sector] == _sector+1 )
			{
				sectorIdx = sector;
				break;
			}
		}

		if( sectorIdx != IMD_MAX_SECTOR_NUM )
		{
			retVal.hasErrors = false;
			EIMDSectorType sectorType = sides[_side][_track].sectors[sectorIdx].type;
			switch( sectorType )
			{
			case EIMDSectorType::NORMAL_DATA_READ_ERROR			:
			case EIMDSectorType::COMPRESSED_DATA_READ_ERROR		:
			case EIMDSectorType::DELETED_DATA_READ_ERROR		:
			case EIMDSectorType::COMPRESSED_DELETED_READ_ERROR	:
				{
					retVal.hasErrors = true;
				}
				break;
			default:break;
			}
			retVal.isInUse   = true;  // FS should check or update this info
			retVal.isWeak    = false;
			retVal.copiesNum = 1; // Number of copies of the sector stored
			retVal.dataSize  = sides[_side][_track].sectors[sectorIdx].data.size();
		}
		else
		{
			retVal.isValid = false;
		}
	}

	return retVal;
}

EIMDSectorType CIMDDiskImage::ByteToSectorType( uint8_t _byte ) const
{
	switch( _byte )
	{
		case 0: return EIMDSectorType::UNAVAILABLE; 							break;
		case 1: return EIMDSectorType::NORMAL_DATA; 							break;
		case 2: return EIMDSectorType::COMPRESSED_DATA; 						break;
		case 3: return EIMDSectorType::NORMAL_DATA_DELETED_ADDRESS_MARK; 		break;
		case 4: return EIMDSectorType::COMPRESSED_DATA_DELETED_ADDRESS_MARK; 	break;
		case 5: return EIMDSectorType::NORMAL_DATA_READ_ERROR;					break;
		case 6: return EIMDSectorType::COMPRESSED_DATA_READ_ERROR;				break;
		case 7: return EIMDSectorType::DELETED_DATA_READ_ERROR;					break;
		case 8: return EIMDSectorType::COMPRESSED_DELETED_READ_ERROR;			break;
		default:return EIMDSectorType::COUNT;									break;
	}
}

uint8_t CIMDDiskImage::SectorTypeToByte( EIMDSectorType _sectorType ) const
{
	switch( _sectorType )
	{
		case EIMDSectorType::UNAVAILABLE							: return 0; break;
		case EIMDSectorType::NORMAL_DATA							: return 1; break;
		case EIMDSectorType::COMPRESSED_DATA						: return 2; break;
		case EIMDSectorType::NORMAL_DATA_DELETED_ADDRESS_MARK		: return 3; break;
		case EIMDSectorType::COMPRESSED_DATA_DELETED_ADDRESS_MARK	: return 4; break;
		case EIMDSectorType::NORMAL_DATA_READ_ERROR					: return 5; break;
		case EIMDSectorType::COMPRESSED_DATA_READ_ERROR				: return 6; break;
		case EIMDSectorType::DELETED_DATA_READ_ERROR				: return 7; break;
		case EIMDSectorType::COMPRESSED_DELETED_READ_ERROR			: return 8; break;
		default: return 9; break;
	}
}

IDiskImageInterface* CIMDDiskImage::NewImage() const
{
	return new CIMDDiskImage;
}
