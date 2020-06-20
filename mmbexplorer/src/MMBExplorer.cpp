#include <FL/Fl.H>
#include <iostream>
#include "MMBFile.h"
#include "MMBE_Commands.h"

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
        cout << "Launching GUI :P" << endl;
    }

    return 0;
}
