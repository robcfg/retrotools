#ifndef JVC_DISK_IMAGE_H
#define JVC_DISK_IMAGE_H

#include <string>
#include "DiskImageInterface.h"

// JVC Header structure with default values
struct JVCHeader {
    unsigned char sectorsPerTrack;    // Default: 18
    unsigned char sideCount;          // Default: 1
    unsigned char sectorSizeCode;     // Default: 1
    unsigned char firstSectorID;      // Default: 1
    unsigned char sectorAttributeFlag; // Default: 0
    
    JVCHeader() : sectorsPerTrack(18), sideCount(1), sectorSizeCode(1), firstSectorID(1), sectorAttributeFlag(0) {}
};

class CJVCDiskImage : public IDiskImageInterface {
public:
    CJVCDiskImage();
    ~CJVCDiskImage();

    // IDiskImageInterface required methods
    bool Load(const std::string& _filename) override;
    bool Save(const std::string& _filename) override;
    unsigned int New(unsigned char uTracks, unsigned char uSides, unsigned char uSecsPerTrack) override;

    int GetSidesNum() const override;
    int GetTracksNum() const override;
    int GetSectorsNum() const override;
    int GetSectorsNum(size_t _side, size_t _track) const override;
    STrackInfo  GetTrackInfo(unsigned int _track, unsigned int _side) const override;
    SSectorInfo GetSectorInfo(unsigned int _track, unsigned int _side, unsigned int _sector) const override;
    const unsigned char* GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector) const override;
    unsigned char* GetSector(unsigned int uTrack, unsigned int uSide, unsigned int uSector) override;
    const unsigned char* GetSectorByID(unsigned int uTrack, unsigned int uSide, unsigned int uSector) const override;
    std::string GetFileSpec() override;
    std::string GetDiskInfo() override;
    bool NeedManualSetup() const override;

    void SetName(const char* newName);
    void SetFileName(const std::string& _filename) { fileName = _filename; }
    void SetHeaderSize(unsigned int size) { headerSize = size; }

    // Add missing GetSectorSize overrides
    size_t GetSectorSize(unsigned int _track, unsigned int _side, unsigned int _sector) override { return 256; }
    size_t GetSectorSize() override { return 256; }

    unsigned int GetHeaderSize() const { return headerSize; }

private:
    bool CheckDragonDOSFSMetadata(FILE* pIn, unsigned char& sideCount);
    bool ValidateJVCHeader(const JVCHeader& header);
    unsigned char* dataBlock;
    unsigned int dataBlockSize;
    std::string fileName;
    JVCHeader header;
    unsigned char tracks;
    unsigned int headerSize;  // -1 = not yet determined, 0 = headerless, >0 = headered with this size
};

#endif // JVC_DISK_IMAGE_H 