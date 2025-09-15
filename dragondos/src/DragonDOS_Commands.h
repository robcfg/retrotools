#pragma once

#ifndef __DRAGONDOS_COMMANDS_H__
#define __DRAGONDOS_COMMANDS_H__

#include <string>
#include <vector>

#include "../../common/DiskImages/DiskImageFactory.h"

bool HelpCommand         ();
bool ListCommand         ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool ExtractCommand      ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool InfoCommand         ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool NewCommand          ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool DeleteCommand       ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool InsertBasicCommand  ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool InsertBinaryCommand ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool InsertDataCommand   ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );
bool ListImagesCommand   ( const std::vector<std::string>& _args, DiskImageFactory& _diskFactory );

#endif