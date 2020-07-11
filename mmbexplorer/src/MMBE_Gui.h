#pragma once

#include <string>
#include <vector>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Table.H>
#include "MMBFile.h"
#include "AcornDFS.h"

#ifdef __APPLE__
    class Fl_Sys_Menu_Bar;
#else
    class Fl_Menu_Bar;
#endif
class Fl_Table;
class Fl_Pixmap;
class CMMBEGui;

//******************************************
//* CAppWindow class
//******************************************
class CAppWindow : public Fl_Window
{
public:
	CAppWindow( int _w, int _h, const char* _label = nullptr );
	CAppWindow( int _x, int _y, int _w, int _h, const char* _label = nullptr );
	virtual ~CAppWindow();

	virtual int handle(int _event);

    void SetDiskContentWidget( Fl_Select_Browser* _diskContent );
    void RefreshDiskContent( unsigned char* _data, size_t _dataSize );
    
    void GetSelectedFiles( std::vector<int>& _dst );

private:
    Fl_Select_Browser* mDiskContent = nullptr;
};

//******************************************
//* CMMBESelectBrowser class
//******************************************
class CMMBESelectBrowser : public Fl_Select_Browser
{
public:
    CMMBESelectBrowser( CMMBEGui* _gui, int _x, int _y, int _w, int _h, const char* _label = 0 );
    virtual ~CMMBESelectBrowser();

	virtual int handle(int _event);

private:
    CMMBEGui* mGui = nullptr;
};

//******************************************
//* CMMBETable class
//******************************************
class CMMBETable : public Fl_Table
{
public:
    enum EMMBETable_SelectionType
    {
        EMMBETable_Single,
        EMMBETable_SingleAdd,
        EMMBETable_Multiple
    };

    CMMBETable( CMMBFile* _mmb, int _x, int _y, int _w, int _h, const char* _label = 0 );
    virtual ~CMMBETable();

    void draw_cell( TableContext context, int _row = 0, int _col = 0, int _x = 0, int _y = 0, int _w = 0, int _h = 0 );

    void   SelectSlot      ( size_t _slot, EMMBETable_SelectionType _selectionType );
    bool   IsSlotSelected  ( size_t _slot );
    size_t GetSelectionSize();
    void   ClearSelection();

    const std::vector<size_t>& GetSelection();
    
    void LockSelectedDisks  ();
    void UnlockSelectedDisks();

    void DoRedraw();

private:
    void DrawHeader( const char* _string, int _x, int _y, int _w, int _h );
    void DrawData( const char* _string, Fl_Pixmap* _icon, int _x, int _y, int _w, int _h, bool _selected );
    void DrawUnused( int _x, int _y, int _w, int _h );
    int  handle( int _event );

    void AddSlotToSelection( size_t _slot );
    void RemoveSlotFromSelection( size_t _slot );

    Fl_Pixmap* mIconEmpty    = nullptr;
    Fl_Pixmap* mIconUnlocked = nullptr;
    Fl_Pixmap* mIconLocked   = nullptr;

    CMMBFile* mMMB = nullptr;

    std::vector<size_t> mSelectedSlots; 
    size_t mLastSelectedSlot = (size_t)-1;   
};

//******************************************
//* CMMBEGui class
//******************************************
class CMMBEGui
{
public:
    CMMBEGui( int _w, int _h, const char* _label );
    virtual ~CMMBEGui();

    int Run( int argc, char** argv );

    void OpenMMB  ( const std::string& _filename ); 
    void CreateMMB( const std::string& _filename );
    void CloseMMB ();

    void InsertDisk ( const std::string& _filename, size_t _slot );
    void ExtractDisk( const std::string& _filename, size_t _slot );
    void RemoveDisk ( size_t _slot );
    void LockDisk   ( size_t _slot );
    void UnlockDisk ( size_t _slot );

    void ExtractSelectedDisks();
    void RemoveSelectedDisks ();
    void LockSelectedDisks   ();
    void UnlockSelectedDisks ();
    void ExtractSelectedFiles();

    void FormatDisk   ( size_t _slot );
    void NameDisk     ( size_t _slot, const std::string& _diskname );
    void InsertFile   ( size_t _slot, const std::string& _filename );
    void RemoveFile   ( size_t _slot, size_t _fileIndex );
    void LockFile     ( size_t _slot, size_t _fileIndex );
    void UnlockFile   ( size_t _slot, size_t _fileIndex );
    void SetBootOption( size_t _slot, unsigned char _bootOption );

    size_t GetSelectionSize ();
    size_t GetNumberOfSlots () const;

    const std::vector<size_t>& GetSelection();

    void GetSelectedFiles( std::vector<int>& _dst );

    void ShowAboutDialog();

    void RefreshDiskContent( size_t _slot );

private:
    void CreateMenuBar();
    void CreateControls();

    bool LoadFile( const std::string& _filename, DFSEntry& _dst, std::string& _errorString );

    void BBCString2Host( std::string& _string );
    void HostString2BBC( std::string& _string );

    CMMBFile mMMB;
    CAppWindow* mMainWindow = nullptr;
    CMMBETable* mTable = nullptr;
    Fl_Box* mFilenameBox = nullptr;
    char mBuffer[32] = { 0 };

#ifdef __APPLE__
    Fl_Sys_Menu_Bar* mMenuBar = nullptr;
    int mMenuBarOffset = 0;
    int mModifierKey = FL_COMMAND;
#else
    Fl_Menu_Bar* mMenuBar = nullptr;
    int mMenuBarOffset = 30;
    int mModifierKey = FL_CTRL;
#endif
};