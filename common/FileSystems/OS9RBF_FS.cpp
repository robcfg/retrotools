////////////////////////////////////////////////////////////////////
//
// OS9RBF_FS.cpp - Implementation for COS9RBF_FS, a helper 
//                 class that allows file operations on a disk
//                 image formatted with OS-9 RBF file system
//
// For info on the OS-9 RBF file system go to:
//               ftp://ttl.arcadetech.org/TTL/Test_Equipment/Fluke/9100%20Series/OS9_Documentation/OS-9%202.4%20Technical%20Manual/tech_7.pdf
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
//
////////////////////////////////////////////////////////////////////
#include <string.h> // for strcasecmp
#include <sstream>
#include "OS9RBF_FS.h"
#include "FS_Utils.h"

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

// Constructor
COS9RBF_FS::COS9RBF_FS()
{
	disk = NULL;
}

// Destructor
COS9RBF_FS::~COS9RBF_FS()
{

}

// Sets the disk to work with and analyzes it
// to check if it's indeed a OS-9 RBF disk.
//
// Returns false if the disk is not
// OS-9 RBF formatted.
bool COS9RBF_FS::SetDisk( IDiskImageInterface* _disk )
{
	disk = _disk;

	/* if( DRAGONDOS_SECTORSPERTRACK != disk->GetSectorsNum() )
	{
		disk = NULL;
		return false;
	}*/

	if( false == ParseDirectory() )
	{
		disk = NULL;
		return false;
	}

	ParseFiles();
/*  if( false == ParseFiles() )
	{
		disk = NULL;
		return false;
	}*/

	return true;
}

// Analyzes the disk image and decodes the RBF directory information.
bool COS9RBF_FS::ParseDirectory()
{
	if( 0 == disk ) // Must use SetDisk() first with a valid image
		return false;

	// Get Id Sector
	const unsigned char* sector = disk->GetSector(0,0,0);
	if( !sector )
		return false;

	idSector.DD_TOT     = (sector[OFF_DD_TOT]*65536)+(sector[OFF_DD_TOT+1]*256)+sector[OFF_DD_TOT+2];
	idSector.DD_TKS     =  sector[OFF_DD_TKS];
	idSector.DD_MAP     = (sector[OFF_DD_MAP]*256)+sector[OFF_DD_MAP+1];
	idSector.DD_BIT     = (sector[OFF_DD_BIT]*256)+sector[OFF_DD_BIT+1];
	idSector.DD_DIR     = (sector[OFF_DD_DIR]*65536)+(sector[OFF_DD_DIR+1]*256)+sector[OFF_DD_DIR+2];
	idSector.DD_OWN     = (sector[OFF_DD_OWN]*256)+sector[OFF_DD_OWN+1];
	idSector.DD_ATT     =  sector[OFF_DD_ATT];
	idSector.DD_DSK     = (sector[OFF_DD_DSK]*256)+sector[OFF_DD_DSK+1];
	idSector.DD_FMT     =  sector[OFF_DD_FMT];
	idSector.DD_SPT     = (sector[OFF_DD_SPT]*256)+sector[OFF_DD_SPT+1];
	idSector.DD_RES     = (sector[OFF_DD_RES]*256)+sector[OFF_DD_RES+1];
	idSector.DD_BT      = (sector[OFF_DD_BT ]*65536)+(sector[OFF_DD_BT+1]*256)+sector[OFF_DD_BT+2];
	idSector.DD_BSZ     = (sector[OFF_DD_BSZ]*256)+sector[OFF_DD_BSZ+1];
	idSector.DD_RES2    =  sector[OFF_DD_RES2];
	idSector.DD_SYNC    = (sector[OFF_DD_SYNC]*(1<<24))+(sector[OFF_DD_SYNC+1]*65536)+(sector[OFF_DD_SYNC+2]*256)+sector[OFF_DD_SYNC+3];
	idSector.DD_MapLSN  = (sector[OFF_DD_MapLSN]*(1<<24))+(sector[OFF_DD_MapLSN+1]*65536)+(sector[OFF_DD_MapLSN+2]*256)+sector[OFF_DD_MapLSN+3];
	idSector.DD_LSNSize = (sector[OFF_DD_LSNSize]*256)+sector[OFF_DD_LSNSize+1];
	idSector.DD_VersID  = (sector[OFF_DD_VersID ]*256)+sector[OFF_DD_VersID +1];

	if( idSector.DD_MapLSN == 0 ) idSector.DD_MapLSN = 1;

	// Sanity check
	if( idSector.DD_TOT != disk->GetSidesNum()*disk->GetTracksNum()*disk->GetSectorsNum() )
		return false;
	if( idSector.DD_TKS != disk->GetSectorsNum() )
		return false;

	memcpy( idSector.DD_DAT, &sector[OFF_DD_DAT], 5  );
	memcpy( idSector.DD_NAM, &sector[OFF_DD_NAM], 32 );
	memcpy( idSector.DD_OPT, &sector[OFF_DD_OPT], 32 );
	
	// Remove highest bit as en-of-string mark.
	for( int iPos = 0; iPos < 32; ++iPos )
	{
		idSector.DD_NAM[iPos] &= 127;
	}
	idSector.DD_NAM[32] = 0;

	//size_t mediaSize = (idSector.DD_LSNSize * 256) + 256;
	root.Clear();
	root.SetName(GetVolumeLabel());
	root.Load( disk, idSector.DD_DIR, idSector.DD_LSNSize );

	return true;
}

