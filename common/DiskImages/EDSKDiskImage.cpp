///////////////////////////////////////////////////////////////////
//
// EDSKDiskImage.cpp - Implementation of the CEDSKDiskImage class.
//                     It's purpose it's to read and write
//                     disk image files in the format defined
//                     by Marco Vieth, Ulrich Doewich and 
//                     Kevin Thacker.
//
// Created 10/02/2017 by Roberto Carlos Fernández Gerhardt.
//
///////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include "EDSKDiskImage.h"

// debug includes ////////
#include <iostream>
//////////////////////////

//////////////////////////////////////////
// Default constructor
//////////////////////////////////////////
CEDSKDiskImage::CEDSKDiskImage()
{
	lastError = "No error.";
	isExtendedDSK = false;
}

//////////////////////////////////////////
// Destructor
//////////////////////////////////////////
CEDSKDiskImage::~CEDSKDiskImage()
{

}

//////////////////////////////////////////
// Load - Read a DSK file and store its
//        data in the adequate structures.
//
// Returns true if load was successful,
// false otherwise.
// In case of error, the cause will be
// stored in the lastError string.
//////////////////////////////////////////
bool CEDSKDiskImage::Load(const std::string& _filename)
{
	// Open File
	FILE* pIn = fopen(_filename.c_str(),"rb");
	if( NULL == pIn )
	{
		lastError = "Could not open file ";
		lastError += _filename;
		lastError += " for reading.";

		return false;
	}

	// Check file size. Must be at least 256 bytes long to be able to read the header
	fseek(pIn,0,SEEK_END);
	size_t fileSize = ftell(pIn);
	fseek(pIn,0,SEEK_SET);

	if( fileSize < 256 )
	{
		lastError = "File ";
		lastError += _filename;
		lastError += " is less than 256 bytes long. It's either corrupted or not a .DSK file.";

		return false;
	}

	// Clear data. Any other info from a previous load will be overwritten.
	diskInfoBlock.trackSizeTable.clear();
	sides[0].clear();
	sides[1].clear();

	// Read DISK INFORMATION BLOCK
	ReadDiskInformationBlock(pIn);

	// Sanity checks
	if( diskInfoBlock.sidesNum > CEDSKFile_MAX_SIDES_NUM )
	{
		return false;
	}

	if( diskInfoBlock.header.compare(0,2,"MV",2) != 0 && diskInfoBlock.header.compare(0,8,"EXTENDED",8) != 0 )
	{
		return false; // Not a normal or extended DSK file
	}

	// Read tracks
	for( unsigned short int track = 0; track < diskInfoBlock.tracksNum; ++track )
	{
		for( unsigned char side = 0; side < diskInfoBlock.sidesNum; ++side )
		{
			// Read Track info block if this is not an extended DSK, or if the disk is an
			// extended DSK and thus the track size table has a non-zero value.
			if( !isExtendedDSK || diskInfoBlock.trackSizeTable[ (track * diskInfoBlock.sidesNum) + side] )
			{
				size_t fileOffset = ftell(pIn);

				if( !ReadTrackInformationBlock(pIn) )
				{
					fclose(pIn);
					return false;
				}

				if( isExtendedDSK )
					fseek( pIn, fileOffset + (diskInfoBlock.trackSizeTable[ (track * diskInfoBlock.sidesNum) + side]*256),SEEK_SET );
			}
			else
			{
				// Insert unformatted track info
				CEDSKFile_TrackInfoBlock tib;
				tib.isUnformatted = true;
				sides[side].push_back(tib);
			}
		}
	}

	// Close file
	fclose(pIn);

	fileName = _filename;

	// Fix extra tracks
	/*diskInfoBlock.tracksNum -= 3;
	diskInfoBlock.trackSizeTable.pop_back();
	diskInfoBlock.trackSizeTable.pop_back();
	diskInfoBlock.trackSizeTable.pop_back();
	sides[0].pop_back();
	sides[0].pop_back();
	sides[0].pop_back();
	std::string newName = fileName;
	newName.resize( newName.length() - 4 );
	newName += "_40T.dsk";
	Save(newName);*/

	return true;
}

