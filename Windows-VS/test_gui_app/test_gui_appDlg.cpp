//
// Test app for 'usb_io_interface' device
//

#include "stdafx.h"
#include "test_gui_app.h"
#include "test_gui_appDlg.h"

#include "usb_io_device.h"
#pragma comment(lib, "usb_io_interface.lib")

#ifndef USB_IO16_MAX_PIN_NUM
#define USB_IO16_MAX_PIN_NUM 16
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define  CHINESE_LANG 1 //Chinese Or English
//TODO normal localization (MUI resources or string table...)
//TODO all messages are RED color, this is confusing. Use different colors for success vs errors.
//  below in Ctest_gui_appDlg::OnCtlColor()

Ctest_gui_appDlg::Ctest_gui_appDlg(CWnd* pParent /*=NULL*/)
    : CDialog(Ctest_gui_appDlg::IDD, pParent),
    m_devList(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest_gui_appDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_findDeviceBtn, m_findDevBtn);
    DDX_Control(pDX, IDC_BUTTON2, m_openDevBtn);
    DDX_Control(pDX, IDC_BUTTON3, m_closeDevBtn);
    DDX_Control(pDX, IDC_COMBO1, m_devListBox);
    DDX_Control(pDX, IDC_COMBO2, m_pinIndexList);
    DDX_Control(pDX, IDC_BUTTON4, m_setPinWorkModeBtn);
    DDX_Control(pDX, IDC_CHECK1, m_pullupFlag);
    DDX_Control(pDX, IDC_BUTTON6, m_openWorkLed);
    DDX_Control(pDX, IDC_BUTTON7, m_closeWorkLed);
    DDX_Control(pDX, IDC_RADIO1, m_outputSelBtn);
    DDX_Control(pDX, IDC_RADIO2, m_inputSelBtn);
    DDX_Control(pDX, IDC_CHECK2, m_hLevelFlag);
}

BEGIN_MESSAGE_MAP(Ctest_gui_appDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_RADIO1, &Ctest_gui_appDlg::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_RADIO2, &Ctest_gui_appDlg::OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_BUTTON3, &Ctest_gui_appDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON2, &Ctest_gui_appDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON4, &Ctest_gui_appDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON6, &Ctest_gui_appDlg::OnBnClickedButton6)
    ON_BN_CLICKED(IDC_BUTTON7, &Ctest_gui_appDlg::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_findDeviceBtn, &Ctest_gui_appDlg::OnBnClickedfinddevicebtn)
    ON_BN_CLICKED(IDCANCEL, &Ctest_gui_appDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON8, &Ctest_gui_appDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// Ctest_gui_appDlg

BOOL Ctest_gui_appDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    this->SetWindowText(_T("Test for usb_io_interface device"));
    SetIcon(m_hIcon, TRUE);			// big icon
    SetIcon(m_hIcon, FALSE);		// small icon

    m_inputSelBtn.SetCheck(TRUE);
    m_pullupFlag.SetCheck(TRUE);

    m_closeDevBtn.EnableWindow(FALSE);
    m_setPinWorkModeBtn.EnableWindow(FALSE);
    m_openWorkLed.EnableWindow(FALSE);
    m_closeWorkLed.EnableWindow(FALSE);

    for (int i = 0; i < (USB_IO16_MAX_PIN_NUM+2); i++)
        // Two more added to test error detection, they should return errors! -- pa
    {
        CString str;
        str.AppendFormat(_T("%i"), i);
        m_pinIndexList.AddString(str.GetString());
    }
    m_pinIndexList.SetCurSel(0);

    if (usb_io_init() != 0)
    {
        ::MessageBox(NULL, _T("usb_io library init failure"), 0, MB_OK);
        exit(1);
    }

    OnBnClickedfinddevicebtn();

    return TRUE;
}


void Ctest_gui_appDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        //
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        //
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

HCURSOR Ctest_gui_appDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void Ctest_gui_appDlg::OnBnClickedRadio1()
{
    //select output mode for pin
    m_outputSelBtn.SetCheck(TRUE);
    m_inputSelBtn.SetCheck(FALSE);

    m_pullupFlag.EnableWindow(FALSE);
    m_hLevelFlag.EnableWindow(TRUE);
}

void Ctest_gui_appDlg::OnBnClickedRadio2()
{
    //select input mode for pin
    m_outputSelBtn.SetCheck(FALSE);
    m_hLevelFlag.EnableWindow(FALSE);

    m_inputSelBtn.SetCheck(TRUE);
    m_pullupFlag.EnableWindow(TRUE);
}

void Ctest_gui_appDlg::OnBnClickedButton2()
{
    //open usb io device
    int nIndex = m_devListBox.GetCurSel();
    CString strSerialNumber;
    if ( nIndex >= 0) {
        m_devListBox.GetLBText( nIndex, strSerialNumber);
    }

    if (strSerialNumber.GetLength() == 0)
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("Please click the refresh button to find device"));
        return;
    }

    struct usb_io_device_info *findDev = NULL;
    struct usb_io_device_info *tmp = m_devList;
    while(tmp)
    {
        if (strSerialNumber == CString(tmp->serial_number))
        {
            findDev = tmp;
            break;
        }
        tmp = tmp->next;
    }

    if (findDev)
    {
        m_currentUSBIoHandle = usb_io_open_device(findDev);
        if (m_currentUSBIoHandle == 0)
        {
            GetDlgItem(IDC_Log)->SetWindowText(_T("open device error, please click the refresh button!"));
        }
        else
        {
            GetDlgItem(IDC_Log)->SetWindowText(_T("open device success!"));
            //open success
            m_closeDevBtn.EnableWindow(TRUE);
            m_openDevBtn.EnableWindow(FALSE);
            m_findDevBtn.EnableWindow(FALSE);

            m_setPinWorkModeBtn.EnableWindow(TRUE);
            m_openWorkLed.EnableWindow(TRUE);
            m_closeWorkLed.EnableWindow(TRUE);
        }
    }
    else
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("open device error, please click the refresh button!"));
    }

}

