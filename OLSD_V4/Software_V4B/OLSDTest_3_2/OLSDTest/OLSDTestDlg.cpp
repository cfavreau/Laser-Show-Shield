// OLSDTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OLSDTest.h"
#include "OLSDTestDlg.h"

#define _USE_MATH_DEFINES 1
#include <math.h>			// M_PI, sin, cos

// Test Frames
#include "TestFrames\ildatesta.h"
#include "TestFrames\ildatestb.h"
#include "TestFrames\laser_media.h"
#include "TestFrames\quadrature.h"
#include "TestFrames\new_test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COLSDTestDlg dialog

COLSDTestDlg::COLSDTestDlg(CWnd* pParent /*=NULL*/) : CDialog(COLSDTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COLSDTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT_BUTTON, m_btnConnect);
	DDX_Control(pDX, IDC_OUTPUT_BUTTON, m_btnOutput);
	DDX_Control(pDX, IDC_INFO_STATIC, m_lblInfo);
	DDX_Control(pDX, IDC_TESTPATTERNS_COMBO, m_cmbTestPattern);
	DDX_Control(pDX, IDC_KPPS_COMBO, m_cmbKPPS);
	DDX_Control(pDX, IDC_INVERTX_CHECK, m_chkInvertX);
	DDX_Control(pDX, IDC_INVERTY_CHECK, m_chkInvertY);
	DDX_Control(pDX, IDC_SWAPXY_CHECK, m_chkSwapXY);
	DDX_Control(pDX, IDC_ENABLERED_CHECK, m_chkEnableRed);
	DDX_Control(pDX, IDC_ENABLEGREEN_CHECK, m_chkEnableGreen);
	DDX_Control(pDX, IDC_ENABLEBLUE_CHECK, m_chkEnableBlue);
	DDX_Control(pDX, IDC_MONO_CHECK, m_chkMonochrome);
	DDX_Control(pDX, IDC_BRIGHTNESS_SLIDER, m_sliderBrightness);
}

BEGIN_MESSAGE_MAP(COLSDTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &COLSDTestDlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_OUTPUT_BUTTON, &COLSDTestDlg::OnBnClickedOutputButton)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_STOP_BUTTON, &COLSDTestDlg::OnBnClickedStopButton)
	ON_CBN_SELCHANGE(IDC_KPPS_COMBO, &COLSDTestDlg::OnCbnSelchangeKppsCombo)
	ON_CBN_SELCHANGE(IDC_TESTPATTERNS_COMBO, &COLSDTestDlg::OnBnClickedOutputButton)
	ON_BN_CLICKED(IDC_INVERTX_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_INVERTY_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_SWAPXY_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_ENABLERED_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_ENABLEGREEN_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_ENABLEBLUE_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	ON_BN_CLICKED(IDC_MONO_CHECK, &COLSDTestDlg::OnHandleCheckBox)
	// TODO HANDLE BRIGHTNESS SLIDER
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BRIGHTNESS_SLIDER, &COLSDTestDlg::OnNMCustomdrawBrightnessSlider)
END_MESSAGE_MAP()


// COLSDTestDlg message handlers

BOOL COLSDTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set the info label text
	m_lblInfo.SetWindowText("Please Connect to the OLSD First");

	// Fill the test frames combo box with the test frames we have to select from
	m_cmbTestPattern.ResetContent();
	int index = m_cmbTestPattern.AddString("ILDA A");
	m_cmbTestPattern.SetItemData(index, 0);
	index = m_cmbTestPattern.AddString("ILDA B");
	m_cmbTestPattern.SetItemData(index, 1);
	index = m_cmbTestPattern.AddString("Laser Media");
	m_cmbTestPattern.SetItemData(index, 2);
	index = m_cmbTestPattern.AddString("Quadrature");
	m_cmbTestPattern.SetItemData(index, 3);
	index = m_cmbTestPattern.AddString("New Test");
	m_cmbTestPattern.SetItemData(index, 4);
	index = m_cmbTestPattern.AddString("Test Ramp 1");
	m_cmbTestPattern.SetItemData(index, 5);
	index = m_cmbTestPattern.AddString("Test Ramp 2");
	m_cmbTestPattern.SetItemData(index, 6);
	m_cmbTestPattern.SetCurSel(0);

	// Fill in the kpps combo box
	m_cmbKPPS.ResetContent();
	index = m_cmbKPPS.AddString("1");
	m_cmbKPPS.SetItemData(index, 1000);
	index = m_cmbKPPS.AddString("5");
	m_cmbKPPS.SetItemData(index, 5000);
	index = m_cmbKPPS.AddString("10");
	m_cmbKPPS.SetItemData(index, 10000);
	index = m_cmbKPPS.AddString("12");
	m_cmbKPPS.SetItemData(index, 12000);
	index = m_cmbKPPS.AddString("16");
	m_cmbKPPS.SetItemData(index, 16000);
	index = m_cmbKPPS.AddString("20");
	m_cmbKPPS.SetItemData(index, 20000);
	index = m_cmbKPPS.AddString("24");
	m_cmbKPPS.SetItemData(index, 24000);
	index = m_cmbKPPS.AddString("30");
	m_cmbKPPS.SetItemData(index, 30000);
	m_cmbKPPS.SetCurSel(3);					// 12kpps

	// Set the default settings for the coordinate modification check boxes
	m_chkEnableRed.SetCheck(BST_CHECKED);
	m_chkEnableGreen.SetCheck(BST_CHECKED);
	m_chkEnableBlue.SetCheck(BST_CHECKED);
	m_chkMonochrome.SetCheck(BST_UNCHECKED);
	m_chkInvertX.SetCheck(BST_UNCHECKED);
	m_chkInvertY.SetCheck(BST_UNCHECKED);
	m_chkSwapXY.SetCheck(BST_UNCHECKED);

	// Configure the brightness slider
	m_sliderBrightness.SetRange(0, 100);
	m_sliderBrightness.SetPos(100);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COLSDTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COLSDTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COLSDTestDlg::OnBnClickedConnectButton()
{
	CString szTemp;

	// Initialize our laser show device
	if (m_laser.Init())
	{
		szTemp.Format("Initialized %s", m_laser.GetName());
	}
	else
	{
		szTemp.Format("Failed to initlize the OLSD");
	}

	// Set the information label
	m_lblInfo.SetWindowText(szTemp);
}

