///////////////////////////////////////////////////////////////////
//
// dskfile.cpp - Implementation of the CDSKFile class.
//               It's purpose it's to read and write
//               disk image files in the format defined
//               by Marco Vieth, Ulrich Doewich and Kevin Thacker.
//
// Created 10/02/2017 by Roberto Carlos Fernández Gerhardt.
//
// Last update on 12/02/2017.
//
///////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cstring>
#include "dskfile.h"

// debug includes ////////
#include <iostream>
//////////////////////////

//////////////////////////////////////////
// Default constructor
//////////////////////////////////////////
CDSKFile::CDSKFile()
{
	lastError = "No error.";
	isExtendedDSK = false;
}

//////////////////////////////////////////
// Destructor
//////////////////////////////////////////
CDSKFile::~CDSKFile()
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
bool CDSKFile::Load(const string& _filename)
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

	// Read tracks
	for( unsigned short int track = 0; track < diskInfoBlock.tracksNum; ++track )
	{
		for( unsigned char side = 0; side < diskInfoBlock.sidesNum; ++side )
		{
			if(diskInfoBlock.trackSize || diskInfoBlock.trackSizeTable[ (track * diskInfoBlock.sidesNum) + side] )
			{
				ReadTrackInformationBlock(pIn);
			}
			else
			{
				// Insert unformatted track info
				CDSKFile_TrackInfoBlock tib;
				tib.isUnformatted = true;
				sides[side].push_back(tib);
			}
	 	}
	}

	// Close file
	fclose(pIn);

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
bool CDSKFile::Save(const string& _filename)
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

///////////////////////////////////////////////////////////////////////
// ReadDiskInformationBlock - Read a CDSKFile_DiskInfoBlock structure
//                            from an open file.
///////////////////////////////////////////////////////////////////////
void CDSKFile::ReadDiskInformationBlock(FILE* _pIn)
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
// WriteDiskInformationBlock - Write a CDSKFile_DiskInfoBlock structure
//                             to an open file.
///////////////////////////////////////////////////////////////////////
void CDSKFile::WriteDiskInformationBlock(FILE* _pOut) const
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
// ReadTrackInformationBlock - Read a CDSKFile_TrackInfoBlock structure
//                             and associated sector info and data 
//                             from an open file.
///////////////////////////////////////////////////////////////////////
void CDSKFile::ReadTrackInformationBlock(FILE* _pIn)
{
	CDSKFile_TrackInfoBlock tib;
	tib.isUnformatted = false;
	char tmpBuf[14];

	// Store current file offset to skip unused data after reading block.
	// The Track info block is 256 bytes long on disk.
	size_t fileOffset = ftell(_pIn);

	// Read header
	memset(tmpBuf,0,14);
	fread(tmpBuf,1,13,_pIn);
	tib.header = tmpBuf;

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
		if( 0 == dataSize )
		{
			if( 0 != tib.sectorSize )
			{
				dataSize = (256 << (tib.sectorSize - 1));
			}
			else
			{
				return;
			}
			
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
}

/////////////////////////////////////////////////////////////////////////
// WriteTrackInformationBlock - Write a CDSKFile_TrackInfoBlock structure
//                              and associated sector info and data 
//                              to an open file.
/////////////////////////////////////////////////////////////////////////
void CDSKFile::WriteTrackInformationBlock(FILE* _pOut, const CDSKFile_TrackInfoBlock& _tib) const
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
		unsigned short int dataSize = _tib.sectorInfoList[sector].dataLength;
		
		fwrite(_tib.sectorData[sector].data(),1,dataSize,_pOut);
	}
}

////////////////////////////////////////////////////////////////////////
// ReadSectorInformationBlock - Read a CDSKFile_SectorInfo structure
//                              from an open file and store it in the
//                              given CDSKFile_TrackInfoBlock structure.
////////////////////////////////////////////////////////////////////////
void CDSKFile::ReadSectorInformationBlock(FILE* _pIn, CDSKFile_TrackInfoBlock& _tib)
{
	CDSKFile_SectorInfo si;

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
// WriteSectorInformationBlock - Write a CDSKFile_SectorInfo structure
//                               to an open file.
//
//                               Returns the number of bytes written
//                               to the file to allow calculation of
//                               the number of padding bytes in the
//                               WriteTrackInformationBlock function.
////////////////////////////////////////////////////////////////////////
size_t CDSKFile::WriteSectorInformationBlock(FILE* _pOut, const CDSKFile_SectorInfo& _si) const
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
