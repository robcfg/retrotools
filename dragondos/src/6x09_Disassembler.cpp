#include "6x09_Disassembler.h"

#include <iomanip>
#include <map>
#include <sstream>

#define MAX_OPCODE_STR_WIDTH 6

#define COLOR_TEXT      'A'
#define COLOR_ERROR     'B'
#define COLOR_OPCODE    'C'
#define COLOR_REGISTER  'D'
#define COLOR_NUMBER    'E'

#define OPCODE_PAGE_2   0x10
#define OPCODE_PAGE_3   0x13

static std::map<unsigned char,Opcode_6x09> opcode_map;
static std::map<unsigned char,Opcode_6x09> opcode_map2;
static std::map<unsigned char,Opcode_6x09> opcode_map3;

static std::string EXG_TFR_Registers[] = { "D","X","Y","U","S","PC","W","V","A","B","CC","DP","0","0","E","F" };
static std::string PSHS_Registers   [] = { "CC","A","B","DP","X","Y","U","PC" };
static std::string PSHU_Registers   [] = { "CC","A","B","DP","X","Y","S","PC" };

// TODO: Create maps for opcodes with more than 1 byte

Disassembler_6x09::Disassembler_6x09()
{
    // 1-byte opcodes
    if( opcode_map.empty() )
    {
        // ABX __________________________________________________________________________
        opcode_map.emplace(0x3A, Opcode_6x09{ "ABX"     , {}        , INHERENT  });
        // ADC __________________________________________________________________________
        opcode_map.emplace(0x89, Opcode_6x09{ "ADCA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x99, Opcode_6x09{ "ADCA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA9, Opcode_6x09{ "ADCA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB9, Opcode_6x09{ "ADCA"    , { {2} }   , EXTENDED  });
        // ADCB _________________________________________________________________________
        opcode_map.emplace(0xC9, Opcode_6x09{ "ADCB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD9, Opcode_6x09{ "ADCB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE9, Opcode_6x09{ "ADCB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF9, Opcode_6x09{ "ADCB"    , { {2} }   , EXTENDED  });
        // ADDA _________________________________________________________________________
        opcode_map.emplace(0x8B, Opcode_6x09{ "ADDA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x9B, Opcode_6x09{ "ADDA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xAB, Opcode_6x09{ "ADDA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBB, Opcode_6x09{ "ADDA"    , { {2} }   , EXTENDED  });
        // ADDB _________________________________________________________________________
        opcode_map.emplace(0xCB, Opcode_6x09{ "ADDB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xDB, Opcode_6x09{ "ADDB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xEB, Opcode_6x09{ "ADDB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFB, Opcode_6x09{ "ADDB"    , { {2} }   , EXTENDED  });
        // ADDD _________________________________________________________________________
        opcode_map.emplace(0xC3, Opcode_6x09{ "ADDD"    , { {2} }   , IMMEDIATE });
        opcode_map.emplace(0xD3, Opcode_6x09{ "ADDD"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE3, Opcode_6x09{ "ADDD"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF3, Opcode_6x09{ "ADDD"    , { {2} }   , EXTENDED  });
        // AIM (6309)____________________________________________________________________
        opcode_map.emplace(0x02, Opcode_6x09{ "AIM"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x62, Opcode_6x09{ "AIM"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x72, Opcode_6x09{ "AIM"     , { {2} }   , EXTENDED  });
        // ANDA _________________________________________________________________________
        opcode_map.emplace(0x84, Opcode_6x09{ "ANDA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x94, Opcode_6x09{ "ANDA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA4, Opcode_6x09{ "ANDA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB4, Opcode_6x09{ "ANDA"    , { {2} }   , EXTENDED  });
        // ANDB _________________________________________________________________________
        opcode_map.emplace(0xC4, Opcode_6x09{ "ANDB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD4, Opcode_6x09{ "ANDB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE4, Opcode_6x09{ "ANDB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF4, Opcode_6x09{ "ANDB"    , { {2} }   , EXTENDED  });
        // ANDCC ________________________________________________________________________
        opcode_map.emplace(0x1C, Opcode_6x09{ "ANDCC"   , { {1} }   , IMMEDIATE });
        // ASLA _________________________________________________________________________
        opcode_map.emplace(0x48, Opcode_6x09{ "ASLA"    , {}        , INHERENT });
        // ASLB _________________________________________________________________________
        opcode_map.emplace(0x58, Opcode_6x09{ "ASLB"    , {}        , INHERENT });
        // ASL __________________________________________________________________________
        opcode_map.emplace(0x08, Opcode_6x09{ "ASL"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x68, Opcode_6x09{ "ASL"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0x78, Opcode_6x09{ "ASL"     , { {2} }   , EXTENDED  });
        // ASRA _________________________________________________________________________
        opcode_map.emplace(0x47, Opcode_6x09{ "ASRA"    , {}        , INHERENT });
        // ASRB _________________________________________________________________________
        opcode_map.emplace(0x57, Opcode_6x09{ "ASRB"    , {}        , INHERENT });
        // ASL __________________________________________________________________________
        opcode_map.emplace(0x07, Opcode_6x09{ "ASR"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x68, Opcode_6x09{ "ASR"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0x77, Opcode_6x09{ "ASR"     , { {2} }   , EXTENDED  });
        // BCC __________________________________________________________________________
        opcode_map.emplace(0x77, Opcode_6x09{ "BCC"     , { {1} }   , RELATIVE  });
        // BCS __________________________________________________________________________
        opcode_map.emplace(0x25, Opcode_6x09{ "BCS"     , { {1} }   , RELATIVE  });
        // BEQ __________________________________________________________________________
        opcode_map.emplace(0x27, Opcode_6x09{ "BEQ"     , { {1} }   , RELATIVE  });
        // BGE __________________________________________________________________________
        opcode_map.emplace(0x2C, Opcode_6x09{ "BGE"     , { {1} }   , RELATIVE  });
        // BGT __________________________________________________________________________
        opcode_map.emplace(0x2E, Opcode_6x09{ "BGT"     , { {1} }   , RELATIVE  });
        // BHI __________________________________________________________________________
        opcode_map.emplace(0x22, Opcode_6x09{ "BHI"     , { {1} }   , RELATIVE  });
        // BHS __________________________________________________________________________
        opcode_map.emplace(0x24, Opcode_6x09{ "BHS"     , { {1} }   , RELATIVE  });
        // BITA _________________________________________________________________________
        opcode_map.emplace(0x85, Opcode_6x09{ "BITA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x95, Opcode_6x09{ "BITA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA5, Opcode_6x09{ "BITA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB5, Opcode_6x09{ "BITA"    , { {2} }   , EXTENDED  });
        // BITB _________________________________________________________________________
        opcode_map.emplace(0xC5, Opcode_6x09{ "BITB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD5, Opcode_6x09{ "BITB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE5, Opcode_6x09{ "BITB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF5, Opcode_6x09{ "BITB"    , { {2} }   , EXTENDED  });
        // BLE __________________________________________________________________________
        opcode_map.emplace(0x2F, Opcode_6x09{ "BLE"     , { {1} }   , RELATIVE  });
        // BLO __________________________________________________________________________
        opcode_map.emplace(0x25, Opcode_6x09{ "BLO"     , { {1} }   , RELATIVE  });
        // BLS __________________________________________________________________________
        opcode_map.emplace(0x23, Opcode_6x09{ "BLS"     , { {1} }   , RELATIVE  });
        // BLT __________________________________________________________________________
        opcode_map.emplace(0x2D, Opcode_6x09{ "BLT"     , { {1} }   , RELATIVE  });
        // BMI __________________________________________________________________________
        opcode_map.emplace(0x2B, Opcode_6x09{ "BMI"     , { {1} }   , RELATIVE  });
        // BNE __________________________________________________________________________
        opcode_map.emplace(0x26, Opcode_6x09{ "BNE"     , { {1} }   , RELATIVE  });
        // BPL __________________________________________________________________________
        opcode_map.emplace(0x2A, Opcode_6x09{ "BPL"     , { {1} }   , RELATIVE  });
        // BRA __________________________________________________________________________
        opcode_map.emplace(0x20, Opcode_6x09{ "BRA"     , { {1} }   , RELATIVE  });
        // BRN __________________________________________________________________________
        opcode_map.emplace(0x21, Opcode_6x09{ "BRN"     , { {1} }   , RELATIVE  });
        // BSR __________________________________________________________________________
        opcode_map.emplace(0x8D, Opcode_6x09{ "BSR"     , { {1} }   , RELATIVE  });
        // BVC __________________________________________________________________________
        opcode_map.emplace(0x28, Opcode_6x09{ "BVC"     , { {1} }   , RELATIVE  });
        // BVS __________________________________________________________________________
        opcode_map.emplace(0x29, Opcode_6x09{ "BVS"     , { {1} }   , RELATIVE  });
        // CLRA _________________________________________________________________________
        opcode_map.emplace(0x4F, Opcode_6x09{ "CLRA"    , {}        , INHERENT });
        // CLRB _________________________________________________________________________
        opcode_map.emplace(0x5F, Opcode_6x09{ "CLRB"    , {}        , INHERENT });
        // CLR __________________________________________________________________________
        opcode_map.emplace(0x0F, Opcode_6x09{ "CLR"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x6F, Opcode_6x09{ "ASR"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0x7F, Opcode_6x09{ "CLR"     , { {2} }   , EXTENDED  });
        // CMPA _________________________________________________________________________
        opcode_map.emplace(0x81, Opcode_6x09{ "CMPA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x91, Opcode_6x09{ "CMPA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA1, Opcode_6x09{ "CMPA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB1, Opcode_6x09{ "CMPA"    , { {2} }   , EXTENDED  });
        // CMPB _________________________________________________________________________
        opcode_map.emplace(0xC1, Opcode_6x09{ "CMPB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD1, Opcode_6x09{ "CMPB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE1, Opcode_6x09{ "CMPB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF1, Opcode_6x09{ "CMPB"    , { {2} }   , EXTENDED  });
        // CMPX _________________________________________________________________________
        opcode_map.emplace(0x8C, Opcode_6x09{ "CMPX"    , { {2} }   , IMMEDIATE });
        opcode_map.emplace(0x9C, Opcode_6x09{ "CMPX"    , { {2} }   , DIRECT    });
        //opcode_map.emplace(0xAC, Opcode_6x09{ "CMPX"    , { {2} }   , INDEXED   });
        opcode_map.emplace(0xBC, Opcode_6x09{ "CMPX"    , { {2} }   , EXTENDED  });
        // COMA _________________________________________________________________________
        opcode_map.emplace(0x43, Opcode_6x09{ "COMA"    , {}        , INHERENT });
        // COMB _________________________________________________________________________
        opcode_map.emplace(0x53, Opcode_6x09{ "COMB"    , {}        , INHERENT });
        // COM __________________________________________________________________________
        opcode_map.emplace(0x03, Opcode_6x09{ "COM"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x63, Opcode_6x09{ "COM"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x73, Opcode_6x09{ "COM"     , { {2} }   , EXTENDED  });
        // CWAI _________________________________________________________________________
        opcode_map.emplace(0x3C, Opcode_6x09{ "CWAI"    , { {1} }   , IMMEDIATE });
        // DAA __________________________________________________________________________
        opcode_map.emplace(0x19, Opcode_6x09{ "DAA"     , {}        , INHERENT });
        // DECA _________________________________________________________________________
        opcode_map.emplace(0x4A, Opcode_6x09{ "DECA"    , {}        , INHERENT });
        // DECB _________________________________________________________________________
        opcode_map.emplace(0x5A, Opcode_6x09{ "DECB"    , {}        , INHERENT });
        // DEC __________________________________________________________________________
        opcode_map.emplace(0x0A, Opcode_6x09{ "DEC"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x6A, Opcode_6x09{ "DEC"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x7A, Opcode_6x09{ "DEC"     , { {2} }   , EXTENDED  });
        // EORA _________________________________________________________________________
        opcode_map.emplace(0x88, Opcode_6x09{ "EORA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x98, Opcode_6x09{ "EORA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA8, Opcode_6x09{ "EORA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB8, Opcode_6x09{ "EORA"    , { {2} }   , EXTENDED  });
        // EORB _________________________________________________________________________
        opcode_map.emplace(0xC8, Opcode_6x09{ "EORB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD8, Opcode_6x09{ "EORB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE8, Opcode_6x09{ "EORB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF8, Opcode_6x09{ "EORB"    , { {2} }   , EXTENDED  });
        // EXG __________________________________________________________________________
        opcode_map.emplace(0x1E, Opcode_6x09{ "EXG"     , { {1} }   , IMMEDIATE });
        // INCA _________________________________________________________________________
        opcode_map.emplace(0x4C, Opcode_6x09{ "INCA"    , {}        , INHERENT });
        // INCB _________________________________________________________________________
        opcode_map.emplace(0x5C, Opcode_6x09{ "INCB"    , {}        , INHERENT });
        // INC __________________________________________________________________________
        opcode_map.emplace(0x0C, Opcode_6x09{ "INC"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x6C, Opcode_6x09{ "INC"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x7C, Opcode_6x09{ "INC"     , { {2} }   , EXTENDED  });
        // JMP __________________________________________________________________________
        opcode_map.emplace(0x0E, Opcode_6x09{ "JMP"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x6E, Opcode_6x09{ "JMP"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x7E, Opcode_6x09{ "JMP"     , { {2} }   , EXTENDED  });
        // JSR __________________________________________________________________________
        opcode_map.emplace(0x9D, Opcode_6x09{ "JSR"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xAD, Opcode_6x09{ "JSR"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBD, Opcode_6x09{ "JSR"     , { {2} }   , EXTENDED  });
        // LBRA _________________________________________________________________________
        opcode_map.emplace(0x16, Opcode_6x09{ "LBRA"    , { {1} }   , RELATIVE  });
        // LBSR _________________________________________________________________________
        opcode_map.emplace(0x17, Opcode_6x09{ "LBSR"    , { {1} }   , RELATIVE  });
        // LDA __________________________________________________________________________
        opcode_map.emplace(0x86, Opcode_6x09{ "LDA"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x96, Opcode_6x09{ "LDA"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA6, Opcode_6x09{ "LDA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB6, Opcode_6x09{ "LDA"     , { {2} }   , EXTENDED  });
        // LDB __________________________________________________________________________
        opcode_map.emplace(0xC6, Opcode_6x09{ "LDB"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD6, Opcode_6x09{ "LDB"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE6, Opcode_6x09{ "LDB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF6, Opcode_6x09{ "LDB"     , { {2} }   , EXTENDED  });
        // LDD __________________________________________________________________________
        opcode_map.emplace(0xCC, Opcode_6x09{ "LDD"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xDC, Opcode_6x09{ "LDD"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xEC, Opcode_6x09{ "LDD"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFC, Opcode_6x09{ "LDD"     , { {2} }   , EXTENDED  });
        // LDQ (6309)____________________________________________________________________
        opcode_map.emplace(0xCD, Opcode_6x09{ "LDQ"     , { {4} }   , IMMEDIATE });
        // LDU __________________________________________________________________________
        opcode_map.emplace(0xCE, Opcode_6x09{ "LDU"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xDE, Opcode_6x09{ "LDU"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xEE, Opcode_6x09{ "LDU"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFE, Opcode_6x09{ "LDU"     , { {2} }   , EXTENDED  });
        // LDX __________________________________________________________________________
        opcode_map.emplace(0x8E, Opcode_6x09{ "LDX"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x9E, Opcode_6x09{ "LDX"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xAE, Opcode_6x09{ "LDX"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBE, Opcode_6x09{ "LDX"     , { {2} }   , EXTENDED  });
        // LEAS _________________________________________________________________________
        //opcode_map.emplace(0x32, Opcode_6x09{ "LEAS"    , { {2} }   , INDEXED   });
        // LEAU _________________________________________________________________________
        //opcode_map.emplace(0x33, Opcode_6x09{ "LEAU"    , { {2} }   , INDEXED   });
        // LEAX _________________________________________________________________________
        //opcode_map.emplace(0x30, Opcode_6x09{ "LEAX"    , { {2} }   , INDEXED   });
        // LEAY _________________________________________________________________________
        //opcode_map.emplace(0x31, Opcode_6x09{ "LEAY"    , { {2} }   , INDEXED   });
        // LSLA _________________________________________________________________________
        opcode_map.emplace(0x48, Opcode_6x09{ "LSLA"    , {}        , INHERENT });
        // LSLB _________________________________________________________________________
        opcode_map.emplace(0x58, Opcode_6x09{ "LSLB"    , {}        , INHERENT });
        // LSL __________________________________________________________________________
        opcode_map.emplace(0x08, Opcode_6x09{ "LSL"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x68, Opcode_6x09{ "LSL"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x78, Opcode_6x09{ "LSL"     , { {2} }   , EXTENDED  });
        // LSRA _________________________________________________________________________
        opcode_map.emplace(0x48, Opcode_6x09{ "LSRA"    , {}        , INHERENT });
        // LSRB _________________________________________________________________________
        opcode_map.emplace(0x58, Opcode_6x09{ "LSRB"    , {}        , INHERENT });
        // LSR __________________________________________________________________________
        opcode_map.emplace(0x08, Opcode_6x09{ "LSR"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x68, Opcode_6x09{ "LSR"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x78, Opcode_6x09{ "LSR"     , { {2} }   , EXTENDED  });
        // MUL __________________________________________________________________________
        opcode_map.emplace(0x3D, Opcode_6x09{ "MUL"     , {}        , INHERENT });
        // NEGA _________________________________________________________________________
        opcode_map.emplace(0x40, Opcode_6x09{ "NEGA"    , {}        , INHERENT });
        // NEGB _________________________________________________________________________
        opcode_map.emplace(0x50, Opcode_6x09{ "NEGB"    , {}        , INHERENT });
        // NEG __________________________________________________________________________
        opcode_map.emplace(0x00, Opcode_6x09{ "NEG"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x60, Opcode_6x09{ "NEG"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x70, Opcode_6x09{ "NEG"     , { {2} }   , EXTENDED  });
        // NOP __________________________________________________________________________
        opcode_map.emplace(0x12, Opcode_6x09{ "NOP"     , {}        , INHERENT });
        // ORA __________________________________________________________________________
        opcode_map.emplace(0x8A, Opcode_6x09{ "ORA"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x9A, Opcode_6x09{ "ORA"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xAA, Opcode_6x09{ "ORA"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBA, Opcode_6x09{ "ORA"     , { {2} }   , EXTENDED  });
        // ORB __________________________________________________________________________
        opcode_map.emplace(0xCA, Opcode_6x09{ "ORB"     , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xDA, Opcode_6x09{ "ORB"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xEA, Opcode_6x09{ "ORB"    , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFA, Opcode_6x09{ "ORB"     , { {2} }   , EXTENDED  });
        // ORCC _________________________________________________________________________
        opcode_map.emplace(0x1A, Opcode_6x09{ "ORCC"    , { {1} }   , IMMEDIATE });
        // OIM (6309)____________________________________________________________________
        opcode_map.emplace(0x01, Opcode_6x09{ "OIM"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0x61, Opcode_6x09{ "OIM"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0x71, Opcode_6x09{ "OIM"    , { {2} }    , EXTENDED  });
        // PSHS _________________________________________________________________________
        opcode_map.emplace(0x34, Opcode_6x09{ "PSHS"    , { {1} }   , IMMEDIATE });
        // PSHU _________________________________________________________________________
        opcode_map.emplace(0x36, Opcode_6x09{ "PSHU"    , { {1} }   , IMMEDIATE });
        // PULS _________________________________________________________________________
        opcode_map.emplace(0x35, Opcode_6x09{ "PULS"    , { {1} }   , IMMEDIATE });
        // PULU _________________________________________________________________________
        opcode_map.emplace(0x37, Opcode_6x09{ "PULU"    , { {1} }   , IMMEDIATE });
        // ROLA _________________________________________________________________________
        opcode_map.emplace(0x49, Opcode_6x09{ "ROLA"    , {}        , INHERENT });
        // ROLB _________________________________________________________________________
        opcode_map.emplace(0x59, Opcode_6x09{ "ROLB"    , {}        , INHERENT });
        // ROL __________________________________________________________________________
        opcode_map.emplace(0x09, Opcode_6x09{ "ROL"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x69, Opcode_6x09{ "ROL"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x79, Opcode_6x09{ "ROL"     , { {2} }   , EXTENDED  });
        // RORA _________________________________________________________________________
        opcode_map.emplace(0x46, Opcode_6x09{ "RORA"    , {}        , INHERENT });
        // RORB _________________________________________________________________________
        opcode_map.emplace(0x56, Opcode_6x09{ "RORB"    , {}        , INHERENT });
        // ROR __________________________________________________________________________
        opcode_map.emplace(0x06, Opcode_6x09{ "ROR"     , { {1} }   , DIRECT    });
        //opcode_map.emplace(0x66, Opcode_6x09{ "ROR"     , { {1} }   , INDEXED   });
        opcode_map.emplace(0x76, Opcode_6x09{ "ROR"     , { {2} }   , EXTENDED  });
        // RTI __________________________________________________________________________
        opcode_map.emplace(0x3B, Opcode_6x09{ "RTI"     , {}        , INHERENT });
        // RTS __________________________________________________________________________
        opcode_map.emplace(0x39, Opcode_6x09{ "RTS"     , {}        , INHERENT });
        // SBCA _________________________________________________________________________
        opcode_map.emplace(0x82, Opcode_6x09{ "SBCA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x92, Opcode_6x09{ "SBCA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA2, Opcode_6x09{ "SBCA"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB2, Opcode_6x09{ "SBCA"    , { {2} }   , EXTENDED  });
        // SBCB _________________________________________________________________________
        opcode_map.emplace(0xC2, Opcode_6x09{ "SBCB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD2, Opcode_6x09{ "SBCB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE2, Opcode_6x09{ "SBCB"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF2, Opcode_6x09{ "SBCB"    , { {2} }   , EXTENDED  });
        // SEX __________________________________________________________________________
        opcode_map.emplace(0x1D, Opcode_6x09{ "SEX"     , {}        , INHERENT });
        // SEXW (6309)___________________________________________________________________
        opcode_map.emplace(0x14, Opcode_6x09{ "SEXW"    , {}        , INHERENT });
        // STA __________________________________________________________________________
        opcode_map.emplace(0x97, Opcode_6x09{ "STA"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xA7, Opcode_6x09{ "STA"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB7, Opcode_6x09{ "STA"    , { {2} }    , EXTENDED  });
        // STB __________________________________________________________________________
        opcode_map.emplace(0xD7, Opcode_6x09{ "STB"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xE7, Opcode_6x09{ "STB"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF7, Opcode_6x09{ "STB"    , { {2} }    , EXTENDED  });
        // STD __________________________________________________________________________
        opcode_map.emplace(0xDD, Opcode_6x09{ "STD"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xED, Opcode_6x09{ "STD"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFD, Opcode_6x09{ "STD"    , { {2} }    , EXTENDED  });
        // STU __________________________________________________________________________
        opcode_map.emplace(0xDF, Opcode_6x09{ "STU"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xEF, Opcode_6x09{ "STU"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xFF, Opcode_6x09{ "STU"    , { {2} }    , EXTENDED  });
        // STX __________________________________________________________________________
        opcode_map.emplace(0x9F, Opcode_6x09{ "STX"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xAF, Opcode_6x09{ "STX"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBF, Opcode_6x09{ "STX"    , { {2} }    , EXTENDED  });
        // SUBA _________________________________________________________________________
        opcode_map.emplace(0x80, Opcode_6x09{ "SUBA"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0x90, Opcode_6x09{ "SUBA"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA0, Opcode_6x09{ "SUBA"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB0, Opcode_6x09{ "SUBA"    , { {2} }   , EXTENDED  });
        // SUBB _________________________________________________________________________
        opcode_map.emplace(0xC0, Opcode_6x09{ "SUBB"    , { {1} }   , IMMEDIATE });
        opcode_map.emplace(0xD0, Opcode_6x09{ "SUBB"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xE0, Opcode_6x09{ "SUBB"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xF0, Opcode_6x09{ "SUBB"    , { {2} }   , EXTENDED  });
        // SUBD _________________________________________________________________________
        opcode_map.emplace(0x83, Opcode_6x09{ "SUBD"    , { {2} }   , IMMEDIATE });
        opcode_map.emplace(0x93, Opcode_6x09{ "SUBD"    , { {1} }   , DIRECT    });
        //opcode_map.emplace(0xA3, Opcode_6x09{ "SUBD"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xB3, Opcode_6x09{ "SUBD"    , { {2} }   , EXTENDED  });
        // SWI __________________________________________________________________________
        opcode_map.emplace(0x3F, Opcode_6x09{ "SWI"     , {}        , INHERENT  });
        // SYNC _________________________________________________________________________
        opcode_map.emplace(0x13, Opcode_6x09{ "SYNC"    , {}        , INHERENT  });
        // TIM (6309)____________________________________________________________________
        opcode_map.emplace(0x0B, Opcode_6x09{ "TIM"     , { {1},{1} }   , DIRECT    });
        //opcode_map.emplace(0x6B, Opcode_6x09{ "TIM"    , { {1},{1} }   , INDEXED   });
        opcode_map.emplace(0x7B, Opcode_6x09{ "TIM"     , { {1},{2} }   , EXTENDED  });
        // TFR (6309)____________________________________________________________________
        opcode_map.emplace(0x1F, Opcode_6x09{ "TFR"     , { {1} }   , IMMEDIATE });
        // TSTA _________________________________________________________________________
        opcode_map.emplace(0x4D, Opcode_6x09{ "TSTA"    , {}        , INHERENT  });
        // TSTB _________________________________________________________________________
        opcode_map.emplace(0x5D, Opcode_6x09{ "TSTB"    , {}        , INHERENT  });
        // TST __________________________________________________________________________
        opcode_map.emplace(0x9F, Opcode_6x09{ "TST"    , { {1} }    , DIRECT    });
        //opcode_map.emplace(0xAF, Opcode_6x09{ "TST"   , { {1} }   , INDEXED   });
        opcode_map.emplace(0xBF, Opcode_6x09{ "TST"    , { {2} }    , EXTENDED  });
    }

    // Page 2 opcodes (0x10??)
    if( opcode_map2.empty() )
    {
        // ADCD (6309)___________________________________________________________________
        opcode_map2.emplace(0x89, Opcode_6x09{ "ADCD"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x99, Opcode_6x09{ "ADCD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA9, Opcode_6x09{ "ADCD"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB9, Opcode_6x09{ "ADCD"    , { {2} }   , EXTENDED  });
        // ADCR (6309)___________________________________________________________________
        opcode_map2.emplace(0x31, Opcode_6x09{ "ADCR"    , { {1} }   , IMMEDIATE });
        // ADDW (6309)___________________________________________________________________
        opcode_map2.emplace(0x8B, Opcode_6x09{ "ADDW"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x9B, Opcode_6x09{ "ADDW"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xAB, Opcode_6x09{ "ADDW"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xBB, Opcode_6x09{ "ADDW"    , { {2} }   , EXTENDED  });
        // ADDR (6309)___________________________________________________________________
        opcode_map2.emplace(0x30, Opcode_6x09{ "ADDR"    , { {1} }   , IMMEDIATE });
        // ANDD (6309)___________________________________________________________________
        opcode_map2.emplace(0x84, Opcode_6x09{ "ANDD"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x94, Opcode_6x09{ "ANDD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA4, Opcode_6x09{ "ANDD"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB4, Opcode_6x09{ "ANDD"    , { {2} }   , EXTENDED  });
        // ANDR (6309)___________________________________________________________________
        opcode_map2.emplace(0x34, Opcode_6x09{ "ANDR"    , { {1} }   , IMMEDIATE });
        // ASLD (6309)___________________________________________________________________
        opcode_map2.emplace(0x48, Opcode_6x09{ "ASLD"    , {}        , IMMEDIATE });
        // ASRD (6309)___________________________________________________________________
        opcode_map2.emplace(0x47, Opcode_6x09{ "ASRD"    , {}        , IMMEDIATE });
        // BITD (6309)___________________________________________________________________
        opcode_map2.emplace(0x85, Opcode_6x09{ "BITD"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x95, Opcode_6x09{ "BITD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA5, Opcode_6x09{ "BITD"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB5, Opcode_6x09{ "BITD"    , { {2} }   , EXTENDED  });
        // CLRD (6309)___________________________________________________________________
        opcode_map2.emplace(0x4F, Opcode_6x09{ "CLRD"    , {}        , INHERENT  });
        // CLRW (6309)___________________________________________________________________
        opcode_map2.emplace(0x5F, Opcode_6x09{ "CLRW"    , {}        , INHERENT  });
        // CMPD _________________________________________________________________________
        opcode_map2.emplace(0x83, Opcode_6x09{ "CMPD"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x93, Opcode_6x09{ "CMPD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA3, Opcode_6x09{ "CMPD"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB3, Opcode_6x09{ "CMPD"    , { {2} }   , EXTENDED  });
        // CMPW (6309)___________________________________________________________________
        opcode_map2.emplace(0x81, Opcode_6x09{ "CMPW"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x91, Opcode_6x09{ "CMPW"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA1, Opcode_6x09{ "CMPW"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB1, Opcode_6x09{ "CMPW"    , { {2} }   , EXTENDED  });
        // CMPY _________________________________________________________________________
        opcode_map2.emplace(0x8C, Opcode_6x09{ "CMPY"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x9C, Opcode_6x09{ "CMPY"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xAC, Opcode_6x09{ "CMPY"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xBC, Opcode_6x09{ "CMPY"    , { {2} }   , EXTENDED  });
        // CMPR (6309)___________________________________________________________________
        opcode_map2.emplace(0x37, Opcode_6x09{ "CMPR"    , { {1} }   , IMMEDIATE });
        // COMD _________________________________________________________________________
        opcode_map2.emplace(0x43, Opcode_6x09{ "COMD"    , {}        , INHERENT });
        // COMW _________________________________________________________________________
        opcode_map2.emplace(0x53, Opcode_6x09{ "COMW"    , {}        , INHERENT });
        // DECD _________________________________________________________________________
        opcode_map2.emplace(0x4A, Opcode_6x09{ "DECD"    , {}        , INHERENT });
        // DECW _________________________________________________________________________
        opcode_map2.emplace(0x5A, Opcode_6x09{ "DECW"    , {}        , INHERENT });
        // EORD (6309)___________________________________________________________________
        opcode_map2.emplace(0x88, Opcode_6x09{ "EORD"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x98, Opcode_6x09{ "EORD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA8, Opcode_6x09{ "EORD"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB8, Opcode_6x09{ "EORD"    , { {2} }   , EXTENDED  });
        // EORR (6309)___________________________________________________________________
        opcode_map2.emplace(0x36, Opcode_6x09{ "EORR"    , { {1} }   , IMMEDIATE });
        // INCD (6309)___________________________________________________________________
        opcode_map2.emplace(0x4C, Opcode_6x09{ "INCD"    , {}        , INHERENT });
        // INCW (6309)___________________________________________________________________
        opcode_map2.emplace(0x5C, Opcode_6x09{ "INCW"    , {}        , INHERENT });
        // LBCC _________________________________________________________________________
        opcode_map2.emplace(0x24, Opcode_6x09{ "LBCC"    , { {2} }   , RELATIVE });
        // LBCS _________________________________________________________________________
        opcode_map2.emplace(0x25, Opcode_6x09{ "LBCS"    , { {2} }   , RELATIVE });
        // LBEQ _________________________________________________________________________
        opcode_map2.emplace(0x27, Opcode_6x09{ "LBEQ"    , { {2} }   , RELATIVE });
        // LBGE _________________________________________________________________________
        opcode_map2.emplace(0x2C, Opcode_6x09{ "LBGE"    , { {2} }   , RELATIVE });
        // LBGT _________________________________________________________________________
        opcode_map2.emplace(0x2E, Opcode_6x09{ "LBGT"    , { {2} }   , RELATIVE });
        // LBHI _________________________________________________________________________
        opcode_map2.emplace(0x22, Opcode_6x09{ "LBHI"    , { {2} }   , RELATIVE });
        // LBHS _________________________________________________________________________
        opcode_map2.emplace(0x24, Opcode_6x09{ "LBHS"    , { {2} }   , RELATIVE });
        // LBLE _________________________________________________________________________
        opcode_map2.emplace(0x2F, Opcode_6x09{ "LBLE"    , { {2} }   , RELATIVE });
        // LBLO _________________________________________________________________________
        opcode_map2.emplace(0x25, Opcode_6x09{ "LBLO"    , { {2} }   , RELATIVE });
        // LBLS _________________________________________________________________________
        opcode_map2.emplace(0x23, Opcode_6x09{ "LBLS"    , { {2} }   , RELATIVE });
        // LBLT _________________________________________________________________________
        opcode_map2.emplace(0x2D, Opcode_6x09{ "LBLT"    , { {2} }   , RELATIVE });
        // LBMI _________________________________________________________________________
        opcode_map2.emplace(0x2B, Opcode_6x09{ "LBMI"    , { {2} }   , RELATIVE });
        // LBNE _________________________________________________________________________
        opcode_map2.emplace(0x26, Opcode_6x09{ "LBNE"    , { {2} }   , RELATIVE });
        // LBPL _________________________________________________________________________
        opcode_map2.emplace(0x2A, Opcode_6x09{ "LBPL"    , { {2} }   , RELATIVE });
        // LBRN _________________________________________________________________________
        opcode_map2.emplace(0x21, Opcode_6x09{ "LBRN"    , { {2} }   , RELATIVE });
        // LBVC _________________________________________________________________________
        opcode_map2.emplace(0x28, Opcode_6x09{ "LBVC"    , { {2} }   , RELATIVE });
        // LBVS _________________________________________________________________________
        opcode_map2.emplace(0x29, Opcode_6x09{ "LBVS"    , { {2} }   , RELATIVE });
        // LDW (6309)____________________________________________________________________
        opcode_map2.emplace(0x86, Opcode_6x09{ "LDW"     , { {1} }   , IMMEDIATE });
        opcode_map2.emplace(0x96, Opcode_6x09{ "LDW"     , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA6, Opcode_6x09{ "LDW"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB6, Opcode_6x09{ "LDW"     , { {2} }   , EXTENDED  });
        // LDQ (6309)____________________________________________________________________
        opcode_map2.emplace(0xDC, Opcode_6x09{ "LDW"     , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xEC, Opcode_6x09{ "LDW"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xFC, Opcode_6x09{ "LDW"     , { {2} }   , EXTENDED  });
        // LSLD (6309)___________________________________________________________________
        opcode_map2.emplace(0x48, Opcode_6x09{ "LSLD"    , {}        , INHERENT });
        // LSRD (6309)___________________________________________________________________
        opcode_map2.emplace(0x44, Opcode_6x09{ "LSRD"    , {}        , INHERENT });
        // LSRW (6309)___________________________________________________________________
        opcode_map2.emplace(0x54, Opcode_6x09{ "LSRW"    , {}        , INHERENT });
        // NEGD _________________________________________________________________________
        opcode_map2.emplace(0x40, Opcode_6x09{ "NEGD"    , {}        , INHERENT });
        // ORD (6309)____________________________________________________________________
        opcode_map2.emplace(0x8A, Opcode_6x09{ "ORD"     , { {1} }   , IMMEDIATE });
        opcode_map2.emplace(0x9A, Opcode_6x09{ "ORD"     , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xAA, Opcode_6x09{ "ORD"    , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xBA, Opcode_6x09{ "ORD"     , { {2} }   , EXTENDED  });
        // ORR (6309)____________________________________________________________________
        opcode_map2.emplace(0x35, Opcode_6x09{ "ORR"     , { {1} }   , IMMEDIATE });
        // PSHSW (6309)__________________________________________________________________
        opcode_map2.emplace(0x38, Opcode_6x09{ "PSHSW"   , {}        , INHERENT });
        // PSHUW (6309)__________________________________________________________________
        opcode_map2.emplace(0x3A, Opcode_6x09{ "PSHUW"   , {}        , INHERENT });
        // PULSW (6309)__________________________________________________________________
        opcode_map2.emplace(0x39, Opcode_6x09{ "PULSW"   , {}        , INHERENT });
        // PULUW (6309)__________________________________________________________________
        opcode_map2.emplace(0x3B, Opcode_6x09{ "PULUW"   , {}        , INHERENT });
        // RORD (6309)___________________________________________________________________
        opcode_map2.emplace(0x46, Opcode_6x09{ "RORD"    , {}        , INHERENT });
        // RORW (6309)___________________________________________________________________
        opcode_map2.emplace(0x56, Opcode_6x09{ "RORW"    , {}        , INHERENT });
        // SBCD (6309)___________________________________________________________________
        opcode_map2.emplace(0x82, Opcode_6x09{ "SBCD"    , { {1} }   , IMMEDIATE });
        opcode_map2.emplace(0x92, Opcode_6x09{ "SBCD"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA2, Opcode_6x09{ "SBCD"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB2, Opcode_6x09{ "SBCD"    , { {2} }   , EXTENDED  });
        // SBCR (6309)___________________________________________________________________
        opcode_map2.emplace(0x33, Opcode_6x09{ "SBCR"    , { {1} }   , IMMEDIATE });
        // STS __________________________________________________________________________
        opcode_map2.emplace(0xDF, Opcode_6x09{ "STS"    , { {1} }    , DIRECT    });
        //opcode_map2.emplace(0xEF, Opcode_6x09{ "STS"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xFF, Opcode_6x09{ "STS"    , { {2} }    , EXTENDED  });
        // STW (6309)____________________________________________________________________
        opcode_map2.emplace(0x97, Opcode_6x09{ "STW"    , { {1} }    , DIRECT    });
        //opcode_map2.emplace(0xA7, Opcode_6x09{ "STW"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB7, Opcode_6x09{ "STW"    , { {2} }    , EXTENDED  });
        // STY __________________________________________________________________________
        opcode_map2.emplace(0x9F, Opcode_6x09{ "STY"    , { {1} }    , DIRECT    });
        //opcode_map2.emplace(0xAF, Opcode_6x09{ "STY"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xBF, Opcode_6x09{ "STY"    , { {2} }    , EXTENDED  });
        // SUBW (6309)___________________________________________________________________
        opcode_map2.emplace(0x80, Opcode_6x09{ "SUBW"    , { {2} }   , IMMEDIATE });
        opcode_map2.emplace(0x90, Opcode_6x09{ "SUBW"    , { {1} }   , DIRECT    });
        //opcode_map2.emplace(0xA0, Opcode_6x09{ "SUBW"   , { {1} }   , INDEXED   });
        opcode_map2.emplace(0xB0, Opcode_6x09{ "SUBW"    , { {2} }   , EXTENDED  });
        // TSTD (6309)___________________________________________________________________
        opcode_map2.emplace(0x4D, Opcode_6x09{ "TSTD"    , {}        , INHERENT  });
        // TSTW (6309)___________________________________________________________________
        opcode_map2.emplace(0x5D, Opcode_6x09{ "TSTW"    , {}        , INHERENT  });
    }

    // Page 3 opcodes (0x11??)
    if( opcode_map3.empty() )
    {
        // ADDE (6309)___________________________________________________________________
        opcode_map3.emplace(0x8B, Opcode_6x09{ "ADDE"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0x9B, Opcode_6x09{ "ADDE"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xAB, Opcode_6x09{ "ADDE"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xBB, Opcode_6x09{ "ADDE"    , { {2} }   , EXTENDED  });
        // ADDF (6309)___________________________________________________________________
        opcode_map3.emplace(0xCB, Opcode_6x09{ "ADDF"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0xDB, Opcode_6x09{ "ADDF"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xEB, Opcode_6x09{ "ADDF"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xFB, Opcode_6x09{ "ADDF"    , { {2} }   , EXTENDED  });
        // *BAND (6309)___________________________________________________________________
        opcode_map3.emplace(0x30, Opcode_6x09{ "BAND"    , { {2} }   , DIRECT    });
        // *BEOR (6309)___________________________________________________________________
        opcode_map3.emplace(0x34, Opcode_6x09{ "BEOR"    , { {2} }   , DIRECT    });
        // *BIAND (6309)__________________________________________________________________
        opcode_map3.emplace(0x31, Opcode_6x09{ "BIAND"   , { {2} }   , DIRECT    });
        // *BIEOR (6309)__________________________________________________________________
        opcode_map3.emplace(0x35, Opcode_6x09{ "BIEOR"   , { {2} }   , DIRECT    });
        // *BIOR (6309)___________________________________________________________________
        opcode_map3.emplace(0x33, Opcode_6x09{ "BIOR"    , { {2} }   , DIRECT    });
        // *BITMD (6309)__________________________________________________________________
        opcode_map3.emplace(0x3C, Opcode_6x09{ "BITMD"   , { {1} }   , IMMEDIATE });
        // *BOR (6309)____________________________________________________________________
        opcode_map3.emplace(0x32, Opcode_6x09{ "BOR"     , { {2} }   , DIRECT    });
        // CLRE (6309)___________________________________________________________________
        opcode_map3.emplace(0x4F, Opcode_6x09{ "CLRE"    , {}        , INHERENT  });
        // CLRF (6309)___________________________________________________________________
        opcode_map3.emplace(0x5F, Opcode_6x09{ "CLRF"    , {}        , INHERENT  });
        // CMPE (6309)___________________________________________________________________
        opcode_map3.emplace(0x81, Opcode_6x09{ "CMPE"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0x91, Opcode_6x09{ "CMPE"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xA1, Opcode_6x09{ "CMPE"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xB1, Opcode_6x09{ "CMPE"    , { {2} }   , EXTENDED  });
        // CMPF (6309)___________________________________________________________________
        opcode_map3.emplace(0xC1, Opcode_6x09{ "CMPF"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0xD1, Opcode_6x09{ "CMPF"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xE1, Opcode_6x09{ "CMPF"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xF1, Opcode_6x09{ "CMPF"    , { {2} }   , EXTENDED  });
        // CMPS _________________________________________________________________________
        opcode_map3.emplace(0x8C, Opcode_6x09{ "CMPS"    , { {2} }   , IMMEDIATE });
        opcode_map3.emplace(0x9C, Opcode_6x09{ "CMPS"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xAC, Opcode_6x09{ "CMPS"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xBC, Opcode_6x09{ "CMPS"    , { {2} }   , EXTENDED  });
        // CMPU _________________________________________________________________________
        opcode_map3.emplace(0x83, Opcode_6x09{ "CMPU"    , { {2} }   , IMMEDIATE });
        opcode_map3.emplace(0x93, Opcode_6x09{ "CMPU"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xA3, Opcode_6x09{ "CMPU"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xB3, Opcode_6x09{ "CMPU"    , { {2} }   , EXTENDED  });
        // COME _________________________________________________________________________
        opcode_map3.emplace(0x43, Opcode_6x09{ "COME"    , {}        , INHERENT });
        // COMF _________________________________________________________________________
        opcode_map3.emplace(0x53, Opcode_6x09{ "COMF"    , {}        , INHERENT });
        // DECE _________________________________________________________________________
        opcode_map3.emplace(0x4A, Opcode_6x09{ "DECE"    , {}        , INHERENT });
        // DECF _________________________________________________________________________
        opcode_map3.emplace(0x5A, Opcode_6x09{ "DECF"    , {}        , INHERENT });
        // DIVD (6309)___________________________________________________________________
        opcode_map3.emplace(0x8D, Opcode_6x09{ "DIVD"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0x9D, Opcode_6x09{ "DIVD"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xAD, Opcode_6x09{ "DIVD"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xBD, Opcode_6x09{ "DIVD"    , { {2} }   , EXTENDED  });
        // DIVQ (6309)___________________________________________________________________
        opcode_map3.emplace(0x8E, Opcode_6x09{ "DIVQ"    , { {2} }   , IMMEDIATE });
        opcode_map3.emplace(0x9E, Opcode_6x09{ "DIVQ"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xAE, Opcode_6x09{ "DIVQ"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xBE, Opcode_6x09{ "DIVQ"    , { {2} }   , EXTENDED  });
        // EIM (6309)____________________________________________________________________
        opcode_map3.emplace(0x05, Opcode_6x09{ "EIM"     , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0x65, Opcode_6x09{ "EIM"     , { {1} }   , INDEXED   });
        opcode_map3.emplace(0x75, Opcode_6x09{ "EIM"     , { {2} }   , EXTENDED  });
        // INCE (6309)___________________________________________________________________
        opcode_map3.emplace(0x4C, Opcode_6x09{ "INCE"    , {}        , INHERENT });
        // INCF (6309)___________________________________________________________________
        opcode_map3.emplace(0x5C, Opcode_6x09{ "INCF"    , {}        , INHERENT });
        // LDE __________________________________________________________________________
        opcode_map3.emplace(0x86, Opcode_6x09{ "LDE"     , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0x96, Opcode_6x09{ "LDE"     , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xA6, Opcode_6x09{ "LDE"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xB6, Opcode_6x09{ "LDE"     , { {2} }   , EXTENDED  });
        // LDF __________________________________________________________________________
        opcode_map3.emplace(0xC6, Opcode_6x09{ "LDF"     , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0xD6, Opcode_6x09{ "LDF"     , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xE6, Opcode_6x09{ "LDF"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xF6, Opcode_6x09{ "LDF"     , { {2} }   , EXTENDED  });
        // LDBT (6309)___________________________________________________________________
        opcode_map3.emplace(0x36, Opcode_6x09{ "LDBT"    , { {2} }   , DIRECT    });
        // LDMD (6309)___________________________________________________________________
        opcode_map3.emplace(0x3D, Opcode_6x09{ "LDMD"    , { {1} }   , IMMEDIATE });
        // MULD (6309)___________________________________________________________________
        opcode_map3.emplace(0x8F, Opcode_6x09{ "MULD"    , { {2} }   , IMMEDIATE });
        opcode_map3.emplace(0x9F, Opcode_6x09{ "MULD"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xAF, Opcode_6x09{ "MUL"    , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xBF, Opcode_6x09{ "MULD"    , { {2} }   , EXTENDED  });
        // STE (6309)____________________________________________________________________
        opcode_map3.emplace(0x97, Opcode_6x09{ "STE"    , { {1} }    , DIRECT    });
        //opcode_map3.emplace(0xA7, Opcode_6x09{ "STE"   , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xB7, Opcode_6x09{ "STE"    , { {2} }    , EXTENDED  });
        // STF (6309)____________________________________________________________________
        opcode_map3.emplace(0xD7, Opcode_6x09{ "STF"    , { {1} }    , DIRECT    });
        //opcode_map3.emplace(0xE7, Opcode_6x09{ "STF"   , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xF7, Opcode_6x09{ "STF"    , { {2} }    , EXTENDED  });
        // STBT (6309)___________________________________________________________________
        opcode_map3.emplace(0x37, Opcode_6x09{ "STBT"   , { {4} }    , DIRECT    });
        // STQ (6309)____________________________________________________________________
        opcode_map3.emplace(0xDD, Opcode_6x09{ "STQ"    , { {1} }    , DIRECT    });
        //opcode_map3.emplace(0xED, Opcode_6x09{ "STQ"   , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xFD, Opcode_6x09{ "STQ"    , { {2} }    , EXTENDED  });
        // SUBE (6309)___________________________________________________________________
        opcode_map3.emplace(0x80, Opcode_6x09{ "SUBE"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0x90, Opcode_6x09{ "SUBE"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xA0, Opcode_6x09{ "SUBE"   , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xB0, Opcode_6x09{ "SUBE"    , { {2} }   , EXTENDED  });
        // SUBF (6309)___________________________________________________________________
        opcode_map3.emplace(0xC0, Opcode_6x09{ "SUBF"    , { {1} }   , IMMEDIATE });
        opcode_map3.emplace(0xD0, Opcode_6x09{ "SUBF"    , { {1} }   , DIRECT    });
        //opcode_map3.emplace(0xE0, Opcode_6x09{ "SUBF"   , { {1} }   , INDEXED   });
        opcode_map3.emplace(0xF0, Opcode_6x09{ "SUBF"    , { {2} }   , EXTENDED  });
        // SUBR (6309)___________________________________________________________________
        opcode_map3.emplace(0x32, Opcode_6x09{ "SUBR"    , { {1} }   , IMMEDIATE });
        // TSTE (6309)___________________________________________________________________
        opcode_map3.emplace(0x4D, Opcode_6x09{ "TSTE"    , {}        , INHERENT  });
        // TSTF (6309)___________________________________________________________________
        opcode_map3.emplace(0x5D, Opcode_6x09{ "TSTF"    , {}        , INHERENT  });
        // *TFM (6309)____________________________________________________________________
        opcode_map3.emplace(0x38, Opcode_6x09{ "TFM"     , { {1} }   , IMMEDIATE });
        // *TFM (6309)____________________________________________________________________
        opcode_map3.emplace(0x39, Opcode_6x09{ "TFM"     , { {1} }   , IMMEDIATE });
        // *TFM (6309)____________________________________________________________________
        opcode_map3.emplace(0x3A, Opcode_6x09{ "TFM"     , { {1} }   , IMMEDIATE });
        // *TFM (6309)____________________________________________________________________
        opcode_map3.emplace(0x3B, Opcode_6x09{ "TFM"     , { {1} }   , IMMEDIATE });
    }

    // * means need for special parameter formatting
    // Add page 2 and 3 opcodes. They are the two-byte opcodes starting with 10 and 11.
    // CMPD,CMPS,CMPU,CMPW(6309),CMPY
}

/*
int main() {
    int hexValue = 0x1A3; // Example hex number

    ss << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << hexValue;

    std::cout << "Formatted hex: " << ss.str() << std::endl;

    return 0;
}
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>


int main() {
    std::vector<std::string> words = {"Apple", "Banana", "Cherry", "Date"};
    int maxWidth = 10;

    std::stringstream ss;
    for (const auto& word : words) {
        ss << std::left << std::setw(maxWidth) << std::setfill(' ') << word << "\n";
    }

    std::string formattedOutput = ss.str();
    std::cout << "Buffered Output:\n" << formattedOutput;

    return 0;
}

 // Reset manipulators by switching back to default mode
 ss << std::dec << std::setfill(' ') << std::setw(0);
*/

void Disassembler_6x09::FormatParameter( unsigned char _opcode, size_t _param, unsigned char _paramSize, Opcode_6x09_Addressing _addressingMode, std::string& _dst, std::string& _dstColors )
{
    std::stringstream ss;
    _dstColors.clear();
    bool isSpecialCase = false;
    // Special cases
    switch( _opcode )
    {
        case 0x1E: // EXG. IMMEDIATE, has only one parameter.
        {
            isSpecialCase = true;
            unsigned char reg1 = _param & 0x0F;
            unsigned char reg2 = (_param & 0xF0) >> 4;
            ss << EXG_TFR_Registers[reg1] << "," << EXG_TFR_Registers[reg2];
            _dstColors.append( EXG_TFR_Registers[reg1].length(), COLOR_REGISTER );
            _dstColors.append( 1, COLOR_TEXT     );
            _dstColors.append( EXG_TFR_Registers[reg2].length(), COLOR_REGISTER );
        }
        break;
        case 0x34: // PSHS, PSHU. IMMEDIATE, only one parameter.
        case 0x36:
        {
            isSpecialCase = true;
            uint8_t mask = 0x01;
            for( uint8_t reg = 0; reg < 8; ++reg )
            {
                if( 0 != (_param & (mask << reg)) )
                {
                    ss << (_opcode == 0x34 ? PSHS_Registers[reg] : PSHU_Registers[reg]) << ",";
                    _dstColors.append( PSHS_Registers[reg].length(), COLOR_REGISTER );
                    _dstColors.append( 1, COLOR_TEXT );
                }
            }

            // Clear last comma
            std::string tmpStr = ss.str();
            if( !tmpStr.empty() )
            {
                tmpStr.pop_back();
                _dstColors.pop_back();
                ss.str(tmpStr);
                ss.clear();
            }
        }
        break;
    }

    if( !isSpecialCase )
    {
        switch(_addressingMode)
        {
            case IMMEDIATE:
            {
                ss << "#$" << std::uppercase << std::setw(_paramSize*2) << std::setfill('0') << std::hex << _param;
                _dstColors.append( 2, COLOR_TEXT );
                _dstColors.append( _paramSize*2, COLOR_NUMBER );
            }
            break;
            case DIRECT:
            {
                ss << "$(DP)" << std::uppercase << std::setw(_paramSize*2) << std::setfill('0') << std::hex << _param;
                _dstColors.append( 2, COLOR_TEXT     );
                _dstColors.append( 2, COLOR_REGISTER );
                _dstColors.append( 1, COLOR_TEXT     );
                _dstColors.append( _paramSize*2, COLOR_NUMBER );
            }
            break;
            case EXTENDED:
            {
                ss << "$" << std::uppercase << std::setw(_paramSize*2) << std::setfill('0') << std::hex << _param;
                _dstColors.append( 1, COLOR_TEXT     );
                _dstColors.append( _paramSize*2, COLOR_NUMBER );
            }
            break;
            case RELATIVE:
            {
                std::string strVal = std::to_string(static_cast<int>(static_cast<int16_t>(_param)));
                ss << strVal;
                _dstColors.append( strVal.length(), COLOR_NUMBER );
            }
            break;
            default: ss << "?>" << std::setw(_paramSize*2) << std::setfill('0') << std::hex << _param; break; 
        }
    }

    _dst = ss.str();
}

size_t Disassembler_6x09::ReadParameter( const std::vector<unsigned char> _data, size_t _pos, unsigned char _paramSize )
{
    if( _paramSize > sizeof(size_t) )
    {
        return 0;
    }

    size_t retVal = 0;

    for( int bytePos = _paramSize - 1; bytePos >= 0; --bytePos )
    {
        retVal |= _data[_pos++] << (bytePos * 8);
    }

    return retVal;
}

void Disassembler_6x09::Disassemble( const std::vector<unsigned char> _data, uint16_t _loadAddress, uint16_t _execAddress, std::string& _dst, std::string& _dstColors )
{
    if( _execAddress < _loadAddress )
    {
        return;
    }

    std::stringstream ss;
    std::string paramBuffer;
    std::string paramColorBuffer;
    size_t paramValue;
    size_t pos = _execAddress - _loadAddress;

    while( pos < _data.size() )
    {
        ss << std::uppercase << std::right << std::setw(4) << std::setfill('0') << std::hex << _loadAddress + pos << ' ';
        _dstColors.append( 5 , COLOR_NUMBER );

        std::map<unsigned char,Opcode_6x09>::const_iterator opcode;
        switch( _data[pos] )
        {
            case OPCODE_PAGE_2: opcode = opcode_map2.find(_data[++pos]); break;
            case OPCODE_PAGE_3: opcode = opcode_map3.find(_data[++pos]); break;
            default:            opcode = opcode_map.find(_data[pos]); break;
        }

        if( opcode != opcode_map.end() && opcode != opcode_map2.end() && opcode != opcode_map3.end() ) 
        {
            ++pos;
            ss << std::left << std::setw(MAX_OPCODE_STR_WIDTH) << std::setfill(' ') << opcode->second.name << std::setw(0);
            _dstColors.append( MAX_OPCODE_STR_WIDTH , COLOR_OPCODE );

            for( auto param : opcode->second.params )
            {
                paramValue = ReadParameter( _data, pos, param.size );
                FormatParameter( opcode->first, paramValue, param.size, opcode->second.addressing, paramBuffer, paramColorBuffer );
                ss << paramBuffer;
                _dstColors.append( paramColorBuffer );
                pos += param.size;
            }

            ss << std::endl;
        }
        else
        {
            ss << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)_data[pos] << " ?" << std::endl;
            _dstColors.append( 2, COLOR_NUMBER );
            _dstColors.append( 2, COLOR_ERROR  );
            ++pos;
        }
        _dstColors.append("\n");
    }

    _dst = ss.str();
}