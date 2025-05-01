#include "6x09_Disassembler.h"

#include <iomanip>
#include <sstream>

#define MAX_OPCODE_STR_WIDTH 10

#define COLOR_TEXT 'A'
#define COLOR_ERROR 'B'
#define COLOR_OPCODE 'C'
#define COLOR_REGISTER 'D'
#define COLOR_NUMBER 'E'

#define OPCODE_PAGE_2 0x10
#define OPCODE_PAGE_3 0x11

static const uint8_t POSTBYTE_MODE_MASK         = 1 << 4;
static const uint8_t POSTBYTE_MODE_NONINDIRECT  = 0;
static const uint8_t POSTBYTE_MODE_INDIRECT     = 1;

static const uint8_t POSTBYTE_OP_MASK                           = 0x0F;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMREG_NOOFFSET    = 0x04;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMREG_5BIT        = 0x80;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMREG_8BIT        = 0x08;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMREG_16BIT       = 0x09;
static const uint8_t POSTBYTE_OP_ACCOFFSETFROMREG_A             = 0x06;
static const uint8_t POSTBYTE_OP_ACCOFFSETFROMREG_B             = 0x05;
static const uint8_t POSTBYTE_OP_ACCOFFSETFROMREG_D             = 0x0B;
static const uint8_t POSTBYTE_OP_AUTOINCDECFROMREG_INC1         = 0x00;
static const uint8_t POSTBYTE_OP_AUTOINCDECFROMREG_INC2         = 0x01;
static const uint8_t POSTBYTE_OP_AUTOINCDECFROMREG_DEC1         = 0x02;
static const uint8_t POSTBYTE_OP_AUTOINCDECFROMREG_DEC2         = 0x03;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMPC_8BIT         = 0x0C;
static const uint8_t POSTBYTE_OP_CONSTOFFSETFROMPC_16BIT        = 0x0D;
static const uint8_t POSTBYTE_OP_EXTENDEDINDIRECT               = 0x0F;

#define POSTBYTE_REGISTER_X = 0x00
#define POSTBYTE_REGISTER_Y = 0x01
#define POSTBYTE_REGISTER_U = 0x10
#define POSTBYTE_REGISTER_S = 0x11

static const uint8_t POSTBYTE_REGISTER_MASK = 0x60;

#define POSTBYTE_REGISTER_X = 0x00
#define POSTBYTE_REGISTER_Y = 0x01
#define POSTBYTE_REGISTER_U = 0x10
#define POSTBYTE_REGISTER_S = 0x11

static std::map<unsigned char, Opcode_6x09>     opcode_map;
static std::map<unsigned char, Opcode_6x09>     opcode_map2;
static std::map<unsigned char, Opcode_6x09>     opcode_map3;
static std::map<unsigned char, Opcode_6x09>     opcode_map_undefined;
static std::map<unsigned char, PostByte_6x09>   postbyte_map;

static std::string EXG_TFR_Registers[] = {"D", "X", "Y", "U", "S", "PC", "W", "V", "A", "B", "CC", "DP", "0", "0", "E", "F"};
static std::string PSHS_Registers[] = {"CC", "A", "B", "DP", "X", "Y", "U", "PC"};
static std::string PSHU_Registers[] = {"CC", "A", "B", "DP", "X", "Y", "S", "PC"};
static std::string Postbyte_Registers[] = {"X", "Y", "U", "S"};
static std::string Postbyte_AutoIncDecFromReg[] = {"+", "++", "-", "--"};

Disassembler_6x09::Disassembler_6x09()
{
    /////////////////////////////////////////////////////////////////////////////////
    // Undefined Opcodes
    // 
    // Invalid Opcodes that produce some effect on 6809 processors but are
    // trapped on 6309 ones.
    //
    // They'll be shown starting with a lowercase 'u' followed by a mnemonic
    // if possible, or the Opcode itself otherwise.
    /////////////////////////////////////////////////////////////////////////////////
    if( opcode_map_undefined.empty() )
    {
        Init_Undefined_Opcode_Map( opcode_map_undefined );
    }

    // 1-byte opcodes
    if (opcode_map.empty())
    {
        Init_Page1_Opcode_Map( opcode_map );
        opcode_map.merge( opcode_map_undefined );
    }

    // Page 2 opcodes (0x10??)
    if (opcode_map2.empty())
    {
        Init_Page2_Opcode_Map( opcode_map2 );
        opcode_map2.merge( opcode_map_undefined );
    }

    // Page 3 opcodes (0x11??)
    if (opcode_map3.empty())
    {
        Init_Page3_Opcode_Map( opcode_map3 );
        opcode_map3.merge( opcode_map_undefined );
    }

    if (postbyte_map.empty())
    {
        Init_Postbyte_Opcode_Map( postbyte_map );
    }   
    // * means need for special parameter formatting
}

