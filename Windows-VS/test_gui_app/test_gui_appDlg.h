// test_gui_appDlg.h
//

#pragma once
#include "afxwin.h"


// Ctest_gui_appDlg
class Ctest_gui_appDlg : public CDialog
{
//
public:
    Ctest_gui_appDlg(CWnd* pParent = NULL);	//

    enum { IDD = IDD_TEST_GUI_APP_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    DECLARE_MESSAGE_MAP()

public:
    CButton m_findDevBtn;
    CButton m_openDevBtn;
    CButton m_closeDevBtn;
    CComboBox m_devListBox;
    CComboBox m_pinIndexList;
    CButton m_setPinWorkModeBtn;
    CButton m_pullupFlag;
    CButton m_openWorkLed;
    CButton m_closeWorkLed;
    CButton m_outputSelBtn;
    CButton m_inputSelBtn;
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton6();
    afx_msg void OnBnClickedButton7();
    afx_msg void OnBnClickedfinddevicebtn();
    afx_msg void OnBnClickedCancel();

private:
    intptr_t     m_currentUSBIoHandle;
    struct usb_io_device_info *m_devList;
public:
    afx_msg void OnBnClickedButton8();
    CButton m_hLevelFlag;
};
