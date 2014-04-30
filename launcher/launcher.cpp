#include "launcher.h"

#include "system.h"
#include "injector.h"

#include <wx/filedlg.h>
#include <wx/statline.h>
#include <wx/dirdlg.h>
#include <wx/stdpaths.h>
#include <shlobj.h>

//static CInjector injector;

void Inject(wxString& appPath, wxString& scriptPath)
{
	wxString dllPath = wxGetCwd() + "\\hook_d3d9.dll";
	CInjector injector;
	injector.InjectAuto(dllPath.ToStdWstring(), appPath.ToStdWstring());
}

IMPLEMENT_APP( TouchenablerApp )
IMPLEMENT_CLASS( TouchenablerApp, wxApp )


BEGIN_EVENT_TABLE( TouchenablerApp, wxApp )

////@begin TouchenablerApp event table entries
////@end TouchenablerApp event table entries

END_EVENT_TABLE()


TouchenablerApp::TouchenablerApp()
{
}

bool TouchenablerApp::OnInit()
{    
////@begin TouchenablerApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
	MainWindow* mainWindow = new MainWindow(NULL);
	/* int returnValue = */ mainWindow->ShowModal();

	mainWindow->Destroy();
	// A modal dialog application should return false to terminate the app.
	return false;
////@end TouchenablerApp initialisation

    return true;
}

int TouchenablerApp::OnExit()
{    
////@begin TouchenablerApp cleanup
	return wxApp::OnExit();
////@end TouchenablerApp cleanup
}

void TouchenablerApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}
 
bool TouchenablerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	wxString app;
	wxString script;
    bool silent_mode = /*parser.Found(wxT("s"), &script) && */parser.Found(wxT("a"), &app);
 
    if(silent_mode)
	{
		Inject(app, script);
		return false;
	}
 
    return true;
}

IMPLEMENT_DYNAMIC_CLASS( MainWindow, wxDialog )

BEGIN_EVENT_TABLE( MainWindow, wxDialog )
    EVT_BUTTON( ID_BUTTON, MainWindow::OnButtonAppClick )
    EVT_BUTTON( ID_BUTTON1, MainWindow::OnButtonScriptClick )
    EVT_BUTTON( ID_BUTTON2, MainWindow::OnButtonLaunchClick )
    EVT_BUTTON( ID_BUTTON3, MainWindow::OnButtonShortcutClick )
END_EVENT_TABLE()

MainWindow::MainWindow( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
}


void MainWindow::CreateControls()
{
    MainWindow* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxSize(300, -1), wxBORDER_THEME|wxTAB_TRAVERSAL );
    itemPanel3->SetBackgroundColour(wxColour(255, 255, 255));
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxStaticBitmap* itemStaticBitmap5 = new wxStaticBitmap( itemPanel3, wxID_STATIC, wxBitmap(_T("touchenabler.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, wxSize(300, 50), wxNO_BORDER );
    itemBoxSizer4->Add(itemStaticBitmap5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Target Direct3D version:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString itemChoice8Strings;
    /*itemChoice8Strings.Add(_("Direct3D 9"));
    itemChoice8Strings.Add(_("Direct3D 10"));*/
    itemChoice8Strings.Add(_("Autodetect"));
    wxChoice* itemChoice8 = new wxChoice( itemDialog1, ID_CHOICE, wxDefaultPosition, wxDefaultSize, itemChoice8Strings, 0 );
    itemChoice8->SetStringSelection(_("Autodetect"));
    itemBoxSizer6->Add(itemChoice8, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Target application:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemTextCtrl11 = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemTextCtrl11, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton12 = new wxButton( itemDialog1, ID_BUTTON, _("..."), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer9->Add(itemButton12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, wxID_STATIC, _("Touchenabler script:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemTextCtrl15 = new wxTextCtrl( itemDialog1, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemBoxSizer13->Add(itemTextCtrl15, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /*wxButton* itemButton16 = new wxButton( itemDialog1, ID_BUTTON1, _("..."), wxDefaultPosition, wxSize(30, -1), 0 );
    itemBoxSizer13->Add(itemButton16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);*/

    wxStaticLine* itemStaticLine17 = new wxStaticLine( itemDialog1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(itemStaticLine17, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer18, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButton19 = new wxButton( itemDialog1, ID_BUTTON2, _("Launch"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton19->SetDefault();
    itemBoxSizer18->Add(itemButton19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton20 = new wxButton( itemDialog1, ID_BUTTON3, _("Create shortcut"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemButton20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Connect events and objects
    itemStaticBitmap5->Connect(wxID_STATIC, wxEVT_LEFT_DOWN, wxMouseEventHandler(MainWindow::OnLeftDown), NULL, this);
}

void MainWindow::OnButtonAppClick( wxCommandEvent& event )
{
	wxFileDialog openFileDialog(this, _("Select application"), "", "", "Application (*.exe)|*.exe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	itemTextCtrl11->SetValue(openFileDialog.GetPath());

	itemTextCtrl15->SetValue(openFileDialog.GetPath().substr(0, openFileDialog.GetPath().find_last_of(".")).append(".tes"));
}

void MainWindow::OnButtonScriptClick( wxCommandEvent& event )
{
	/*wxFileDialog openFileDialog(this, _("Select script"), "", "", "TouchEnabler scripts (*.tes)|*.tes", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	itemTextCtrl15->SetValue(openFileDialog.GetPath());*/
}


void MainWindow::OnButtonLaunchClick( wxCommandEvent& event )
{
	Inject(itemTextCtrl11->GetValue(), wxString());
}

void MainWindow::OnLeftDown( wxMouseEvent& event )
{
	wxMessageBox(_("TouchEnabler (alpha)\nProject URL: github.com/lenny93/TouchEnabler"), _("About"));
}

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCSTR lpszPathLink, LPCWSTR lpszDesc, LPCWSTR args)
{
    HRESULT hres;
    IShellLink* psl;
 
    // Get a pointer to the IShellLink interface.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
 
        // Set the path to the shortcut target and add the description.
        psl->SetPath(lpszPathObj);
		psl->SetArguments(args);
        psl->SetDescription(lpszDesc);
 
        // Query IShellLink for the IPersistFile interface for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
 
        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH];
 
            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);
         
            // Add code here to check return value from MultiByteWideChar
            // for success.
 
            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

void MainWindow::OnButtonShortcutClick( wxCommandEvent& event )
{
    wxFileDialog openFileDialog(this, _("Save shortcut"), "", "", "Shortcut (*.lnk)|*.lnk", wxFD_SAVE);

	if(openFileDialog.ShowModal() == wxID_OK)
	{
		wxString lnk = openFileDialog.GetPath();
		if(lnk.Right(4) != ".lnk") lnk.Append(".lnk");

		wxString args = "\" -a \"" + itemTextCtrl11->GetValue() + "\"";
		CreateLink(wxStandardPaths::Get().GetExecutablePath(), lnk.c_str(), L"Launch with TouchEnabler", args);
	}

}


