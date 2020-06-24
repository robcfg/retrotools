#include <iostream>
#include <string>
#include "MMBFile.h"
#include "MMBE_Commands.h"
#include "MMBE_Gui.h"

using namespace std;

int main( int argc, char** argv )
{
    string errorString;

    if( ProcessArguments( argc, argv, errorString ) )
    {
        if( !errorString.empty() )
        {
            cout << "[Error] " << errorString << endl;
            return -1;
        }
    }
    else
    {
        string windowTitle = "MMBExplorer by Robcfg (";
        windowTitle += GetSCMVersion();
        windowTitle += ")";
        CMMBEGui gui( 640, 480, windowTitle.c_str() );
        return gui.Run();    
    }

    return 0;
}
