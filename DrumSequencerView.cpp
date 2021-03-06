// DrumSequencerView.cpp : implementation of the CDrumSequencerView class
//

#include "stdafx.h"
#include "DrumSequencerDoc.h"
#include "DrumSequencerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDrumSequencerView

IMPLEMENT_DYNCREATE(CDrumSequencerView, CScrollView)

BEGIN_MESSAGE_MAP(CDrumSequencerView, CScrollView)
        ON_WM_LBUTTONDOWN()
        ON_WM_ERASEBKGND()
        ON_WM_CREATE()
        ON_WM_MOUSEMOVE()
        ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

#define BKGND_COLOR RGB(0xEE, 0xEE, 0xEE)

// CDrumSequencerView construction/destruction

const LPCTSTR Instruments[] = {
    _T("Closed Hi-Hat"),
    _T("Pedal Hi-Hat"),
    _T("Open Hi-Hat"),
    _T("Crash Cymbal"),
    _T("Ride Cymbal"),
    _T("Snare Drum"),
    _T("High Tom"),
    _T("Mid Tom"),
    _T("Low Tom"),
    _T("Bass Drum")
};

BOOL CDrumSequencerView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CScrollView::PreCreateWindow(cs);
}

// CDrumSequencerView drawing

void CDrumSequencerView::OnDraw(CDC* pDC)
{
    auto* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    const auto& sequence = pDoc->GetSequence();
    if (sequence.subdivisions() > 0) {
        DrawInstruments(pDC);
        m_grid.Draw(pDC);

        DrawBeats(pDC, pDoc->GetSequence());
    }
}

// CDrumSequencerView printing

BOOL CDrumSequencerView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

#ifdef _DEBUG

CDrumSequencerDoc* CDrumSequencerView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrumSequencerDoc)));
    return dynamic_cast<CDrumSequencerDoc*>(m_pDocument);
}
#endif //_DEBUG

BOOL CDrumSequencerView::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    pDC->SetBkMode(TRANSPARENT);
    pDC->GetClipBox(&rc);
    pDC->FillRect(&rc, &m_bkgndBrush);
    return TRUE;
}

int CDrumSequencerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CScrollView::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_bkgndBrush.CreateSolidBrush(BKGND_COLOR))
        return -1;

    // Create the font
    CClientDC dc(this);
    if (!m_font.CreatePointFont(70, _T("Verdana"), &dc)) {
        return -1;
    }

    return 0;
}

void CDrumSequencerView::DrawInstruments(CDC* pDC)
{
    CRect rc;
    pDC->GetClipBox(rc);

    const auto pOldFont = pDC->SelectObject(&m_font);

    auto y = BeatGrid::CY_OFFSET + 2;
    for (auto Instrument : Instruments) {
        const auto sz = pDC->GetTextExtent(Instrument);
        const int x = BeatGrid::CX_OFFSET - sz.cx - 2;
        pDC->TextOut(x, y, Instrument);
        y += BeatGrid::CY_SUB;
    }

    pDC->SelectObject(pOldFont);
}

void CDrumSequencerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if (pHint != nullptr) {
        CClientDC dc(this);

        auto* pSeq = dynamic_cast<Sequence*>(pHint);
        ASSERT_VALID(pSeq);

        short x = LOWORD(lHint), y = HIWORD(lHint);
        if (x >= 0 && y >= 0) { // beat update
            CRect rc;
            m_grid.GetBeatRect(x, y, rc);
            InvalidateRect(rc);
        } else if (m_grid.Create(&dc, *pSeq)) { // new grid created
            SetScrollSizes(MM_TEXT, m_grid.GetBoundingSize());
            Invalidate();
        } else {
            Invalidate();
        }
    } else {
        CScrollView::OnUpdate(pSender, lHint, pHint);
    }
}

void CDrumSequencerView::DrawBeats(CDC* pDC, const Sequence& seq)
{
    CRect rc;

    auto resolution = seq.resolution();

    for (auto i = 0; i < Sequence::NINSTRUMENTS; i++) {
        const auto color = BeatGrid::GetInstColor(i);
        for (auto j = 0; j < seq.subdivisions(); j++) {
            if (seq.beat(j, i)) {
                m_grid.GetBeatRect(j, i, rc);
                rc.DeflateRect(1, 1, 0, 0);

                if (j % resolution == resolution - 1) {
                    rc.right -= 1; // thick pen
                }
                if (i == Sequence::NINSTRUMENTS - 1) {
                    rc.bottom -= 1;
                }
                pDC->FillSolidRect(rc, color);
            }
        }
    }
}

void CDrumSequencerView::OnLButtonDown(UINT nFlags, CPoint point)
{
    auto pDoc = GetDocument();

    if (m_grid.PointOnGrid(point)) {
        auto sub = m_grid.GetSubdivision(point);
        if (sub != m_activeSub) {
            m_activeSub = sub;
            pDoc->ToggleSub(sub);
        }
    }

    CScrollView::OnLButtonDown(nFlags, point);
}

void CDrumSequencerView::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_activeSub = {-1, -1};
    CScrollView::OnLButtonUp(nFlags, point);
}

void CDrumSequencerView::OnMouseMove(UINT nFlags, CPoint point)
{
    if ((nFlags & MK_LBUTTON) == MK_LBUTTON) {
        OnLButtonDown(nFlags, point);
    }

    CScrollView::OnMouseMove(nFlags, point);
}

void CDrumSequencerView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    SetScrollSizes(MM_TEXT, m_grid.GetBoundingSize());
}