void COS9RBF_FS::ParseFiles()
{
	ParseFileNode( GetFSRoot(), "" );
}

void COS9RBF_FS::ParseFileNode( const CDirectoryEntryWrapper& _entry, const std::string& _parentName )
{
	if( _entry.IsDirectory() )
	{
		std::string parentDirName = _parentName;

		// Root directory entry is the volume label, so skip adding its name.
		if( !parentDirName.empty() )
		{
			parentDirName += _entry.GetName();
		}

		parentDirName += '/';

		const std::vector<CDirectoryEntryWrapper*>& children = _entry.GetChildren();
		for( auto kid : children )
		{
			ParseFileNode( *kid, parentDirName );
		}
	}
	else
	{
		SOS9RBFFile file;
		file.name = _parentName;
		file.name += _entry.GetName();
		
		ExtractFile( file.name, file.data, false );
		
		//printf("%s (%zu bytes)\n", file.name.c_str(), file.data.size());

		mFiles.push_back( file );
	}
}

// IFilesystemInterface implementation
bool COS9RBF_FS::Load(IDiskImageInterface* _disk)
{
	return SetDisk( _disk );
}

bool COS9RBF_FS::Save(const std::string& _filename)
{
	return 0;
}

size_t COS9RBF_FS::GetFilesNum() const
{
	return mFiles.size();
}

std::string COS9RBF_FS::GetFileName(size_t _fileIdx) const
{
	if( _fileIdx < GetFilesNum() )
	{
		return mFiles[_fileIdx].name;
	}

	return "";
}

size_t COS9RBF_FS::GetFileSize( size_t _fileIdx ) const
{
	if( _fileIdx < GetFilesNum() )
	{
		return mFiles[_fileIdx].data.size();
	}

	return 0;
}

size_t COS9RBF_FS::GetFreeSize() const
{
	return 0;
}

std::string COS9RBF_FS::GetFSName() const
{
	return "OS-9 RBF";
}

std::string COS9RBF_FS::GetFSVariant() const
{
	return "";
}

std::string COS9RBF_FS::GetVolumeLabel() const
{
	return idSector.DD_NAM; // Get this from Id Sector
}

SFileInfo COS9RBF_FS::GetFileInfo(size_t _fileIdx) const
{
	SFileInfo retVal;

	retVal.isOk = false;

	/*if( _fileIdx < GetNumberOfFiles() )
	{
		retVal.isOk = true;
		retVal.name = files[_fileIdx].GetFileName();
		retVal.size = files[_fileIdx].GetFileSize();
		retVal.attr = files[_fileIdx].GetFileProtected() ? FA_PROTECTED : 0;
	}*/

	return retVal;
}

