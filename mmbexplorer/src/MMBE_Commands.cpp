#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include "scmversion.h"
#include "MMBFile.h"
#include "MMBE_Commands.h"

using namespace std;

// Command list
const string MMBE_CMD_HELP    = "help";
const string MMBE_CMD_LIST    = "list";
const string MMBE_CMD_CREATE  = "create";
const string MMBE_CMD_REMOVE  = "remove";
const string MMBE_CMD_EXTRACT = "extract";
const string MMBE_CMD_LOCK    = "lock";
const string MMBE_CMD_UNLOCK  = "unlock";
const string MMBE_CMD_ADD     = "add";

size_t CheckSlotNumber( const CMMBFile& _mmb, const string& _slot, string& _errorString )
{
    size_t slot = (size_t)strtoul( _slot.c_str(), nullptr, 0 );
    if( slot >= _mmb.GetNumberOfDisks() )
    {
        _errorString = "Slot number out of range or invalid: ";
        _errorString += _slot;
        _errorString += " ( max slot number is ";
        _errorString += to_string( _mmb.GetNumberOfDisks() - 1);
        _errorString += ").";
        return (size_t)-1;
    }

    return slot;
}

void ShowHelp()
{
    cout << "MMBExplorer by Robcfg (Git:" << scmVersion << ")." << endl << endl;
    cout << "Usage:" << endl;
    cout << "      mmbexplorer                   - Without parameters, launch"          << endl;
    cout << "                                      the graphical user interface."       << endl << endl;
    cout << "      mmbexplorer COMMAND [ARGS...] - Executes the given command with"     << endl;
    cout << "                                      the following arguments."            << endl << endl;
    cout << "      Available commands:"                                                 << endl;
    cout << "          help                              - Show this text."             << endl;
    cout << "          create 'filename' slots           - Create a new MMB file with"  << endl;
    cout << "                                              a number of SSD image slots" << endl;
    cout << "          list 'filename'                   - List content of a MMB file." << endl;
    cout << "          add 'filename' 'ssdname' slot     - Add SSD image to a given"    << endl;
    cout << "                                              slot in the MMB file."       << endl;
    cout << "          remove 'filename' slot            - Remove a SSD image from a"   << endl;
    cout << "                                              given slot on a MMB file."   << endl;
    cout << "          lock 'filename' slot              - Lock image in given slot."   << endl;
    cout << "          unlock 'filename' slot            - Unlock image in given slot." << endl;
    cout << "          extract 'filename' slot           - Extracts given slot disk as" << endl;
    cout << "                                              'slot.sdd' (i.e. 34.ssd)."   << endl;
    cout << "          extract 'filename' 'ssdname' slot - Extracts given slot disk as" << endl;
    cout << "                                              given SSD image name."       << endl;
}

void ListMMB( const string& _filename, string& _errorString )
{
    CMMBFile mmb;

    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Show directory
    cout << "Number of disk slots = " << mmb.GetNumberOfDisks() << endl;
     
    const SMMBDirectoryEntry* dir = mmb.GetDirectory();
    for( size_t disk = 0; disk < MMB_MAXNUMBEROFDISKS; ++disk )
    {
        switch( dir[disk].diskAttributes )
        {
            case MMB_DISKATTRIBUTE_UNFORMATTED:
            {
                //cout << disk << " : [UNFORMATTED]" << endl;
                break;
            }
            case MMB_DISKATTRIBUTE_UNLOCKED:
            {
                cout << disk << " : " << dir[disk].name << endl;
                break;
            }
            case MMB_DISKATTRIBUTE_LOCKED:
            {
                cout << disk << " : " << dir[disk].name << " (L)" << endl;
                break;
            }
        }
    }
}

void CreateMMB( const string& _filename, const string& _slotsNum, string& _errorString )
{
    size_t slotsNum = (size_t)strtoul( _slotsNum.c_str(), nullptr, 0 );
    slotsNum = min( slotsNum, MMB_MAXNUMBEROFDISKS );

    CMMBFile mmb;
    mmb.Create( _filename, slotsNum, _errorString );
}

void RemoveImage( const string& _filename, const string& _slot, string& _errorString )
{
    // Open destination MMB
    CMMBFile mmb;
    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Check slot number
    size_t slot = CheckSlotNumber( mmb, _slot, _errorString );
    if( !_errorString.empty() )
    {
        return;
    }

    mmb.RemoveImageFromSlot( slot, _errorString );
}

void LockImage( const string& _filename, const string& _slot, string& _errorString )
{
    // Open destination MMB
    CMMBFile mmb;
    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Check slot number
    size_t slot = CheckSlotNumber( mmb, _slot, _errorString );
    if( !_errorString.empty() )
    {
        return;
    }

    mmb.LockImageInSlot( slot, _errorString );
}

