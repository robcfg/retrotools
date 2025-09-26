#pragma once

#include <cstdint> // For linux to pick standard types such as uint8_t, uint16_t, etc.
#include <map>
#include <string>
#include <vector>

enum Opcode_6x09_Addressing
{
    AM_IMMEDIATE = 0,
    AM_DIRECT,
    AM_INDEXED,
    AM_EXTENDED,
    AM_INHERENT,
    AM_RELATIVE,
    AM_POSTBYTE
};

struct Opcode_6x09_Param
{
    size_t size;
};

struct Opcode_6x09
{
    std::string name;
    std::vector<Opcode_6x09_Param> params;
    Opcode_6x09_Addressing addressing;
};

struct PostByte_6x09
{
    std::string name;
    std::string colors;
    std::vector<Opcode_6x09_Param> params;
};

class Disassembler_6x09
{
public:
	enum EProcessor
	{
		M6809,
		M6309
	};
    
	Disassembler_6x09();
    ~Disassembler_6x09() {}

	void Init		( EProcessor _processor );
    void Disassemble( const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors );

private:
    void 	FormatParameters	( uint8_t _opcodeID, const Opcode_6x09& _opcode, const std::vector<unsigned char>& _data, uint16_t& _pos, uint16_t _execAddress, std::vector<uint8_t>& _bytes, std::stringstream &_dst, std::string &_dstColors);
	void 	FormatInvalidOpcode	( uint16_t _address, const std::vector<uint8_t>& _bytes, std::stringstream &_dst, std::string &_dstColors );
	void 	FormatUnusedPrefix 	( uint16_t _address, uint8_t _prefix, std::stringstream &_dst, std::string &_dstColors );
	void 	FormatBytes			( std::vector<uint8_t>& _bytes, std::string& _bytesStr );
	size_t 	ReadParameter		(const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize, std::vector<uint8_t>& _bytes );
    int 	TwosComplement		( size_t _param, uint8_t _paramSize );

    void Init_Page1_Opcode_Map		( std::map<unsigned char, Opcode_6x09>& 	_map, EProcessor _processor );
    void Init_Page2_Opcode_Map		( std::map<unsigned char, Opcode_6x09>& 	_map, EProcessor _processor );
    void Init_Page3_Opcode_Map		( std::map<unsigned char, Opcode_6x09>& 	_map, EProcessor _processor );
    void Init_Undefined_Opcode_Map	( std::map<unsigned char, Opcode_6x09>& 	_map, EProcessor _processor );
    void Init_Postbyte_Opcode_Map	( std::map<unsigned char, PostByte_6x09>& 	_map, EProcessor _processor );

	std::map<unsigned char, Opcode_6x09>     opcode_map;
	std::map<unsigned char, Opcode_6x09>     opcode_map2;
	std::map<unsigned char, Opcode_6x09>     opcode_map3;
	std::map<unsigned char, Opcode_6x09>     opcode_map_undefined;
	std::map<unsigned char, PostByte_6x09>   postbyte_map;
};