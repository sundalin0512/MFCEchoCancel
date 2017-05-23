
// MFCEchoCancelDlg.h : ͷ�ļ�
//

#pragma once

enum class AlgorithmEnum
{
	LMS = 0,
	NLMS = 1,
	RLS = 2
};

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
	AlgorithmEnum selectAlgorithm;

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
};
