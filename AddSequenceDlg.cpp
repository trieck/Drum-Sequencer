#include "stdafx.h"
#include "AddSequenceDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(AddSequenceDlg, CDialogEx)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(AddSequenceDlg, CDialogEx)

AddSequenceDlg::AddSequenceDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_ADDSEQUENCE, pParent)
{
}

AddSequenceDlg::~AddSequenceDlg()
{
}

int AddSequenceDlg::sequenceSize() const
{
    return m_nSeqSize;
}

int AddSequenceDlg::timeSigTop() const
{
    return m_nTsTop;
}

int AddSequenceDlg::timeSigBottom() const
{
    return m_nTsBottom;
}

int AddSequenceDlg::resolution() const
{
    return m_nResolution;
}

void AddSequenceDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEQUENCE_SIZE, m_seqSize);
    DDX_Control(pDX, IDC_TS_TOP, m_tsTop);
    DDX_Control(pDX, IDC_TS_BOTTOM, m_tsBottom);
    DDX_Control(pDX, IDC_RESOLUTION, m_resolution);
}

BOOL AddSequenceDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_seqSize.SetCurSel(0);     // 1
    m_tsTop.SetCurSel(2);       // 4
    m_tsBottom.SetCurSel(1);    // 4
    m_resolution.SetCurSel(4);  // 16

    return TRUE;
}

void AddSequenceDlg::OnOK()
{
    UpdateData(TRUE);

    m_nSeqSize = m_seqSize.GetCurSel() + 1;
    m_nTsTop = m_tsTop.GetCurSel() + 2;
    m_nTsBottom = 1 << (m_tsBottom.GetCurSel() + 1);
    m_nResolution = 1 << m_resolution.GetCurSel();

    EndDialog(IDOK);
}
