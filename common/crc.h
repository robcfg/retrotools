///////////////////////////////////////////////
// CRC.h - Function declarations for different
//         Cyclic Redundancy Check algorithms.
///////////////////////////////////////////////

#ifndef __CRC_H__
#define __CRC_H__

namespace crc 
{
    const unsigned short int CRC16_CCITT_POLY = 0x8408;
	
    unsigned short int CRC16_MSB( const unsigned char* _data, unsigned short int _datasize, unsigned short int _poly, unsigned short int _start );
    unsigned short int CRC16_LSB( const unsigned char* _data, unsigned short int _datasize, unsigned short int _poly, unsigned short int _start );
}

#endif // __CRC_H__