//////////////////////////////////////////
// Save - Write current data to a DSK 
//        file.
//
// Returns true if load was successful,
// false otherwise.
// In case of error, the cause will be
// stored in the lastError string.
//////////////////////////////////////////
bool CEDSKDiskImage::Save(const std::string& _filename)
{
	// Open File
	FILE* pOut = fopen(_filename.c_str(),"wb");
	if( NULL == pOut )
	{
		lastError = "Could not open file ";
		lastError += _filename;
		lastError += " for writing.";

		return false;
	}

	// Write DISK INFORMATION BLOCK
	WriteDiskInformationBlock(pOut);

	// Write tracks
	for( unsigned short int track = 0; track < diskInfoBlock.tracksNum; ++track )
	{
		for( unsigned char side = 0; side < diskInfoBlock.sidesNum; ++side )
		{
			WriteTrackInformationBlock(pOut,sides[side][track]);
		}
	}

	// Close File
	fclose(pOut);

	return true;
}

unsigned int CEDSKDiskImage::New(unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize)
{
	// Clear data. Any other info from a previous load will be overwritten.
	diskInfoBlock.trackSizeTable.clear();
	sides[0].clear();
	sides[1].clear();

	// Compute and check shift factor
	unsigned int quotient = uSectorSize / 128;
	unsigned char shiftFactor = static_cast<unsigned char>(log2(quotient));

	// Fill Disk Info Block data
	diskInfoBlock.creator	= "CEDSKDiskImage";
	diskInfoBlock.header 	= "MV - CPCEMU Disk-File\r\nDisk-Info\r\n";
	diskInfoBlock.sidesNum	= uSides;
	diskInfoBlock.trackSize	= (uSecsPerTrack * uSectorSize) + 256;
	diskInfoBlock.tracksNum = uTracks;

	// Create sector info and data
	CEDSKFile_SectorInfo emptySectorInfo;

	emptySectorInfo.sectorSize = shiftFactor;
	emptySectorInfo.FDCStatus1 = 0;
	emptySectorInfo.FDCStatus2 = 0;
	emptySectorInfo.dataLength = 0;

	uint8vector emptySectorData;
	emptySectorData.insert( emptySectorData.begin(), uSectorSize, 0xE5 );

	// Fill tracks
	CEDSKFile_TrackInfoBlock emptyTrack;
	emptyTrack.dataRate = 0;
	emptyTrack.fillerByte = 0xE5;
	emptyTrack.gap3Length = 0x4E;
	emptyTrack.header = "Track-Info\r\n";
	emptyTrack.isUnformatted = false;
	emptyTrack.recordingMode = 0;
	emptyTrack.sectorsNum = uSecsPerTrack;
	emptyTrack.sectorSize = shiftFactor;
	emptyTrack.unused[0] = 0;
	emptyTrack.unused[1] = 0;
	emptyTrack.unused[2] = 0;
	for( unsigned char sector = 0; sector < uSecsPerTrack; ++sector )
	{
		emptyTrack.sectorData.push_back( emptySectorData );
	}

	for( unsigned short int track = 0; track < diskInfoBlock.tracksNum; ++track )
	{
		for( unsigned char side = 0; side < diskInfoBlock.sidesNum; ++side )
		{
			emptyTrack.sideNumber  = side;
			emptyTrack.trackNumber = track;
			emptyTrack.sectorInfoList.clear();
			for( unsigned char sector = 0; sector < uSecsPerTrack; ++sector )
			{
				emptySectorInfo.side		= side;
				emptySectorInfo.track		= track;
				emptySectorInfo.sectorID	= sector;
				
				emptyTrack.sectorInfoList.push_back(emptySectorInfo);
			}
			sides[side].push_back( emptyTrack );
		}
	}

	return uSides * uTracks * uSecsPerTrack * uSectorSize;
}