size_t Disassembler_6x09::ReadParameter(const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize)
{
    if (_paramSize > sizeof(size_t))
    {
        return 0;
    }

    size_t retVal = 0;
    size_t mask = 0;

    for (int bytePos = _paramSize - 1; bytePos >= 0; --bytePos)
    {
        retVal |= _data[_pos++] << (bytePos * 8);
        mask |= 0xFF << (bytePos * 8);
    }

    return retVal;
}

void Disassembler_6x09::FormatParameters(const Opcode_6x09& _opcode, const std::vector<unsigned char>& _data, uint16_t& _pos, uint16_t _execAddress, std::stringstream& _ss, std::string& _dstColors)
{
    if( _opcode.params.empty() )
    {
        ++_pos;
        return;
    }

    uint8_t opcodeID = _data[_pos++];

    // Read first param and depnding on the opcode read more if needed.
    size_t paramIdx = 0;
    size_t paramSize =  _opcode.params[paramIdx].size;
    size_t param = ReadParameter(_data, _pos, paramSize);
    _pos += paramSize;

    bool isSpecialCase = false;
    // Special cases
    switch( opcodeID )
    {
        case 0x1E: // EXG. IMMEDIATE, has only one parameter.
        {
            isSpecialCase = true;
            unsigned char reg1 = param & 0x0F;
            unsigned char reg2 = (param & 0xF0) >> 4;
            _ss << EXG_TFR_Registers[reg1] << "," << EXG_TFR_Registers[reg2];
            _dstColors.append(EXG_TFR_Registers[reg1].length(), COLOR_REGISTER);
            _dstColors.append(1, COLOR_TEXT);
            _dstColors.append(EXG_TFR_Registers[reg2].length(), COLOR_REGISTER);
        }
        break;
        case 0x34: // PSHS, PSHU, PULS, PULU. IMMEDIATE, only one parameter.
        case 0x35:
        case 0x36:
        case 0x37:
        {
            isSpecialCase = true;
            uint8_t mask = 0x01;
            for (uint8_t reg = 0; reg < 8; ++reg)
            {
                if (0 != (param & (mask << reg)))
                {
                    _ss << (opcodeID == 0x34 || opcodeID == 0x35 ? PSHS_Registers[reg] : PSHU_Registers[reg]) << ",";
                    _dstColors.append(PSHS_Registers[reg].length(), COLOR_REGISTER);
                    _dstColors.append(1, COLOR_TEXT);
                }
            }

            // Clear last comma
            std::string tmpStr = _ss.str();
            if (!tmpStr.empty())
            {
                tmpStr.pop_back();
                _dstColors.pop_back();
                // Note: Assigning tmpStr directly to _ss.str assigns the string back but causes the new data
                //       added to the stringstream to overwrite it from the beginning instead of appending it.
                //       The workaround is to assign the empty string and then inserting the contents back.
                _ss.str("");
                _ss.clear();
                _ss << tmpStr;
            }
        }
        break;
    }

    if (!isSpecialCase)
    {
        switch (_opcode.addressing)
        {
            case INHERENT:break;
            case IMMEDIATE:
            {
                _ss << "#$" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
                _dstColors.append(1, COLOR_TEXT);
                _dstColors.append((paramSize * 2)+1, COLOR_NUMBER);
            }
            break;
            case DIRECT:
            {
                _ss << "$(DP)" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
                _dstColors.append(1, COLOR_NUMBER);
                _dstColors.append(1, COLOR_TEXT);
                _dstColors.append(2, COLOR_REGISTER);
                _dstColors.append(1, COLOR_TEXT);
                _dstColors.append(paramSize * 2, COLOR_NUMBER);
            }
            break;
            case EXTENDED:
            {
                _ss << "$" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
                _dstColors.append((paramSize * 2)+1, COLOR_NUMBER);
            }
            break;
            case RELATIVE:
            {
                size_t newPC = _pos + _execAddress;
                if (paramSize == 1)
                {
                    newPC += static_cast<int>(static_cast<int8_t>(param));
                    newPC = newPC & 0xFFFF;
                    _ss << "$" << std::uppercase << std::setw(4) << std::right << std::setfill('0') << std::hex << newPC;
                    _dstColors.append(5, COLOR_NUMBER);
                }
                else if (paramSize == 2)
                {
                    newPC += static_cast<int>(static_cast<int16_t>(param));
                    newPC = newPC & 0xFFFF;
                    _ss << "$" << std::uppercase << std::setw(4) << std::right << std::setfill('0') << std::hex << newPC;
                    _dstColors.append(5, COLOR_NUMBER);
                }
                else
                {
                    std::string badParamSize = "Unsupported parameter size!";
                    _ss << badParamSize;
                    _dstColors.append(badParamSize.length(), COLOR_ERROR);
                }
            }
            break;
            case INDEXED:
            {
                // Get the postbyte
                uint8_t postByte = param & 0xFF;
                std::map<unsigned char, PostByte_6x09>::const_iterator postbyteIter = postbyte_map.find(postByte);
                if( postbyteIter != postbyte_map.end() )
                {
                    if( postbyteIter->second.params.empty() )
                    {
                        _ss << postbyteIter->second.name;
                    }
                    else
                    {
                        size_t postbyteParam = ReadParameter(_data, _pos, postbyteIter->second.params[0].size);
                        _pos += postbyteIter->second.params[0].size;
                        size_t postbyteParamWidth = postbyteIter->second.params[0].size * 2;
                        std::stringstream formatter;
                        std::string postbyteStr = postbyteIter->second.name;
                        formatter << "$" << std::hex << std::setfill('0') << std::right << std::setw(postbyteParamWidth) << std::uppercase << postbyteParam;
                        size_t strPos = postbyteStr.find('?');
                        if( strPos != std::string::npos )
                        {
                            _ss << postbyteStr.replace(strPos, 1, formatter.str());
                        }
                        else
                        {
                            _ss << postbyteIter->second.name;
                        }
                    }
                    _dstColors.append(postbyteIter->second.colors);
                }
            }
            break;
            case POSTBYTE:
            break;
        }
    }
}

