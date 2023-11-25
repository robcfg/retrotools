/////////////////////////////////////////////////////////////////////////
//
// DragonDOS_BASIC.cpp - Implementation of DragonDOS_BASIC,
//                       a namespace with a function that translates
//                       tokenized BASIC files to plain text.
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
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string>
#include <string.h>

#include "DragonDOS_BASIC.h"

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

namespace DragonDOS_BASIC
{
    static const int  DDBD_TOKEN_ENDOFLINE         = 0x00;
    static const int  DDBD_TOKEN_START             = 0x80;
    static const int  DDBD_TOKEN_DUMMY             = 0xFE;
    static const int  DDBD_TOKEN_FUNCTION          = 0xFF;
    static const int  DDBD_TOKEN_MAX               = 0xCD;
    static const int  DDBD_TOKEN_MAX_DDOS          = 0xE7;
    static const int  DDBD_TOKEN_FUNCTION_MAX      = 0xA1;
    static const int  DDBD_TOKEN_FUNCTION_MAX_DDOS = 0xA8;
    static const int  DDBD_RESERVED_WORDS_NUM      = 104;
    static const int  DDBD_FUNCTIONS_NUM           = 41;
    static const char DDBD_COLOR_TEXT              = 'A';
    static const char DDBD_COLOR_ERROR             = 'B';
    static const char DDBD_COLOR_BASIC_TOKEN       = 'C';
    static const char DDBD_COLOR_STRING            = 'D';
    static const char DDBD_COLOR_LINE_NUMBER       = 'E';
    static const char DDBD_KEYWORD_REM             = 2;

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
        "LET",
        "RUN",
        "RESTORE",
        "RETURN",
        "STOP",
        "POKE",
        "CONT",
        "LIST",
        "CLEAR",
        "NEW",
        "DEF",
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
        "DEL",
        "EDIT",
        "TRON",
        "TROFF",
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
        "TAB(",
        "TO",
        "SUB",
        "FN",
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
        "USING",
        // Dragon DOS reserved Words
        "AUTO",
        "BACKUP",
        "BEEP",
        "BOOT",
        "CHAIN",
        "COPY",
        "CREATE",	
        "DIR",
        "DRIVE",
        "DSKINIT",
        "FREAD",
        "FWRITE",
        "ERROR",
        "KILL",
        "LOAD",
        "MERGE",
        "PROTECT",
        "WAIT",
        "RENAME",
        "SAVE",
        "SREAD",
        "SWRITE",
        "VERIFY",
        "FROM",
        "FLREAD",
        "SWAP"
    };

    // Function tokens are also indexed as byte - 0x80, but
    // preceded by an 0xFF byte
    static const std::string functionTokens[] = 
    {
        "SGN",
        "INT",
        "ABS",
        "POS",
        "RND",
        "SQR",
        "LOG",
        "EXP",
        "SIN",
        "COS",
        "TAN",
        "ATN",
        "PEEK",
        "LEN",
        "STR$",
        "VAL",
        "ASC",
        "CHR$",
        "EOF",
        "JOYSTK",
        "FIX",
        "HEX$",
        "LEFT$",
        "RIGHT$",
        "MID$",
        "POINT",
        "INKEY$",
        "MEM",
        "VARPTR",
        "INSTR",
        "TIMER",
        "PPOINT",
        "STRING$",
        "USR",
        // Dragon DOS functions
        "LOF",
        "FREE",
        "ERL",
        "ERR",
        "HIMEM",
        "LOC",
        "FRE$"
    };

//TODO: ELSE is not just 0x84, it's 0x3A84. Don't ask me why...
    bool Decode( const  std::vector<unsigned char>& _in, 
                        std::stringstream& _out, 
                        std::string& _fltkTextColor, 
                        unsigned short int& _lineAddress,
                        bool _bIgnoreLineHeader, 
                        bool _bShowLineAddresses )
    {
        int maxToken = DDBD_TOKEN_MAX_DDOS;
        int maxFunctionToken = DDBD_TOKEN_FUNCTION_MAX_DDOS;
        size_t pos = 0;
        unsigned char token = 0;
        bool isString = false;

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
                _fltkTextColor.append( strLineNumber.length() + 1, DDBD_COLOR_LINE_NUMBER );
            }

            // Parse tokens
            token = _in[pos++];

            if( pos >= _in.size() )
            {
                return false;
            }

            do
            {
                if( token == DDBD_TOKEN_FUNCTION )
                {
                    token = _in[pos++];
                    if( token >= DDBD_TOKEN_START && token <= maxFunctionToken)
                    {
                        _out << functionTokens[token - DDBD_TOKEN_START];
                        _fltkTextColor.append( functionTokens[token - DDBD_TOKEN_START].length(), DDBD_COLOR_BASIC_TOKEN );
                    }
                }
                else if( token >= DDBD_TOKEN_START )
                {
                    if( token >= DDBD_TOKEN_START && token <= maxToken)
                    {
                        _out << reservedWords[token - DDBD_TOKEN_START];
                        _fltkTextColor.append( reservedWords[token - DDBD_TOKEN_START].length(), DDBD_COLOR_BASIC_TOKEN );
                    }
                }
                else if( token >= 0x20 && token < DDBD_TOKEN_START )
                {
                    if( token == '\"' && !isString )
                    {
                        isString = true;
                        _out << (char)token;
                        _fltkTextColor += isString ? DDBD_COLOR_STRING : DDBD_COLOR_TEXT;
                    }
                    else if( token == '\"' && isString )
                    {
                        _out << (char)token;
                        _fltkTextColor += isString ? DDBD_COLOR_STRING : DDBD_COLOR_TEXT;
                        isString = false;
                    }
                    else
                    {
                        _out << (char)token;
                        _fltkTextColor += isString ? DDBD_COLOR_STRING : DDBD_COLOR_TEXT;
                    }
                }

                token = _in[pos++];
            }
            while( token != DDBD_TOKEN_ENDOFLINE && (pos < _in.size()) );

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
                    for( size_t keyword = 0; keyword < DDBD_RESERVED_WORDS_NUM; ++keyword )
                    {
                        if( 0 == strncmp( &_in.data()[dataPos], reservedWords[keyword].c_str(), reservedWords[keyword].length()) )
                        {
                            lineData.push_back( (unsigned char)(keyword+DDBD_TOKEN_START) );
                            dataPos += reservedWords[keyword].length();
                            if( keyword == DDBD_KEYWORD_REM )
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
                    for( size_t func = 0; func < DDBD_FUNCTIONS_NUM; ++func )
                    {
                        if( 0 == strncmp( &_in.data()[dataPos], functionTokens[func].c_str(), functionTokens[func].length() ) )
                        {
                            lineData.push_back( DDBD_TOKEN_FUNCTION );
                            lineData.push_back( (unsigned char)(func+DDBD_TOKEN_START) );
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
                if( _in[dataPos] >= 0x20 /*&& _in[dataPos] < DDBD_TOKEN_START*/ )
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

            lineData.push_back( DDBD_TOKEN_ENDOFLINE );
            nextLineAddress += lineData.size() + 2;
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
