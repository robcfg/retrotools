/////////////////////////////////////////////////////////////////////
//
// DragonDOS_Common.cpp - Common functions for DragonDOS/DragonDOSUI
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 06/03/2025
//
////////////////////////////////////////////////////////////////////

#include "DragonDOS_Common.h"
#include "DragonDOS_FS.h"

// Returns true and sets loadAddress and execAddress if the file contains the DragonDOS header, false otherwise.
bool GetBinaryFileHeaderParams( FILE* file, size_t fileSize, unsigned short int& loadAddress, unsigned short int& execAddress )
{
    unsigned char header[9] = {0,0,0,0,0,0,0,0,0};
    
    fseek( file, 0, SEEK_SET );
    
    // Read header size bytes from the file, fail if we read less than that.
    size_t bytesRead = fread( header, 1, DRAGONDOS_FILEHEADER_SIZE, file );
    fseek( file, 0, SEEK_SET );
    if( DRAGONDOS_FILEHEADER_SIZE != bytesRead )
        return false;

    // Check header start, end and file type.
    if( header[0] != DRAGONDOS_FILE_HEADER_BEGIN || header[1] != DRAGONDOS_FILETYPE_BINARY || header[8] != DRAGONDOS_FILE_HEADER_END )
        return false;

    // Check file size
    size_t headerFileSize = ((header[4] << 8) | header[5]) + DRAGONDOS_FILEHEADER_SIZE;
    if( headerFileSize != fileSize )
        return false;
    
    // All checks passed, so the file already has the header in it. Set the load and exec addresses.
    loadAddress = (header[2] << 8) | header[3];
    execAddress = (header[6] << 8) | header[7];

    return true;
}
