/////////////////////////////////////////////////////////////////////////
//
// DiskExtendedColorBasic.cpp - Implementation of DECBasic, a namespace 
//                              with a function that translates
//                              tokenized BASIC files to plain text.
//
// By Roberto Carlos Fernández Gerhardt aka robcfg
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string>
#include <string.h>

#include "DiskExtendedColorBasic.h"

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

namespace DECBasic
{
	static const int  DECB_TOKEN_ENDOFLINE         = 0x00;
	static const int  DECB_TOKEN_START             = 0x80;
	static const int  DECB_TOKEN_DUMMY             = 0xFE;
	static const int  DECB_TOKEN_FUNCTION          = 0xFF;
	static const int  DECB_TOKEN_MAX               = 0xCD;
	static const int  DECB_TOKEN_MAX_DDOS          = 0xF6;
	static const int  DECB_TOKEN_FUNCTION_MAX      = 0xAD;
	static const int  DECB_RESERVED_WORDS_NUM      = 121;
	static const int  DECB_FUNCTIONS_NUM           = 46;
	static const char DECB_COLOR_TEXT              = 'A';
	static const char DECB_COLOR_ERROR             = 'B';
	static const char DECB_COLOR_BASIC_TOKEN       = 'C';
	static const char DECB_COLOR_STRING            = 'D';
	static const char DECB_COLOR_LINE_NUMBER       = 'E';
	static const char DECB_KEYWORD_REM             = 2;

	// Token index is byte - 0x80
	static const std::string reservedWords[] = 
	{
		"FOR",
		"GO",
		"REM",
		"'",
		"ELSE",
		"IF",
		"DATA",
		"PRINT",
		"ON",
		"INPUT",
		"END",
		"NEXT",
		"DIM",
		"READ",
		"RUN",
		"RESTORE",
		"RETURN",
		"STOP",
		"POKE",
		"CONT",
		"LIST",
		"CLEAR",
		"NEW",
		"CLOAD",
		"CSAVE",
		"OPEN",
		"CLOSE",
		"LLIST",
		"SET",
		"RESET",
		"CLS",
		"MOTOR",
		"SOUND",
		"AUDIO",
		"EXEC",
		"SKIPF",
		"TAB(",
		"TO",
		"SUB",
		"THEN",
		"NOT",
		"STEP",
		"OFF",
		"+",
		"-",
		"*",
		"/",
		"^",
		"AND",
		"OR",
		">",
		"=",
		"<",
		"DEL",
		"EDIT",
		"TRON",
		"TROFF",
		"DEF",
		"LET",
		"LINE",
		"PCLS",
		"PSET",
		"PRESET",
		"SCREEN",
		"PCLEAR",
		"COLOR",
		"CIRCLE",
		"PAINT",
		"GET",
		"PUT",
		"DRAW",
		"PCOPY",
		"PMODE",
		"PLAY",
		"DLOAD",
		"RENUM",
		"FN",
		"USING",
		"DIR",
		"DRIVE",
		"FIELD",
		"FILES",
		"KILL",
		"LOAD",
		"LSET",
		"MERGE",
		"RENAME",
		"RSET",
		"SAVE",
		"WRITE",
		"VERIFY",
		"UNLOAD",
		"DSKINI",
		"BACKUP",
		"COPY",
		"DSKI$",
		"DSKO$",
		"DOS",
		"WIDTH",
		"PALETTE",
		"HSCREEN",
		"LPOKE",
		"HCLS",
		"HCOLOR",
		"HPAINT",
		"HCIRCLE",
		"HLINE",
		"HGET",
		"HPUT",
		"HBUFF",
		"HPRINT",
		"ERR",
		"BRK",
		"LOCATE",
		"HSTAT",
		"HSET",
		"HRESET",
		"HDRAW",
		"CMP",
		"RGB",
		"ATTR"
	};