void UnlockImage( const string& _filename, const string& _slot, string& _errorString )
{
    // Open destination MMB
    CMMBFile mmb;
    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Check slot number
    size_t slot = CheckSlotNumber( mmb, _slot, _errorString );
    if( !_errorString.empty() )
    {
        return;
    }

    mmb.UnlockImageInSlot( slot, _errorString );
}

void AddImage( const string& _filename, const string& _imageName, const string& _slot, string& _errorString )
{
    // Open destination MMB
    CMMBFile mmb;
    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Check slot number
    size_t slot = CheckSlotNumber( mmb, _slot, _errorString );
    if( !_errorString.empty() )
    {
        return;
    }

    mmb.InsertImageInSlot( _imageName, slot, _errorString );
}

void ExtractImage( const std::string& _filename, const std::string& _imageName, const std::string& _slot, std::string& _errorString )
{
    // Open destination MMB
    CMMBFile mmb;
    if( !mmb.Open(_filename, _errorString) )
    {
        return;
    }

    // Check slot number
    size_t slot = CheckSlotNumber( mmb, _slot, _errorString );
    if( !_errorString.empty() )
    {
        return;
    }

    mmb.ExtractImageInSlot( _imageName, slot, _errorString );
}

// Execute specified command. Returns true if processed or false for launching gui.
bool ProcessArguments( int argc, char** argv, string& _errorString )
{
    // No command specified, launch gui ___________________________________________
    if( argc == 1 )
    {
        return false;
    }
    // Commands with no arguments _________________________________________________
    else if( argc == 2 )
    {
        string command = argv[1];
        transform( command.begin(), command.end(), command.begin(), ::tolower );

        if( 0 == command.compare(MMBE_CMD_HELP) )
        {
            ShowHelp();
            return true;
        }
        else
        {
            // Check if parameter is a file name
            FILE* pFile = fopen( command.c_str(), "r" );
            if( nullptr != pFile )
            {
                fclose( pFile );
                return false;
            }

            // Otherwise show help
            ShowHelp();
            _errorString = "Unknown command or file '";
            _errorString += command;
            _errorString += "'.";
            return true;
        }
    }
    // Commands with 1 argument ___________________________________________________
    else if( argc == 3 )
    {
        string command = argv[1];
        transform( command.begin(), command.end(), command.begin(), ::tolower );

        if( 0 == command.compare(MMBE_CMD_LIST) )
        {
            ListMMB( argv[2], _errorString );
            return true;
        }
        else
        {
            ShowHelp();
            _errorString = "Unknown command '";
            _errorString += command;
            _errorString += "'.";
            return true;
        }
    }
    // Commands with 2 arguments __________________________________________________
    else if( argc == 4 )
    {
        string command = argv[1];
        transform( command.begin(), command.end(), command.begin(), ::tolower );

        if( 0 == command.compare(MMBE_CMD_CREATE) )
        {
            CreateMMB( argv[2], argv[3], _errorString );
            return true;
        }
        else if( 0 == command.compare(MMBE_CMD_REMOVE) )
        {
            RemoveImage( argv[2], argv[3], _errorString );
            return true;
        }
        else if( 0 == command.compare(MMBE_CMD_LOCK) )
        {
            LockImage( argv[2], argv[3], _errorString );
            return true;
        }
        else if( 0 == command.compare(MMBE_CMD_UNLOCK) )
        {
            UnlockImage( argv[2], argv[3], _errorString );
            return true;
        }
        else if( 0 == command.compare(MMBE_CMD_EXTRACT) )
        {
            string ssdName = argv[3];
            ssdName += ".ssd";
            ExtractImage( argv[2], ssdName, argv[3], _errorString );
            return true;
        }
        else
        {
            ShowHelp();
            _errorString = "Unknown command '";
            _errorString += command;
            _errorString += "'.";
            return true;
        }
    }
    // Commands with 3 arguments __________________________________________________
    else if( argc == 5 )
    {
        string command = argv[1];
        transform( command.begin(), command.end(), command.begin(), ::tolower );

        if( 0 == command.compare(MMBE_CMD_ADD) )
        {
            AddImage( argv[2], argv[3], argv[4], _errorString );
            return true;
        }
        else if( 0 == command.compare(MMBE_CMD_EXTRACT) )
        {
            ExtractImage( argv[2], argv[3], argv[4], _errorString );
            return true;
        }
        else
        {
            ShowHelp();
            _errorString = "Unknown command '";
            _errorString += command;
            _errorString += "'.";
            return true;
        }
    }
    // Ooops! Too many arguments, maybe spaces in filename ________________________
    else
    {
        _errorString = "Too many arguments! Check for unescaped spaces in file name.";
        return true;
    }

    return false;
}

const char* GetSCMVersion()
{
    return scmVersion;
}