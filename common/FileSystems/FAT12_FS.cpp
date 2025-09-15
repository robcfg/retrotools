////////////////////////////////////////////////////////////////////
//
// FAT12_FS.cpp - Implementation of CFAT12_FS, a helper class that
//                allows file operations on a disk image formatted
//                with the FAT12 file system.
//
// By Roberto Carlos Fernandez Gerhardt aka robcfg
//
// Notes:
//
////////////////////////////////////////////////////////////////////

#include "FAT12_FS.h"
#include "FS_Utils.h"
#include <sstream>
#include <string.h>

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

CFAT12_FS::CFAT12_FS()
{
	disk = NULL;
}

CFAT12_FS::~CFAT12_FS()
{

}

bool CFAT12_FS::Load(IDiskImageInterface* _disk)
{
	// Check if disk has the right sector size
	// if( _disk->GetSectorSize() == 512 )
	disk = _disk;

	const unsigned char* bootSec = _disk->GetSector(0,0,0);
	if( !bootSec )
		return false;

	// Read Boot sector data
	const unsigned char* pData = &bootSec[11];

	bs.bytesPerSector        = *((unsigned short int *)pData); pData += 2;
	bs.sectorsPerCluster     = *pData++;
	bs.reservedSectorsNum    = *((unsigned short int *)pData); pData += 2;
	bs.numberOfFATs          = *pData++;
	bs.maxRootDirEntries     = *((unsigned short int *)pData); pData += 2;
	bs.totalSectorCount      = *((unsigned short int *)pData); pData += 2;
	++pData; // unsigned char      ignore;
	bs.sectorsPerFAT         = *((unsigned short int *)pData); pData += 2;
	bs.sectorsPerTrack       = *((unsigned short int *)pData); pData += 2;
	bs.numberOfHeads         = *((unsigned short int *)pData); pData += 2;
	pData += 4; // unsigned int       ignore;
	bs.totalSectorCountFAT32 = *((unsigned int *)pData); pData += 4;
	pData += 2; //unsigned short int ignore;
	bs.bootSignature         = *pData++;
	bs.volumeID              = *((unsigned int *)pData); pData += 4;
	for( unsigned char tmp = 0; tmp < 11; ++ tmp )
		bs.volumeLabel[tmp] = *pData++;
	bs.volumeLabel[11] = 0;
	for( unsigned char tmp = 0; tmp < 8; ++ tmp )
		bs.fsType[tmp] = *pData++;
	bs.fsType[8] = 0;

	// Sanity check
	if( bs.bytesPerSector != 512 ) return false; // > 0?
	if( bs.sectorsPerCluster < 1 ) return false;
	if( bs.numberOfHeads != disk->GetSidesNum() ) return false;
	// Read FATs
	// Values have the following meaning:
	// 0x00          Unused
	// 0xFF0-0xFF6   Reserved cluster
	// 0xFF7         Bad cluster
	// 0xFF8-0xFFF   Last cluster in a file
	// anything else Number of the next cluster in the file
	//
	// There are 2 12-bit entries packed every 3 bytes.
	size_t packedFatSize = 512 * bs.sectorsPerFAT;
	size_t sector = 1;
	size_t side = 0;
	size_t track = sector / bs.sectorsPerTrack;
	const unsigned char* sec = NULL;
	unsigned short int val0 = 0;
	unsigned short int val1 = 0;
	unsigned short int val2 = 0;

	for( size_t fatNum = 0; fatNum < bs.numberOfFATs; ++fatNum )
	{
		std::vector<unsigned short int> newFat;

		unsigned char* packedFat = new unsigned char[packedFatSize];

		// Copy packed FAT data
		for( size_t secsPerFAT = 0; secsPerFAT < bs.sectorsPerFAT; ++secsPerFAT )
		{
			const unsigned char* sec = _disk->GetSector(track,0,sector);

			memcpy( &packedFat[512*secsPerFAT], sec, 512 );

			++sector;
			if(sector >= bs.sectorsPerTrack)
			{
				++track;
				sector -= bs.sectorsPerTrack;
			}
		}

		// Unpack FAT data
		for( size_t cnt = 0; cnt < packedFatSize; cnt += 3 )
		{
			val0 = packedFat[cnt  ];
			val1 = packedFat[cnt+1];
			val2 = packedFat[cnt+2];

			val0 |= ((val1 & 0x0F) << 8);
			val1  = ((val1 & 0xF0) >> 4);
			val1 |=  (val2 << 4);

			newFat.push_back(val0);
			newFat.push_back(val1);
		}

		fats.push_back(newFat);

		delete[] packedFat;
	}

    unsigned short int rootDirSectors = ((bs.maxRootDirEntries * 32) + (bs.bytesPerSector - 1)) / bs.bytesPerSector;
    unsigned short int base = bs.reservedSectorsNum + (bs.numberOfFATs * bs.sectorsPerFAT) + rootDirSectors;

	// Read root directory
	sector = bs.reservedSectorsNum + (bs.numberOfFATs * bs.sectorsPerFAT);
	if( disk->GetSectorsNum() > 18 )
		sector *= 2; // Dirty hack for reading DMF root directories! More info needed!
                     // May be actually that clusters are 4 sectors wide, check against docs.
	track = LSNTrack(*disk,sector);

	sec = _disk->GetSector(LSNTrack(*disk,sector),LSNHead(*disk,sector),LSNSector(*disk,sector));
	if( !sec )
		return false;
	
	size_t secOffset = 0;

	for( size_t entry = 0; entry < bs.maxRootDirEntries; ++entry )
	{
		if( !sec[secOffset] )
			break;
		
		// Read root directory entry
		SFAT12_Directory tmpEntry;

		for( unsigned char tmp = 0; tmp < 8; ++tmp ) tmpEntry.name[tmp] = sec[secOffset++];
		for( unsigned char tmp = 0; tmp < 3; ++tmp ) tmpEntry.ext [tmp] = sec[secOffset++];
		tmpEntry.name[8]             = 0;
		tmpEntry.ext [3]             = 0;
		tmpEntry.attributes          = sec[secOffset++];
		tmpEntry.reserved            = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.creationTime        = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.creationDate        = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.lastAccessDate      = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.ignore              = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.lastWriteTime       = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.lastWriteDate       = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.firstLogicalCluster = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
		tmpEntry.fileSize            = *((unsigned int*)&sec[secOffset]); secOffset += 4;

        if( (tmpEntry.attributes & SFAT12Attribute_VolumeLabel) == SFAT12Attribute_VolumeLabel &&
            (tmpEntry.attributes & SFAT12Attribute_Archive)     == SFAT12Attribute_Archive )
        {
            snprintf( (char*)bs.volumeLabel, FAT12_VOLUME_LABEL_LENGTH+1, "%s%s", tmpEntry.name, tmpEntry.ext );
        }

		if( tmpEntry.name[0] && 
            tmpEntry.name[0] != 0xE5 && 
            !(tmpEntry.attributes & SFAT12Attribute_VolumeLabel) &&
            tmpEntry.fileSize != 0xFFFFFFFF )
			directory.push_back( tmpEntry );

		if( secOffset >= 512 )
		{
			secOffset = 0;

			++sector;

			sec = _disk->GetSector(LSNHead(*disk,sector),LSNTrack(*disk,sector),LSNSector(*disk,sector));
			if( !sec )
				return false;
		}
	}

	// Scan directories recursively
	size_t dirIter = 0;
	size_t dirMax = directory.size();
	for( ; dirIter < dirMax; ++ dirIter )
	{
		ExploreDirectory(directory[dirIter]);
	}

    ExportHierarchy();

	return true;
}

