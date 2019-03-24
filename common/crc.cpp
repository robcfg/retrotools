///////////////////////////////////////////////
// CRC.cpp - CRC functions
///////////////////////////////////////////////

#include "crc.h"

namespace crc 
{
    // Most significant bit first (big-endian)
    // x^16+x^12+x^5+1 = (1) 0001 0000 0010 0001 = 0x1021
    unsigned short int CRC16_MSB( const unsigned char* _data, unsigned short int _datasize, unsigned short int _poly, unsigned short int _start )
    {
        unsigned int rem = _start;
        // A popular variant complements rem here
        for( int i = 0; i < _datasize; ++i )
        {
            unsigned int byte = _data[i];
            rem = rem ^ (byte << 8);   // n = 16 in this example
            for(int j = 0; j < 8; ++j )
            {
                if( rem & 0x8000 )
                {   // if leftmost (most significant) bit is set
                    rem  = (rem << 1) ^ _poly;
                }
                else
                {
                    rem  = rem << 1;
                }

                rem  = rem & 0xffff;      // Trim remainder to 16 bits
            }
        }
        // A popular variant complements rem here
        return (unsigned short int)rem;
    }

    // Least significant bit first (little-endian)
    // x^16+x^12+x^5+1 = 1000 0100 0000 1000 (1) = 0x8408
    unsigned short int CRC16_LSB( const unsigned char* _data, unsigned short int _datasize, unsigned short int _poly, unsigned short int _start )
    {
        unsigned int rem = _start;

        // A popular variant complements rem here
        for( int i = 0; i < _datasize; ++i )
        {
            unsigned int data = _data[i];

            rem  = rem ^ data;
            for( int j = 0; j < 8; ++j )
            {   // Assuming 8 bits per byte
                if( rem & 1 )
                {   // if rightmost (most significant) bit is set
                    rem  = (rem >> 1) ^ _poly;
                } 
                else 
                {
                    rem  = rem >> 1;
                }
            }
        }
        // A popular variant complements rem here
        return (unsigned short int)rem;
    }
}
