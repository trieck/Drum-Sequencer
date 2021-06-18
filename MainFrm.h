// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "DrumSequencerToolbar.h"
#include "DrumSequencerMenubar.h"
#include "SettingsPane.h"

class CMainFrame : public CFrameWndEx
{
protected: // create from serialization only
    CMainFrame() = default;
DECLARE_DYNCREATE(CMainFrame)

    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

    // Implementation
    virtual ~CMainFrame() = default;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

protected: // control bar embedded members
    DrumSequencerMenubar m_wndMenuBar;
    DrumSequencerToolbar m_wndToolBar;
    SettingsPane m_wndSettingsPane;

    CMFCStatusBar m_wndStatusBar;
    CMFCToolBarImages m_UserImages;

    // Generated message map functions
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

DECLARE_MESSAGE_MAP()
};
