#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Page2_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map, EProcessor _processor )
{
	// CMPD _________________________________________________________________________
	_map.emplace(0x83, Opcode_6x09{"CMPD", { {2} }, AM_IMMEDIATE });
	_map.emplace(0x93, Opcode_6x09{"CMPD", { {1} }, AM_DIRECT    });
	_map.emplace(0xA3, Opcode_6x09{"CMPD", { {1} }, AM_INDEXED   });
	_map.emplace(0xB3, Opcode_6x09{"CMPD", { {2} }, AM_EXTENDED  });
	// CMPY _________________________________________________________________________
	_map.emplace(0x8C, Opcode_6x09{"CMPY", { {2} }, AM_IMMEDIATE });
	_map.emplace(0x9C, Opcode_6x09{"CMPY", { {1} }, AM_DIRECT    });
	_map.emplace(0xAC, Opcode_6x09{"CMPY", { {1} }, AM_INDEXED   });
	_map.emplace(0xBC, Opcode_6x09{"CMPY", { {2} }, AM_EXTENDED  });
	// COMD _________________________________________________________________________
	_map.emplace(0x43, Opcode_6x09{"COMD", {     }, AM_INHERENT  });
	// COMW _________________________________________________________________________
	_map.emplace(0x53, Opcode_6x09{"COMW", {     }, AM_INHERENT  });
	// DECD _________________________________________________________________________
	_map.emplace(0x4A, Opcode_6x09{"DECD", {     }, AM_INHERENT  });
	// DECW _________________________________________________________________________
	_map.emplace(0x5A, Opcode_6x09{"DECW", {     }, AM_INHERENT  });
	// LBCC _________________________________________________________________________
	_map.emplace(0x24, Opcode_6x09{"LBCC", { {2} }, AM_RELATIVE  });
	// LBCS _________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"LBCS", { {2} }, AM_RELATIVE  });
	// LBEQ _________________________________________________________________________
	_map.emplace(0x27, Opcode_6x09{"LBEQ", { {2} }, AM_RELATIVE  });
	// LBGE _________________________________________________________________________
	_map.emplace(0x2C, Opcode_6x09{"LBGE", { {2} }, AM_RELATIVE  });
	// LBGT _________________________________________________________________________
	_map.emplace(0x2E, Opcode_6x09{"LBGT", { {2} }, AM_RELATIVE  });
	// LBHI _________________________________________________________________________
	_map.emplace(0x22, Opcode_6x09{"LBHI", { {2} }, AM_RELATIVE  });
	// LBHS _________________________________________________________________________
	_map.emplace(0x24, Opcode_6x09{"LBHS", { {2} }, AM_RELATIVE  });
	// LBLE _________________________________________________________________________
	_map.emplace(0x2F, Opcode_6x09{"LBLE", { {2} }, AM_RELATIVE  });
	// LBLO _________________________________________________________________________
	_map.emplace(0x25, Opcode_6x09{"LBLO", { {2} }, AM_RELATIVE  });
	// LBLS _________________________________________________________________________
	_map.emplace(0x23, Opcode_6x09{"LBLS", { {2} }, AM_RELATIVE  });
	// LBLT _________________________________________________________________________
	_map.emplace(0x2D, Opcode_6x09{"LBLT", { {2} }, AM_RELATIVE  });
	// LBMI _________________________________________________________________________
	_map.emplace(0x2B, Opcode_6x09{"LBMI", { {2} }, AM_RELATIVE  });
	// LBNE _________________________________________________________________________
	_map.emplace(0x26, Opcode_6x09{"LBNE", { {2} }, AM_RELATIVE  });
	// LBPL _________________________________________________________________________
	_map.emplace(0x2A, Opcode_6x09{"LBPL", { {2} }, AM_RELATIVE  });
	// LBRN _________________________________________________________________________
	_map.emplace(0x21, Opcode_6x09{"LBRN", { {2} }, AM_RELATIVE  });
	// LBVC _________________________________________________________________________
	_map.emplace(0x28, Opcode_6x09{"LBVC", { {2} }, AM_RELATIVE  });
	// LBVS _________________________________________________________________________
	_map.emplace(0x29, Opcode_6x09{"LBVS", { {2} }, AM_RELATIVE  });
	// LDS __________________________________________________________________________
	_map.emplace(0xCE, Opcode_6x09{"LDS",  { {2} }, AM_IMMEDIATE });
	_map.emplace(0xDE, Opcode_6x09{"LDS",  { {1} }, AM_DIRECT    });
	_map.emplace(0xEE, Opcode_6x09{"LDS",  { {1} }, AM_INDEXED   });
	_map.emplace(0xFF, Opcode_6x09{"LDS",  { {2} }, AM_EXTENDED  });
	// LDY __________________________________________________________________________
	_map.emplace(0x8E, Opcode_6x09{"LDY",  { {2} }, AM_IMMEDIATE });
	_map.emplace(0x9E, Opcode_6x09{"LDY",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAE, Opcode_6x09{"LDY",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBE, Opcode_6x09{"LDY",  { {2} }, AM_EXTENDED  });
	// STY __________________________________________________________________________
	_map.emplace(0x9F, Opcode_6x09{"STY",  { {1} }, AM_DIRECT    });
	_map.emplace(0xAF, Opcode_6x09{"STY",  { {1} }, AM_INDEXED   });
	_map.emplace(0xBF, Opcode_6x09{"STY",  { {2} }, AM_EXTENDED  });

	if( _processor == EProcessor::M6309 )
	{
		// ADCD (6309)___________________________________________________________________
		_map.emplace(0x89, Opcode_6x09{"ADCD", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x99, Opcode_6x09{"ADCD", { {1} }, AM_DIRECT    });
		_map.emplace(0xA9, Opcode_6x09{"ADCD", { {1} }, AM_INDEXED   });
		_map.emplace(0xB9, Opcode_6x09{"ADCD", { {2} }, AM_EXTENDED  });
		// ADCR (6309)___________________________________________________________________
		_map.emplace(0x31, Opcode_6x09{"ADCR", { {1} }, AM_IMMEDIATE });
		// ADDW (6309)___________________________________________________________________
		_map.emplace(0x8B, Opcode_6x09{"ADDW", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x9B, Opcode_6x09{"ADDW", { {1} }, AM_DIRECT    });
		_map.emplace(0xAB, Opcode_6x09{"ADDW", { {1} }, AM_INDEXED   });
		_map.emplace(0xBB, Opcode_6x09{"ADDW", { {2} }, AM_EXTENDED  });
		// ADDR (6309)___________________________________________________________________
		_map.emplace(0x30, Opcode_6x09{"ADDR", { {1} }, AM_IMMEDIATE });
		// ANDD (6309)___________________________________________________________________
		_map.emplace(0x84, Opcode_6x09{"ANDD", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x94, Opcode_6x09{"ANDD", { {1} }, AM_DIRECT    });
		_map.emplace(0xA4, Opcode_6x09{"ANDD", { {1} }, AM_INDEXED   });
		_map.emplace(0xB4, Opcode_6x09{"ANDD", { {2} }, AM_EXTENDED  });
		// ANDR (6309)___________________________________________________________________
		_map.emplace(0x34, Opcode_6x09{"ANDR", { {1} }, AM_IMMEDIATE });
		// ASLD (6309)___________________________________________________________________
		_map.emplace(0x48, Opcode_6x09{"ASLD", {     }, AM_IMMEDIATE });
		// ASRD (6309)___________________________________________________________________
		_map.emplace(0x47, Opcode_6x09{"ASRD", {     }, AM_IMMEDIATE });
		// BITD (6309)___________________________________________________________________
		_map.emplace(0x85, Opcode_6x09{"BITD", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x95, Opcode_6x09{"BITD", { {1} }, AM_DIRECT    });
		_map.emplace(0xA5, Opcode_6x09{"BITD", { {1} }, AM_INDEXED   });
		_map.emplace(0xB5, Opcode_6x09{"BITD", { {2} }, AM_EXTENDED  });
		// CLRD (6309)___________________________________________________________________
		_map.emplace(0x4F, Opcode_6x09{"CLRD", {     }, AM_INHERENT  });
		// CLRW (6309)___________________________________________________________________
		_map.emplace(0x5F, Opcode_6x09{"CLRW", {     }, AM_INHERENT  });
		// CMPW (6309)___________________________________________________________________
		_map.emplace(0x81, Opcode_6x09{"CMPW", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x91, Opcode_6x09{"CMPW", { {1} }, AM_DIRECT    });
		_map.emplace(0xA1, Opcode_6x09{"CMPW", { {1} }, AM_INDEXED   });
		_map.emplace(0xB1, Opcode_6x09{"CMPW", { {2} }, AM_EXTENDED  });
		// CMPR (6309)___________________________________________________________________
		_map.emplace(0x37, Opcode_6x09{"CMPR", { {1} }, AM_IMMEDIATE });
		// EORD (6309)___________________________________________________________________
		_map.emplace(0x88, Opcode_6x09{"EORD", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x98, Opcode_6x09{"EORD", { {1} }, AM_DIRECT    });
		_map.emplace(0xA8, Opcode_6x09{"EORD", { {1} }, AM_INDEXED   });
		_map.emplace(0xB8, Opcode_6x09{"EORD", { {2} }, AM_EXTENDED  });
		// EORR (6309)___________________________________________________________________
		_map.emplace(0x36, Opcode_6x09{"EORR", { {1} }, AM_IMMEDIATE });
		// INCD (6309)___________________________________________________________________
		_map.emplace(0x4C, Opcode_6x09{"INCD", {     }, AM_INHERENT  });
		// INCW (6309)___________________________________________________________________
		_map.emplace(0x5C, Opcode_6x09{"INCW", {     }, AM_INHERENT  });
		// LDW (6309)____________________________________________________________________
		_map.emplace(0x86, Opcode_6x09{"LDW",  { {2} }, AM_IMMEDIATE });
		_map.emplace(0x96, Opcode_6x09{"LDW",  { {1} }, AM_DIRECT    });
		_map.emplace(0xA6, Opcode_6x09{"LDW",  { {1} }, AM_INDEXED   });
		_map.emplace(0xB6, Opcode_6x09{"LDW",  { {2} }, AM_EXTENDED  });
		// LDQ (6309)____________________________________________________________________
		_map.emplace(0xDC, Opcode_6x09{"LDQ",  { {1} }, AM_DIRECT    });
		_map.emplace(0xEC, Opcode_6x09{"LDQ",  { {1} }, AM_INDEXED   });
		_map.emplace(0xFC, Opcode_6x09{"LDQ",  { {2} }, AM_EXTENDED  });
		// LSLD (6309)___________________________________________________________________
		_map.emplace(0x48, Opcode_6x09{"LSLD", {     }, AM_INHERENT  });
		// LSRD (6309)___________________________________________________________________
		_map.emplace(0x44, Opcode_6x09{"LSRD", {     }, AM_INHERENT  });
		// LSRW (6309)___________________________________________________________________
		_map.emplace(0x54, Opcode_6x09{"LSRW", {     }, AM_INHERENT  });
		// NEGD _________________________________________________________________________
		_map.emplace(0x40, Opcode_6x09{"NEGD", {     }, AM_INHERENT  });
		// ORD (6309)____________________________________________________________________
		_map.emplace(0x8A, Opcode_6x09{"ORD",  { {1} }, AM_IMMEDIATE });
		_map.emplace(0x9A, Opcode_6x09{"ORD",  { {1} }, AM_DIRECT    });
		_map.emplace(0xAA, Opcode_6x09{"ORD",  { {1} }, AM_INDEXED   });
		_map.emplace(0xBA, Opcode_6x09{"ORD",  { {2} }, AM_EXTENDED  });
		// ORR (6309)____________________________________________________________________
		_map.emplace(0x35, Opcode_6x09{"ORR",  { {1} }, AM_IMMEDIATE });
		// PSHSW (6309)__________________________________________________________________
		_map.emplace(0x38, Opcode_6x09{"PSHSW",{     }, AM_INHERENT  });
		// PSHUW (6309)__________________________________________________________________
		_map.emplace(0x3A, Opcode_6x09{"PSHUW",{     }, AM_INHERENT  });
		// PULSW (6309)__________________________________________________________________
		_map.emplace(0x39, Opcode_6x09{"PULSW",{     }, AM_INHERENT  });
		// PULUW (6309)__________________________________________________________________
		_map.emplace(0x3B, Opcode_6x09{"PULUW",{     }, AM_INHERENT  });
		// RORD (6309)___________________________________________________________________
		_map.emplace(0x46, Opcode_6x09{"RORD", {     }, AM_INHERENT  });
		// RORW (6309)___________________________________________________________________
		_map.emplace(0x56, Opcode_6x09{"RORW", {     }, AM_INHERENT  });
		// SBCD (6309)___________________________________________________________________
		_map.emplace(0x82, Opcode_6x09{"SBCD", { {1} }, AM_IMMEDIATE });
		_map.emplace(0x92, Opcode_6x09{"SBCD", { {1} }, AM_DIRECT    });
		_map.emplace(0xA2, Opcode_6x09{"SBCD", { {1} }, AM_INDEXED   });
		_map.emplace(0xB2, Opcode_6x09{"SBCD", { {2} }, AM_EXTENDED  });
		// SBCR (6309)___________________________________________________________________
		_map.emplace(0x33, Opcode_6x09{"SBCR", { {1} }, AM_IMMEDIATE });
		// STS __________________________________________________________________________
		_map.emplace(0xDF, Opcode_6x09{"STS",  { {1} }, AM_DIRECT    });
		_map.emplace(0xEF, Opcode_6x09{"STS",  { {1} }, AM_INDEXED   });
		_map.emplace(0xFF, Opcode_6x09{"STS",  { {2} }, AM_EXTENDED  });
		// STW (6309)____________________________________________________________________
		_map.emplace(0x97, Opcode_6x09{"STW",  { {1} }, AM_DIRECT    });
		_map.emplace(0xA7, Opcode_6x09{"STW",  { {1} }, AM_INDEXED   });
		_map.emplace(0xB7, Opcode_6x09{"STW",  { {2} }, AM_EXTENDED  });
		// SUBW (6309)___________________________________________________________________
		_map.emplace(0x80, Opcode_6x09{"SUBW", { {2} }, AM_IMMEDIATE });
		_map.emplace(0x90, Opcode_6x09{"SUBW", { {1} }, AM_DIRECT    });
		_map.emplace(0xA0, Opcode_6x09{"SUBW", { {1} }, AM_INDEXED   });
		_map.emplace(0xB0, Opcode_6x09{"SUBW", { {2} }, AM_EXTENDED  });
		// SWI2 _________________________________________________________________________
		_map.emplace(0x3F, Opcode_6x09{"SWI2", {     }, AM_INHERENT  });
		// TSTD (6309)___________________________________________________________________
		_map.emplace(0x4D, Opcode_6x09{"TSTD", {     }, AM_INHERENT  });
		// TSTW (6309)___________________________________________________________________
		_map.emplace(0x5D, Opcode_6x09{"TSTW", {     }, AM_INHERENT  });
	}
}