void CFAT12_FS::ExploreDirectory( SFAT12_Directory& _dir )
{
	size_t dirIter = 0;
	size_t dirMax = directory.size();
	size_t sector = 0;

	if( _dir.attributes & SFAT12Attribute_Subdir )
	{
		sector = _dir.firstLogicalCluster + 33 - 2; // TODO:Check this against FAT

		const unsigned char* sec = disk->GetSector(LSNTrack(*disk,sector),LSNHead(*disk,sector),LSNSector(*disk,sector));
		if( !sec )
			return;

		size_t secOffset = 0;

		while( secOffset < 512 )
		{
			if( !sec[secOffset] )
				break;
				
			// Read directory entry
			SFAT12_Directory tmpEntry;

			for( unsigned char tmp = 0; tmp < 8; ++tmp ) tmpEntry.name[tmp] = sec[secOffset++];
			for( unsigned char tmp = 0; tmp < 3; ++tmp ) tmpEntry.ext [tmp] = sec[secOffset++];
			tmpEntry.name[8]             = 0;
			tmpEntry.ext [3]             = 0;
			tmpEntry.attributes          = sec[secOffset++];
			tmpEntry.reserved            = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.creationTime        = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.creationDate        = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.lastAccessDate      = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.ignore              = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.lastWriteTime       = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.lastWriteDate       = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.firstLogicalCluster = *((unsigned short int*)&sec[secOffset]); secOffset += 2;
			tmpEntry.fileSize            = *((unsigned int*)&sec[secOffset]); secOffset += 4;

			if( tmpEntry.name[0] && tmpEntry.name[0] != 0xE5 && tmpEntry.name[0] != '.' && !(tmpEntry.attributes & SFAT12Attribute_VolumeLabel) )
			{
				_dir.children.push_back( tmpEntry );
			}
		}

		// Scan children
		for( size_t childIter = 0; childIter < _dir.children.size(); ++childIter )
		{
			ExploreDirectory( _dir.children[childIter] );
		}
	}
}

