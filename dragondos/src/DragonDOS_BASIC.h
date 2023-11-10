/////////////////////////////////////////////////////////////////////
//
// DragonDOS_BASIC.h - Header file for DragonDOS_BASIC,
//                     a namespace with a function that translates
//                     tokenized BASIC files to plain text.
//
// For info on BASIC's data structure and tokens:
//              http://dragon32.info/info/basicfmt.txt
//              http://dragon32.info/info/bastoken.txt
//              http://dragon32.info/info/dostoken.txt
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
// Last update: 07/11/2023
//
////////////////////////////////////////////////////////////////////

#include <sstream>
#include <vector>

namespace DragonDOS_BASIC
{
    bool Decode( const std::vector<unsigned char>& _in, 
                 std::stringstream& _out, 
                 std::string& _fltkTextColor, 
                 unsigned short int& _lineAddress,
                 bool _bIgnoreLineHeader = false, 
                 bool _bShowLineAddresses = false );

    bool Encode(
                 const std::vector<unsigned char>& _out );
}