#pragma once

#include <string>

// Command functions
void ShowHelp    ();
void ListMMB     ( const std::string& _filename, std::string& _errorString );
void CreateMMB   ( const std::string& _filename, const std::string& _slotsNum, std::string& _errorString );
void RemoveImage ( const std::string& _filename, const std::string& _slot,     std::string& _errorString );
void LockImage   ( const std::string& _filename, const std::string& _slot,     std::string& _errorString );
void UnlockImage ( const std::string& _filename, const std::string& _slot,     std::string& _errorString );
void AddImage    ( const std::string& _filename, const std::string& _imageName, const std::string& _slot, std::string& _errorString );
void ExtractImage( const std::string& _filename, const std::string& _imageName, const std::string& _slot, std::string& _errorString );

// Execute specified command. Returns true if processed or false for launching gui.
bool ProcessArguments( int argc, char** argv, std::string& _errorString );

// Auxiliary functions
const char* GetSCMVersion();