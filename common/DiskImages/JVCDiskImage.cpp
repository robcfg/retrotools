#include <cmath>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "JVCDiskImage.h"

// Standard disk image geometry sizes
const unsigned int JVC_HEADER_DATA_SIZE 		= 5; // 5 bytes for standard JVC header
const unsigned int JVC_HEADER_MAX_SECTOR_SIZE 	= 1024;
const unsigned int JVC_HEADER_SECTOR_SIZE_BASE	= 128;


CJVCDiskImage::CJVCDiskImage() : dataBlock(nullptr), dataBlockSize(0), tracks(40), headerSize(-1) {}

CJVCDiskImage::~CJVCDiskImage()
{
	if (dataBlock)
	{
		delete[] dataBlock;
	}
}

// TODO:Move this into the main program 
bool CheckDragonDOSFSMetadata(FILE* pIn, unsigned char& sideCount)
{
	unsigned char tracksOnDisk, sectorsPerTrackDisk, tracksComplement, sectorsComplement;

	// Constants
	const int track = 20;
	const int sector = 1; // sector 1 (assuming sector numbering starts at 1)
	const int sectorSize = 256;
	const int sectorsPerTrack = 18;
	const int sigOffset = sectorSize - 4; // final 4 bytes of the sector

	// Check first position (single-sided 80 track)
	long singleSidedOffset = (track * sectorsPerTrack + (sector - 1)) * sectorSize + sigOffset;
	fseek(pIn, singleSidedOffset, SEEK_SET);
	fread(&tracksOnDisk, 1, 1, pIn);
	fread(&sectorsPerTrackDisk, 1, 1, pIn);
	fread(&tracksComplement, 1, 1, pIn);
	fread(&sectorsComplement, 1, 1, pIn);
	// printf("[DEBUG] Single-sided offset: %ld, tracksOnDisk: %u, sectorsPerTrackDisk: %u, tracksComplement: %u, sectorsComplement: %u\n", singleSidedOffset, tracksOnDisk, sectorsPerTrackDisk, tracksComplement, sectorsComplement);

	if (tracksComplement == (unsigned char)~tracksOnDisk && sectorsComplement == (unsigned char)~sectorsPerTrackDisk)
	{
		if (tracksOnDisk == 80 && sectorsPerTrackDisk == 18)
		{
			sideCount = 1; // Valid: single-sided 80 track
			return true;
		}
	}

	// Check second position (double-sided 40 track)
	long doubleSidedOffset = (track * sectorsPerTrack * 2 + (sector - 1)) * sectorSize + sigOffset;
	fseek(pIn, doubleSidedOffset, SEEK_SET);
	fread(&tracksOnDisk, 1, 1, pIn);
	fread(&sectorsPerTrackDisk, 1, 1, pIn);
	fread(&tracksComplement, 1, 1, pIn);
	fread(&sectorsComplement, 1, 1, pIn);
	// printf("[DEBUG] Double-sided offset: %ld, tracksOnDisk: %u, sectorsPerTrackDisk: %u, tracksComplement: %u, sectorsComplement: %u\n", doubleSidedOffset, tracksOnDisk, sectorsPerTrackDisk, tracksComplement, sectorsComplement);

	if (tracksComplement == (unsigned char)~tracksOnDisk && sectorsComplement == (unsigned char)~sectorsPerTrackDisk) {
		if (tracksOnDisk == 40 && sectorsPerTrackDisk == 36) {
			sideCount = 2; // Valid: double-sided 40 track
			return true;
		}
	}

	return false; // No valid signature/geometry match found
}

