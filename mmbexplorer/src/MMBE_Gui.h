#pragma once

#include <string>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include "MMBFile.h"

#ifdef __APPLE__
    class Fl_Sys_Menu_Bar;
#else
    class Fl_Menu_Bar;
#endif
class Fl_Table;
class Fl_Pixmap;
class RateTable;

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

private:
    unsigned char* mTestData = nullptr;
    size_t mTestDataSize = 0;
};

//******************************************
//* CMMBETable class
//******************************************
class CMMBETable : public Fl_Table
{
public:
    CMMBETable( CMMBFile* _mmb, int _x, int _y, int _w, int _h, const char* _label = 0 );
    virtual ~CMMBETable();

    void draw_cell( TableContext context, int _row = 0, int _col = 0, int _x = 0, int _y = 0, int _w = 0, int _h = 0 );

    void SelectSlot( size_t _slot );

private:
    void DrawHeader( const char* _string, int _x, int _y, int _w, int _h );
    void DrawData( const char* _string, Fl_Pixmap* _icon, int _x, int _y, int _w, int _h, bool _selected );
    void DrawUnused( int _x, int _y, int _w, int _h );
    int  handle( int _event );

    Fl_Pixmap* mIconEmpty    = nullptr;
    Fl_Pixmap* mIconUnlocked = nullptr;
    Fl_Pixmap* mIconLocked   = nullptr;

    CMMBFile* mMMB = nullptr;

    size_t mSelectedSlot = (size_t)-1;
};

//******************************************
//* CMMBEGui class
//******************************************
class CMMBEGui
{
public:
    CMMBEGui( int _w, int _h, const char* _label );
    virtual ~CMMBEGui();

    int Run();
    void Stop();

    void OpenMMB  ( const std::string& _filename ); 
    void CreateMMB( const std::string& _filename );
    void CloseMMB ();

private:
    void CreateMenuBar();
    void CreateControls();

    CMMBFile mMMB;
    CAppWindow* mMainWindow = nullptr;
    CMMBETable* mTable = nullptr;
    Fl_Box* mFilenameBox = nullptr;
    RateTable* pTable = nullptr;
    char mBuffer[32] = { 0 };

#ifdef __APPLE__
    Fl_Sys_Menu_Bar* mMenuBar = nullptr;
    int mMenuBarOffset = 0;
    int mModifierKey = FL_COMMAND;
#else
    Fl_Menu_Bar* mMenuBar = nullptr;
    int mMenuBarOffset = 30;
    int mModifierKey = FL_ALT;
#endif
};