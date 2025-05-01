#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Page3_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map )
{
    // ADDE (6309)___________________________________________________________________
    _map.emplace(0x8B, Opcode_6x09{"ADDE", { {1} }, IMMEDIATE });
    _map.emplace(0x9B, Opcode_6x09{"ADDE", { {1} }, DIRECT    });
    _map.emplace(0xAB, Opcode_6x09{"ADDE", { {1} }, INDEXED   });
    _map.emplace(0xBB, Opcode_6x09{"ADDE", { {2} }, EXTENDED  });
    // ADDF (6309)___________________________________________________________________
    _map.emplace(0xCB, Opcode_6x09{"ADDF", { {1} }, IMMEDIATE });
    _map.emplace(0xDB, Opcode_6x09{"ADDF", { {1} }, DIRECT    });
    _map.emplace(0xEB, Opcode_6x09{"ADDF", { {1} }, INDEXED   });
    _map.emplace(0xFB, Opcode_6x09{"ADDF", { {2} }, EXTENDED  });
    // *BAND (6309)___________________________________________________________________
    _map.emplace(0x30, Opcode_6x09{"BAND", { {2} }, DIRECT    });
    // *BEOR (6309)___________________________________________________________________
    _map.emplace(0x34, Opcode_6x09{"BEOR", { {2} }, DIRECT    });
    // *BIAND (6309)__________________________________________________________________
    _map.emplace(0x31, Opcode_6x09{"BIAND",{ {2} }, DIRECT    });
    // *BIEOR (6309)__________________________________________________________________
    _map.emplace(0x35, Opcode_6x09{"BIEOR",{ {2} }, DIRECT    });
    // *BIOR (6309)___________________________________________________________________
    _map.emplace(0x33, Opcode_6x09{"BIOR", { {2} }, DIRECT    });
    // *BITMD (6309)__________________________________________________________________
    _map.emplace(0x3C, Opcode_6x09{"BITMD",{ {1} }, IMMEDIATE });
    // *BOR (6309)____________________________________________________________________
    _map.emplace(0x32, Opcode_6x09{"BOR",  { {2} }, DIRECT    });
    // CLRE (6309)___________________________________________________________________
    _map.emplace(0x4F, Opcode_6x09{"CLRE", {     }, INHERENT  });
    // CLRF (6309)___________________________________________________________________
    _map.emplace(0x5F, Opcode_6x09{"CLRF", {     }, INHERENT  });
    // CMPE (6309)___________________________________________________________________
    _map.emplace(0x81, Opcode_6x09{"CMPE", { {1} }, IMMEDIATE });
    _map.emplace(0x91, Opcode_6x09{"CMPE", { {1} }, DIRECT    });
    _map.emplace(0xA1, Opcode_6x09{"CMPE", { {1} }, INDEXED   });
    _map.emplace(0xB1, Opcode_6x09{"CMPE", { {2} }, EXTENDED  });
    // CMPF (6309)___________________________________________________________________
    _map.emplace(0xC1, Opcode_6x09{"CMPF", { {1} }, IMMEDIATE });
    _map.emplace(0xD1, Opcode_6x09{"CMPF", { {1} }, DIRECT    });
    _map.emplace(0xE1, Opcode_6x09{"CMPF", { {1} }, INDEXED   });
    _map.emplace(0xF1, Opcode_6x09{"CMPF", { {2} }, EXTENDED  });
    // CMPS _________________________________________________________________________
    _map.emplace(0x8C, Opcode_6x09{"CMPS", { {2} }, IMMEDIATE });
    _map.emplace(0x9C, Opcode_6x09{"CMPS", { {1} }, DIRECT    });
    _map.emplace(0xAC, Opcode_6x09{"CMPS", { {1} }, INDEXED   });
    _map.emplace(0xBC, Opcode_6x09{"CMPS", { {2} }, EXTENDED  });
    // CMPU _________________________________________________________________________
    _map.emplace(0x83, Opcode_6x09{"CMPU", { {2} }, IMMEDIATE });
    _map.emplace(0x93, Opcode_6x09{"CMPU", { {1} }, DIRECT    });
    _map.emplace(0xA3, Opcode_6x09{"CMPU", { {1} }, INDEXED   });
    _map.emplace(0xB3, Opcode_6x09{"CMPU", { {2} }, EXTENDED  });
    // COME _________________________________________________________________________
    _map.emplace(0x43, Opcode_6x09{"COME", {     }, INHERENT  });
    // COMF _________________________________________________________________________
    _map.emplace(0x53, Opcode_6x09{"COMF", {     }, INHERENT  });
    // DECE _________________________________________________________________________
    _map.emplace(0x4A, Opcode_6x09{"DECE", {     }, INHERENT  });
    // DECF _________________________________________________________________________
    _map.emplace(0x5A, Opcode_6x09{"DECF", {     }, INHERENT  });
    // DIVD (6309)___________________________________________________________________
    _map.emplace(0x8D, Opcode_6x09{"DIVD", { {1} }, IMMEDIATE });
    _map.emplace(0x9D, Opcode_6x09{"DIVD", { {1} }, DIRECT    });
    _map.emplace(0xAD, Opcode_6x09{"DIVD", { {1} }, INDEXED   });
    _map.emplace(0xBD, Opcode_6x09{"DIVD", { {2} }, EXTENDED  });
    // DIVQ (6309)___________________________________________________________________
    _map.emplace(0x8E, Opcode_6x09{"DIVQ", { {2} }, IMMEDIATE });
    _map.emplace(0x9E, Opcode_6x09{"DIVQ", { {1} }, DIRECT    });
    _map.emplace(0xAE, Opcode_6x09{"DIVQ", { {1} }, INDEXED   });
    _map.emplace(0xBE, Opcode_6x09{"DIVQ", { {2} }, EXTENDED  });
    // EIM (6309)____________________________________________________________________
    _map.emplace(0x05, Opcode_6x09{"EIM",  { {1} }, DIRECT    });
    _map.emplace(0x65, Opcode_6x09{"EIM",  { {1} }, INDEXED   });
    _map.emplace(0x75, Opcode_6x09{"EIM",  { {2} }, EXTENDED  });
    // INCE (6309)___________________________________________________________________
    _map.emplace(0x4C, Opcode_6x09{"INCE", {     }, INHERENT  });
    // INCF (6309)___________________________________________________________________
    _map.emplace(0x5C, Opcode_6x09{"INCF", {     }, INHERENT  });
    // LDE __________________________________________________________________________
    _map.emplace(0x86, Opcode_6x09{"LDE",  { {1} }, IMMEDIATE });
    _map.emplace(0x96, Opcode_6x09{"LDE",  { {1} }, DIRECT    });
    _map.emplace(0xA6, Opcode_6x09{"LDE",  { {1} }, INDEXED   });
    _map.emplace(0xB6, Opcode_6x09{"LDE",  { {2} }, EXTENDED  });
    // LDF __________________________________________________________________________
    _map.emplace(0xC6, Opcode_6x09{"LDF",  { {1} }, IMMEDIATE });
    _map.emplace(0xD6, Opcode_6x09{"LDF",  { {1} }, DIRECT    });
    _map.emplace(0xE6, Opcode_6x09{"LDF",  { {1} }, INDEXED   });
    _map.emplace(0xF6, Opcode_6x09{"LDF",  { {2} }, EXTENDED  });
    // LDBT (6309)___________________________________________________________________
    _map.emplace(0x36, Opcode_6x09{"LDBT", { {2} }, DIRECT    });
    // LDMD (6309)___________________________________________________________________
    _map.emplace(0x3D, Opcode_6x09{"LDMD", { {1} }, IMMEDIATE });
    // MULD (6309)___________________________________________________________________
    _map.emplace(0x8F, Opcode_6x09{"MULD", { {2} }, IMMEDIATE });
    _map.emplace(0x9F, Opcode_6x09{"MULD", { {1} }, DIRECT    });
    _map.emplace(0xAF, Opcode_6x09{"MUL",  { {1} }, INDEXED   });
    _map.emplace(0xBF, Opcode_6x09{"MULD", { {2} }, EXTENDED  });
    // STE (6309)____________________________________________________________________
    _map.emplace(0x97, Opcode_6x09{"STE",  { {1} }, DIRECT    });
    _map.emplace(0xA7, Opcode_6x09{"STE",  { {1} }, INDEXED   });
    _map.emplace(0xB7, Opcode_6x09{"STE",  { {2} }, EXTENDED  });
    // STF (6309)____________________________________________________________________
    _map.emplace(0xD7, Opcode_6x09{"STF",  { {1} }, DIRECT    });
    _map.emplace(0xE7, Opcode_6x09{"STF",  { {1} }, INDEXED   });
    _map.emplace(0xF7, Opcode_6x09{"STF",  { {2} }, EXTENDED  });
    // STBT (6309)___________________________________________________________________
    _map.emplace(0x37, Opcode_6x09{"STBT", { {4} }, DIRECT    });
    // STQ (6309)____________________________________________________________________
    _map.emplace(0xDD, Opcode_6x09{"STQ",  { {1} }, DIRECT    });
    _map.emplace(0xED, Opcode_6x09{"STQ",  { {1} }, INDEXED   });
    _map.emplace(0xFD, Opcode_6x09{"STQ",  { {2} }, EXTENDED  });
    // SUBE (6309)___________________________________________________________________
    _map.emplace(0x80, Opcode_6x09{"SUBE", { {1} }, IMMEDIATE });
    _map.emplace(0x90, Opcode_6x09{"SUBE", { {1} }, DIRECT    });
    _map.emplace(0xA0, Opcode_6x09{"SUBE", { {1} }, INDEXED   });
    _map.emplace(0xB0, Opcode_6x09{"SUBE", { {2} }, EXTENDED  });
    // SUBF (6309)___________________________________________________________________
    _map.emplace(0xC0, Opcode_6x09{"SUBF", { {1} }, IMMEDIATE });
    _map.emplace(0xD0, Opcode_6x09{"SUBF", { {1} }, DIRECT    });
    _map.emplace(0xE0, Opcode_6x09{"SUBF", { {1} }, INDEXED   });
    _map.emplace(0xF0, Opcode_6x09{"SUBF", { {2} }, EXTENDED  });
    // SUBR (6309)___________________________________________________________________
    _map.emplace(0x32, Opcode_6x09{"SUBR", { {1} }, IMMEDIATE });
    // SWI3 _________________________________________________________________________
    _map.emplace(0x3F, Opcode_6x09{"SWI3", {     }, INHERENT  });
    // TSTE (6309)___________________________________________________________________
    _map.emplace(0x4D, Opcode_6x09{"TSTE", {     }, INHERENT  });
    // TSTF (6309)___________________________________________________________________
    _map.emplace(0x5D, Opcode_6x09{"TSTF", {     }, INHERENT  });
    // *TFM (6309)____________________________________________________________________
    _map.emplace(0x38, Opcode_6x09{"TFM",  { {1} }, IMMEDIATE });
    // *TFM (6309)____________________________________________________________________
    _map.emplace(0x39, Opcode_6x09{"TFM",  { {1} }, IMMEDIATE });
    // *TFM (6309)____________________________________________________________________
    _map.emplace(0x3A, Opcode_6x09{"TFM",  { {1} }, IMMEDIATE });
    // *TFM (6309)____________________________________________________________________
    _map.emplace(0x3B, Opcode_6x09{"TFM",  { {1} }, IMMEDIATE });
}