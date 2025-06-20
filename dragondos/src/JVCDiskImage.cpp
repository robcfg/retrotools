#include "JVCDiskImage.h"
#include <stdio.h>
#include <string.h>
#include <sstream>

// Standard disk image geometry sizes
const unsigned int STANDARD_SIZE = 40 * 18 * 256; // Standard DragonDOS disk drive geometry
const unsigned int EXTENDED_SIZE = 80 * 18 * 256; // Double-sided 40 track or single-sided 80 track disk
const unsigned int DOUBLE_EXTENDED_SIZE = 80 * 18 * 256 * 2; // Double-sided 80 track disk
const unsigned int JVC_NORMAL_HEADER_SIZE = 5; // 5 bytes for standard JVC header

CJVCDiskImage::CJVCDiskImage() : dataBlock(nullptr), dataBlockSize(0), tracks(40), headerSize(-1) {}

CJVCDiskImage::~CJVCDiskImage() {
    if (dataBlock) {
        delete[] dataBlock;
    }
}

bool CJVCDiskImage::CheckDragonDOSFSMetadata(FILE* pIn, unsigned char& sideCount) {
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

    if (tracksComplement == (unsigned char)~tracksOnDisk && sectorsComplement == (unsigned char)~sectorsPerTrackDisk) {
        if (tracksOnDisk == 80 && sectorsPerTrackDisk == 18) {
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

bool CJVCDiskImage::ValidateJVCHeader(const JVCHeader& header) {
    // Validate header fields with reasonable ranges
    return (header.sectorsPerTrack == 18) && 
           (header.sideCount == 1 || header.sideCount == 2) && 
           (header.sectorSizeCode == 1) && 
           (header.firstSectorID == 0 || header.firstSectorID == 1) && 
           (header.sectorAttributeFlag == 0);
}

bool CJVCDiskImage::Load(const std::string& _filename) {
    FILE* pIn = fopen(_filename.c_str(), "rb");
    if (!pIn) {
        return false;
    }

    // Get file size
    fseek(pIn, 0, SEEK_END);
    unsigned int fileSize = ftell(pIn);
    fseek(pIn, 0, SEEK_SET);

    // Calculate header length: filesize mod 256
    headerSize = fileSize % 256;
    
    // Initialize header with default values
    header = JVCHeader();
    
    if (headerSize > 0) {
        // This is a headered image
        unsigned int dataSize = fileSize - headerSize;
        
        // Read the header - use the actual header size, but only read up to 5 bytes max
        unsigned int bytesToRead = (headerSize > 5) ? 5 : headerSize;
        
        if (bytesToRead >= 1) fread(&header.sectorsPerTrack, 1, 1, pIn);
        if (bytesToRead >= 2) fread(&header.sideCount, 1, 1, pIn);
        if (bytesToRead >= 3) fread(&header.sectorSizeCode, 1, 1, pIn);
        if (bytesToRead >= 4) fread(&header.firstSectorID, 1, 1, pIn);
        if (bytesToRead >= 5) fread(&header.sectorAttributeFlag, 1, 1, pIn);
        
        // Validate header
        if (!ValidateJVCHeader(header)) {
            headerSize = 0; // Treat as headerless
            dataSize = fileSize;
            fseek(pIn, 0, SEEK_SET); // Reset to beginning
        }
        
        // Determine geometry based on data size
        if (dataSize == STANDARD_SIZE && header.sideCount == 1) {
            tracks = 40;
        } else if (dataSize == EXTENDED_SIZE) {
            // For headerSize <= 1, we can't read side count from header, so check metadata
            if (headerSize <= 1) {
                if (!CheckDragonDOSFSMetadata(pIn, header.sideCount)) {
                    fclose(pIn);
                    return false;
                }
            }
            tracks = (header.sideCount == 1) ? 80 : 40;
        } else if (dataSize == DOUBLE_EXTENDED_SIZE && header.sideCount == 2) {
            tracks = 80;
        } else {
            fclose(pIn);
            return false;
        }
        
        dataBlockSize = dataSize;
    }
    if (headerSize == 0) {
        // This is a headerless image
        headerSize = 0;
        dataBlockSize = fileSize;
        
        // Determine geometry based on file size
        if (fileSize == STANDARD_SIZE) {
            tracks = 40;
            header.sideCount = 1;
        } else if (fileSize == EXTENDED_SIZE) {
            // For headerless images, we must check metadata to determine sidedness
            if (!CheckDragonDOSFSMetadata(pIn, header.sideCount)) {
                fclose(pIn);
                return false;
            }
            tracks = (header.sideCount == 1) ? 80 : 40;
        } else if (fileSize == DOUBLE_EXTENDED_SIZE) {
            tracks = 80;
            header.sideCount = 2;
        } else {
            fclose(pIn);
            return false;
        }
    }

    // Allocate and read data block
    dataBlock = new unsigned char[dataBlockSize];
    if (!dataBlock) {
        fclose(pIn);
        return false;
    }

    // Position file pointer correctly for data reading
    if (headerSize > 0) {
        // For headered images, skip the header
        fseek(pIn, headerSize, SEEK_SET);
    } else {
        // For headerless images, we're already at the beginning
        fseek(pIn, 0, SEEK_SET);
    }
    
    size_t bytesRead = fread(dataBlock, 1, dataBlockSize, pIn);
    
    fclose(pIn);
    fileName = _filename;
    return true;
}

bool CJVCDiskImage::Save(const std::string& _filename) {
    FILE* pOut = fopen(_filename.c_str(), "wb");
    if (!pOut) {
        return false;
    }

    // Write header if the image was loaded with a header
    if (headerSize > 0) {
        // Write the header bytes based on the original header size
        unsigned int bytesToWrite = (headerSize > 5) ? 5 : headerSize;
        
        if (bytesToWrite >= 1) fwrite(&header.sectorsPerTrack, 1, 1, pOut);
        if (bytesToWrite >= 2) fwrite(&header.sideCount, 1, 1, pOut);
        if (bytesToWrite >= 3) fwrite(&header.sectorSizeCode, 1, 1, pOut);
        if (bytesToWrite >= 4) fwrite(&header.firstSectorID, 1, 1, pOut);
        if (bytesToWrite >= 5) fwrite(&header.sectorAttributeFlag, 1, 1, pOut);
        
        // If header size is larger than 5 bytes, pad with zeros
        for (unsigned int i = 5; i < headerSize; ++i) {
            unsigned char zero = 0;
            fwrite(&zero, 1, 1, pOut);
        }
    }

    // Write data
    fwrite(dataBlock, 1, dataBlockSize, pOut);
    fclose(pOut);

    return true;
}

unsigned int CJVCDiskImage::New(unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack) {
    tracks = uTracks;
    header.sideCount = uSides;
    header.sectorsPerTrack = uSecsPerTrack;
    header.sectorSizeCode = 1; // Default sector size of 256 bytes
    header.firstSectorID = 1;
    header.sectorAttributeFlag = 0;

    // Set header state based on file extension
    if (fileName.length() >= 4) {
        std::string ext = fileName.substr(fileName.length() - 4);
        if (ext == ".jvc") {
            headerSize = JVC_NORMAL_HEADER_SIZE;
        } else if (ext == ".dsk") {
            headerSize = 0;
        }
    }

    unsigned int sectorSize = 128 << header.sectorSizeCode;
    dataBlockSize = header.sectorsPerTrack * header.sideCount * tracks * sectorSize;

    if (dataBlock) {
        delete[] dataBlock;
    }

    dataBlock = new unsigned char[dataBlockSize];
    if (!dataBlock) {
        return 0;
    }

    memset(dataBlock, 0, dataBlockSize);
    return dataBlockSize;
}

void CJVCDiskImage::SetName(const char* newName) {
    // JVC format does not support disk names, so this is a no-op
}

const unsigned char* CJVCDiskImage::GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector) const 
{
    // Check values
    if( 0 == dataBlock || uTrack >= tracks || uSide >= header.sideCount || uSector >= header.sectorsPerTrack) return nullptr;

    // Do some math
    unsigned int uTrackStart = 256 * header.sideCount * header.sectorsPerTrack * uTrack;
    unsigned int uSectorPos = uTrackStart + (256 * (( uSide * header.sectorsPerTrack ) + uSector));

    return &dataBlock[uSectorPos];
}

unsigned char* CJVCDiskImage::GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector)
{
    return const_cast<unsigned char*>(const_cast<const CJVCDiskImage*>(this)->GetSector(uTrack, uSide, uSector));
}

// IDiskImageInterface required method stubs for CJVCDiskImage

bool CJVCDiskImage::NeedManualSetup() const {
    return false; // We handle disk geometry automatically
}

int CJVCDiskImage::GetSidesNum() const {
    return header.sideCount;
}

int CJVCDiskImage::GetTracksNum() const {
    return tracks;
}

int CJVCDiskImage::GetSectorsNum() const {
    return header.sectorsPerTrack;
}

int CJVCDiskImage::GetSectorsNum(size_t _side, size_t _track) const {
     return header.sectorsPerTrack;
}

STrackInfo CJVCDiskImage::GetTrackInfo(unsigned int _track, unsigned int _side) const {
    STrackInfo retVal;
    retVal.isValid     = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum());
    retVal.isFormatted = true;
    retVal.sectorsNum  = GetSectorsNum();
    retVal.dataSize    = retVal.sectorsNum * 256;
    return retVal;
}