bool CFAT12_FS::Save( const std::string& _filename )
{
	return false;
}

size_t CFAT12_FS::GetFilesNum() const
{
	return directory.size(); // TODO:process subdirectories and create a file vector
}

std::string CFAT12_FS::GetFileName(size_t _fileIdx) const
{
	if( _fileIdx < directory.size() )
	{
		std::stringstream sstr;

		sstr << (char*)directory[_fileIdx].name;
		sstr << ".";
		sstr << (char*)directory[_fileIdx].ext;

		// Add info
		sstr << " ";
		(directory[_fileIdx].attributes & SFAT12Attribute_ReadOnly) ? sstr << "R" : sstr << ".";
		(directory[_fileIdx].attributes & SFAT12Attribute_Archive ) ? sstr << "A" : sstr << ".";
		(directory[_fileIdx].attributes & SFAT12Attribute_System  ) ? sstr << "S" : sstr << ".";
		(directory[_fileIdx].attributes & SFAT12Attribute_Hidden  ) ? sstr << "H" : sstr << ".";

		(directory[_fileIdx].attributes & SFAT12Attribute_Subdir  ) ? sstr << " (dir)" : sstr << " ";

		sstr << directory[_fileIdx].fileSize;

		return sstr.str();
	}
	return "";
}

size_t CFAT12_FS::GetFileSize(size_t _fileIdx) const
{
	if( _fileIdx < directory.size() )
	{
		return directory[_fileIdx].fileSize;
	}

	return 0;
}

std::string CFAT12_FS::GetFSName() const
{
	return "12-bit File Allocation Table (FAT12)";
}

std::string CFAT12_FS::GetFSVariant() const
{
	return "";
}

std::string CFAT12_FS::GetVolumeLabel() const
{
	std::string retVal = (char*)bs.volumeLabel;
	if( retVal.empty() )
		retVal = "FAT12 Disk";

	return retVal;
}

SFileInfo CFAT12_FS::GetFileInfo(size_t _fileIdx) const
{
	SFileInfo retVal;

	retVal.isOk = false;

	return retVal;
}

const CDirectoryEntryWrapper& CFAT12_FS::GetFSRoot() const
{
    return rootDir;
}

void CFAT12_FS::ExportHierarchy()
{
    rootDir.Clear();
    rootDir.SetIsDirectory( true );
    rootDir.SetName( GetVolumeLabel() );

    for( auto dirEntry : directory )
    {
        ExportDirectoryEntry( dirEntry, &rootDir );
    }
}

