#ifndef _LAUNCHER_H
#define _LAUNCHER_H

#include <windows.h>
#include <wx/wx.h>
#include <wx/cmdline.h>

class TouchenablerApp: public wxApp
{    
    DECLARE_CLASS( TouchenablerApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    TouchenablerApp();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

};

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
     { wxCMD_LINE_SWITCH, "a", "app", "Target application path",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
     { wxCMD_LINE_SWITCH, "s", "script", "TouchEnabler script path",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY  },
     { wxCMD_LINE_NONE }
};
 

DECLARE_APP(TouchenablerApp)

#define ID_MAIN 10000
#define ID_PANEL 10001
#define ID_CHOICE 10002
#define ID_TEXTCTRL 10003
#define ID_BUTTON 10004
#define ID_TEXTCTRL1 10005
#define ID_BUTTON1 10006
#define ID_BUTTON2 10007
#define ID_BUTTON3 10008
#define SYMBOL_MAIN_STYLE wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_MAIN_TITLE _("TouchEnabler alpha")
#define SYMBOL_MAIN_SIZE wxSize(400, 300)
#define SYMBOL_MAIN_POSITION wxDefaultPosition

class MainWindow : public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( MainWindow )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
	MainWindow() {};
    MainWindow( wxWindow* parent, wxWindowID id = ID_MAIN, const wxString& caption = SYMBOL_MAIN_TITLE, 
		const wxPoint& pos = SYMBOL_MAIN_POSITION, const wxSize& size = SYMBOL_MAIN_SIZE, long style = SYMBOL_MAIN_STYLE);

    /// Creates the controls and sizers
    void CreateControls();


    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnButtonAppClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
    void OnButtonScriptClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
    void OnButtonLaunchClick( wxCommandEvent& event );

    void OnButtonShortcutClick( wxCommandEvent& event );

	void OnLeftDown( wxMouseEvent& event );

	wxTextCtrl* itemTextCtrl11;
	wxTextCtrl* itemTextCtrl15;


};

#endif