///////////////////////////////////////////////////////////////////////
// ReadDiskInformationBlock - Read a CEDSKFile_DiskInfoBlock structure
//                            from an open file.
///////////////////////////////////////////////////////////////////////
void CEDSKDiskImage::ReadDiskInformationBlock(FILE* _pIn)
{
	char tmpBuf[35];

	// Read header string
	memset(tmpBuf,0,35);
	fread(tmpBuf,1,34,_pIn);
	diskInfoBlock.header = tmpBuf;
	isExtendedDSK = (diskInfoBlock.header.substr(0,8) == "EXTENDED");

	// Read creator string
	memset(tmpBuf,0,35);
	fread(tmpBuf,1,14,_pIn);
	diskInfoBlock.creator = tmpBuf;

	// Read track number and size, and side number
	fread(&diskInfoBlock.tracksNum,1,1,_pIn);
	fread(&diskInfoBlock.sidesNum ,1,1,_pIn);
	fread(&diskInfoBlock.trackSize,1,2,_pIn);

	// Read track size table
	unsigned int tableLength = diskInfoBlock.tracksNum * diskInfoBlock.sidesNum;
	for( unsigned int tableByte = 0; tableByte < tableLength; ++tableByte )
	{
		fread(&tmpBuf[0],1,1,_pIn);
		diskInfoBlock.trackSizeTable.push_back(tmpBuf[0]);
	}

	// Skip unused block data
	fseek( _pIn, 256, SEEK_SET );
}

///////////////////////////////////////////////////////////////////////
// WriteDiskInformationBlock - Write a CEDSKFile_DiskInfoBlock structure
//                             to an open file.
///////////////////////////////////////////////////////////////////////
void CEDSKDiskImage::WriteDiskInformationBlock(FILE* _pOut) const
{
	size_t written = 0;
	char tmpBuf[35];

	// Write header string
	written += fwrite(diskInfoBlock.header.c_str(),1,34,_pOut);

	// Write creator string
	written += fwrite(diskInfoBlock.creator.c_str(),1,14,_pOut);

	// Write track number and size, and side number
	written += fwrite(&diskInfoBlock.tracksNum,1,1,_pOut);
	written += fwrite(&diskInfoBlock.sidesNum ,1,1,_pOut);
	written += fwrite(&diskInfoBlock.trackSize,1,2,_pOut);

	// Write track size table
	written += fwrite(diskInfoBlock.trackSizeTable.data(),1,diskInfoBlock.trackSizeTable.size(),_pOut);

	// Add padding bytes
	tmpBuf[0] = 0;
	size_t padding = 256 - written;
	for( size_t paddingByte = 0; paddingByte < padding; ++paddingByte )
	{
		fwrite(&tmpBuf[0],1,1,_pOut);
	}
}

///////////////////////////////////////////////////////////////////////
// ReadTrackInformationBlock - Read a CEDSKFile_TrackInfoBlock
//                             structure and associated sector info
//                             and data from an open file.
///////////////////////////////////////////////////////////////////////
bool CEDSKDiskImage::ReadTrackInformationBlock(FILE* _pIn)
{
	CEDSKFile_TrackInfoBlock tib;
	tib.isUnformatted = false;
	char tmpBuf[14];

	// Store current file offset to skip unused data after reading block.
	// The Track info block is 256 bytes long on disk.
	size_t fileOffset = ftell(_pIn);

	// Read header
	memset(tmpBuf,0,14);
	fread(tmpBuf,1,13,_pIn);
	tib.header = tmpBuf;

	if( tib.header.compare("Track-Info\r\n") != 0 )
	{
		return false;
	}

	// Read data
	fread( tib.unused       ,1,3,_pIn);
	fread(&tib.trackNumber  ,1,1,_pIn);
	fread(&tib.sideNumber   ,1,1,_pIn);
	fread(&tib.dataRate     ,1,1,_pIn);
	fread(&tib.recordingMode,1,1,_pIn);
	fread(&tib.sectorSize   ,1,1,_pIn);
	fread(&tib.sectorsNum   ,1,1,_pIn);
	fread(&tib.gap3Length   ,1,1,_pIn);
	fread(&tib.fillerByte   ,1,1,_pIn);
	
	// Read sector info blocks
	for( unsigned char sector = 0; sector < tib.sectorsNum; ++sector )
	{
		ReadSectorInformationBlock(_pIn,tib);		
	}

	// Skip unused data
	fseek( _pIn, fileOffset+256, SEEK_SET );

	// Read actual sector data
	for( unsigned char sector = 0; sector < tib.sectorsNum; ++sector )
	{
		unsigned short int dataSize = tib.sectorInfoList[sector].dataLength;
		if( !isExtendedDSK )
		{
			dataSize = (unsigned short int)(128 * pow(2,tib.sectorSize));
		}
		uint8vector tmpVec;

		for( unsigned short int dataByte = 0; dataByte < dataSize; ++dataByte )
		{
			fread(&tmpBuf[0],1,1,_pIn);
			tmpVec.push_back(tmpBuf[0]);
		}

		tib.sectorData.push_back(tmpVec);
	}

	// Insert track data
	sides[tib.sideNumber].push_back(tib);

	return true;
}

