// DrumSequencer.h : main header file for the DrumSequencer application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "sequencer.h"

class CDrumSequencerApp : public CWinAppEx
{
public:
    CDrumSequencerApp();
    virtual ~CDrumSequencerApp() = default;

    // Overrides
    BOOL InitInstance() override;
    void SetTempo(short bpm);
    short Tempo() const;

    // Implementation
    UINT m_nAppLook{};
    BOOL m_bHiColorIcons{};

    void PreLoadState() override;
    void LoadCustomState() override;
    void SaveCustomState() override;

    afx_msg void OnAppAbout();
DECLARE_MESSAGE_MAP()
private:
    Sequencer m_sequencer;
public:
    int ExitInstance() override;
    BOOL IsPlaying() const;
    BOOL Play(const Sequence& sequence);
    BOOL OutFront(const Sequence& sequence);
    BOOL OutBack(const Sequence& sequence);
    BOOL Stop();

    afx_msg void OnSequencerStop();
    afx_msg void OnUpdateSequencerPlay(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSequencerStop(CCmdUI* pCmdUI);
    afx_msg void OnUpdateAddSequence(CCmdUI* pCmdUI);
};

extern CDrumSequencerApp theApp;

int GetTextWidth(CWnd* pWnd, LPCTSTR str);
