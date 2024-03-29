#include <algorithm>
#include <cstdio>
#include <iostream>

#include "DragonDOS_Commands.h"

bool ProcessCommand( const std::vector<std::string> _args )
{
    std::string command = _args[1];

    std::transform( command.begin(), command.end(), command.begin(), ::tolower );

    if     ( 0 == command.compare("help")        ) return HelpCommand        ();
    else if( 0 == command.compare("list")        ) return ListCommand        ( _args );
    else if( 0 == command.compare("extract")     ) return ExtractCommand     ( _args );
    else if( 0 == command.compare("info")        ) return InfoCommand        ( _args );
    else if( 0 == command.compare("new")         ) return NewCommand         ( _args );
    else if( 0 == command.compare("delete")      ) return DeleteCommand      ( _args );
    else if( 0 == command.compare("insertbasic") ) return InsertBasicCommand ( _args );
    else if( 0 == command.compare("insertbinary")) return InsertBinaryCommand( _args );
    else if( 0 == command.compare("insertdata")  ) return InsertDataCommand  ( _args );

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

    std::vector<std::string> args;
    args.insert( args.begin(), argv, &argv[argc] );

    return (ProcessCommand( args ) ? 0 : -1);
}