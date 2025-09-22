#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Page1_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map, EProcessor _processor )
{
	// ABX __________________________________________________________________________
	_map.emplace(0x3A, Opcode_6x09{"ABX" , {     }, INHERENT  });
	// ADC __________________________________________________________________________
	_map.emplace(0x89, Opcode_6x09{"ADCA", { {1} }, IMMEDIATE });
	_map.emplace(0x99, Opcode_6x09{"ADCA", { {1} }, DIRECT    });
	_map.emplace(0xA9, Opcode_6x09{"ADCA", { {1} }, INDEXED   });
	_map.emplace(0xB9, Opcode_6x09{"ADCA", { {2} }, EXTENDED  });
	// ADCB _________________________________________________________________________
	_map.emplace(0xC9, Opcode_6x09{"ADCB", { {1} }, IMMEDIATE });
	_map.emplace(0xD9, Opcode_6x09{"ADCB", { {1} }, DIRECT    });
	_map.emplace(0xE9, Opcode_6x09{"ADCB", { {1} }, INDEXED   });
	_map.emplace(0xF9, Opcode_6x09{"ADCB", { {2} }, EXTENDED  });
	// ADDA _________________________________________________________________________
	_map.emplace(0x8B, Opcode_6x09{"ADDA", { {1} }, IMMEDIATE });
	_map.emplace(0x9B, Opcode_6x09{"ADDA", { {1} }, DIRECT    });
	_map.emplace(0xAB, Opcode_6x09{"ADDA", { {1} }, INDEXED   });
	_map.emplace(0xBB, Opcode_6x09{"ADDA", { {2} }, EXTENDED  });
	// ADDB _________________________________________________________________________
	_map.emplace(0xCB, Opcode_6x09{"ADDB", { {1} }, IMMEDIATE });
	_map.emplace(0xDB, Opcode_6x09{"ADDB", { {1} }, DIRECT    });
	_map.emplace(0xEB, Opcode_6x09{"ADDB", { {1} }, INDEXED   });
	_map.emplace(0xFB, Opcode_6x09{"ADDB", { {2} }, EXTENDED  });
	// ADDD _________________________________________________________________________
	_map.emplace(0xC3, Opcode_6x09{"ADDD", { {2} }, IMMEDIATE });
	_map.emplace(0xD3, Opcode_6x09{"ADDD", { {1} }, DIRECT    });
	_map.emplace(0xE3, Opcode_6x09{"ADDD", { {1} }, INDEXED   });
	_map.emplace(0xF3, Opcode_6x09{"ADDD", { {2} }, EXTENDED  });
	// ANDA _________________________________________________________________________
	_map.emplace(0x84, Opcode_6x09{"ANDA", { {1} }, IMMEDIATE });
	_map.emplace(0x94, Opcode_6x09{"ANDA", { {1} }, DIRECT    });
	_map.emplace(0xA4, Opcode_6x09{"ANDA", { {1} }, INDEXED   });
	_map.emplace(0xB4, Opcode_6x09{"ANDA", { {2} }, EXTENDED  });
	// ANDB _________________________________________________________________________
	_map.emplace(0xC4, Opcode_6x09{"ANDB", { {1} }, IMMEDIATE });
	_map.emplace(0xD4, Opcode_6x09{"ANDB", { {1} }, DIRECT    });
	_map.emplace(0xE4, Opcode_6x09{"ANDB", { {1} }, INDEXED   });
	_map.emplace(0xF4, Opcode_6x09{"ANDB", { {2} }, EXTENDED  });
	// ANDCC ________________________________________________________________________
	_map.emplace(0x1C, Opcode_6x09{"ANDCC",{ {1} }, IMMEDIATE });
	// ASLA _________________________________________________________________________
	_map.emplace(0x48, Opcode_6x09{"ASLA", {     }, INHERENT  });
	// ASLB _________________________________________________________________________
	_map.emplace(0x58, Opcode_6x09{"ASLB", {     }, INHERENT  });
	// ASL __________________________________________________________________________
	_map.emplace(0x08, Opcode_6x09{"ASL",  { {1} }, DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"ASL",  { {1} }, INDEXED   });
	_map.emplace(0x78, Opcode_6x09{"ASL",  { {2} }, EXTENDED  });
	// ASRA _________________________________________________________________________
	_map.emplace(0x47, Opcode_6x09{"ASRA", {     }, INHERENT  });
	// ASRB _________________________________________________________________________
	_map.emplace(0x57, Opcode_6x09{"ASRB", {     }, INHERENT  });
	// ASL __________________________________________________________________________
	_map.emplace(0x07, Opcode_6x09{"ASR",  { {1} }, DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"ASR",  { {1} }, INDEXED   });
	_map.emplace(0x77, Opcode_6x09{"ASR",  { {2} }, EXTENDED  });
	// BCC __________________________________________________________________________
	_map.emplace(0x77, Opcode_6x09{"BCC",  { {1} }, RELATIVE  });
	// BCS __________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"BCS",  { {1} }, RELATIVE  });
	// BEQ __________________________________________________________________________
	_map.emplace(0x27, Opcode_6x09{"BEQ",  { {1} }, RELATIVE  });
	// BGE __________________________________________________________________________
	_map.emplace(0x2C, Opcode_6x09{"BGE",  { {1} }, RELATIVE  });
	// BGT __________________________________________________________________________
	_map.emplace(0x2E, Opcode_6x09{"BGT",  { {1} }, RELATIVE  });
	// BHI __________________________________________________________________________
	_map.emplace(0x22, Opcode_6x09{"BHI",  { {1} }, RELATIVE  });
	// BHS __________________________________________________________________________
	_map.emplace(0x24, Opcode_6x09{"BHS",  { {1} }, RELATIVE  });
	// BITA _________________________________________________________________________
	_map.emplace(0x85, Opcode_6x09{"BITA", { {1} }, IMMEDIATE });
	_map.emplace(0x95, Opcode_6x09{"BITA", { {1} }, DIRECT    });
	_map.emplace(0xA5, Opcode_6x09{"BITA", { {1} }, INDEXED   });
	_map.emplace(0xB5, Opcode_6x09{"BITA", { {2} }, EXTENDED  });
	// BITB _________________________________________________________________________
	_map.emplace(0xC5, Opcode_6x09{"BITB", { {1} }, IMMEDIATE });
	_map.emplace(0xD5, Opcode_6x09{"BITB", { {1} }, DIRECT    });
	_map.emplace(0xE5, Opcode_6x09{"BITB", { {1} }, INDEXED   });
	_map.emplace(0xF5, Opcode_6x09{"BITB", { {2} }, EXTENDED  });
	// BLE __________________________________________________________________________
	_map.emplace(0x2F, Opcode_6x09{"BLE",  { {1} }, RELATIVE  });
	// BLO __________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"BLO",  { {1} }, RELATIVE  });
	// BLS __________________________________________________________________________
	_map.emplace(0x23, Opcode_6x09{"BLS",  { {1} }, RELATIVE  });
	// BLT __________________________________________________________________________
	_map.emplace(0x2D, Opcode_6x09{"BLT",  { {1} }, RELATIVE  });
	// BMI __________________________________________________________________________
	_map.emplace(0x2B, Opcode_6x09{"BMI",  { {1} }, RELATIVE  });
	// BNE __________________________________________________________________________
	_map.emplace(0x26, Opcode_6x09{"BNE",  { {1} }, RELATIVE  });
	// BPL __________________________________________________________________________
	_map.emplace(0x2A, Opcode_6x09{"BPL",  { {1} }, RELATIVE  });
	// BRA __________________________________________________________________________
	_map.emplace(0x20, Opcode_6x09{"BRA",  { {1} }, RELATIVE  });
	// BRN __________________________________________________________________________
	_map.emplace(0x21, Opcode_6x09{"BRN",  { {1} }, RELATIVE  });
	// BSR __________________________________________________________________________
	_map.emplace(0x8D, Opcode_6x09{"BSR",  { {1} }, RELATIVE  });
	// BVC __________________________________________________________________________
	_map.emplace(0x28, Opcode_6x09{"BVC",  { {1} }, RELATIVE  });
	// BVS __________________________________________________________________________
	_map.emplace(0x29, Opcode_6x09{"BVS",  { {1} }, RELATIVE  });
	// CLRA _________________________________________________________________________
	_map.emplace(0x4F, Opcode_6x09{"CLRA", {     }, INHERENT  });
	// CLRB _________________________________________________________________________
	_map.emplace(0x5F, Opcode_6x09{"CLRB", {     }, INHERENT  });
	// CLR __________________________________________________________________________
	_map.emplace(0x0F, Opcode_6x09{"CLR",  { {1} }, DIRECT    });
	_map.emplace(0x6F, Opcode_6x09{"ASR",  { {1} }, INDEXED   });
	_map.emplace(0x7F, Opcode_6x09{"CLR",  { {2} }, EXTENDED  });
	// CMPA _________________________________________________________________________
	_map.emplace(0x81, Opcode_6x09{"CMPA", { {1} }, IMMEDIATE });
	_map.emplace(0x91, Opcode_6x09{"CMPA", { {1} }, DIRECT    });
	_map.emplace(0xA1, Opcode_6x09{"CMPA", { {1} }, INDEXED   });
	_map.emplace(0xB1, Opcode_6x09{"CMPA", { {2} }, EXTENDED  });
	// CMPB _________________________________________________________________________
	_map.emplace(0xC1, Opcode_6x09{"CMPB", { {1} }, IMMEDIATE });
	_map.emplace(0xD1, Opcode_6x09{"CMPB", { {1} }, DIRECT    });
	_map.emplace(0xE1, Opcode_6x09{"CMPB", { {1} }, INDEXED   });
	_map.emplace(0xF1, Opcode_6x09{"CMPB", { {2} }, EXTENDED  });
	// CMPX _________________________________________________________________________
	_map.emplace(0x8C, Opcode_6x09{"CMPX", { {2} }, IMMEDIATE });
	_map.emplace(0x9C, Opcode_6x09{"CMPX", { {2} }, DIRECT    });
	_map.emplace(0xAC, Opcode_6x09{"CMPX", { {2} }, INDEXED   });
	_map.emplace(0xBC, Opcode_6x09{"CMPX", { {2} }, EXTENDED  });
	// COMA _________________________________________________________________________
	_map.emplace(0x43, Opcode_6x09{"COMA", {     }, INHERENT  });
	// COMB _________________________________________________________________________
	_map.emplace(0x53, Opcode_6x09{"COMB", {     }, INHERENT  });
	// COM __________________________________________________________________________
	_map.emplace(0x03, Opcode_6x09{"COM",  { {1} }, DIRECT    });
	_map.emplace(0x63, Opcode_6x09{"COM",  { {1} }, INDEXED   });
	_map.emplace(0x73, Opcode_6x09{"COM",  { {2} }, EXTENDED  });
	// CWAI _________________________________________________________________________
	_map.emplace(0x3C, Opcode_6x09{"CWAI", { {1} }, IMMEDIATE });
	// DAA __________________________________________________________________________
	_map.emplace(0x19, Opcode_6x09{"DAA",  {     }, INHERENT  });
	// DECA _________________________________________________________________________
	_map.emplace(0x4A, Opcode_6x09{"DECA", {     }, INHERENT  });
	// DECB _________________________________________________________________________
	_map.emplace(0x5A, Opcode_6x09{"DECB", {     }, INHERENT  });
	// DEC __________________________________________________________________________
	_map.emplace(0x0A, Opcode_6x09{"DEC",  { {1} }, DIRECT    });
	_map.emplace(0x6A, Opcode_6x09{"DEC",  { {1} }, INDEXED   });
	_map.emplace(0x7A, Opcode_6x09{"DEC",  { {2} }, EXTENDED  });
	// EORA _________________________________________________________________________
	_map.emplace(0x88, Opcode_6x09{"EORA", { {1} }, IMMEDIATE });
	_map.emplace(0x98, Opcode_6x09{"EORA", { {1} }, DIRECT    });
	_map.emplace(0xA8, Opcode_6x09{"EORA", { {1} }, INDEXED   });
	_map.emplace(0xB8, Opcode_6x09{"EORA", { {2} }, EXTENDED  });
	// EORB _________________________________________________________________________
	_map.emplace(0xC8, Opcode_6x09{"EORB", { {1} }, IMMEDIATE });
	_map.emplace(0xD8, Opcode_6x09{"EORB", { {1} }, DIRECT    });
	_map.emplace(0xE8, Opcode_6x09{"EORB", { {1} }, INDEXED   });
	_map.emplace(0xF8, Opcode_6x09{"EORB", { {2} }, EXTENDED  });
	// EXG __________________________________________________________________________
	_map.emplace(0x1E, Opcode_6x09{"EXG",  { {1} }, IMMEDIATE });
	// INCA _________________________________________________________________________
	_map.emplace(0x4C, Opcode_6x09{"INCA", {     }, INHERENT  });
	// INCB _________________________________________________________________________
	_map.emplace(0x5C, Opcode_6x09{"INCB", {     }, INHERENT  });
	// INC __________________________________________________________________________
	_map.emplace(0x0C, Opcode_6x09{"INC",  { {1} }, DIRECT    });
	_map.emplace(0x6C, Opcode_6x09{"INC",  { {1} }, INDEXED   });
	_map.emplace(0x7C, Opcode_6x09{"INC",  { {2} }, EXTENDED  });
	// JMP __________________________________________________________________________
	_map.emplace(0x0E, Opcode_6x09{"JMP",  { {1} }, DIRECT    });
	_map.emplace(0x6E, Opcode_6x09{"JMP",  { {1} }, INDEXED   });
	_map.emplace(0x7E, Opcode_6x09{"JMP",  { {2} }, EXTENDED  });
	// JSR __________________________________________________________________________
	_map.emplace(0x9D, Opcode_6x09{"JSR",  { {1} }, DIRECT    });
	_map.emplace(0xAD, Opcode_6x09{"JSR",  { {1} }, INDEXED   });
	_map.emplace(0xBD, Opcode_6x09{"JSR",  { {2} }, EXTENDED  });
	// LBRA _________________________________________________________________________
	_map.emplace(0x16, Opcode_6x09{"LBRA", { {2} }, RELATIVE  });
	// LBSR _________________________________________________________________________
	_map.emplace(0x17, Opcode_6x09{"LBSR", { {2} }, RELATIVE  });
	// LDA __________________________________________________________________________
	_map.emplace(0x86, Opcode_6x09{"LDA",  { {1} }, IMMEDIATE });
	_map.emplace(0x96, Opcode_6x09{"LDA",  { {1} }, DIRECT    });
	_map.emplace(0xA6, Opcode_6x09{"LDA",  { {1} }, INDEXED   });
	_map.emplace(0xB6, Opcode_6x09{"LDA",  { {2} }, EXTENDED  });
	// LDB __________________________________________________________________________
	_map.emplace(0xC6, Opcode_6x09{"LDB",  { {1} }, IMMEDIATE });
	_map.emplace(0xD6, Opcode_6x09{"LDB",  { {1} }, DIRECT    });
	_map.emplace(0xE6, Opcode_6x09{"LDB",  { {1} }, INDEXED   });
	_map.emplace(0xF6, Opcode_6x09{"LDB",  { {2} }, EXTENDED  });
	// LDD __________________________________________________________________________
	_map.emplace(0xCC, Opcode_6x09{"LDD",  { {2} }, IMMEDIATE });
	_map.emplace(0xDC, Opcode_6x09{"LDD",  { {1} }, DIRECT    });
	_map.emplace(0xEC, Opcode_6x09{"LDD",  { {1} }, INDEXED   });
	_map.emplace(0xFC, Opcode_6x09{"LDD",  { {2} }, EXTENDED  });
	// LDU __________________________________________________________________________
	_map.emplace(0xCE, Opcode_6x09{"LDU",  { {2} }, IMMEDIATE });
	_map.emplace(0xDE, Opcode_6x09{"LDU",  { {1} }, DIRECT    });
	_map.emplace(0xEE, Opcode_6x09{"LDU",  { {1} }, INDEXED   });
	_map.emplace(0xFE, Opcode_6x09{"LDU",  { {2} }, EXTENDED  });
	// LDX __________________________________________________________________________
	_map.emplace(0x8E, Opcode_6x09{"LDX",  { {2} }, IMMEDIATE });
	_map.emplace(0x9E, Opcode_6x09{"LDX",  { {1} }, DIRECT    });
	_map.emplace(0xAE, Opcode_6x09{"LDX",  { {1} }, INDEXED   });
	_map.emplace(0xBE, Opcode_6x09{"LDX",  { {2} }, EXTENDED  });
	// LEAS _________________________________________________________________________
	_map.emplace(0x32, Opcode_6x09{"LEAS", { {1} }, INDEXED   });
	// LEAU _________________________________________________________________________
	_map.emplace(0x33, Opcode_6x09{"LEAU", { {1} }, INDEXED   });
	// LEAX _________________________________________________________________________
	_map.emplace(0x30, Opcode_6x09{"LEAX", { {1} }, INDEXED   });
	// LEAY _________________________________________________________________________
	_map.emplace(0x31, Opcode_6x09{"LEAY", { {1} }, INDEXED   });
	// LSLA _________________________________________________________________________
	_map.emplace(0x48, Opcode_6x09{"LSLA", {     }, INHERENT  });
	// LSLB _________________________________________________________________________
	_map.emplace(0x58, Opcode_6x09{"LSLB", {     }, INHERENT  });
	// LSL __________________________________________________________________________
	_map.emplace(0x08, Opcode_6x09{"LSL",  { {1} }, DIRECT    });
	_map.emplace(0x68, Opcode_6x09{"LSL",  { {1} }, INDEXED   });
	_map.emplace(0x78, Opcode_6x09{"LSL",  { {2} }, EXTENDED  });
	// LSRA _________________________________________________________________________
	_map.emplace(0x44, Opcode_6x09{"LSRA", {     }, INHERENT  });
	// LSRB _________________________________________________________________________
	_map.emplace(0x54, Opcode_6x09{"LSRB", {     }, INHERENT  });
	// LSR __________________________________________________________________________
	_map.emplace(0x04, Opcode_6x09{"LSR",  { {1} }, DIRECT    });
	_map.emplace(0x64, Opcode_6x09{"LSR",  { {1} }, INDEXED   });
	_map.emplace(0x74, Opcode_6x09{"LSR",  { {2} }, EXTENDED  });
	// MUL __________________________________________________________________________
	_map.emplace(0x3D, Opcode_6x09{"MUL",  {     }, INHERENT  });
	// NEGA _________________________________________________________________________
	_map.emplace(0x40, Opcode_6x09{"NEGA", {     }, INHERENT  });
	// NEGB _________________________________________________________________________
	_map.emplace(0x50, Opcode_6x09{"NEGB", {     }, INHERENT  });
	// NEG __________________________________________________________________________
	_map.emplace(0x00, Opcode_6x09{"NEG",  { {1} }, DIRECT    });
	_map.emplace(0x60, Opcode_6x09{"NEG",  { {1} }, INDEXED   });
	_map.emplace(0x70, Opcode_6x09{"NEG",  { {2} }, EXTENDED  });
	// NOP __________________________________________________________________________
	_map.emplace(0x12, Opcode_6x09{"NOP",  {     }, INHERENT  });
	// ORA __________________________________________________________________________
	_map.emplace(0x8A, Opcode_6x09{"ORA",  { {1} }, IMMEDIATE });
	_map.emplace(0x9A, Opcode_6x09{"ORA",  { {1} }, DIRECT    });
	_map.emplace(0xAA, Opcode_6x09{"ORA",  { {1} }, INDEXED   });
	_map.emplace(0xBA, Opcode_6x09{"ORA",  { {2} }, EXTENDED  });
	// ORB __________________________________________________________________________
	_map.emplace(0xCA, Opcode_6x09{"ORB",  { {1} }, IMMEDIATE });
	_map.emplace(0xDA, Opcode_6x09{"ORB",  { {1} }, DIRECT    });
	_map.emplace(0xEA, Opcode_6x09{"ORB",  { {1} }, INDEXED   });
	_map.emplace(0xFA, Opcode_6x09{"ORB",  { {2} }, EXTENDED  });
	// ORCC _________________________________________________________________________
	_map.emplace(0x1A, Opcode_6x09{"ORCC", { {1} }, IMMEDIATE });
	// PSHS _________________________________________________________________________
	_map.emplace(0x34, Opcode_6x09{"PSHS", { {1} }, IMMEDIATE });
	// PSHU _________________________________________________________________________
	_map.emplace(0x36, Opcode_6x09{"PSHU", { {1} }, IMMEDIATE });
	// PULS _________________________________________________________________________
	_map.emplace(0x35, Opcode_6x09{"PULS", { {1} }, IMMEDIATE });
	// PULU _________________________________________________________________________
	_map.emplace(0x37, Opcode_6x09{"PULU", { {1} }, IMMEDIATE });
	// ROLA _________________________________________________________________________
	_map.emplace(0x49, Opcode_6x09{"ROLA", {     }, INHERENT  });
	// ROLB _________________________________________________________________________
	_map.emplace(0x59, Opcode_6x09{"ROLB", {     }, INHERENT  });
	// ROL __________________________________________________________________________
	_map.emplace(0x09, Opcode_6x09{"ROL",  { {1} }, DIRECT    });
	_map.emplace(0x69, Opcode_6x09{"ROL",  { {1} }, INDEXED   });
	_map.emplace(0x79, Opcode_6x09{"ROL",  { {2} }, EXTENDED  });
	// RORA _________________________________________________________________________
	_map.emplace(0x46, Opcode_6x09{"RORA", {     }, INHERENT  });
	// RORB _________________________________________________________________________
	_map.emplace(0x56, Opcode_6x09{"RORB", {     }, INHERENT  });
	// ROR __________________________________________________________________________
	_map.emplace(0x06, Opcode_6x09{"ROR",  { {1} }, DIRECT    });
	_map.emplace(0x66, Opcode_6x09{"ROR",  { {1} }, INDEXED   });
	_map.emplace(0x76, Opcode_6x09{"ROR",  { {2} }, EXTENDED  });
	// RTI __________________________________________________________________________
	_map.emplace(0x3B, Opcode_6x09{"RTI",  {     }, INHERENT  });
	// RTS __________________________________________________________________________
	_map.emplace(0x39, Opcode_6x09{"RTS",  {     }, INHERENT  });
	// SBCA _________________________________________________________________________
	_map.emplace(0x82, Opcode_6x09{"SBCA", { {1} }, IMMEDIATE });
	_map.emplace(0x92, Opcode_6x09{"SBCA", { {1} }, DIRECT    });
	_map.emplace(0xA2, Opcode_6x09{"SBCA", { {1} }, INDEXED   });
	_map.emplace(0xB2, Opcode_6x09{"SBCA", { {2} }, EXTENDED  });
	// SBCB _________________________________________________________________________
	_map.emplace(0xC2, Opcode_6x09{"SBCB", { {1} }, IMMEDIATE });
	_map.emplace(0xD2, Opcode_6x09{"SBCB", { {1} }, DIRECT    });
	_map.emplace(0xE2, Opcode_6x09{"SBCB", { {1} }, INDEXED   });
	_map.emplace(0xF2, Opcode_6x09{"SBCB", { {2} }, EXTENDED  });
	// SEX __________________________________________________________________________
	_map.emplace(0x1D, Opcode_6x09{"SEX",  {     }, INHERENT  });
	// STA __________________________________________________________________________
	_map.emplace(0x97, Opcode_6x09{"STA",  { {1} }, DIRECT    });
	_map.emplace(0xA7, Opcode_6x09{"STA",  { {1} }, INDEXED   });
	_map.emplace(0xB7, Opcode_6x09{"STA",  { {2} }, EXTENDED  });
	// STB __________________________________________________________________________
	_map.emplace(0xD7, Opcode_6x09{"STB",  { {1} }, DIRECT    });
	_map.emplace(0xE7, Opcode_6x09{"STB",  { {1} }, INDEXED   });
	_map.emplace(0xF7, Opcode_6x09{"STB",  { {2} }, EXTENDED  });
	// STD __________________________________________________________________________
	_map.emplace(0xDD, Opcode_6x09{"STD",  { {1} }, DIRECT    });
	_map.emplace(0xED, Opcode_6x09{"STD",  { {1} }, INDEXED   });
	_map.emplace(0xFD, Opcode_6x09{"STD",  { {2} }, EXTENDED  });
	// STU __________________________________________________________________________
	_map.emplace(0xDF, Opcode_6x09{"STU",  { {1} }, DIRECT    });
	_map.emplace(0xEF, Opcode_6x09{"STU",  { {1} }, INDEXED   });
	_map.emplace(0xFF, Opcode_6x09{"STU",  { {2} }, EXTENDED  });
	// STX __________________________________________________________________________
	_map.emplace(0x9F, Opcode_6x09{"STX",  { {1} }, DIRECT    });
	_map.emplace(0xAF, Opcode_6x09{"STX",  { {1} }, INDEXED   });
	_map.emplace(0xBF, Opcode_6x09{"STX",  { {2} }, EXTENDED  });
	// SUBA _________________________________________________________________________
	_map.emplace(0x80, Opcode_6x09{"SUBA", { {1} }, IMMEDIATE });
	_map.emplace(0x90, Opcode_6x09{"SUBA", { {1} }, DIRECT    });
	_map.emplace(0xA0, Opcode_6x09{"SUBA", { {1} }, INDEXED   });
	_map.emplace(0xB0, Opcode_6x09{"SUBA", { {2} }, EXTENDED  });
	// SUBB _________________________________________________________________________
	_map.emplace(0xC0, Opcode_6x09{"SUBB", { {1} }, IMMEDIATE });
	_map.emplace(0xD0, Opcode_6x09{"SUBB", { {1} }, DIRECT    });
	_map.emplace(0xE0, Opcode_6x09{"SUBB", { {1} }, INDEXED   });
	_map.emplace(0xF0, Opcode_6x09{"SUBB", { {2} }, EXTENDED  });
	// SUBD _________________________________________________________________________
	_map.emplace(0x83, Opcode_6x09{"SUBD", { {2} }, IMMEDIATE });
	_map.emplace(0x93, Opcode_6x09{"SUBD", { {1} }, DIRECT    });
	_map.emplace(0xA3, Opcode_6x09{"SUBD", { {1} }, INDEXED   });
	_map.emplace(0xB3, Opcode_6x09{"SUBD", { {2} }, EXTENDED  });
	// SWI __________________________________________________________________________
	_map.emplace(0x3F, Opcode_6x09{"SWI",  {     }, INHERENT  });
	// SYNC _________________________________________________________________________
	_map.emplace(0x13, Opcode_6x09{"SYNC", {     }, INHERENT  });
	// TFR __________________________________________________________________________
	_map.emplace(0x1F, Opcode_6x09{"TFR",  { {1} }, IMMEDIATE });
	// TSTA _________________________________________________________________________
	_map.emplace(0x4D, Opcode_6x09{"TSTA", {     }, INHERENT  });
	// TSTB _________________________________________________________________________
	_map.emplace(0x5D, Opcode_6x09{"TSTB", {     }, INHERENT  });
	// TST __________________________________________________________________________
	_map.emplace(0x0D, Opcode_6x09{"TST",  { {1} }, DIRECT    });
	_map.emplace(0x6D, Opcode_6x09{"TST",  { {1} }, INDEXED   });
	_map.emplace(0x7D, Opcode_6x09{"TST",  { {2} }, EXTENDED  });

	if( _processor == EProcessor::M6309 )
	{
		// AIM (6309)____________________________________________________________________
		_map.emplace(0x02, Opcode_6x09{"AIM",  { {1} }, DIRECT    });
		_map.emplace(0x62, Opcode_6x09{"AIM",  { {1} }, INDEXED   });
		_map.emplace(0x72, Opcode_6x09{"AIM",  { {2} }, EXTENDED  });
		// LDQ (6309)____________________________________________________________________
		_map.emplace(0xCD, Opcode_6x09{"LDQ",  { {4} }, IMMEDIATE });
		// OIM (6309)____________________________________________________________________
		_map.emplace(0x01, Opcode_6x09{"OIM",  { {1} }, DIRECT    });
		_map.emplace(0x61, Opcode_6x09{"OIM",  { {1} }, INDEXED   });
		_map.emplace(0x71, Opcode_6x09{"OIM",  { {2} }, EXTENDED  });
		// SEXW (6309)___________________________________________________________________
		_map.emplace(0x14, Opcode_6x09{"SEXW", {     }, INHERENT  });
		// TIM (6309)____________________________________________________________________
		_map.emplace(0x0B, Opcode_6x09{"TIM",  { {1}, {1} }, DIRECT    });
		_map.emplace(0x6B, Opcode_6x09{"TIM",  { {1}, {1} }, INDEXED   });
		_map.emplace(0x7B, Opcode_6x09{"TIM",  { {1}, {2} }, EXTENDED  });
	}
}