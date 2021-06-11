#pragma once

#include "sequence.h"

class BeatGrid
{
public:
    BeatGrid();
    ~BeatGrid();

    BOOL PointOnGrid(const CPoint& pt) const;
    CPoint GetSubdivision(const CPoint& pt) const;
    CSize GetBoundingSize() const;
    static COLORREF GetInstColor(int i);
    BOOL Create(CDC* pDC, const Sequence& sequence);
    void Draw(CDC* pDC);
    void GetBeatRect(int x, int y, CRect& rc) const;
    void GetBoundingRect(CRect & rc) const;
    void GetDimensions(CRect &rc) const;
        
    enum { CX_SUB = 30 };       // width of each subdivision
    enum { CY_SUB = 20 };       // height of subdivision
    enum { CX_OFFSET = 80 };    // horz. border
    enum { CY_OFFSET = 20 };    // vert. border

private:
    void CreateBitmap(CDC* pDC);
    void PaintBitmap();

    CBitmap m_bitmap;
    CBrush m_bkgBrush;
    CDC m_memDC;
    CPen m_thinPen, m_thickPen;
    CRgn m_region;
    int m_cxGrid, m_cyGrid, m_resolution, m_subdivisions;
};