bool CJVCDiskImage::Load(const std::string& _filename)
{
	FILE* pIn = fopen(_filename.c_str(), "rb");
	if (!pIn)
	{
		return false;
	}

	// Get file size
	fseek(pIn, 0, SEEK_END);
	unsigned int fileSize = ftell(pIn);
	fseek(pIn, 0, SEEK_SET);

	// Read and check header parameters
	headerSize = fileSize % 256;
	size_t dataSize = fileSize - headerSize;
	if( headerSize == 0 )
	{
		return false; // If it's a raw disk image, leave it to the RawDiskImage class.
	}

	// Initialize header with default values
	header = JVCHeader();
	
	// Read the header - use the actual header size, but only read up to 5 bytes max
	unsigned int bytesToRead = (headerSize > JVC_HEADER_DATA_SIZE) ? JVC_HEADER_DATA_SIZE : headerSize;
	
	if (bytesToRead >= 1) fread(&header.sectorsPerTrack		, 1, 1, pIn);
	if (bytesToRead >= 2) fread(&header.sideCount			, 1, 1, pIn);
	if (bytesToRead >= 3) fread(&header.sectorSizeCode		, 1, 1, pIn);
	if (bytesToRead >= 4) fread(&header.firstSectorID		, 1, 1, pIn);
	if (bytesToRead >= 5) fread(&header.sectorAttributeFlag	, 1, 1, pIn);
	
	// Validate header
	bool   hasSectorAttribute = header.sectorAttributeFlag != 0;
	size_t sectorSize 		  = (128 << header.sectorSizeCode) + (hasSectorAttribute ? 1 : 0);
	
	tracks = dataSize / (header.sectorsPerTrack * (sectorSize + (hasSectorAttribute ? 1 : 0)) ) / header.sideCount;

	if( header.sideCount > 2 || sectorSize > JVC_HEADER_MAX_SECTOR_SIZE )
	{
		return false;
	}

	dataBlockSize = dataSize;

	// Allocate and read data block
	dataBlock = new unsigned char[dataBlockSize];
	if (!dataBlock)
	{
		fclose(pIn);
		return false;
	}

	// Position file pointer correctly for data reading
	fseek(pIn, headerSize, SEEK_SET);
	
	size_t bytesRead = fread(dataBlock, 1, dataBlockSize, pIn);
	
	fclose(pIn);
	fileName = _filename;

	return true;
}

bool CJVCDiskImage::Save(const std::string& _filename)
{
	FILE* pOut = fopen(_filename.c_str(), "wb");
	if (!pOut) {
		return false;
	}

	// Write header if the image was loaded with a header
	if (headerSize > 0)
	{
		// Write the header bytes based on the original header size
		unsigned int bytesToWrite = (headerSize > JVC_HEADER_DATA_SIZE) ? JVC_HEADER_DATA_SIZE : headerSize;
		
		if (bytesToWrite >= 1) fwrite(&header.sectorsPerTrack, 1, 1, pOut);
		if (bytesToWrite >= 2) fwrite(&header.sideCount, 1, 1, pOut);
		if (bytesToWrite >= 3) fwrite(&header.sectorSizeCode, 1, 1, pOut);
		if (bytesToWrite >= 4) fwrite(&header.firstSectorID, 1, 1, pOut);
		if (bytesToWrite >= 5) fwrite(&header.sectorAttributeFlag, 1, 1, pOut);
		
		// If header size is larger than 5 bytes, pad with zeros
		for (unsigned int i = JVC_HEADER_DATA_SIZE; i < headerSize; ++i) {
			unsigned char zero = 0;
			fwrite(&zero, 1, 1, pOut);
		}
	}

	// Write data
	fwrite(dataBlock, 1, dataBlockSize, pOut);
	fclose(pOut);

	return true;
}

unsigned int CJVCDiskImage::New(unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack, unsigned int uSectorSize) 
{
	// Check sector size and that it's multiple of 128
	if( uSectorSize > JVC_HEADER_MAX_SECTOR_SIZE || uSectorSize % JVC_HEADER_SECTOR_SIZE_BASE != 0 )
	{
		return 0;
	}

	// Compute and check shift factor
	unsigned int quotient = uSectorSize / JVC_HEADER_SECTOR_SIZE_BASE;
	unsigned char shiftFactor = static_cast<unsigned char>(log2(quotient));

	tracks = uTracks;
	header.sideCount = uSides;
	header.sectorsPerTrack = uSecsPerTrack;
	header.sectorSizeCode = shiftFactor;
	header.firstSectorID = 1;
	header.sectorAttributeFlag = 0;

	headerSize = JVC_HEADER_DATA_SIZE;

	unsigned int sectorSize = 128 << header.sectorSizeCode;
	dataBlockSize = header.sectorsPerTrack * header.sideCount * tracks * sectorSize;

	if (dataBlock)
	{
		delete[] dataBlock;
	}

	dataBlock = new unsigned char[dataBlockSize];
	if (!dataBlock)	
	{
		return 0;
	}

	memset(dataBlock, 0, dataBlockSize);
	return dataBlockSize;
}

