#pragma once

#ifndef __DRAGONDOS_COMMANDS_H__
#define __DRAGONDOS_COMMANDS_H__

#include <string>
#include <vector>

bool HelpCommand    ();
bool ListCommand    ( const std::vector<std::string>& _args );
bool ExtractCommand ( const std::vector<std::string>& _args );
bool InfoCommand    ( const std::vector<std::string>& _args );
bool NewCommand     ( const std::vector<std::string>& _args );
bool DeleteCommand  ( const std::vector<std::string>& _args );
bool InsertCommand  ( const std::vector<std::string>& _args );

#endif