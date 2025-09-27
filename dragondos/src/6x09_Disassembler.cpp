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

static std::string EXG_TFR_Registers[] = {"D", "X", "Y", "U", "S", "PC", "W", "V", "A", "B", "CC", "DP", "0", "0", "E", "F"};
static std::string PSHS_Registers[] = {"CC", "A", "B", "DP", "X", "Y", "U", "PC"};
static std::string PSHU_Registers[] = {"CC", "A", "B", "DP", "X", "Y", "S", "PC"};
static std::string Postbyte_Registers[] = {"X", "Y", "U", "S"};
static std::string Postbyte_AutoIncDecFromReg[] = {"+", "++", "-", "--"};
static std::string UNUSED_PAGE_2_PREFIX_STR = "Unused page 2 prefix";
static std::string UNUSED_PAGE_3_PREFIX_STR = "Unused page 3 prefix";

static const size_t BYTES_FIELD_MAX_LENGTH = 16;

Disassembler_6x09::Disassembler_6x09()
{
}

void  Disassembler_6x09::Init( EProcessor _processor )
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
		Init_Undefined_Opcode_Map( opcode_map_undefined, _processor );
	}

	// 1-byte opcodes
	if (opcode_map.empty())
	{
		Init_Page1_Opcode_Map( opcode_map, _processor );
		opcode_map.merge( opcode_map_undefined );
	}

	// Page 2 opcodes (0x10??)
	if (opcode_map2.empty())
	{
		Init_Page2_Opcode_Map( opcode_map2, _processor );
		opcode_map2.merge( opcode_map_undefined );
	}

	// Page 3 opcodes (0x11??)
	if (opcode_map3.empty())
	{
		Init_Page3_Opcode_Map( opcode_map3, _processor );
		opcode_map3.merge( opcode_map_undefined );
	}

	if (postbyte_map.empty())
	{
		Init_Postbyte_Opcode_Map( postbyte_map, _processor );
	}   
}

size_t Disassembler_6x09::ReadParameter(const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize, std::vector<uint8_t>& _bytes )
{
	if (_paramSize > sizeof(size_t))
	{
		return 0;
	}

	size_t retVal = 0;
	size_t mask = 0;

	for (int bytePos = _paramSize - 1; bytePos >= 0; --bytePos)
	{
		_bytes.push_back( _data[_pos] );
		retVal |= _data[_pos++] << (bytePos * 8);
		mask |= 0xFF << (bytePos * 8);
	}

	return retVal;
}

void Disassembler_6x09::FormatParameters(uint8_t _opcodeID, const Opcode_6x09& _opcode, const std::vector<unsigned char>& _data, uint16_t& _pos, uint16_t _execAddress, std::vector<uint8_t>& _bytes, std::stringstream& _ss, std::string& _dstColors)
{
	_ss.str("");
	_ss.clear();
	_dstColors = "";

	if( _opcode.params.empty() )
	{
		return;
	}

	// Read first param and depending on the opcode read more if needed.
	size_t paramIdx = 0;
	size_t paramSize =  _opcode.params[paramIdx].size;
	size_t param = ReadParameter(_data, _pos, paramSize, _bytes);
	_pos += paramSize;

	bool isSpecialCase = false;
	// Special cases
	switch( _opcodeID )
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
					_ss << (_opcodeID == 0x34 || _opcodeID == 0x35 ? PSHS_Registers[reg] : PSHU_Registers[reg]) << ",";
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
			case AM_INHERENT:break;
			case AM_IMMEDIATE:
			{
				_ss << "#$" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
				_dstColors.append(1, COLOR_TEXT);
				_dstColors.append((paramSize * 2)+1, COLOR_NUMBER);
			}
			break;
			case AM_DIRECT:
			{
				_ss << "<$" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
				_dstColors.append(1, COLOR_TEXT);
				_dstColors.append((paramSize * 2)+1, COLOR_NUMBER);
			}
			break;
			case AM_EXTENDED:
			{
				_ss << "$" << std::uppercase << std::setw(paramSize * 2) << std::right << std::setfill('0') << std::hex << param;
				_dstColors.append((paramSize * 2)+1, COLOR_NUMBER);
			}
			break;
			case AM_RELATIVE:
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
			case AM_INDEXED:
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
						size_t postbyteParam = ReadParameter(_data, _pos, postbyteIter->second.params[0].size, _bytes);
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
			case AM_POSTBYTE:
			break;
		}
	}
}

void Disassembler_6x09::FormatUnusedPrefix( uint16_t _address, uint8_t _prefix, std::stringstream &_dst, std::string &_dstColors )
{
	uint16_t opcodeStartAddress = _address;
	_dst << std::uppercase << std::right << std::setw(4) << std::setfill('0') << std::hex << opcodeStartAddress << ": ";
	_dstColors.append(6, COLOR_NUMBER);
	_dst << std::setw(2) << (unsigned int)_prefix << std::string(BYTES_FIELD_MAX_LENGTH - 2, ' ');
	_dst << (_prefix == OPCODE_PAGE_2 ? UNUSED_PAGE_2_PREFIX_STR : UNUSED_PAGE_3_PREFIX_STR ) << std::endl;
	_dstColors.append(BYTES_FIELD_MAX_LENGTH + UNUSED_PAGE_2_PREFIX_STR.length(), COLOR_TEXT);
	_dstColors += "\n";
}

