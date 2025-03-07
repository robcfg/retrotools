/////////////////////////////////////////////////////////////////////
//
// DragonDOS_Common.h - Header file for DragonDOS/DragonDOSUI
//                      common functions.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 06/03/2025
//
////////////////////////////////////////////////////////////////////

#include <stdio.h>

bool GetBinaryFileHeaderParams( FILE* file, size_t fileSize, unsigned short int& loadAddress, unsigned short int& execAddress );