void Disassembler_6x09::Disassemble(const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors)
{
    if (_execAddress < _loadAddress)
    {
        return;
    }

    std::stringstream ss;
    std::string paramBuffer;
    std::string paramColorBuffer;
    size_t paramValue;
    uint16_t pos = _execAddress - _loadAddress;
    uint8_t pagePrefix = 0x0;

    while (pos < _data.size())
    {
        size_t startLen = ss.str().length();
        ss << std::uppercase << std::right << std::setw(4) << std::setfill('0') << std::hex << _loadAddress + pos << ' ';
        _dstColors.append(5, COLOR_NUMBER);

        pagePrefix = 0x0;
        while( _data[pos] == OPCODE_PAGE_2 || _data[pos] == OPCODE_PAGE_3 )
        {
            pagePrefix = _data[pos++];
        }

        std::map<unsigned char, Opcode_6x09>::const_iterator opcode;
        switch (pagePrefix)
        {
        case OPCODE_PAGE_2:
            opcode = opcode_map2.find(_data[pos]);
            if( opcode == opcode_map2.end() )
            {
                opcode = opcode_map.find(_data[pos]);
            }
            break;
        case OPCODE_PAGE_3:
            opcode = opcode_map3.find(_data[pos]);
            if( opcode == opcode_map3.end() )
            {
                opcode = opcode_map.find(_data[pos]);
            }
            break;
        default:
            opcode = opcode_map.find(_data[pos]);
            break;
        }

        if (opcode != opcode_map.end() && opcode != opcode_map2.end() && opcode != opcode_map3.end())
        {
            ss << std::left << std::setw(MAX_OPCODE_STR_WIDTH) << std::setfill(' ') << opcode->second.name << std::setw(0);
            _dstColors.append(MAX_OPCODE_STR_WIDTH, COLOR_OPCODE);

            FormatParameters(opcode->second, _data, pos, _execAddress, ss, _dstColors);

            ss << std::endl;
        }
        else
        {
            ss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)_data[pos] << " ?" << std::endl;
            _dstColors.append(2, COLOR_NUMBER);
            _dstColors.append(2, COLOR_ERROR);
            ++pos;
        }
        _dstColors.append("\n");
    }

    _dst = ss.str();
}

int Disassembler_6x09::TwosComplement( size_t _param, uint8_t _paramSize )
{
    switch( _paramSize )
    {
        case 1: return static_cast<int>(static_cast<int8_t>(_param)); break;
        case 2: return static_cast<int>(static_cast<int16_t>(_param)); break;
        default: return 0;
    }
}