void CFileDescriptor::Load( IDiskImageInterface* _disk, unsigned long int _lsn, size_t _sectorSize )
{
	unsigned short int head   = LSNHead(*_disk, _lsn);
	unsigned short int track  = LSNTrack(*_disk, _lsn);
	unsigned short int sector = LSNSector(*_disk, _lsn);

	const unsigned char* _data = _disk->GetSector(track, head, sector);

	FD_ATT =  _data[OFF_FD_ATT];
	FD_OWN = (_data[OFF_FD_OWN]*256)+_data[OFF_FD_OWN+1];
	FD_LNK =  _data[OFF_FD_LNK];
	FD_SIZ = (_data[OFF_FD_SIZ]*(1<<24))+(_data[OFF_FD_SIZ+1]*65536)+(_data[OFF_FD_SIZ+2]*256)+_data[OFF_FD_SIZ+3];
	
	SetIsDirectory((FD_ATT & ATT_FD_DIRECTORY) == ATT_FD_DIRECTORY);

	memcpy( FD_DAT  , &_data[OFF_FD_DAT  ], 5 );
	memcpy( FD_CREAT, &_data[OFF_FD_CREAT], 3 );
	
	// Process Segments
	unsigned long int segOffset = OFF_FD_SEG;
	while( segOffset < _sectorSize )
	{
		SFileDescriptorSegment segment;
		segment.LSN  = (_data[segOffset]*65536)+(_data[segOffset+1]*256)+_data[segOffset+2];
		segment.size = (_data[segOffset+3]*256)+ _data[segOffset+4];

		if( segment.LSN != 0 && segment.size != 0 )
		{
			segments.push_back(segment);
		}

		segOffset += FD_SEG_SIZE;
	}

	// Process directory entries
	if( IsDirectory() )
	{
		// For every segment, every sector of segment...
		for( auto curSegment : segments )
		{
			unsigned long int sectorLSN = curSegment.LSN;

			for( unsigned short int uSector = 0; uSector < curSegment.size; ++uSector )
			{
				// Get sector data
				head   = (sectorLSN % (_disk->GetSectorsNum() * _disk->GetSidesNum()) / _disk->GetSectorsNum());
				track  = (sectorLSN/(_disk->GetSidesNum() * _disk->GetSectorsNum()));
				sector = (sectorLSN % (_disk->GetSectorsNum() * _disk->GetSidesNum()) % _disk->GetSectorsNum());
				++sectorLSN;

				//++sector;
				_data = _disk->GetSector(track, head, sector);
				if( !_data )
				{
					// TODO:Mark in some way that this descriptor is invalid or flag problem.
					return;
				}

				// Get directory entries
				size_t offset = 0;
				while( offset < _sectorSize )
				{
					// Check EOF
					if( _data[offset] == 0xE5 )
					{
						offset = _sectorSize;
						continue;
					}
					CFileDescriptor* tmpDir = new CFileDescriptor;
					std::string entryName;

					// Read name
					if( _data[offset] == 0 ) // Deleted or unused entry
					{
						offset += FD_DIR_SIZE;
						continue;
					}

					unsigned char nameOffset = 0;
					while( nameOffset < FD_DIR_NAME_SIZE )
					{
						if( _data[offset+nameOffset] != 0 )
						{
							entryName += _data[offset+nameOffset] & 127;
						}
						++nameOffset;
					}
					tmpDir->SetName( entryName );

					// Load new directory
					if( entryName.compare(0,1,".",1) != 0 && entryName.compare(0,2,"..",2) != 0 )
					{
						size_t lsnOffset = offset + OFF_FD_DIR_LSN;
						unsigned long int dirLSN = (_data[lsnOffset]*65536)+(_data[lsnOffset+1]*256)+_data[lsnOffset+2];
						tmpDir->Load( _disk, dirLSN, _sectorSize );
						//entries.push_back( tmpDir );
						AddChild( tmpDir );
					}

					offset += FD_DIR_SIZE;
				}
			}
		}
	}
}

const CDirectoryEntryWrapper& COS9RBF_FS::GetFSRoot() const
{
	return root;
}

bool COS9RBF_FS::ExtractFile( const std::string& _fileName, std::vector<unsigned char>& dst, bool _withBinaryHeader ) const
{
	// Tokenize file name
	std::vector<std::string> strings;
	std::istringstream f(_fileName);
	std::string s;    
	while (getline(f, s,'/')) {
		//cout << s << endl;
		strings.push_back(s);
	}

	const CDirectoryEntryWrapper* fileEntry = FindDirectoryEntry( &GetFSRoot(), strings, 1);

	if( fileEntry )
	{
		CFileDescriptor* fd = (CFileDescriptor*)fileEntry;

		size_t remaining = fd->GetFileSize();

		unsigned char t,h,s;
		size_t dataSize;

		for( auto segment : fd->GetFileSegments() )
		{
			for( auto sectorNum = segment.LSN; sectorNum < segment.LSN+segment.size; ++sectorNum )
			{
				t = LSNTrack (*disk, sectorNum);
				h = LSNHead  (*disk, sectorNum);
				s = LSNSector(*disk, sectorNum);

				const unsigned char* sector = disk->GetSector(t,h,s);
				if( !sector )
					return false;

				dataSize = std::min(remaining,disk->GetSectorSize(t,h,s));
				dst.insert( dst.end(), sector, sector + dataSize );

				remaining -= dataSize;
			}
		}

		return true;
	}

	return false;
}

bool COS9RBF_FS::InsertFile( const std::string& _fileName, const std::vector<unsigned char>& src, bool _binaryFile )
{
	return false;
}

bool COS9RBF_FS::DeleteFile( const std::string& _fileName )
{
	return false;
}

bool COS9RBF_FS::InitDisk( IDiskImageInterface* _disk )
{
	return false;
}