	// Function tokens are also indexed as byte - 0x80, but
	// preceded by an 0xFF byte
	static const std::string functionTokens[] = 
	{
		"SGN",
		"INT",
		"ABS",
		"USR",
		"RND",
		"SIN",
		"PEEK",
		"LEN",
		"STR$",
		"VAL",
		"ASC",
		"CHR$",
		"EOF",
		"JOYSTK",
		"LEFT$",
		"RIGHT$",
		"MID$",
		"POINT",
		"INKEY$",
		"MEM",
		"ATN",
		"COS",
		"TAN",
		"EXP",
		"FIX",
		"LOG",
		"POS",
		"SQR",
		"HEX$",
		"VARPTR",
		"INSTR",
		"TIMER",
		"PPOINT",
		"STRING$",
		"CVN",
		"FREE",
		"LOC",
		"LOF",
		"MKN$",
		"AS",
		"",
		"LPEEK",
		"BUTTON",
		"HPOINT",
		"ERNO",
		"ERLIN"	
	};

//TODO: ELSE is not just 0x84, it's 0x3A84. Don't ask me why...
	bool Decode( const  std::vector<unsigned char>& _in, 
						std::stringstream& _out, 
						std::string& _fltkTextColor, 
						unsigned short int& _lineAddress,
						bool _bIgnoreLineHeader, 
						bool _bShowLineAddresses )
	{
		int maxToken = DECB_TOKEN_MAX_DDOS;
		int maxFunctionToken = DECB_TOKEN_FUNCTION_MAX;
		size_t pos = 3;
		unsigned char token = 0;
		bool isString = false;

		// Starting 0xFF byte plus 2-byte data size.
		if( _in.size() < 3 )
		{
			return false;
		}

		while( pos < _in.size() )
		{
			// Parse header (Pointer + line number)
			if( !_bIgnoreLineHeader )
			{
				token = _in[pos++];

				if( _bShowLineAddresses )
				{
					_out << "[" << std::hex << std::uppercase << _lineAddress << "] " << std::nouppercase << std::dec;
				}

				// Skip 1st word or detect end of file
				unsigned char token2 = _in[pos++]; 
				if( 0 == token && 0 == token2 )
				{
					return false;
				}
				
				_lineAddress = (token << 8) | token2;

				// Read line number
				int lineNumber = (_in[pos++] * 256);
				lineNumber += _in[pos++];
				_out << lineNumber << " ";
				std::string strLineNumber = std::to_string(lineNumber);
				_fltkTextColor.append( strLineNumber.length() + 1, DECB_COLOR_LINE_NUMBER );
			}

			// Parse tokens
			token = _in[pos++];

			if( pos >= _in.size() )
			{
				return false;
			}

			do
			{
				if( token == DECB_TOKEN_FUNCTION )
				{
					token = _in[pos++];
					if( token >= DECB_TOKEN_START && token <= maxFunctionToken)
					{
						_out << functionTokens[token - DECB_TOKEN_START];
						_fltkTextColor.append( functionTokens[token - DECB_TOKEN_START].length(), DECB_COLOR_BASIC_TOKEN );
					}
				}
				else if( token >= DECB_TOKEN_START )
				{
					if( token >= DECB_TOKEN_START && token <= maxToken)
					{
						_out << reservedWords[token - DECB_TOKEN_START];
						_fltkTextColor.append( reservedWords[token - DECB_TOKEN_START].length(), DECB_COLOR_BASIC_TOKEN );
					}
				}
				else if( token >= 0x20 && token < DECB_TOKEN_START )
				{
					if( token == '\"' && !isString )
					{
						isString = true;
						_out << (char)token;
						_fltkTextColor += isString ? DECB_COLOR_STRING : DECB_COLOR_TEXT;
					}
					else if( token == '\"' && isString )
					{
						_out << (char)token;
						_fltkTextColor += isString ? DECB_COLOR_STRING : DECB_COLOR_TEXT;
						isString = false;
					}
					else
					{
						_out << (char)token;
						_fltkTextColor += isString ? DECB_COLOR_STRING : DECB_COLOR_TEXT;
					}
				}

				token = _in[pos++];
			}
			while( token != DECB_TOKEN_ENDOFLINE && (pos < _in.size()) );

			isString = false;
			_out << std::endl;
			_fltkTextColor += '\n';
		}

		return true;
	}