void Disassembler_6x09::FormatBytes( std::vector<uint8_t>& _bytes, std::string& _bytesStr )
{
	std::stringstream ssBytes;
	ssBytes << std::uppercase << std::right << std::setw(2) << std::setfill('0') << std::hex;

	for( auto byte : _bytes )
	{
		ssBytes << std::setw(2) << std::setfill('0') << (unsigned int)byte << ' ';
	}

	_bytesStr = ssBytes.str();
	if( _bytesStr.length() < BYTES_FIELD_MAX_LENGTH )
	{
		_bytesStr.append( BYTES_FIELD_MAX_LENGTH - _bytesStr.length(), ' ' );
	}
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

void Disassembler_6x09::Disassemble(const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors)
{
	if (_execAddress < _loadAddress)
	{
		return;
	}

	std::stringstream ss;
	std::stringstream ssParams;
	std::string paramColors;
	std::string bytesString;
	std::string bytesColors;
	bytesColors.append( BYTES_FIELD_MAX_LENGTH, COLOR_TEXT );

	// We need opcode name, the parameters and the bytes in order to format everything properly.
	// Comments regarding known values as certain memory addresses can be added at the end of the strings.
	std::string opcodeName;
	std::vector<uint8_t> opcodeBytes;

	uint16_t pos = _execAddress - _loadAddress; // size_t
	uint8_t  pagePrefix = 0x0;
	uint8_t  currentByte = 0;
	uint8_t  nextByte = 0;
	uint16_t currentAddress = 0;
	uint16_t opcodeStart = 0;

	while( pos < _data.size() )
	{
		currentByte = _data[pos];
		currentAddress = _loadAddress + pos;
		pos++;

		// Opcode page prefix
		if( currentByte == OPCODE_PAGE_2 || currentByte == OPCODE_PAGE_3 )
		{
			nextByte = _data[pos];
			if( nextByte == OPCODE_PAGE_2 || nextByte == OPCODE_PAGE_3 )
			{
				// If a page prefix follows another page prefix, the latter will be selected, making the 
				// current one useless.
				FormatUnusedPrefix( currentAddress, currentByte, ss, _dstColors );
			}

			opcodeBytes.clear();
			opcodeBytes.push_back( currentByte );
			pagePrefix = currentByte;

			// Fetch next byte
			continue;
		}
		opcodeStart = currentAddress;

		// Opcode
		opcodeBytes.push_back( currentByte );

		Opcode_6x09 opcode {"INVALID", {}, AM_IMMEDIATE};
		std::map<unsigned char, Opcode_6x09>::const_iterator opcodeIter;
		switch (pagePrefix)
		{
		case OPCODE_PAGE_2:
			opcodeIter = opcode_map2.find(currentByte);
			if( opcodeIter == opcode_map2.end() )
			{
				opcodeIter = opcode_map.find(currentByte);
				if( opcodeIter != opcode_map.end() )
				{
					opcode = opcodeIter->second;
				}
			}
			else
			{
				opcode = opcodeIter->second;
			}
			break;
		case OPCODE_PAGE_3:
			opcodeIter = opcode_map3.find(currentByte);
			if( opcodeIter == opcode_map3.end() )
			{
				opcodeIter = opcode_map.find(currentByte);
				if( opcodeIter != opcode_map.end() )
				{
					opcode = opcodeIter->second;
				}
			}
			else
			{
				opcode = opcodeIter->second;
			}
			break;
		default:
			opcodeIter = opcode_map.find(currentByte);
			if( opcodeIter != opcode_map.end() )
			{
				opcode = opcodeIter->second;
			}
			break;
		}

		// Valid opcode found?
		if ( 0 != opcode.name.compare("INVALID") )
		{
			// Read and format parameters
			FormatParameters(currentByte,opcode, _data, pos, _execAddress, opcodeBytes, ssParams, paramColors);
			// Format bytes
			FormatBytes( opcodeBytes, bytesString );
			// Compose final line
			ss << std::uppercase << std::right << std::setw(4) << std::setfill('0') << std::hex << opcodeStart << ": ";
			_dstColors.append(6, COLOR_NUMBER);
			ss << bytesString;
			_dstColors += bytesColors;
			ss << std::left << std::setw(MAX_OPCODE_STR_WIDTH) << std::setfill(' ') << opcode.name << std::setw(0);
			_dstColors.append(MAX_OPCODE_STR_WIDTH, COLOR_OPCODE);
			ss << ssParams.str();
			_dstColors += paramColors;
			ss << std::endl;
			_dstColors += '\n';

			// Clear temp buffers.
			opcodeBytes.clear();
			pagePrefix = 0;
		}
		else
		{
			// Format error string.
			FormatBytes( opcodeBytes, bytesString );
			// Compose final line
			ss << std::uppercase << std::right << std::setw(4) << std::setfill('0') << std::hex << opcodeStart << ": ";
			_dstColors.append(6, COLOR_NUMBER);
			ss << bytesString;
			_dstColors += bytesColors;
			ss << "?" << std::endl;
			_dstColors += COLOR_ERROR;
			_dstColors += "\n";
			opcodeBytes.clear();
			pagePrefix = 0;
		}
	}

	_dst = ss.str();
}