void COLSDTestDlg::GenerateTestRamp1(int (*pFrame)[5], int &frame_size)
{
	int iPoint;
	int x = 0;
	int y = 0;
	for (iPoint = 0; iPoint < 1024; iPoint++)
	{
		if (iPoint < 256)
		{
			x += 128;
			y -= 128;
		}
		else
		if (iPoint < 512)
		{
			x += 128;
			y += 128;
		}
		else
		if (iPoint < 768)
		{
			x -= 128;
			y += 128;
		}
		else
		{
			x -= 128;
			y -= 128;
		}

		//if (x > 32767) x = 32767;
		//if (x < -32767) x = -32767;
		//if (y > 32767) y = 32767;
		//if (y < -32767) y = -32767;
		
		// X
		m_test_ramp_frame[iPoint][0] = (x - 32768) / 2;

		// Y
		m_test_ramp_frame[iPoint][1] = (y / 2);

		// R
		m_test_ramp_frame[iPoint][2] = iPoint % 256;

		// G
		m_test_ramp_frame[iPoint][3] = (iPoint + 64) % 256;

		// B
		m_test_ramp_frame[iPoint][4] = (iPoint + 128) % 256;
	}

	pFrame = m_test_ramp_frame;
	frame_size = 1024;
}

void COLSDTestDlg::GenerateTestRamp2(int (*pFrame)[5], int &frame_size)
{
	int count = 0;
	
	/*
	// GENERATE CIRCLE
	double dAngle;
	double dInc = (2.0 * M_PI) / 1200.0;
	for (dAngle = 0.0; dAngle < (2.0 * M_PI); dAngle += dInc)
	{
		double x = sin(dAngle);
		double y = cos(dAngle);

		x = x * 32767.0;
		y = y * 32767.0;
		
		// X
		m_test_ramp_frame[count][0] = (int)x;
		
		// Y
		m_test_ramp_frame[count][1] = (int)y;
		
		// R
		m_test_ramp_frame[count][2] = 255;

		// G
		m_test_ramp_frame[count][3] = 255;

		// B
		m_test_ramp_frame[count][4] = 255;

		count++;
	}
	*/

	///*
	int iPoint;
	for (iPoint = 0; iPoint < 1024; iPoint++)
	{
		// X
		m_test_ramp_frame[iPoint][0] = ((iPoint * 64) % 65536) - 32768;
		
		// Y
		m_test_ramp_frame[iPoint][1] = ((iPoint * 64) % 65536) - 32768;
		
		// R
		m_test_ramp_frame[iPoint][2] = iPoint % 256;

		// G
		m_test_ramp_frame[iPoint][3] = iPoint % 256;

		// B
		m_test_ramp_frame[iPoint][4] = iPoint % 256;
	}
	count = iPoint;
	//*/

	pFrame = m_test_ramp_frame;
	frame_size = count;
}

void COLSDTestDlg::OnBnClickedOutputButton()
{
	m_bEnableOutput = true;
	UpdateOutputStatus();
}

void COLSDTestDlg::OnClose()
{
	m_bEnableOutput = false;

	// Stop the laser output
	m_laser.Stop();

	// Blank and Center the Laser Beam
	m_laser.BlankAndCenter();

	CDialog::OnClose();
}

