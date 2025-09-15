/////////////////////////////////////////////////////////////////////
//
// DragonDOS_Common.h - Header file for DragonDOS/DragonDOSUI
//                      common functions.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#include <stdio.h>

bool GetBinaryFileHeaderParams( FILE* file, size_t fileSize, unsigned short int& loadAddress, unsigned short int& execAddress );
