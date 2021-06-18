// DrumSequencerDoc.cpp : implementation of the CDrumSequencerDoc class
//

#include "stdafx.h"
#include "DrumSequencer.h"
#include "DrumSequencerDoc.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "AddSequenceDlg.h"

// CDrumSequencerDoc

IMPLEMENT_DYNCREATE(CDrumSequencerDoc, CDocument)

CDrumSequencerDoc::CDrumSequencerDoc()
{
}

BOOL CDrumSequencerDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    theApp.Stop();

    return TRUE;
}

// CDrumSequencerDoc serialization

void CDrumSequencerDoc::Serialize(CArchive& ar)
{
    m_sequence.Serialize(ar);
}

// CDrumSequencerDoc diagnostics

#ifdef _DEBUG
void CDrumSequencerDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDrumSequencerDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CDrumSequencerDoc commands

void CDrumSequencerDoc::ToggleSub(const CPoint& pt)
{
    m_sequence.toggleSub(pt);
    SetModifiedFlag();
    UpdateAllViews(nullptr, MAKELONG(pt.x, pt.y), &m_sequence);
}

void CDrumSequencerDoc::DeleteContents()
{
    m_sequence.clear();
    UpdateAllViews(nullptr, MAKELONG(-1, -1), &m_sequence);
}

BEGIN_MESSAGE_MAP(CDrumSequencerDoc, CDocument)
        ON_COMMAND(ID_SEQUENCER_PLAY, &CDrumSequencerDoc::OnSequencerPlay)
        ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CDrumSequencerDoc::OnUpdateFileSave)
        ON_COMMAND(ID_TOGGLE_PLAY, &CDrumSequencerDoc::OnTogglePlay)
        ON_COMMAND(ID_REENCODE_FRONT, &CDrumSequencerDoc::OnReencodeFront)
        ON_COMMAND(ID_REENCODE_BACK, &CDrumSequencerDoc::OnReencodeBack)
        ON_COMMAND(ID_SEQUENCER_ADD_SEQUENCE, &CDrumSequencerDoc::OnAddSequence)
        ON_UPDATE_COMMAND_UI(IDS_SEQUENCE, &CDrumSequencerDoc::OnUpdateSequence)

END_MESSAGE_MAP()

void CDrumSequencerDoc::OnSequencerPlay()
{
    theApp.Play(m_sequence);
}

void CDrumSequencerDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsModified());
}

void CDrumSequencerDoc::OnUpdateSequence(CCmdUI* pCmdUI)
{
    CString str;
    if (m_sequence.subdivisions() > 0) {
        auto ts = m_sequence.timeSig();
        str.Format(_T("%d bar(s) of %d/%d; Resolution: 1/%d"), 
            m_sequence.bars(), ts.first, ts.second, m_sequence.resolution());
        pCmdUI->Enable(TRUE);
    } else {
        str.Format(_T("Sequence Length"));
        pCmdUI->Enable(FALSE);
    }

    pCmdUI->SetText(str);

    auto* pBar = dynamic_cast<CMFCStatusBar*>(pCmdUI->m_pOther);
    ASSERT_VALID(pBar);

    auto width = GetTextWidth(pBar, str);
    pBar->SetPaneWidth(1, width);
}

void CDrumSequencerDoc::OnTogglePlay()
{
    if (theApp.IsPlaying()) {
        theApp.Stop();
    } else {
        theApp.Play(m_sequence);
    }
}

void CDrumSequencerDoc::OnReencodeFront()
{
    theApp.OutFront(m_sequence);
}

void CDrumSequencerDoc::OnReencodeBack()
{
    theApp.OutBack(m_sequence);
}

void CDrumSequencerDoc::OnAddSequence()
{
    AddSequenceDlg dlg;
    if (dlg.DoModal() == IDOK) {
        auto size = dlg.sequenceSize();
        auto tsTop = dlg.timeSigTop();
        auto tsBottom = dlg.timeSigBottom();
        auto resolution = dlg.resolution();

        m_sequence.create(size, tsTop, tsBottom, resolution);

        UpdateAllViews(nullptr, MAKELONG(-1,-1), &m_sequence);
    }
}