/////////////////////////////////////////////////////////////////////////
// WriteTrackInformationBlock - Write a CEDSKFile_TrackInfoBlock structure
//                              and associated sector info and data 
//                              to an open file.
/////////////////////////////////////////////////////////////////////////
void CEDSKDiskImage::WriteTrackInformationBlock(FILE* _pOut, const CEDSKFile_TrackInfoBlock& _tib) const
{
	// Write only formatted tracks
	if( _tib.isUnformatted )
		return;

	size_t written = 0;
	char tmpBuf[35];

	// Write header
	written += fwrite(_tib.header.c_str(),1,13,_pOut);

	// Write data
	written += fwrite( _tib.unused       ,1,3,_pOut);
	written += fwrite(&_tib.trackNumber  ,1,1,_pOut);
	written += fwrite(&_tib.sideNumber   ,1,1,_pOut);
	written += fwrite(&_tib.dataRate     ,1,1,_pOut);
	written += fwrite(&_tib.recordingMode,1,1,_pOut);
	written += fwrite(&_tib.sectorSize   ,1,1,_pOut);
	written += fwrite(&_tib.sectorsNum   ,1,1,_pOut);
	written += fwrite(&_tib.gap3Length   ,1,1,_pOut);
	written += fwrite(&_tib.fillerByte   ,1,1,_pOut);
	
	// Write sector info blocks
	for( unsigned char sector = 0; sector < _tib.sectorsNum; ++sector )
	{
		written += WriteSectorInformationBlock(_pOut,_tib.sectorInfoList[sector]);
	}

	// Add padding bytes
	tmpBuf[0] = 0;
	size_t padding = 256 - written;
	for( size_t paddingByte = 0; paddingByte < padding; ++paddingByte )
	{
		fwrite(&tmpBuf[0],1,1,_pOut);
	}

	// Write sector data
	for( unsigned char sector = 0; sector < _tib.sectorsNum; ++sector )
	{
		unsigned short int dataSize = (_tib.sectorInfoList[sector].dataLength != 0) ? _tib.sectorInfoList[sector].dataLength : _tib.sectorData[sector].size();
		
		fwrite(_tib.sectorData[sector].data(),1,dataSize,_pOut);
	}
}

////////////////////////////////////////////////////////////////////////
// ReadSectorInformationBlock - Read a CEDSKFile_SectorInfo structure
//                              from an open file and store it in the
//                              given CEDSKFile_TrackInfoBlock structure.
////////////////////////////////////////////////////////////////////////
void CEDSKDiskImage::ReadSectorInformationBlock(FILE* _pIn, CEDSKFile_TrackInfoBlock& _tib)
{
	CEDSKFile_SectorInfo si;

	fread(&si.track     ,1,1,_pIn);
	fread(&si.side      ,1,1,_pIn);
	fread(&si.sectorID  ,1,1,_pIn);
	fread(&si.sectorSize,1,1,_pIn);
	fread(&si.FDCStatus1,1,1,_pIn);
	fread(&si.FDCStatus2,1,1,_pIn);
	fread(&si.dataLength,1,2,_pIn);

	_tib.sectorInfoList.push_back(si);
}

////////////////////////////////////////////////////////////////////////
// WriteSectorInformationBlock - Write a CEDSKFile_SectorInfo structure
//                               to an open file.
//
//                               Returns the number of bytes written
//                               to the file to allow calculation of
//                               the number of padding bytes in the
//                               WriteTrackInformationBlock function.
////////////////////////////////////////////////////////////////////////
size_t CEDSKDiskImage::WriteSectorInformationBlock(FILE* _pOut, const CEDSKFile_SectorInfo& _si) const
{
	size_t written = 0;

	written += fwrite(&_si.track     ,1,1,_pOut);
	written += fwrite(&_si.side      ,1,1,_pOut);
	written += fwrite(&_si.sectorID  ,1,1,_pOut);
	written += fwrite(&_si.sectorSize,1,1,_pOut);
	written += fwrite(&_si.FDCStatus1,1,1,_pOut);
	written += fwrite(&_si.FDCStatus2,1,1,_pOut);
	written += fwrite(&_si.dataLength,1,2,_pOut);

	return written;
}