LRESULT COLSDTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Catch the Boot Loader Progress Messages & Next Frame Messages
	switch (message)
	{
	case (WM_LASEROUTPUT_NEXTFRAME):
		// ToDo: Read all of the controls and reprocess the frame!
		//OnBnClickedOutputButton();
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void COLSDTestDlg::OnBnClickedStopButton()
{
	m_laser.Stop();
	m_bEnableOutput = false;
}

void COLSDTestDlg::OnCbnSelchangeKppsCombo()
{
	m_laser.SetScanSpeed((WORD)m_cmbKPPS.GetItemData(m_cmbKPPS.GetCurSel()));
}

void COLSDTestDlg::OnCbnSelChangeTestPatternCombo()
{
	UpdateOutputStatus();
}

void COLSDTestDlg::OnHandleCheckBox()
{
	UpdateOutputStatus();
}

void COLSDTestDlg::UpdateOutputStatus()
{
	if (!m_bEnableOutput) return;

	int (*pFrame)[5] = NULL;
	int frame_size = 0;

	// Get the brightness from the slider
	double brightness = (double)m_sliderBrightness.GetPos() / 100.0;

	// Figure out which test frame we have and make a pointer to it and get its size
	switch(m_cmbTestPattern.GetItemData(m_cmbTestPattern.GetCurSel()))
	{
	case (0):		// ILDA A
		pFrame = ildatesta_frame;
		frame_size = ILDATESTA_FRAME_SIZE;
		break;
	case (1):		// ILDA B
		pFrame = ildatestb_frame;
		frame_size = ILDATESTB_FRAME_SIZE;
		break;
	case (2):		// Laser Media
		pFrame = lasermedia_frame;
		frame_size = LASERMEDIA_FRAME_SIZE;
		break;
	case (3):		// Quadrature
		pFrame = quadrature_frame;
		frame_size = QUADRATURE_FRAME_SIZE;
		break;
	case (4):		// NEW Test - Tuning Frame
		pFrame = newtest_frame;
		frame_size = NEWTEST_FRAME_SIZE;
		break;
	case (5):		// Test Ramp 1
		GenerateTestRamp1(pFrame, frame_size);
		pFrame = m_test_ramp_frame;
		break;
	case (6):		// Test Ramp 2
		GenerateTestRamp2(pFrame, frame_size);
		pFrame = m_test_ramp_frame;
		break;
	}

	// Check to see if there is a frame selected
	if ((frame_size < 1) || (!pFrame)) return;

	// Copy our test frame into a LASER SHOW DEVICE POINT LIST and then attach that to a frame
	struct LASER_SHOW_DEVICE_POINT *pPoints = new struct LASER_SHOW_DEVICE_POINT[frame_size];
	if (!pPoints) return;

	// Get the modification flags from the check boxes
	bool bEnableRed = (m_chkEnableRed.GetCheck() == BST_CHECKED);
	bool bEnableGreen = (m_chkEnableGreen.GetCheck() == BST_CHECKED);
	bool bEnableBlue = (m_chkEnableBlue.GetCheck() == BST_CHECKED);
	bool bMonochrome = (m_chkMonochrome.GetCheck() == BST_CHECKED);
	bool bInvertX = (m_chkInvertX.GetCheck() == BST_CHECKED);
	bool bInvertY = (m_chkInvertY.GetCheck() == BST_CHECKED);
	bool bSwapXY = (m_chkSwapXY.GetCheck() == BST_CHECKED);

	int i;
	int x;
	int y;
	int temp;
	for (i = 0; i < frame_size; i++)
	{
		x = pFrame[i][0] + 32768;
		if (bInvertX) x = x * -1;
		y = pFrame[i][1] + 32768;
		if (bInvertY) y = y * -1;
		if (bSwapXY)
		{
			temp = x;
			x = y;
			y = temp;
		}
		pPoints[i].x = x;
		pPoints[i].y = y;
		
		if (bEnableRed)
			pPoints[i].r = (int)((double)pFrame[i][2] * brightness);
		else
			pPoints[i].r = 0;

		if (bEnableGreen)
			pPoints[i].g = (int)((double)pFrame[i][3] * brightness);
		else
			pPoints[i].g = 0;
		
		if (bEnableBlue)
			pPoints[i].b = (int)((double)pFrame[i][4] * brightness);
		else
			pPoints[i].b = 0;

		if (bMonochrome)
		{
			// Set the laser to the max intensity
			int iMax = pPoints[i].r;
			if (iMax < pPoints[i].g) iMax = pPoints[i].g;
			if (iMax < pPoints[i].b) iMax = pPoints[i].b;
			pPoints[i].r = iMax;
			pPoints[i].g = iMax;
			pPoints[i].b = iMax;
		}

		// Set the intesity to 0 for now
		pPoints[i].i = 0;
	}

	// Get the device speed
	int speed = (int)m_cmbKPPS.GetItemData(m_cmbKPPS.GetCurSel());

	struct LASER_SHOW_DEVICE_FRAME frame;
	frame.point_count = frame_size;
	frame.display_speed = speed;
	frame.points = pPoints;

	// Send the frame to the output device
	m_laser.PlayFrame(&frame, false, false, false);

	// Start playing the frame
	m_laser.Start();

	// Clean up our allocated memory
	delete [] pPoints;
	pPoints = NULL;
}

void COLSDTestDlg::OnNMCustomdrawBrightnessSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	// Handle getting the brightness slider
	if (m_laser.IsInitialized())
	{
		UpdateOutputStatus();
	}
}