void CJVCDiskImage::SetName(const char* newName)
{
	// JVC format does not support disk names, so this is a no-op
}

const unsigned char* CJVCDiskImage::GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector) const 
{
	// Check values
	if( 0 == dataBlock || uTrack >= tracks || uSide >= header.sideCount || uSector >= header.sectorsPerTrack) return nullptr;

	// Do some math
	unsigned int uSectorSize = (128 << header.sectorSizeCode) + (header.sectorAttributeFlag != 0 ? 1 : 0);
	unsigned int uTrackStart = uSectorSize * header.sideCount * header.sectorsPerTrack * uTrack;
	unsigned int uSectorPos = uTrackStart + (uSectorSize * (( uSide * header.sectorsPerTrack ) + uSector));

	return &dataBlock[uSectorPos];
}

unsigned char* CJVCDiskImage::GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector)
{
	return const_cast<unsigned char*>(const_cast<const CJVCDiskImage*>(this)->GetSector(uTrack, uSide, uSector));
}

// IDiskImageInterface required method stubs for CJVCDiskImage

bool CJVCDiskImage::NeedManualSetup() const
{
	return false; // We handle disk geometry automatically
}

int CJVCDiskImage::GetSidesNum() const
{
	return header.sideCount;
}

int CJVCDiskImage::GetTracksNum() const
{
	return tracks;
}

int CJVCDiskImage::GetSectorsNum() const
{
	return header.sectorsPerTrack;
}

int CJVCDiskImage::GetSectorsNum(size_t _side, size_t _track) const
{
	return header.sectorsPerTrack;
}

STrackInfo CJVCDiskImage::GetTrackInfo(unsigned int _track, unsigned int _side) const
{
	STrackInfo retVal;
	retVal.isValid     = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum());
	retVal.isFormatted = true;
	retVal.sectorsNum  = GetSectorsNum();
	retVal.dataSize    = retVal.sectorsNum * 128 << (header.sectorSizeCode);
	return retVal;
}

SSectorInfo CJVCDiskImage::GetSectorInfo(unsigned int _track, unsigned int _side, unsigned int _sector) const
{
	SSectorInfo retVal;

	retVal.isValid   = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum()) && (_sector < (unsigned int)GetSectorsNum());
	retVal.isInUse   = true;  // FS should check or update this info
	retVal.isWeak    = false;
	retVal.copiesNum = 1; // Number of copies of the sector stored
	retVal.dataSize  = 128 << header.sectorSizeCode;
	retVal.hasErrors = false;
	// TODO: if sector attribute is present, check if sector has errors.

	return retVal;
}

std::string CJVCDiskImage::GetFileSpec()
{
	return "JVC disk images \t*.{jvc,dsk}\n";
}

std::string CJVCDiskImage::GetDiskInfo()
{
	std::string retVal;

	if( fileName.empty() )
	{
		return retVal;
	}

	std::stringstream sstream;

	// Get short filename
	std::string shortFileName;
	size_t found = fileName.find_last_of("/\\");
	shortFileName = found ? fileName.substr(found + 1) : fileName;

	// Calculate disk size
	unsigned int uSectorSize = (128 << header.sectorSizeCode) + (header.sectorAttributeFlag != 0 ? 1 : 0);
	int diskSize = GetTracksNum() * GetSidesNum() * GetSectorsNum() * uSectorSize;

	sstream << shortFileName << std::endl << std::endl;
	sstream << "Tracks      : " << GetTracksNum() << std::endl;
	sstream << "Sides       : " << GetSidesNum() << std::endl;
	sstream << "Sectors     : " << GetSectorsNum() << std::endl;
	sstream << "Sector size : " << uSectorSize << " bytes" << std::endl;
	sstream << "Total size  : " << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;

	retVal = sstream.str();
	return retVal;
}

const unsigned char* CJVCDiskImage::GetSectorByID( unsigned int uTrack, unsigned int uSide, unsigned int uSectorID ) const
{
	return GetSector( uTrack, uSide, uSectorID-1 ); // -1 to convert from 1-based index to 0-based index
}

unsigned int CJVCDiskImage::GetSectorID( unsigned int uTrack, unsigned int uSide, unsigned int uSector ) const
{
	return uSector+1;
}

IDiskImageInterface* CJVCDiskImage::NewImage() const
{
	return new CJVCDiskImage;
}
