#pragma once

#ifndef __DOS68_COMMANDS_H__
#define __DOS68_COMMANDS_H__

#include <string>
#include <vector>

using namespace std;

bool HelpCommand();
bool ListCommand( const vector<string>& _args );
bool ExtractCommand( const vector<string>& _args );
bool InfoCommand( const vector<string>& _args );
bool NewCommand( const vector<string>& _args );
bool DeleteCommand( const vector<string>& _args );
bool InsertCommand( const vector<string>& _args );

#endif