void CFAT12_FS::ExportDirectoryEntry( const SFAT12_Directory& _source , CDirectoryEntryWrapper* _target )
{
    CDirectoryEntryWrapper* newEntry = new CDirectoryEntryWrapper;
    std::string name = (char*)_source.name;
    name += ".";
    name += (char*)_source.ext;

    newEntry->SetIsDirectory( IsDirectory(_source) );
    newEntry->SetName( name );

    _target->AddChild( newEntry );

    if( IsDirectory(_source) )
    {
        for( auto child : _source.children )
        {
            ExportDirectoryEntry( child, newEntry );
        }
    }
}

bool CFAT12_FS::IsDirectory( const SFAT12_Directory& _entry ) const
{
    return ((_entry.attributes & SFAT12Attribute_Subdir) == SFAT12Attribute_Subdir);
}

void CFAT12_FS::FAT12_FindDirectoryEntry( const SFAT12_Directory& 	_parent, 
                                          std::vector<std::string>& _tokens, 
                                          size_t                  	_curToken, 
                                          SFAT12_Directory&       	_dst ) const
{
    if( _curToken >= _tokens.size() )
    {
        return;
    }

    std::string fullName = (char*)_parent.name;
    fullName += ".";
    fullName += (char*)_parent.ext;

    if( 0 == _stricmp(_tokens[_curToken].c_str(), fullName.c_str()) )
    {
        if( IsDirectory(_parent) )
        {
            for( auto child : _parent.children )
            {
                FAT12_FindDirectoryEntry( child, _tokens, _curToken+1, _dst );
            }
        }
        else
        {
            _dst = _parent;
            return;
        }
    }
}

bool CFAT12_FS::ExtractFile( const std::string& _fileName, std::vector<unsigned char>& dst, bool _withBinaryHeader ) const
{
    // If DMF disk, still don't know how to address it, so bail out
    if( disk->GetSectorsNum() > 18 )
    {
        return false;
    }

    // Tokenize file name
    std::vector<std::string> strings;
    std::istringstream f(_fileName);
    std::string s;    
    while (getline(f, s,'/')) {
        strings.push_back(s);
    }

    SFAT12_Directory fileEntry;
    fileEntry.attributes = SFAT12Attribute_Invalid;
    
    for( auto entry : directory )
    {
        FAT12_FindDirectoryEntry( entry, strings, 1, fileEntry );
    }

    if( fileEntry.attributes != SFAT12Attribute_Invalid )
    {
        unsigned short int rootDirSectors = ((bs.maxRootDirEntries * 32) + (bs.bytesPerSector - 1)) / bs.bytesPerSector;
        unsigned short int base = bs.reservedSectorsNum + (bs.numberOfFATs * bs.sectorsPerFAT) + rootDirSectors;
        unsigned short int currentCluster = fileEntry.firstLogicalCluster;

        size_t currentSize = 0;
        while( currentCluster < 0xFF0 && currentSize <= fileEntry.fileSize )
        {
            const unsigned char* data = disk->GetSector( LSNTrack (*disk,base + currentCluster - 2),
                                                         LSNHead  (*disk,base + currentCluster - 2),
                                                         LSNSector(*disk,base + currentCluster - 2));
            dst.insert( dst.end(), data, data + bs.bytesPerSector );
            currentCluster = fats[0][currentCluster];
            currentSize += bs.bytesPerSector;
        }

        dst.resize(fileEntry.fileSize);

        return true;
    }

    return false;
}

bool CFAT12_FS::InsertFile( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile )
{
	return 0;
}

bool CFAT12_FS::DeleteFile( const std::string& _fileName )
{
	return false;
}

bool CFAT12_FS::InitDisk( IDiskImageInterface* _disk )
{
	return false;
}

size_t CFAT12_FS::GetFreeSize() const
{
	return 0;
}

IFileSystemInterface* CFAT12_FS::NewFileSystem()
{
	return new CFAT12_FS;
}
