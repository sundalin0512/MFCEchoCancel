
// MFCEchoCancelDlg.h : 头文件
//

#pragma once



// CMFCEchoCancelDlg 对话框
class CMFCEchoCancelDlg : public CDialogEx
{
// 构造
public:
	CMFCEchoCancelDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCECHOCANCEL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPickfarendfile();
	afx_msg void OnBnClickedPicknearendfile();
	afx_msg void OnBnClickedSelectlms();
	afx_msg void OnBnClickedSelectnlms();
	afx_msg void OnBnClickedSelectrls();
	afx_msg void OnBnClickedPlayfarendsound();
	afx_msg void OnBnClickedStopplaysound();
	afx_msg void OnBnClickedPlaynearendsound();
	afx_msg void OnBnClickedPlaymixsound();
	afx_msg void OnEnChangeDelaysetms();
	afx_msg void OnBnClickedStaticdelay();
	afx_msg void OnBnClickedRandondelay();
	afx_msg void OnEnChangeMinrandondelay();
	afx_msg void OnEnChangeMaxrandondelay();
	afx_msg void OnBnClickedPlayprocessedsound();
};