int CEDSKDiskImage::GetSidesNum() const
{
	return (int)diskInfoBlock.sidesNum;
}

int CEDSKDiskImage::GetTracksNum() const
{
	return (int)diskInfoBlock.tracksNum;
}

int CEDSKDiskImage::GetSectorsNum() const
{
	if( 0 < GetSidesNum() && 0 < GetTracksNum() )
		return sides[0][0].sectorInfoList.size();

	return 0;
}

int CEDSKDiskImage::GetSectorsNum(size_t _side, size_t _track) const
{
	if( _side < (size_t)GetSidesNum() && _track < (size_t)GetTracksNum() )
		return sides[_side][_track].sectorInfoList.size();

	return 0;
}

std::string CEDSKDiskImage::GetFileSpec()
{
	return "DSK/EDSK files\t*.{dsk}\n"; // FLTK Native chooser format.
}

std::string CEDSKDiskImage::GetDiskInfo()
{
	std::string retVal;

	if( fileName.empty() )
		return retVal;

	std::stringstream sstream;

	//int diskSize = diskInfoBlock.tracksNum * diskInfoBlock.sidesNum * diskInfoBlock.sectorsNum * diskInfoBlock.sectorSize;
	int diskSize = 0;
	int sectorsNum = 0;
	int sectorSize = 0;

	for( int side = 0; side < diskInfoBlock.sidesNum; ++side )
	{
		for( size_t track = 0; track < sides[side].size(); ++track )
		{
			size_t curSectorsNum = sides[side][track].sectorInfoList.size();

			if( sectorsNum == 0 )
				sectorsNum = curSectorsNum;
			else if( sectorsNum > 0 && curSectorsNum > 0 && sectorsNum != curSectorsNum )
				sectorsNum = -1;

			for( size_t sector = 0; sector < sides[side][track].sectorInfoList.size(); ++sector )
			{
				int curSectorSize = (int)(128 * pow(2,sides[side][track].sectorInfoList[sector].sectorSize));
				diskSize += curSectorSize;

				if(sectorSize == 0)
					sectorSize = curSectorSize;
				else if(sectorSize > 0 && sectorSize != curSectorSize)
					sectorSize = -1; // We have sectors with variable size
			}
		}
	}

	std::string shortFileName;
	size_t found = fileName.find_last_of("/\\");
	if( found )
		shortFileName = fileName.substr(found+1);
	else
		shortFileName = fileName;

	sstream << shortFileName.c_str() << std::endl << std::endl;
	sstream << "Creator     : "   <<      diskInfoBlock.creator.c_str() << std::endl;
	sstream << "Tracks      : "   << (int)diskInfoBlock.tracksNum       << std::endl;
	sstream << "Sides       : "   << (int)diskInfoBlock.sidesNum        << std::endl;
	if( sectorsNum >= 0 )
		sstream << "Sectors     : "   << sectorsNum << std::endl;
	else
		sstream << "Sectors     : "   << "Variable" << std::endl;
	if( sectorSize >= 0 )
		sstream << "Sector size : "   << sectorSize << " bytes" << std::endl;
	else
		sstream << "Sector size : "   << "Variable" << std::endl;
	sstream << "Total size  : "   << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;
	//sstream << "Gap #3      : "   << diskInfoBlock.gap3Length << endl;
	//sstream << "Filler byte : 0x" << hex << diskInfoBlock.fillerByte << dec << endl;
	retVal = sstream.str();

	return retVal;
}

unsigned char* CEDSKDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector )
{
	if( uTrack < (unsigned int)GetTracksNum() && uSide < (unsigned int)GetSidesNum() && uSector < sides[uSide][uTrack].sectorsNum )
	{
		return sides[uSide][uTrack].sectorData[uSector].data();
	}

	return NULL;
}

