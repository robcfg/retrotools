#include "6x09_Disassembler.h"

void Disassembler_6x09::Init_Undefined_Opcode_Map( std::map<unsigned char, Opcode_6x09>& _map, EProcessor _processor )
{
	// Only the 6809 has undefined opcodes.
	if( _processor != EProcessor::M6809 )
	{
		return;
	}

	_map.emplace(0x01, Opcode_6x09{"uNEG"        , { {1} }, DIRECT     });
	_map.emplace(0x02, Opcode_6x09{"uNEG/COM"    , { {1} }, DIRECT     });
	_map.emplace(0x05, Opcode_6x09{"uLSR"        , { {1} }, DIRECT     });
	_map.emplace(0x0B, Opcode_6x09{"uDEC"        , { {1} }, DIRECT     });
	_map.emplace(0x14, Opcode_6x09{"uHCF"        , {     }, INHERENT   });
	_map.emplace(0x15, Opcode_6x09{"uHCF"        , {     }, INHERENT   });
	_map.emplace(0x18, Opcode_6x09{"u$18"        , {     }, INHERENT   });
	_map.emplace(0x1B, Opcode_6x09{"uNOP"        , {     }, INHERENT   });
	_map.emplace(0x20, Opcode_6x09{"uLBRA"       , { {2} }, RELATIVE   });
	_map.emplace(0x38, Opcode_6x09{"uANDCC"      , { {1} }, IMMEDIATE  });
	_map.emplace(0x3E, Opcode_6x09{"uRESET"      , {     }, INHERENT   });
	_map.emplace(0x41, Opcode_6x09{"uNEGA"       , {     }, INHERENT   });
	_map.emplace(0x42, Opcode_6x09{"uNEGA/COMA"  , {     }, INHERENT   });
	_map.emplace(0x45, Opcode_6x09{"uLSRA"       , {     }, INHERENT   });
	_map.emplace(0x4B, Opcode_6x09{"uDECA"       , {     }, INHERENT   });
	_map.emplace(0x4E, Opcode_6x09{"uCLRA"       , {     }, INHERENT   });
	_map.emplace(0x51, Opcode_6x09{"uNEGB"       , {     }, INHERENT   });
	_map.emplace(0x52, Opcode_6x09{"uNEGB/COMB"  , {     }, INHERENT   });
	_map.emplace(0x55, Opcode_6x09{"uLSRB"       , {     }, INHERENT   });
	_map.emplace(0x5B, Opcode_6x09{"uDECB"       , {     }, INHERENT   });
	_map.emplace(0x5E, Opcode_6x09{"uCLRB"       , {     }, INHERENT   });
	_map.emplace(0x61, Opcode_6x09{"uNEG"        , { {1} }, INDEXED    });
	_map.emplace(0x62, Opcode_6x09{"uNEG/COM"    , { {1} }, INDEXED    });
	_map.emplace(0x65, Opcode_6x09{"uLSR"        , { {1} }, INDEXED    });
	_map.emplace(0x6B, Opcode_6x09{"uDEC"        , { {1} }, INDEXED    });
	_map.emplace(0x71, Opcode_6x09{"uNEG"        , { {2} }, EXTENDED   });
	_map.emplace(0x72, Opcode_6x09{"uNEG/COM"    , { {2} }, EXTENDED   });
	_map.emplace(0x75, Opcode_6x09{"uLSR"        , { {2} }, EXTENDED   });
	_map.emplace(0x7B, Opcode_6x09{"uDEC"        , { {2} }, EXTENDED   });
	_map.emplace(0x87, Opcode_6x09{"u$87"        , { {1} }, IMMEDIATE  });
	_map.emplace(0x8F, Opcode_6x09{"uSTX"        , { {2} }, IMMEDIATE  });
	_map.emplace(0xC7, Opcode_6x09{"u$C7"        , { {1} }, IMMEDIATE  });
	_map.emplace(0xCF, Opcode_6x09{"uSTU"        , { {2} }, IMMEDIATE  });
	_map.emplace(0xCD, Opcode_6x09{"uHCF"        , {     }, INHERENT   });
}