// DrumSequencer.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DrumSequencer.h"
#include "MainFrm.h"
#include "DrumSequencerDoc.h"
#include "DrumSequencerView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDrumSequencerApp

BEGIN_MESSAGE_MAP(CDrumSequencerApp, CWinAppEx)
        ON_COMMAND(ID_APP_ABOUT, &CDrumSequencerApp::OnAppAbout)
        // Standard file based document commands
        ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
        ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
        // Standard print setup command
        ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
        ON_COMMAND(ID_SEQUENCER_STOP, &CDrumSequencerApp::OnSequencerStop)
        ON_UPDATE_COMMAND_UI(ID_SEQUENCER_PLAY, &CDrumSequencerApp::OnUpdateSequencerPlay)
        ON_UPDATE_COMMAND_UI(ID_SEQUENCER_STOP, &CDrumSequencerApp::OnUpdateSequencerStop)
        ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CDrumSequencerApp::OnUpdateAddSequence)
END_MESSAGE_MAP()

static auto constexpr DEFAULT_TEMPO = 120;

CDrumSequencerApp::CDrumSequencerApp()
{
    m_bHiColorIcons = TRUE;
    SetAppID(_T("DrumSequencer.AppID.NoVersion"));
}

CDrumSequencerApp theApp;

BOOL CDrumSequencerApp::InitInstance()
{
    if (!m_sequencer.Initialize()) {
        return FALSE;
    }

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    EnableTaskbarInteraction(FALSE);

    SetRegistryKey(_T("Rieck Enterprises"));
    LoadStdProfileSettings(4); // Load standard INI file options (including MRU)

    auto tempo = GetProfileInt(_T("Settings"), _T("Tempo"), DEFAULT_TEMPO);
    m_sequencer.SetTempo(tempo);

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    auto pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CDrumSequencerDoc),
        RUNTIME_CLASS(CMainFrame), // main SDI frame window
        RUNTIME_CLASS(CDrumSequencerView));
    if (!pDocTemplate) {
        return FALSE;
    }

    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    if (!ProcessShellCommand(cmdInfo)) {
        return FALSE;
    }

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

void CDrumSequencerApp::SetTempo(short bpm)
{
    m_sequencer.SetTempo(bpm);
}

short CDrumSequencerApp::Tempo() const
{
    return m_sequencer.Tempo();
}

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CDrumSequencerApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

void CDrumSequencerApp::PreLoadState()
{
}

void CDrumSequencerApp::LoadCustomState()
{
}

void CDrumSequencerApp::SaveCustomState()
{
}

int CDrumSequencerApp::ExitInstance()
{
    if (m_sequencer.HasStream()) {
        WriteProfileInt(_T("Settings"), _T("Tempo"), m_sequencer.Tempo());
    }

    m_sequencer.Close();

    return CWinAppEx::ExitInstance();
}

BOOL CDrumSequencerApp::IsPlaying() const
{
    return m_sequencer.IsPlaying();
}

BOOL CDrumSequencerApp::Play(const Sequence& sequence)
{
    return m_sequencer.Play(sequence);
}

BOOL CDrumSequencerApp::OutFront(const Sequence& sequence)
{
    return m_sequencer.OutFront(sequence);
}

BOOL CDrumSequencerApp::OutBack(const Sequence& sequence)
{
    return m_sequencer.OutBack(sequence);
}

BOOL CDrumSequencerApp::Stop()
{
    return m_sequencer.Stop();
}

void CDrumSequencerApp::OnSequencerStop()
{
    (void)Stop();
}

void CDrumSequencerApp::OnUpdateSequencerPlay(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_sequencer.IsPlaying());
}

void CDrumSequencerApp::OnUpdateSequencerStop(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_sequencer.IsPlaying());
}

void CDrumSequencerApp::OnUpdateAddSequence(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_sequencer.IsPlaying());
}

int GetTextWidth(CWnd* pWnd, LPCTSTR str)
{
    ASSERT_VALID(pWnd);
    ASSERT(IsWindow(*pWnd));
    ASSERT(str != NULL);

    auto* pDC = pWnd->GetDC();
    ASSERT_VALID(pDC);

    auto* pFont = pWnd->GetFont();
    ASSERT_VALID(pFont);

    auto* pOldFont = pDC->SelectObject(pFont);

    SIZE sz;
    GetTextExtentPoint32(*pDC, str, static_cast<int>(_tcslen(str)), &sz);

    pDC->SelectObject(pOldFont);

    pWnd->ReleaseDC(pDC);

    return sz.cx;
}