#pragma once

#include <map>
#include <string>
#include <vector>

enum Opcode_6x09_Addressing
{
    IMMEDIATE = 0,
    DIRECT,
    INDEXED,
    EXTENDED,
    INHERENT,
    RELATIVE,
    POSTBYTE
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
    Disassembler_6x09();
    ~Disassembler_6x09() {}

    void Disassemble( const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors );

private:
    void   FormatParameters(const Opcode_6x09& _opcode, const std::vector<unsigned char>& _data, uint16_t& _pos, uint16_t _execAddress, std::stringstream &_dst, std::string &_dstColors);
    size_t ReadParameter( const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize );
    int    TwosComplement( size_t _param, uint8_t _paramSize );

    void Init_Page1_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map );
    void Init_Page2_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map );
    void Init_Page3_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map );
    void Init_Undocumented_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map );
    void Init_Postbyte_Opcode_Map( std::map<unsigned char, PostByte_6x09>& _map );
};