void Ctest_gui_appDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    //设备
    //close usb io device
    usb_io_close_device(m_currentUSBIoHandle);
    m_currentUSBIoHandle = 0;

    m_openDevBtn.EnableWindow(TRUE);
    m_findDevBtn.EnableWindow(TRUE);
    m_closeDevBtn.EnableWindow(FALSE);

    m_setPinWorkModeBtn.EnableWindow(FALSE);
    m_openWorkLed.EnableWindow(FALSE);
    m_closeWorkLed.EnableWindow(FALSE);

    GetDlgItem(IDC_Log)->SetWindowText(_T("close device success!"));
}


void Ctest_gui_appDlg::OnBnClickedButton4()
{
    //set the work mode of pin
    unsigned pinIndex = m_pinIndexList.GetCurSel();
    pin_mode pinMode = (m_outputSelBtn.GetCheck() == 1) ? OUTPUT_MODE : INPUT_MODE;
    input_pin_mode pullUp = NO_INNNER_PULL_UP;
    if (pinMode == INPUT_MODE)
        pullUp = (m_pullupFlag.GetCheck() == 1) ? INNER_PULL_UP : NO_INNER_PULL_UP;

    pin_level outLevel = (m_hLevelFlag.GetCheck() == 1) ? HIGH_LVL : LOW_LVL;

    if (0 == usb_io_set_pin_mode(m_currentUSBIoHandle, pinIndex, pinMode, pullUp))
    {
        if (pinMode == OUTPUT_MODE)
        {
            usb_io_write_output_pin_value(m_currentUSBIoHandle, pinIndex, outLevel);
        }

        CString str;
        if (pinMode == OUTPUT_MODE)
        {
            if (outLevel)
                str.AppendFormat(_T("Set the pin#%d to output work mode, output High-Level "), pinIndex);
            else
                str.AppendFormat(_T("Set the pin#%d to output work mode, output Low-Level "), pinIndex);
        }
        else
        {
            str.AppendFormat(_T("Set the pin#%d to input work mode"), pinIndex);
            if (pullUp)
                str.AppendFormat(_T(" with INNER_PULL_UP"));
            else
               str.AppendFormat(_T(" with NO_INNNER_PULL_UP"));
        }

        GetDlgItem(IDC_Log)->SetWindowText(str.GetString());
    }
    else
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("set usb io pin work mode error!"));
    }

}

void Ctest_gui_appDlg::OnBnClickedButton6()
{
    //open work led
    if(0 != usb_io_set_work_led_mode(m_currentUSBIoHandle, WORK_LED_BLINK))
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("open work led failure!"));
    }
    else
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("open work led success!"));
    }
}

void Ctest_gui_appDlg::OnBnClickedButton7()
{
    //close work led
    if(0 != usb_io_set_work_led_mode(m_currentUSBIoHandle, WORK_LED_OFF))
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("close work led failure!"));
    }
    else
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("close work led success!"));
    }
}

void Ctest_gui_appDlg::OnBnClickedfinddevicebtn()
{
    //find the usb io devices connected to pc from usb
    int cnt = 0;
    if (m_devList)
    {
        usb_io_free_device_list(m_devList);
        m_devList = NULL;
    }

    m_devList = usb_io_get_device_list();
    struct usb_io_device_info *pTmp = m_devList;
    m_devListBox.ResetContent();
    while (pTmp)
    {
        CString ts((char const*)pTmp->serial_number);
        m_devListBox.AddString(ts);
        pTmp = pTmp->next;
        cnt++;
    }
    if (m_devList)
    {
        m_devListBox.SetCurSel(0);
        CString str;
        str.AppendFormat(_T("Found %d USB IO device(s)"), cnt);
        GetDlgItem(IDC_Log)->SetWindowText(str.GetString());
    }
    else
    {
        GetDlgItem(IDC_Log)->SetWindowText(_T("No compatible devices found"));
    }
}

void Ctest_gui_appDlg::OnBnClickedCancel()
{
    // TODO:
}

void Ctest_gui_appDlg::OnBnClickedButton8()
{
    // TODO:
    usb_io_uninit();
    OnCancel();
}

HBRUSH Ctest_gui_appDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC,   pWnd,   nCtlColor);
    if (CTLCOLOR_STATIC == nCtlColor)
    {
        if (pWnd->GetDlgCtrlID() == IDC_Log)
        {
            //pDC->SetBkColor(RGB(25,25,25));
            pDC->SetTextColor(RGB(255,0,0));

            return (HBRUSH)::CreateSolidBrush(GetSysColor(BLACK_BRUSH));
        }

    }
    return hbr;
}