SSectorInfo CJVCDiskImage::GetSectorInfo(unsigned int _track, unsigned int _side, unsigned int _sector) const {
	SSectorInfo retVal;

	retVal.isValid   = (_side < (unsigned int)GetSidesNum()) && (_track < (unsigned int)GetTracksNum()) && (_sector < (unsigned int)GetSectorsNum());
	retVal.hasErrors = false;
	retVal.isInUse   = true;  // FS should check or update this info
	retVal.isWeak    = false;
	retVal.copiesNum = 1; // Number of copies of the sector stored
	retVal.dataSize  = 256;

	return retVal;
}

const unsigned char* CJVCDiskImage::GetSectorByID(unsigned int uTrack, unsigned int uSide, unsigned int uSector) const
{
    return GetSector(uTrack, uSide, uSector);
}

std::string CJVCDiskImage::GetFileSpec() {
    return "DSK/JVC disk images \t*.{dsk,jvc}\n";
}

std::string CJVCDiskImage::GetDiskInfo() {
    std::string retVal;

    if (fileName.empty()) {
        return retVal;
    }

    std::stringstream sstream;

    // Get short filename
    std::string shortFileName;
    size_t found = fileName.find_last_of("/\\");
    if (found) {
        shortFileName = fileName.substr(found + 1);
    } else {
        shortFileName = fileName;
    }

    // Calculate disk size
    int diskSize = GetTracksNum() * GetSidesNum() * GetSectorsNum() * 256; // 256 bytes per sector

    // Format header state
    std::string headerStateStr = (headerSize > 0) ? "With Header" : "Headerless";

    sstream << shortFileName << std::endl << std::endl;
    sstream << "Format      : " << headerStateStr << std::endl;
    sstream << "Tracks      : " << GetTracksNum() << std::endl;
    sstream << "Sides       : " << GetSidesNum() << std::endl;
    sstream << "Sectors     : " << GetSectorsNum() << std::endl;
    sstream << "Sector size : 256 bytes" << std::endl;
    sstream << "Total size  : " << diskSize << " bytes/" << diskSize / 1024 << " KB" << std::endl;

    retVal = sstream.str();
    return retVal;
}