	size_t FindNextChar( char _char, const std::vector<char>& _in, size_t _pos )
	{
		while( _in[_pos] != _char && _pos < _in.size() )
		{
			++_pos;
		}

		return _pos;
	}

	bool Encode( const std::vector<char>& _in, std::vector<unsigned char>& _out )
	{
		size_t dataPos = 0;
		size_t newDataPos = 0;
		unsigned short int nextLineAddress = DRAGONDOS_BASIC_PROGRAM_START;

		std::vector<unsigned char> lineData;
		std::string tmpString;
		bool isString = false;
		bool isRemark = false;

		while( dataPos < _in.size() )
		{
			lineData.clear();

			// Line Number
			int lineNumber = atoi( &_in.data()[dataPos] );

			unsigned char lineNumberHigh = (lineNumber / 256) & 0xFF;
			unsigned char lineNumberLow  = lineNumber  & 0xFF;
			lineData.push_back( lineNumberHigh );
			lineData.push_back( lineNumberLow  );

			dataPos = FindNextChar( ' ', _in, dataPos ) + 1;
			bool hasBeenProcessed = false;

			while( dataPos < _in.size() && _in[dataPos] != 0x0D && _in[dataPos] != 0x0A )
			{
				hasBeenProcessed = false;

				if( !isString && !isRemark )
				{
					// Search for keywords
					for( size_t keyword = 0; keyword < DECB_RESERVED_WORDS_NUM; ++keyword )
					{
						if( 0 == strncmp( &_in.data()[dataPos], reservedWords[keyword].c_str(), reservedWords[keyword].length()) )
						{
							lineData.push_back( (unsigned char)(keyword+DECB_TOKEN_START) );
							dataPos += reservedWords[keyword].length();
							if( keyword == DECB_KEYWORD_REM )
							{
								isRemark = true;
							}
							hasBeenProcessed = true;
							break;
						}
					}
					if( hasBeenProcessed )
					{
						continue;
					}

					// Search for function names
					for( size_t func = 0; func < DECB_FUNCTIONS_NUM; ++func )
					{
						if( 0 == strncmp( &_in.data()[dataPos], functionTokens[func].c_str(), functionTokens[func].length() ) )
						{
							lineData.push_back( DECB_TOKEN_FUNCTION );
							lineData.push_back( (unsigned char)(func+DECB_TOKEN_START) );
							dataPos += functionTokens[func].length();
							hasBeenProcessed = true;
							break;
						}
					}
					if( hasBeenProcessed )
					{
						continue;
					}
				}

				// Regular ASCII characters
				if( _in[dataPos] >= 0x20 /*&& _in[dataPos] < DECB_TOKEN_START*/ )
				{
					if( _in[dataPos] == '\"' )
					{
						isString = !isString;
					}
					lineData.push_back( _in[dataPos++] );
				}
				else
				{
					break;
				}
			}

			lineData.push_back( DECB_TOKEN_ENDOFLINE );
			nextLineAddress += (unsigned short int)lineData.size() + 2;
			_out.push_back( (nextLineAddress/256) & 0xFF);
			_out.push_back( nextLineAddress & 0xFF);
			_out.insert( _out.end(), lineData.begin(), lineData.end() );
			++dataPos;
			isString = false;
			isRemark = false;
		}

		// Add end of data marker (two zero bytes)
		_out.push_back( 0x00 );
		_out.push_back( 0x00 );

		return true;
	}
}