const unsigned char* CEDSKDiskImage::GetSector( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
	return GetSector( uTrack, uSide, uSector );
}

unsigned int CEDSKDiskImage::GetSectorID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
	if( uTrack < (unsigned int)GetTracksNum() && uSide < (unsigned int)GetSidesNum() )
	{
		if( !sides[uSide].empty() && !sides[uSide][uTrack].sectorInfoList.empty() )
			return sides[uSide][uTrack].sectorInfoList[uSector].sectorID;
	}

	return DISK_IMAGE_INTERFACE_INVALID;
}

const unsigned char* CEDSKDiskImage::GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const
{
	if( uTrack < (unsigned int)GetTracksNum() && uSide < (unsigned int)GetSidesNum() && uTrack < sides[uSide].size() && !sides[uSide][uTrack].isUnformatted )
	{
		int currSector = 0;
		while( currSector < sides[uSide][uTrack].sectorsNum )
		{
			if( uSectorID == GetSectorID(uTrack, uSide, currSector) )
			{
				return GetSector(uTrack, uSide, currSector);
			}

			++currSector;
		}
	}

	return NULL;
}

STrackInfo CEDSKDiskImage::GetTrackInfo ( unsigned int _track, unsigned int _side  ) const
{
	STrackInfo retVal;

	retVal.isValid     = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum());
	if( retVal.isValid )
	{
		retVal.isFormatted = !sides[_side][_track].isUnformatted;
		retVal.sectorsNum  =  sides[_side][_track].sectorInfoList.size();
		retVal.dataSize    =  0;
		for( size_t sec = 0; sec < sides[_side][_track].sectorInfoList.size(); ++sec )
		{
			retVal.dataSize += (size_t)(128 * pow(2,sides[_side][_track].sectorInfoList[sec].sectorSize));
		}
	}

	return retVal;
}

SSectorInfo CEDSKDiskImage::GetSectorInfo( unsigned int _track, unsigned int _side, unsigned int _sector ) const
{
	SSectorInfo retVal;

	retVal.isValid   = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum()) && (_sector < sides[_side][_track].sectorsNum);
	if( retVal.isValid )
	{
		retVal.hasErrors = sides[_side][_track].sectorInfoList[_sector].FDCStatus1 != 0 || sides[_side][_track].sectorInfoList[_sector].FDCStatus2 != 0;
		retVal.isInUse   = true;  // FS should check or update this info
		retVal.dataSize  = (size_t)(128 * pow(2,sides[_side][_track].sectorInfoList[_sector].sectorSize));
		retVal.copiesNum = sides[_side][_track].sectorInfoList[_sector].dataLength / retVal.dataSize; // Number of copies of the sector stored
		retVal.isWeak    = retVal.copiesNum > 1;
	}

	return retVal;
}

size_t CEDSKDiskImage::GetSectorSize( unsigned int _track, unsigned int _side, unsigned int _sector )
{
	SSectorInfo si = GetSectorInfo( _track, _side, _sector );
	
	return si.isValid ? si.dataSize : 0;
}

size_t CEDSKDiskImage::GetSectorSize()
{
	size_t retVal = GetSectorSize(0,0,0);

	for( unsigned int side = 0; side < GetSidesNum(); ++side )
	{
		for( unsigned int track = 0; track < sides[side].size(); ++track )
		{
			for( unsigned int sector = 0; sector < sides[side][track].sectorsNum; ++sector )
			{
				if( GetSectorSize(side, track, sector) != retVal )
				{
					return 0;
				}
			}
		}
	}

	return retVal;
}

size_t CEDSKDiskImage::GetDataSize()
{
	size_t retVal = 0;

	for( size_t side = 0; side < CEDSKFile_MAX_SIDES_NUM; ++side )
	{
		for( size_t track = 0; track < sides[side].size(); ++track )
		{
			size_t curSectorsNum = sides[side][track].sectorInfoList.size();

			for( size_t sector = 0; sector < sides[side][track].sectorInfoList.size(); ++sector )
			{
				int curSectorSize = (int)(128 * pow(2,sides[side][track].sectorInfoList[sector].sectorSize));
				retVal += curSectorSize;
			}
		}
	}

	return retVal;
}
