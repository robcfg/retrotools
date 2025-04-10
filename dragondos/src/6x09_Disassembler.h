#pragma once

#include <string>
#include <vector>

enum Opcode_6x09_Addressing
{
    IMMEDIATE = 0,
    DIRECT,
    INDEXED,
    EXTENDED,
    INHERENT,
    RELATIVE
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

class Disassembler_6x09
{
public:
    Disassembler_6x09();
    ~Disassembler_6x09() {}

    void Disassemble( const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors );

private:
    void   FormatParameter( unsigned char _opcode, size_t _param, unsigned char _paramSize, Opcode_6x09_Addressing _addressingMode, std::string& _dst, std::string& _dstColors );
    size_t ReadParameter( const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize );
};