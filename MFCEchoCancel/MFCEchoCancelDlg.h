
// MFCEchoCancelDlg.h : ͷ�ļ�
//

#pragma once



// CMFCEchoCancelDlg �Ի���
class CMFCEchoCancelDlg : public CDialogEx
{
// ����
public:
	CMFCEchoCancelDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCECHOCANCEL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	

	// ���ɵ���Ϣӳ�亯��
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
