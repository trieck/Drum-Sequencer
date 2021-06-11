#pragma once

#include "BeatGrid.h"

class CDrumSequencerView : public CScrollView
{
protected: // create from serialization only
    CDrumSequencerView() = default;
    virtual ~CDrumSequencerView() = default;

DECLARE_DYNCREATE(CDrumSequencerView)

    CDrumSequencerDoc* GetDocument() const;

    void OnDraw(CDC* pDC) override; // overridden to draw this view
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
    BOOL OnPreparePrinting(CPrintInfo* pInfo) override;

DECLARE_MESSAGE_MAP()
private:
    BeatGrid m_grid;
    CBrush m_bkgndBrush;
    CFont m_font;
    CPoint m_activeSub{-1, -1};
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    void OnInitialUpdate() override;
    void DrawBeats(CDC* pDC, const Sequence& seq);
    void DrawInstruments(CDC* pDC);
    void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) override;
};

#ifndef _DEBUG  // debug version in DrumSequencerView.cpp
inline CDrumSequencerDoc* CDrumSequencerView::GetDocument() const
{
    return reinterpret_cast<CDrumSequencerDoc*>(m_pDocument);
}
#endif
