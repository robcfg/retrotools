#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Page2_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map )
{
    // ADCD (6309)___________________________________________________________________
    _map.emplace(0x89, Opcode_6x09{"ADCD", { {2} }, IMMEDIATE });
    _map.emplace(0x99, Opcode_6x09{"ADCD", { {1} }, DIRECT    });
    _map.emplace(0xA9, Opcode_6x09{"ADCD", { {1} }, INDEXED   });
    _map.emplace(0xB9, Opcode_6x09{"ADCD", { {2} }, EXTENDED  });
    // ADCR (6309)___________________________________________________________________
    _map.emplace(0x31, Opcode_6x09{"ADCR", { {1} }, IMMEDIATE });
    // ADDW (6309)___________________________________________________________________
    _map.emplace(0x8B, Opcode_6x09{"ADDW", { {2} }, IMMEDIATE });
    _map.emplace(0x9B, Opcode_6x09{"ADDW", { {1} }, DIRECT    });
    _map.emplace(0xAB, Opcode_6x09{"ADDW", { {1} }, INDEXED   });
    _map.emplace(0xBB, Opcode_6x09{"ADDW", { {2} }, EXTENDED  });
    // ADDR (6309)___________________________________________________________________
    _map.emplace(0x30, Opcode_6x09{"ADDR", { {1} }, IMMEDIATE });
    // ANDD (6309)___________________________________________________________________
    _map.emplace(0x84, Opcode_6x09{"ANDD", { {2} }, IMMEDIATE });
    _map.emplace(0x94, Opcode_6x09{"ANDD", { {1} }, DIRECT    });
    _map.emplace(0xA4, Opcode_6x09{"ANDD", { {1} }, INDEXED   });
    _map.emplace(0xB4, Opcode_6x09{"ANDD", { {2} }, EXTENDED  });
    // ANDR (6309)___________________________________________________________________
    _map.emplace(0x34, Opcode_6x09{"ANDR", { {1} }, IMMEDIATE });
    // ASLD (6309)___________________________________________________________________
    _map.emplace(0x48, Opcode_6x09{"ASLD", {     }, IMMEDIATE });
    // ASRD (6309)___________________________________________________________________
    _map.emplace(0x47, Opcode_6x09{"ASRD", {     }, IMMEDIATE });
    // BITD (6309)___________________________________________________________________
    _map.emplace(0x85, Opcode_6x09{"BITD", { {2} }, IMMEDIATE });
    _map.emplace(0x95, Opcode_6x09{"BITD", { {1} }, DIRECT    });
    _map.emplace(0xA5, Opcode_6x09{"BITD", { {1} }, INDEXED   });
    _map.emplace(0xB5, Opcode_6x09{"BITD", { {2} }, EXTENDED  });
    // CLRD (6309)___________________________________________________________________
    _map.emplace(0x4F, Opcode_6x09{"CLRD", {     }, INHERENT  });
    // CLRW (6309)___________________________________________________________________
    _map.emplace(0x5F, Opcode_6x09{"CLRW", {     }, INHERENT  });
    // CMPD _________________________________________________________________________
    _map.emplace(0x83, Opcode_6x09{"CMPD", { {2} }, IMMEDIATE });
    _map.emplace(0x93, Opcode_6x09{"CMPD", { {1} }, DIRECT    });
    _map.emplace(0xA3, Opcode_6x09{"CMPD", { {1} }, INDEXED   });
    _map.emplace(0xB3, Opcode_6x09{"CMPD", { {2} }, EXTENDED  });
    // CMPW (6309)___________________________________________________________________
    _map.emplace(0x81, Opcode_6x09{"CMPW", { {2} }, IMMEDIATE });
    _map.emplace(0x91, Opcode_6x09{"CMPW", { {1} }, DIRECT    });
    _map.emplace(0xA1, Opcode_6x09{"CMPW", { {1} }, INDEXED   });
    _map.emplace(0xB1, Opcode_6x09{"CMPW", { {2} }, EXTENDED  });
    // CMPY _________________________________________________________________________
    _map.emplace(0x8C, Opcode_6x09{"CMPY", { {2} }, IMMEDIATE });
    _map.emplace(0x9C, Opcode_6x09{"CMPY", { {1} }, DIRECT    });
    _map.emplace(0xAC, Opcode_6x09{"CMPY", { {1} }, INDEXED   });
    _map.emplace(0xBC, Opcode_6x09{"CMPY", { {2} }, EXTENDED  });
    // CMPR (6309)___________________________________________________________________
    _map.emplace(0x37, Opcode_6x09{"CMPR", { {1} }, IMMEDIATE });
    // COMD _________________________________________________________________________
    _map.emplace(0x43, Opcode_6x09{"COMD", {     }, INHERENT  });
    // COMW _________________________________________________________________________
    _map.emplace(0x53, Opcode_6x09{"COMW", {     }, INHERENT  });
    // DECD _________________________________________________________________________
    _map.emplace(0x4A, Opcode_6x09{"DECD", {     }, INHERENT  });
    // DECW _________________________________________________________________________
    _map.emplace(0x5A, Opcode_6x09{"DECW", {     }, INHERENT  });
    // EORD (6309)___________________________________________________________________
    _map.emplace(0x88, Opcode_6x09{"EORD", { {2} }, IMMEDIATE });
    _map.emplace(0x98, Opcode_6x09{"EORD", { {1} }, DIRECT    });
    _map.emplace(0xA8, Opcode_6x09{"EORD", { {1} }, INDEXED   });
    _map.emplace(0xB8, Opcode_6x09{"EORD", { {2} }, EXTENDED  });
    // EORR (6309)___________________________________________________________________
    _map.emplace(0x36, Opcode_6x09{"EORR", { {1} }, IMMEDIATE });
    // INCD (6309)___________________________________________________________________
    _map.emplace(0x4C, Opcode_6x09{"INCD", {     }, INHERENT  });
    // INCW (6309)___________________________________________________________________
    _map.emplace(0x5C, Opcode_6x09{"INCW", {     }, INHERENT  });
    // LBCC _________________________________________________________________________
    _map.emplace(0x24, Opcode_6x09{"LBCC", { {2} }, RELATIVE  });
    // LBCS _________________________________________________________________________
    _map.emplace(0x25, Opcode_6x09{"LBCS", { {2} }, RELATIVE  });
    // LBEQ _________________________________________________________________________
    _map.emplace(0x27, Opcode_6x09{"LBEQ", { {2} }, RELATIVE  });
    // LBGE _________________________________________________________________________
    _map.emplace(0x2C, Opcode_6x09{"LBGE", { {2} }, RELATIVE  });
    // LBGT _________________________________________________________________________
    _map.emplace(0x2E, Opcode_6x09{"LBGT", { {2} }, RELATIVE  });
    // LBHI _________________________________________________________________________
    _map.emplace(0x22, Opcode_6x09{"LBHI", { {2} }, RELATIVE  });
    // LBHS _________________________________________________________________________
    _map.emplace(0x24, Opcode_6x09{"LBHS", { {2} }, RELATIVE  });
    // LBLE _________________________________________________________________________
    _map.emplace(0x2F, Opcode_6x09{"LBLE", { {2} }, RELATIVE  });
    // LBLO _________________________________________________________________________
    _map.emplace(0x25, Opcode_6x09{"LBLO", { {2} }, RELATIVE  });
    // LBLS _________________________________________________________________________
    _map.emplace(0x23, Opcode_6x09{"LBLS", { {2} }, RELATIVE  });
    // LBLT _________________________________________________________________________
    _map.emplace(0x2D, Opcode_6x09{"LBLT", { {2} }, RELATIVE  });
    // LBMI _________________________________________________________________________
    _map.emplace(0x2B, Opcode_6x09{"LBMI", { {2} }, RELATIVE  });
    // LBNE _________________________________________________________________________
    _map.emplace(0x26, Opcode_6x09{"LBNE", { {2} }, RELATIVE  });
    // LBPL _________________________________________________________________________
    _map.emplace(0x2A, Opcode_6x09{"LBPL", { {2} }, RELATIVE  });
    // LBRN _________________________________________________________________________
    _map.emplace(0x21, Opcode_6x09{"LBRN", { {2} }, RELATIVE  });
    // LBVC _________________________________________________________________________
    _map.emplace(0x28, Opcode_6x09{"LBVC", { {2} }, RELATIVE  });
    // LBVS _________________________________________________________________________
    _map.emplace(0x29, Opcode_6x09{"LBVS", { {2} }, RELATIVE  });
    // LDS __________________________________________________________________________
    _map.emplace(0xCE, Opcode_6x09{"LDS",  { {2} }, IMMEDIATE });
    _map.emplace(0xDE, Opcode_6x09{"LDS",  { {1} }, DIRECT    });
    _map.emplace(0xEE, Opcode_6x09{"LDS",  { {1} }, INDEXED   });
    _map.emplace(0xFF, Opcode_6x09{"LDS",  { {2} }, EXTENDED  });
    // LDW (6309)____________________________________________________________________
    _map.emplace(0x86, Opcode_6x09{"LDW",  { {2} }, IMMEDIATE });
    _map.emplace(0x96, Opcode_6x09{"LDW",  { {1} }, DIRECT    });
    _map.emplace(0xA6, Opcode_6x09{"LDW",  { {1} }, INDEXED   });
    _map.emplace(0xB6, Opcode_6x09{"LDW",  { {2} }, EXTENDED  });
    // LDY __________________________________________________________________________
    _map.emplace(0x8E, Opcode_6x09{"LDY",  { {2} }, IMMEDIATE });
    _map.emplace(0x9E, Opcode_6x09{"LDY",  { {1} }, DIRECT    });
    _map.emplace(0xAE, Opcode_6x09{"LDY",  { {1} }, INDEXED   });
    _map.emplace(0xBE, Opcode_6x09{"LDY",  { {2} }, EXTENDED  });
    // LDQ (6309)____________________________________________________________________
    _map.emplace(0xDC, Opcode_6x09{"LDQ",  { {1} }, DIRECT    });
    _map.emplace(0xEC, Opcode_6x09{"LDQ",  { {1} }, INDEXED   });
    _map.emplace(0xFC, Opcode_6x09{"LDQ",  { {2} }, EXTENDED  });
    // LSLD (6309)___________________________________________________________________
    _map.emplace(0x48, Opcode_6x09{"LSLD", {     }, INHERENT  });
    // LSRD (6309)___________________________________________________________________
    _map.emplace(0x44, Opcode_6x09{"LSRD", {     }, INHERENT  });
    // LSRW (6309)___________________________________________________________________
    _map.emplace(0x54, Opcode_6x09{"LSRW", {     }, INHERENT  });
    // NEGD _________________________________________________________________________
    _map.emplace(0x40, Opcode_6x09{"NEGD", {     }, INHERENT  });
    // ORD (6309)____________________________________________________________________
    _map.emplace(0x8A, Opcode_6x09{"ORD",  { {1} }, IMMEDIATE });
    _map.emplace(0x9A, Opcode_6x09{"ORD",  { {1} }, DIRECT    });
    _map.emplace(0xAA, Opcode_6x09{"ORD",  { {1} }, INDEXED   });
    _map.emplace(0xBA, Opcode_6x09{"ORD",  { {2} }, EXTENDED  });
    // ORR (6309)____________________________________________________________________
    _map.emplace(0x35, Opcode_6x09{"ORR",  { {1} }, IMMEDIATE });
    // PSHSW (6309)__________________________________________________________________
    _map.emplace(0x38, Opcode_6x09{"PSHSW",{     }, INHERENT  });
    // PSHUW (6309)__________________________________________________________________
    _map.emplace(0x3A, Opcode_6x09{"PSHUW",{     }, INHERENT  });
    // PULSW (6309)__________________________________________________________________
    _map.emplace(0x39, Opcode_6x09{"PULSW",{     }, INHERENT  });
    // PULUW (6309)__________________________________________________________________
    _map.emplace(0x3B, Opcode_6x09{"PULUW",{     }, INHERENT  });
    // RORD (6309)___________________________________________________________________
    _map.emplace(0x46, Opcode_6x09{"RORD", {     }, INHERENT  });
    // RORW (6309)___________________________________________________________________
    _map.emplace(0x56, Opcode_6x09{"RORW", {     }, INHERENT  });
    // SBCD (6309)___________________________________________________________________
    _map.emplace(0x82, Opcode_6x09{"SBCD", { {1} }, IMMEDIATE });
    _map.emplace(0x92, Opcode_6x09{"SBCD", { {1} }, DIRECT    });
    _map.emplace(0xA2, Opcode_6x09{"SBCD", { {1} }, INDEXED   });
    _map.emplace(0xB2, Opcode_6x09{"SBCD", { {2} }, EXTENDED  });
    // SBCR (6309)___________________________________________________________________
    _map.emplace(0x33, Opcode_6x09{"SBCR", { {1} }, IMMEDIATE });
    // STS __________________________________________________________________________
    _map.emplace(0xDF, Opcode_6x09{"STS",  { {1} }, DIRECT    });
    _map.emplace(0xEF, Opcode_6x09{"STS",  { {1} }, INDEXED   });
    _map.emplace(0xFF, Opcode_6x09{"STS",  { {2} }, EXTENDED  });
    // STW (6309)____________________________________________________________________
    _map.emplace(0x97, Opcode_6x09{"STW",  { {1} }, DIRECT    });
    _map.emplace(0xA7, Opcode_6x09{"STW",  { {1} }, INDEXED   });
    _map.emplace(0xB7, Opcode_6x09{"STW",  { {2} }, EXTENDED  });
    // STY __________________________________________________________________________
    _map.emplace(0x9F, Opcode_6x09{"STY",  { {1} }, DIRECT    });
    _map.emplace(0xAF, Opcode_6x09{"STY",  { {1} }, INDEXED   });
    _map.emplace(0xBF, Opcode_6x09{"STY",  { {2} }, EXTENDED  });
    // SUBW (6309)___________________________________________________________________
    _map.emplace(0x80, Opcode_6x09{"SUBW", { {2} }, IMMEDIATE });
    _map.emplace(0x90, Opcode_6x09{"SUBW", { {1} }, DIRECT    });
    _map.emplace(0xA0, Opcode_6x09{"SUBW", { {1} }, INDEXED   });
    _map.emplace(0xB0, Opcode_6x09{"SUBW", { {2} }, EXTENDED  });
    // SWI2 _________________________________________________________________________
    _map.emplace(0x3F, Opcode_6x09{"SWI2", {     }, INHERENT  });
    // TSTD (6309)___________________________________________________________________
    _map.emplace(0x4D, Opcode_6x09{"TSTD", {     }, INHERENT  });
    // TSTW (6309)___________________________________________________________________
    _map.emplace(0x5D, Opcode_6x09{"TSTW", {     }, INHERENT  });
}