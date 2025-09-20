#include <algorithm>
#include <cstdio>
#include <iostream>

// Disk image formats
#include "../../common/DiskImages/DiskImageFactory.h"
#include "../../common/DiskImages/VDKDiskImage.h"
#include "../../common/DiskImages/JVCDiskImage.h"
#include "../../common/DiskImages/IMDDiskImage.h"
#include "../../common/DiskImages/RawDiskImage.h"

// File systems
#include "../../common/FileSystems/DragonDOS_FS.h"

#include "DragonDOS_Commands.h"

bool ProcessCommand( const std::vector<std::string> _args, DiskImageFactory& _diskFactory )
{
    std::string command = _args[1];

    std::transform( command.begin(), command.end(), command.begin(), ::tolower );

    if     ( 0 == command.compare("help")        ) return HelpCommand        ();
    else if( 0 == command.compare("list")        ) return ListCommand        ( _args, _diskFactory );
    else if( 0 == command.compare("extract")     ) return ExtractCommand     ( _args, _diskFactory );
    else if( 0 == command.compare("info")        ) return InfoCommand        ( _args, _diskFactory );
    else if( 0 == command.compare("new")         ) return NewCommand         ( _args, _diskFactory );
    else if( 0 == command.compare("delete")      ) return DeleteCommand      ( _args, _diskFactory );
    else if( 0 == command.compare("insertbasic") ) return InsertBasicCommand ( _args, _diskFactory );
    else if( 0 == command.compare("insertbinary")) return InsertBinaryCommand( _args, _diskFactory );
    else if( 0 == command.compare("insertdata")  ) return InsertDataCommand  ( _args, _diskFactory );
    else if( 0 == command.compare("listimages")  ) return ListImagesCommand  ( _args, _diskFactory );

    HelpCommand();

    return false;
}

int main( int argc, char** argv )
{
    // Check arguments. We need at least one, a valid command.
    if( argc < 2 )
    {
        HelpCommand();
        return -1;
    }

	DiskImageFactory diskFactory;
	diskFactory.RegisterDiskImageFormat( new CVDKDiskImage );
	diskFactory.RegisterDiskImageFormat( new CJVCDiskImage );
	diskFactory.RegisterDiskImageFormat( new CIMDDiskImage );
	diskFactory.RegisterDiskImageFormat( new CRAWDiskImage );

    std::vector<std::string> args;
    args.insert( args.begin(), argv, &argv[argc] );

    return (ProcessCommand( args, diskFactory ) ? 0 : -1);
}