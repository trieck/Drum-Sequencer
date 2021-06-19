#include "StdAfx.h"
#include "BeatGrid.h"

constexpr auto COLOR_GRID = RGB(0xc0, 0xc0, 0xc0);
constexpr auto COLOR_BAR = RGB(0x80, 0x80, 0x80);
constexpr auto COLOR_BKGND = RGB(0xEE, 0xEE, 0xFF);

constexpr COLORREF INST_COLORS[Sequence::NINSTRUMENTS] = {
    RGB(192, 0, 0),
    RGB(255, 0, 0),
    RGB(255, 192, 0),
    RGB(255, 255, 0),
    RGB(146, 208, 80),
    RGB(0, 176, 80),
    RGB(0, 176, 240),
    RGB(0, 112, 192),
    RGB(0, 32, 96),
    RGB(112, 48, 160)
};

BeatGrid::BeatGrid() : m_cxGrid(0), m_cyGrid(0), m_resolution(0), m_subdivisions(0)
{
    m_bkgBrush.CreateSolidBrush(COLOR_BKGND);
    m_thinPen.CreatePen(PS_SOLID, 0, COLOR_GRID);
    m_thickPen.CreatePen(PS_SOLID, 2, COLOR_BAR);
}

BOOL BeatGrid::Create(CDC* pDC, const Sequence& sequence)
{
    ASSERT_VALID(pDC);

    if (m_resolution == sequence.resolution() &&
        m_subdivisions == sequence.subdivisions()) {
        return FALSE;   // nothing to re-create
    }

    m_ts = sequence.timeSig();
    m_resolution = sequence.resolution();
    m_subdivisions = sequence.subdivisions();
            
    m_cxGrid = CX_SUB * m_subdivisions;     // width of grid
    m_cyGrid = m_subdivisions == 0 ? 0 :
        CY_SUB * Sequence::NINSTRUMENTS;    // height of grid

    m_region.DeleteObject();
    m_memDC.DeleteDC();
    m_bitmap.DeleteObject();

    if (m_subdivisions > 0) {
        CreateBitmap(pDC);
    }

    return TRUE;
}

void BeatGrid::Draw(CDC* pDC)
{
    if (m_subdivisions > 0) {
        CRect rc;
        pDC->GetClipBox(rc);

        auto* pOldBitmap = m_memDC.SelectObject(&m_bitmap);

        auto aRect(rc);
        aRect.OffsetRect(-CX_OFFSET, -CY_OFFSET);

        pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC,
            aRect.left, aRect.top, SRCCOPY);

        m_memDC.SelectObject(pOldBitmap);
    }
}

void BeatGrid::CreateBitmap(CDC* pDC)
{
    CRect rcRegion;
    rcRegion.right = rcRegion.left + m_cxGrid;
    rcRegion.bottom = rcRegion.top + m_cyGrid;

    if (!m_region.CreateRectRgnIndirect(&rcRegion))
        AfxThrowResourceException();

    if (!m_memDC.CreateCompatibleDC(pDC))
        AfxThrowResourceException();

    m_memDC.SetMapMode(pDC->GetMapMode());

    if (!m_bitmap.CreateCompatibleBitmap(pDC, rcRegion.Width(),
                                         rcRegion.Height())) {
        AfxThrowResourceException();
    }

    PaintBitmap();
}

void BeatGrid::PaintBitmap()
{
    CRect rcBoard;
    m_region.GetRgnBox(rcBoard);

    auto* pOldBitmap = m_memDC.SelectObject(&m_bitmap);

    auto pOldBrush = static_cast<CBrush*>(m_memDC.SelectObject(&m_bkgBrush));
    auto pOldPen = static_cast<CPen*>(m_memDC.SelectObject(&m_thinPen));

    m_memDC.Rectangle(rcBoard);

    auto cx = rcBoard.Width() / m_subdivisions;
    auto cy = rcBoard.Height() / Sequence::NINSTRUMENTS;

    CPoint ptStart(rcBoard.left + cx, rcBoard.top);
    m_memDC.MoveTo(ptStart);

    auto step = m_resolution / m_ts.second;
    auto bar = step * m_ts.first;

    // Draw vertical lines
    for (auto i = 1; ptStart.x < rcBoard.right; ++i) {
        if (i % bar == 0) {
            m_memDC.SelectObject(&m_thickPen);
        } else {
            m_memDC.SelectObject(&m_thinPen);
        }
        auto ptEnd = CPoint(ptStart.x, rcBoard.bottom - 1);
        m_memDC.LineTo(ptEnd);
        m_memDC.MoveTo(ptStart.x += cx, ptStart.y);
    }

    m_memDC.SelectObject(&m_thinPen);

    // Draw horizontal lines
    ptStart = CPoint(rcBoard.left, rcBoard.top + cy);
    m_memDC.MoveTo(ptStart);

    while (ptStart.y < rcBoard.bottom) {
        auto ptEnd = CPoint(rcBoard.right - 1, ptStart.y);
        m_memDC.LineTo(ptEnd);
        m_memDC.MoveTo(ptStart.x, ptStart.y += cy);
    }

    m_memDC.SelectObject(pOldBrush);
    m_memDC.SelectObject(pOldPen);
    m_memDC.SelectObject(pOldBitmap);
}

void BeatGrid::GetDimensions(CRect& rc) const
{
    rc.SetRectEmpty();
    rc.right = m_cxGrid;
    rc.bottom = m_cyGrid;
}

void BeatGrid::GetBoundingRect(CRect& rc) const
{
    GetDimensions(rc);
    rc.right += 2 * CX_OFFSET;
    rc.bottom += 2 * CY_OFFSET;
}

BOOL BeatGrid::PointOnGrid(const CPoint& pt) const
{
    CRect rc;
    GetDimensions(rc);
    rc.OffsetRect(CX_OFFSET, CY_OFFSET);
    return rc.PtInRect(pt);
}

CPoint BeatGrid::GetSubdivision(const CPoint& pt) const
{
    if (m_subdivisions == 0) {
        return { -1, -1 };
    }

    auto aPoint(pt);
    aPoint.x -= CX_OFFSET;
    aPoint.y -= CY_OFFSET;

    CPoint sub;
    sub.x = min(max(0, (aPoint.x / CX_SUB)), m_subdivisions-1);
    sub.y = min(max(0, (aPoint.y / CY_SUB)), Sequence::NINSTRUMENTS-1);

    return sub;
}

CSize BeatGrid::GetBoundingSize() const
{
    return { m_cxGrid + 2 * CX_OFFSET, m_cyGrid + 2 * CY_OFFSET };
}

void BeatGrid::GetBeatRect(int x, int y, CRect& rc) const
{
    rc.SetRectEmpty();

    if (m_subdivisions > 0) {
        x = x % m_subdivisions;
        y = y % Sequence::NINSTRUMENTS;

        rc.left = CX_OFFSET + x * CX_SUB;
        rc.top = CY_OFFSET + y * CY_SUB;
        rc.right = rc.left + CX_SUB;
        rc.bottom = rc.top + CY_SUB;
    }
}

COLORREF BeatGrid::GetInstColor(int i)
{
    return INST_COLORS[i % Sequence::NINSTRUMENTS];
}
