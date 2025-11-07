/////////////////////////////////////////////////////////////////////
//
// DiskExtendedColorBasic.h - Header file for DECBasic, a namespace 
//                            with a function that translates
//                            tokenized BASIC files to plain text.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
////////////////////////////////////////////////////////////////////

#include <sstream>
#include <vector>

#define DRAGONDOS_BASIC_PROGRAM_START 0x2401

namespace DECBasic
{
	bool Decode( const std::vector<unsigned char>& _in, 
				 std::stringstream& _out, 
				 std::string& _fltkTextColor, 
				 unsigned short int& _lineAddress,
				 bool _bIgnoreLineHeader = false, 
				 bool _bShowLineAddresses = false );

	bool Encode( const std::vector<char>& _in,
					   std::vector<unsigned char>& _out );
}