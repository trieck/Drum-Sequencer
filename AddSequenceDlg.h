#pragma once

#include "resource.h"

class AddSequenceDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AddSequenceDlg)

public:
    AddSequenceDlg(CWnd* pParent = nullptr);   // standard constructor
    virtual ~AddSequenceDlg();

    int sequenceSize() const;
    int timeSigTop() const;
    int timeSigBottom() const;
    int resolution() const;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    BOOL OnInitDialog() override;
    void OnOK() override;
private:
    CComboBox m_seqSize, m_tsTop, m_tsBottom, m_resolution;
    int m_nSeqSize = 0, m_nTsTop = 0, m_nTsBottom = 0, m_nResolution = 0;
};
