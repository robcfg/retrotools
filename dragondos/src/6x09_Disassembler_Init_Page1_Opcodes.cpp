#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Page1_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map, EProcessor _processor )
{
	// ABX __________________________________________________________________________
	_map.emplace(0x3A, Opcode_6x09{"ABX" , {     }, AM_INHERENT  });
	// ADC __________________________________________________________________________
	_map.emplace(0x89, Opcode_6x09{"ADCA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x99, Opcode_6x09{"ADCA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA9, Opcode_6x09{"ADCA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB9, Opcode_6x09{"ADCA", { {2} }, AM_EXTENDED  });
	// ADCB _________________________________________________________________________
	_map.emplace(0xC9, Opcode_6x09{"ADCB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD9, Opcode_6x09{"ADCB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE9, Opcode_6x09{"ADCB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF9, Opcode_6x09{"ADCB", { {2} }, AM_EXTENDED  });
	// ADDA _________________________________________________________________________
	_map.emplace(0x8B, Opcode_6x09{"ADDA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x9B, Opcode_6x09{"ADDA", { {1} }, AM_DIRECT    });
	_map.emplace(0xAB, Opcode_6x09{"ADDA", { {1} }, AM_INDEXED   });
	_map.emplace(0xBB, Opcode_6x09{"ADDA", { {2} }, AM_EXTENDED  });
	// ADDB _________________________________________________________________________
	_map.emplace(0xCB, Opcode_6x09{"ADDB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xDB, Opcode_6x09{"ADDB", { {1} }, AM_DIRECT    });
	_map.emplace(0xEB, Opcode_6x09{"ADDB", { {1} }, AM_INDEXED   });
	_map.emplace(0xFB, Opcode_6x09{"ADDB", { {2} }, AM_EXTENDED  });
	// ADDD _________________________________________________________________________
	_map.emplace(0xC3, Opcode_6x09{"ADDD", { {2} }, AM_IMMEDIATE });
	_map.emplace(0xD3, Opcode_6x09{"ADDD", { {1} }, AM_DIRECT    });
	_map.emplace(0xE3, Opcode_6x09{"ADDD", { {1} }, AM_INDEXED   });
	_map.emplace(0xF3, Opcode_6x09{"ADDD", { {2} }, AM_EXTENDED  });
	// ANDA _________________________________________________________________________
	_map.emplace(0x84, Opcode_6x09{"ANDA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x94, Opcode_6x09{"ANDA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA4, Opcode_6x09{"ANDA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB4, Opcode_6x09{"ANDA", { {2} }, AM_EXTENDED  });
	// ANDB _________________________________________________________________________
	_map.emplace(0xC4, Opcode_6x09{"ANDB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD4, Opcode_6x09{"ANDB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE4, Opcode_6x09{"ANDB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF4, Opcode_6x09{"ANDB", { {2} }, AM_EXTENDED  });
	// ANDCC ________________________________________________________________________
	_map.emplace(0x1C, Opcode_6x09{"ANDCC",{ {1} }, AM_IMMEDIATE });
	// ASLA _________________________________________________________________________
	_map.emplace(0x48, Opcode_6x09{"ASLA", {     }, AM_INHERENT  });
	// ASLB _________________________________________________________________________
	_map.emplace(0x58, Opcode_6x09{"ASLB", {     }, AM_INHERENT  });
	// ASL __________________________________________________________________________
	_map.emplace(0x08, Opcode_6x09{"ASL",  { {1} }, AM_DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"ASL",  { {1} }, AM_INDEXED   });
	_map.emplace(0x78, Opcode_6x09{"ASL",  { {2} }, AM_EXTENDED  });
	// ASRA _________________________________________________________________________
	_map.emplace(0x47, Opcode_6x09{"ASRA", {     }, AM_INHERENT  });
	// ASRB _________________________________________________________________________
	_map.emplace(0x57, Opcode_6x09{"ASRB", {     }, AM_INHERENT  });
	// ASL __________________________________________________________________________
	_map.emplace(0x07, Opcode_6x09{"ASR",  { {1} }, AM_DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"ASR",  { {1} }, AM_INDEXED   });
	_map.emplace(0x77, Opcode_6x09{"ASR",  { {2} }, AM_EXTENDED  });
	// BCC __________________________________________________________________________
	_map.emplace(0x77, Opcode_6x09{"BCC",  { {1} }, AM_RELATIVE  });
	// BCS __________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"BCS",  { {1} }, AM_RELATIVE  });
	// BEQ __________________________________________________________________________
	_map.emplace(0x27, Opcode_6x09{"BEQ",  { {1} }, AM_RELATIVE  });
	// BGE __________________________________________________________________________
	_map.emplace(0x2C, Opcode_6x09{"BGE",  { {1} }, AM_RELATIVE  });
	// BGT __________________________________________________________________________
	_map.emplace(0x2E, Opcode_6x09{"BGT",  { {1} }, AM_RELATIVE  });
	// BHI __________________________________________________________________________
	_map.emplace(0x22, Opcode_6x09{"BHI",  { {1} }, AM_RELATIVE  });
	// BHS __________________________________________________________________________
	_map.emplace(0x24, Opcode_6x09{"BHS",  { {1} }, AM_RELATIVE  });
	// BITA _________________________________________________________________________
	_map.emplace(0x85, Opcode_6x09{"BITA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x95, Opcode_6x09{"BITA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA5, Opcode_6x09{"BITA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB5, Opcode_6x09{"BITA", { {2} }, AM_EXTENDED  });
	// BITB _________________________________________________________________________
	_map.emplace(0xC5, Opcode_6x09{"BITB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD5, Opcode_6x09{"BITB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE5, Opcode_6x09{"BITB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF5, Opcode_6x09{"BITB", { {2} }, AM_EXTENDED  });
	// BLE __________________________________________________________________________
	_map.emplace(0x2F, Opcode_6x09{"BLE",  { {1} }, AM_RELATIVE  });
	// BLO __________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"BLO",  { {1} }, AM_RELATIVE  });
	// BLS __________________________________________________________________________
	_map.emplace(0x23, Opcode_6x09{"BLS",  { {1} }, AM_RELATIVE  });
	// BLT __________________________________________________________________________
	_map.emplace(0x2D, Opcode_6x09{"BLT",  { {1} }, AM_RELATIVE  });
	// BMI __________________________________________________________________________
	_map.emplace(0x2B, Opcode_6x09{"BMI",  { {1} }, AM_RELATIVE  });
	// BNE __________________________________________________________________________
	_map.emplace(0x26, Opcode_6x09{"BNE",  { {1} }, AM_RELATIVE  });
	// BPL __________________________________________________________________________
	_map.emplace(0x2A, Opcode_6x09{"BPL",  { {1} }, AM_RELATIVE  });
	// BRA __________________________________________________________________________
	_map.emplace(0x20, Opcode_6x09{"BRA",  { {1} }, AM_RELATIVE  });
	// BRN __________________________________________________________________________
	_map.emplace(0x21, Opcode_6x09{"BRN",  { {1} }, AM_RELATIVE  });
	// BSR __________________________________________________________________________
	_map.emplace(0x8D, Opcode_6x09{"BSR",  { {1} }, AM_RELATIVE  });
	// BVC __________________________________________________________________________
	_map.emplace(0x28, Opcode_6x09{"BVC",  { {1} }, AM_RELATIVE  });
	// BVS __________________________________________________________________________
	_map.emplace(0x29, Opcode_6x09{"BVS",  { {1} }, AM_RELATIVE  });
	// CLRA _________________________________________________________________________
	_map.emplace(0x4F, Opcode_6x09{"CLRA", {     }, AM_INHERENT  });
	// CLRB _________________________________________________________________________
	_map.emplace(0x5F, Opcode_6x09{"CLRB", {     }, AM_INHERENT  });
	// CLR __________________________________________________________________________
	_map.emplace(0x0F, Opcode_6x09{"CLR",  { {1} }, AM_DIRECT    });
	_map.emplace(0x6F, Opcode_6x09{"ASR",  { {1} }, AM_INDEXED   });
	_map.emplace(0x7F, Opcode_6x09{"CLR",  { {2} }, AM_EXTENDED  });
	// CMPA _________________________________________________________________________
	_map.emplace(0x81, Opcode_6x09{"CMPA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x91, Opcode_6x09{"CMPA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA1, Opcode_6x09{"CMPA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB1, Opcode_6x09{"CMPA", { {2} }, AM_EXTENDED  });
	// CMPB _________________________________________________________________________
	_map.emplace(0xC1, Opcode_6x09{"CMPB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD1, Opcode_6x09{"CMPB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE1, Opcode_6x09{"CMPB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF1, Opcode_6x09{"CMPB", { {2} }, AM_EXTENDED  });
	// CMPX _________________________________________________________________________
	_map.emplace(0x8C, Opcode_6x09{"CMPX", { {2} }, AM_IMMEDIATE });
	_map.emplace(0x9C, Opcode_6x09{"CMPX", { {2} }, AM_DIRECT    });
	_map.emplace(0xAC, Opcode_6x09{"CMPX", { {2} }, AM_INDEXED   });
	_map.emplace(0xBC, Opcode_6x09{"CMPX", { {2} }, AM_EXTENDED  });
	// COMA _________________________________________________________________________
	_map.emplace(0x43, Opcode_6x09{"COMA", {     }, AM_INHERENT  });
	// COMB _________________________________________________________________________
	_map.emplace(0x53, Opcode_6x09{"COMB", {     }, AM_INHERENT  });
	// COM __________________________________________________________________________
	_map.emplace(0x03, Opcode_6x09{"COM",  { {1} }, AM_DIRECT    });
	_map.emplace(0x63, Opcode_6x09{"COM",  { {1} }, AM_INDEXED   });
	_map.emplace(0x73, Opcode_6x09{"COM",  { {2} }, AM_EXTENDED  });
	// CWAI _________________________________________________________________________
	_map.emplace(0x3C, Opcode_6x09{"CWAI", { {1} }, AM_IMMEDIATE });
	// DAA __________________________________________________________________________
	_map.emplace(0x19, Opcode_6x09{"DAA",  {     }, AM_INHERENT  });
	// DECA _________________________________________________________________________
	_map.emplace(0x4A, Opcode_6x09{"DECA", {     }, AM_INHERENT  });
	// DECB _________________________________________________________________________
	_map.emplace(0x5A, Opcode_6x09{"DECB", {     }, AM_INHERENT  });
	// DEC __________________________________________________________________________
	_map.emplace(0x0A, Opcode_6x09{"DEC",  { {1} }, AM_DIRECT    });
	_map.emplace(0x6A, Opcode_6x09{"DEC",  { {1} }, AM_INDEXED   });
	_map.emplace(0x7A, Opcode_6x09{"DEC",  { {2} }, AM_EXTENDED  });
	// EORA _________________________________________________________________________
	_map.emplace(0x88, Opcode_6x09{"EORA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x98, Opcode_6x09{"EORA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA8, Opcode_6x09{"EORA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB8, Opcode_6x09{"EORA", { {2} }, AM_EXTENDED  });
	// EORB _________________________________________________________________________
	_map.emplace(0xC8, Opcode_6x09{"EORB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD8, Opcode_6x09{"EORB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE8, Opcode_6x09{"EORB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF8, Opcode_6x09{"EORB", { {2} }, AM_EXTENDED  });
	// EXG __________________________________________________________________________
	_map.emplace(0x1E, Opcode_6x09{"EXG",  { {1} }, AM_IMMEDIATE });
	// INCA _________________________________________________________________________
	_map.emplace(0x4C, Opcode_6x09{"INCA", {     }, AM_INHERENT  });
	// INCB _________________________________________________________________________
	_map.emplace(0x5C, Opcode_6x09{"INCB", {     }, AM_INHERENT  });
	// INC __________________________________________________________________________
	_map.emplace(0x0C, Opcode_6x09{"INC",  { {1} }, AM_DIRECT    });
	_map.emplace(0x6C, Opcode_6x09{"INC",  { {1} }, AM_INDEXED   });
	_map.emplace(0x7C, Opcode_6x09{"INC",  { {2} }, AM_EXTENDED  });
	// JMP __________________________________________________________________________
	_map.emplace(0x0E, Opcode_6x09{"JMP",  { {1} }, AM_DIRECT    });
	_map.emplace(0x6E, Opcode_6x09{"JMP",  { {1} }, AM_INDEXED   });
	_map.emplace(0x7E, Opcode_6x09{"JMP",  { {2} }, AM_EXTENDED  });
	// JSR __________________________________________________________________________
	_map.emplace(0x9D, Opcode_6x09{"JSR",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAD, Opcode_6x09{"JSR",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBD, Opcode_6x09{"JSR",  { {2} }, AM_EXTENDED  });
	// LBRA _________________________________________________________________________
	_map.emplace(0x16, Opcode_6x09{"LBRA", { {2} }, AM_RELATIVE  });
	// LBSR _________________________________________________________________________
	_map.emplace(0x17, Opcode_6x09{"LBSR", { {2} }, AM_RELATIVE  });
	// LDA __________________________________________________________________________
	_map.emplace(0x86, Opcode_6x09{"LDA",  { {1} }, AM_IMMEDIATE });
	_map.emplace(0x96, Opcode_6x09{"LDA",  { {1} }, AM_DIRECT    });
	_map.emplace(0xA6, Opcode_6x09{"LDA",  { {1} }, AM_INDEXED   });
	_map.emplace(0xB6, Opcode_6x09{"LDA",  { {2} }, AM_EXTENDED  });
	// LDB __________________________________________________________________________
	_map.emplace(0xC6, Opcode_6x09{"LDB",  { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD6, Opcode_6x09{"LDB",  { {1} }, AM_DIRECT    });
	_map.emplace(0xE6, Opcode_6x09{"LDB",  { {1} }, AM_INDEXED   });
	_map.emplace(0xF6, Opcode_6x09{"LDB",  { {2} }, AM_EXTENDED  });
	// LDD __________________________________________________________________________
	_map.emplace(0xCC, Opcode_6x09{"LDD",  { {2} }, AM_IMMEDIATE });
	_map.emplace(0xDC, Opcode_6x09{"LDD",  { {1} }, AM_DIRECT    });
	_map.emplace(0xEC, Opcode_6x09{"LDD",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFC, Opcode_6x09{"LDD",  { {2} }, AM_EXTENDED  });
	// LDU __________________________________________________________________________
	_map.emplace(0xCE, Opcode_6x09{"LDU",  { {2} }, AM_IMMEDIATE });
	_map.emplace(0xDE, Opcode_6x09{"LDU",  { {1} }, AM_DIRECT    });
	_map.emplace(0xEE, Opcode_6x09{"LDU",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFE, Opcode_6x09{"LDU",  { {2} }, AM_EXTENDED  });
	// LDX __________________________________________________________________________
	_map.emplace(0x8E, Opcode_6x09{"LDX",  { {2} }, AM_IMMEDIATE });
	_map.emplace(0x9E, Opcode_6x09{"LDX",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAE, Opcode_6x09{"LDX",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBE, Opcode_6x09{"LDX",  { {2} }, AM_EXTENDED  });
	// LEAS _________________________________________________________________________
	_map.emplace(0x32, Opcode_6x09{"LEAS", { {1} }, AM_INDEXED   });
	// LEAU _________________________________________________________________________
	_map.emplace(0x33, Opcode_6x09{"LEAU", { {1} }, AM_INDEXED   });
	// LEAX _________________________________________________________________________
	_map.emplace(0x30, Opcode_6x09{"LEAX", { {1} }, AM_INDEXED   });
	// LEAY _________________________________________________________________________
	_map.emplace(0x31, Opcode_6x09{"LEAY", { {1} }, AM_INDEXED   });
	// LSLA _________________________________________________________________________
	_map.emplace(0x48, Opcode_6x09{"LSLA", {     }, AM_INHERENT  });
	// LSLB _________________________________________________________________________
	_map.emplace(0x58, Opcode_6x09{"LSLB", {     }, AM_INHERENT  });
	// LSL __________________________________________________________________________
	_map.emplace(0x08, Opcode_6x09{"LSL",  { {1} }, AM_DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"LSL",  { {1} }, AM_INDEXED   });
	_map.emplace(0x78, Opcode_6x09{"LSL",  { {2} }, AM_EXTENDED  });
	// LSRA _________________________________________________________________________
	_map.emplace(0x44, Opcode_6x09{"LSRA", {     }, AM_INHERENT  });
	// LSRB _________________________________________________________________________
	_map.emplace(0x54, Opcode_6x09{"LSRB", {     }, AM_INHERENT  });
	// LSR __________________________________________________________________________
	_map.emplace(0x04, Opcode_6x09{"LSR",  { {1} }, AM_DIRECT    });
	_map.emplace(0x64, Opcode_6x09{"LSR",  { {1} }, AM_INDEXED   });
	_map.emplace(0x74, Opcode_6x09{"LSR",  { {2} }, AM_EXTENDED  });
	// MUL __________________________________________________________________________
	_map.emplace(0x3D, Opcode_6x09{"MUL",  {     }, AM_INHERENT  });
	// NEGA _________________________________________________________________________
	_map.emplace(0x40, Opcode_6x09{"NEGA", {     }, AM_INHERENT  });
	// NEGB _________________________________________________________________________
	_map.emplace(0x50, Opcode_6x09{"NEGB", {     }, AM_INHERENT  });
	// NEG __________________________________________________________________________
	_map.emplace(0x00, Opcode_6x09{"NEG",  { {1} }, AM_DIRECT    });
	_map.emplace(0x60, Opcode_6x09{"NEG",  { {1} }, AM_INDEXED   });
	_map.emplace(0x70, Opcode_6x09{"NEG",  { {2} }, AM_EXTENDED  });
	// NOP __________________________________________________________________________
	_map.emplace(0x12, Opcode_6x09{"NOP",  {     }, AM_INHERENT  });
	// ORA __________________________________________________________________________
	_map.emplace(0x8A, Opcode_6x09{"ORA",  { {1} }, AM_IMMEDIATE });
	_map.emplace(0x9A, Opcode_6x09{"ORA",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAA, Opcode_6x09{"ORA",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBA, Opcode_6x09{"ORA",  { {2} }, AM_EXTENDED  });
	// ORB __________________________________________________________________________
	_map.emplace(0xCA, Opcode_6x09{"ORB",  { {1} }, AM_IMMEDIATE });
	_map.emplace(0xDA, Opcode_6x09{"ORB",  { {1} }, AM_DIRECT    });
	_map.emplace(0xEA, Opcode_6x09{"ORB",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFA, Opcode_6x09{"ORB",  { {2} }, AM_EXTENDED  });
	// ORCC _________________________________________________________________________
	_map.emplace(0x1A, Opcode_6x09{"ORCC", { {1} }, AM_IMMEDIATE });
	// PSHS _________________________________________________________________________
	_map.emplace(0x34, Opcode_6x09{"PSHS", { {1} }, AM_IMMEDIATE });
	// PSHU _________________________________________________________________________
	_map.emplace(0x36, Opcode_6x09{"PSHU", { {1} }, AM_IMMEDIATE });
	// PULS _________________________________________________________________________
	_map.emplace(0x35, Opcode_6x09{"PULS", { {1} }, AM_IMMEDIATE });
	// PULU _________________________________________________________________________
	_map.emplace(0x37, Opcode_6x09{"PULU", { {1} }, AM_IMMEDIATE });
	// ROLA _________________________________________________________________________
	_map.emplace(0x49, Opcode_6x09{"ROLA", {     }, AM_INHERENT  });
	// ROLB _________________________________________________________________________
	_map.emplace(0x59, Opcode_6x09{"ROLB", {     }, AM_INHERENT  });
	// ROL __________________________________________________________________________
	_map.emplace(0x09, Opcode_6x09{"ROL",  { {1} }, AM_DIRECT    });
	_map.emplace(0x69, Opcode_6x09{"ROL",  { {1} }, AM_INDEXED   });
	_map.emplace(0x79, Opcode_6x09{"ROL",  { {2} }, AM_EXTENDED  });
	// RORA _________________________________________________________________________
	_map.emplace(0x46, Opcode_6x09{"RORA", {     }, AM_INHERENT  });
	// RORB _________________________________________________________________________
	_map.emplace(0x56, Opcode_6x09{"RORB", {     }, AM_INHERENT  });
	// ROR __________________________________________________________________________
	_map.emplace(0x06, Opcode_6x09{"ROR",  { {1} }, AM_DIRECT    });
	_map.emplace(0x66, Opcode_6x09{"ROR",  { {1} }, AM_INDEXED   });
	_map.emplace(0x76, Opcode_6x09{"ROR",  { {2} }, AM_EXTENDED  });
	// RTI __________________________________________________________________________
	_map.emplace(0x3B, Opcode_6x09{"RTI",  {     }, AM_INHERENT  });
	// RTS __________________________________________________________________________
	_map.emplace(0x39, Opcode_6x09{"RTS",  {     }, AM_INHERENT  });
	// SBCA _________________________________________________________________________
	_map.emplace(0x82, Opcode_6x09{"SBCA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x92, Opcode_6x09{"SBCA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA2, Opcode_6x09{"SBCA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB2, Opcode_6x09{"SBCA", { {2} }, AM_EXTENDED  });
	// SBCB _________________________________________________________________________
	_map.emplace(0xC2, Opcode_6x09{"SBCB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD2, Opcode_6x09{"SBCB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE2, Opcode_6x09{"SBCB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF2, Opcode_6x09{"SBCB", { {2} }, AM_EXTENDED  });
	// SEX __________________________________________________________________________
	_map.emplace(0x1D, Opcode_6x09{"SEX",  {     }, AM_INHERENT  });
	// STA __________________________________________________________________________
	_map.emplace(0x97, Opcode_6x09{"STA",  { {1} }, AM_DIRECT    });
	_map.emplace(0xA7, Opcode_6x09{"STA",  { {1} }, AM_INDEXED   });
	_map.emplace(0xB7, Opcode_6x09{"STA",  { {2} }, AM_EXTENDED  });
	// STB __________________________________________________________________________
	_map.emplace(0xD7, Opcode_6x09{"STB",  { {1} }, AM_DIRECT    });
	_map.emplace(0xE7, Opcode_6x09{"STB",  { {1} }, AM_INDEXED   });
	_map.emplace(0xF7, Opcode_6x09{"STB",  { {2} }, AM_EXTENDED  });
	// STD __________________________________________________________________________
	_map.emplace(0xDD, Opcode_6x09{"STD",  { {1} }, AM_DIRECT    });
	_map.emplace(0xED, Opcode_6x09{"STD",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFD, Opcode_6x09{"STD",  { {2} }, AM_EXTENDED  });
	// STU __________________________________________________________________________
	_map.emplace(0xDF, Opcode_6x09{"STU",  { {1} }, AM_DIRECT    });
	_map.emplace(0xEF, Opcode_6x09{"STU",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFF, Opcode_6x09{"STU",  { {2} }, AM_EXTENDED  });
	// STX __________________________________________________________________________
	_map.emplace(0x9F, Opcode_6x09{"STX",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAF, Opcode_6x09{"STX",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBF, Opcode_6x09{"STX",  { {2} }, AM_EXTENDED  });
	// SUBA _________________________________________________________________________
	_map.emplace(0x80, Opcode_6x09{"SUBA", { {1} }, AM_IMMEDIATE });
	_map.emplace(0x90, Opcode_6x09{"SUBA", { {1} }, AM_DIRECT    });
	_map.emplace(0xA0, Opcode_6x09{"SUBA", { {1} }, AM_INDEXED   });
	_map.emplace(0xB0, Opcode_6x09{"SUBA", { {2} }, AM_EXTENDED  });
	// SUBB _________________________________________________________________________
	_map.emplace(0xC0, Opcode_6x09{"SUBB", { {1} }, AM_IMMEDIATE });
	_map.emplace(0xD0, Opcode_6x09{"SUBB", { {1} }, AM_DIRECT    });
	_map.emplace(0xE0, Opcode_6x09{"SUBB", { {1} }, AM_INDEXED   });
	_map.emplace(0xF0, Opcode_6x09{"SUBB", { {2} }, AM_EXTENDED  });
	// SUBD _________________________________________________________________________
	_map.emplace(0x83, Opcode_6x09{"SUBD", { {2} }, AM_IMMEDIATE });
	_map.emplace(0x93, Opcode_6x09{"SUBD", { {1} }, AM_DIRECT    });
	_map.emplace(0xA3, Opcode_6x09{"SUBD", { {1} }, AM_INDEXED   });
	_map.emplace(0xB3, Opcode_6x09{"SUBD", { {2} }, AM_EXTENDED  });
	// SWI __________________________________________________________________________
	_map.emplace(0x3F, Opcode_6x09{"SWI",  {     }, AM_INHERENT  });
	// SYNC _________________________________________________________________________
	_map.emplace(0x13, Opcode_6x09{"SYNC", {     }, AM_INHERENT  });
	// TFR __________________________________________________________________________
	_map.emplace(0x1F, Opcode_6x09{"TFR",  { {1} }, AM_IMMEDIATE });
	// TSTA _________________________________________________________________________
	_map.emplace(0x4D, Opcode_6x09{"TSTA", {     }, AM_INHERENT  });
	// TSTB _________________________________________________________________________
	_map.emplace(0x5D, Opcode_6x09{"TSTB", {     }, AM_INHERENT  });
	// TST __________________________________________________________________________
	_map.emplace(0x0D, Opcode_6x09{"TST",  { {1} }, AM_DIRECT    });
	_map.emplace(0x6D, Opcode_6x09{"TST",  { {1} }, AM_INDEXED   });
	_map.emplace(0x7D, Opcode_6x09{"TST",  { {2} }, AM_EXTENDED  });

	if( _processor == EProcessor::M6309 )
	{
		// AIM (6309)____________________________________________________________________
		_map.emplace(0x02, Opcode_6x09{"AIM",  { {1} }, AM_DIRECT    });
		_map.emplace(0x62, Opcode_6x09{"AIM",  { {1} }, AM_INDEXED   });
		_map.emplace(0x72, Opcode_6x09{"AIM",  { {2} }, AM_EXTENDED  });
		// LDQ (6309)____________________________________________________________________
		_map.emplace(0xCD, Opcode_6x09{"LDQ",  { {4} }, AM_IMMEDIATE });
		// OIM (6309)____________________________________________________________________
		_map.emplace(0x01, Opcode_6x09{"OIM",  { {1} }, AM_DIRECT    });
		_map.emplace(0x61, Opcode_6x09{"OIM",  { {1} }, AM_INDEXED   });
		_map.emplace(0x71, Opcode_6x09{"OIM",  { {2} }, AM_EXTENDED  });
		// SEXW (6309)___________________________________________________________________
		_map.emplace(0x14, Opcode_6x09{"SEXW", {     }, AM_INHERENT  });
		// TIM (6309)____________________________________________________________________
		_map.emplace(0x0B, Opcode_6x09{"TIM",  { {1}, {1} }, AM_DIRECT    });
		_map.emplace(0x6B, Opcode_6x09{"TIM",  { {1}, {1} }, AM_INDEXED   });
		_map.emplace(0x7B, Opcode_6x09{"TIM",  { {1}, {2} }, AM_EXTENDED  });
	}
}