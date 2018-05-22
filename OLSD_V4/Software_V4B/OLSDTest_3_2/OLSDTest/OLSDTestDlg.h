// OLSDTestDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "laser_output.h"		// Laser Show Device Class Object
#include "afxcmn.h"

// COLSDTestDlg dialog
class COLSDTestDlg : public CDialog
{
// Construction
public:
	COLSDTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OLSDTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	bool m_bEnableOutput;

	CLaserOutput m_laser;

	// Generated Frame
	int m_test_ramp_frame[8000][5];

	void GenerateTestRamp1(int (*pFrame)[5], int &frame_size);
	void GenerateTestRamp2(int (*pFrame)[5], int &frame_size);

	void UpdateOutputStatus();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnConnect;
	CButton m_btnOutput;
	CStatic m_lblInfo;
	CComboBox m_cmbTestPattern;

	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedOutputButton();
	afx_msg void OnClose();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CComboBox m_cmbKPPS;
	afx_msg void OnBnClickedStopButton();
	CButton m_chkInvertX;
	CButton m_chkInvertY;
	CButton m_chkSwapXY;
	CButton m_chkEnableRed;
	CButton m_chkEnableGreen;
	CButton m_chkEnableBlue;
	CButton m_chkMonochrome;
	afx_msg void OnCbnSelchangeKppsCombo();
	afx_msg void OnCbnSelChangeTestPatternCombo();
	afx_msg void OnHandleCheckBox();
	CSliderCtrl m_sliderBrightness;
public:
	afx_msg void OnNMCustomdrawBrightnessSlider(NMHDR *pNMHDR, LRESULT *pResult);
};

