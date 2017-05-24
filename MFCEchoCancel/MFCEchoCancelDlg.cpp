
// MFCEchoCancelDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCEchoCancel.h"
#include "MFCEchoCancelDlg.h"
#include "afxdialogex.h"
#include "AudioClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹���"�˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCEchoCancelDlg �Ի���



CMFCEchoCancelDlg::CMFCEchoCancelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCECHOCANCEL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCEchoCancelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCEchoCancelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PickFarEndFile, &CMFCEchoCancelDlg::OnBnClickedPickfarendfile)
	ON_BN_CLICKED(IDC_PickNearEndFile, &CMFCEchoCancelDlg::OnBnClickedPicknearendfile)
	ON_BN_CLICKED(IDC_SelectLMS, &CMFCEchoCancelDlg::OnBnClickedSelectlms)
	ON_BN_CLICKED(IDC_SelectNLMS, &CMFCEchoCancelDlg::OnBnClickedSelectnlms)
	ON_BN_CLICKED(IDC_SelectRLS, &CMFCEchoCancelDlg::OnBnClickedSelectrls)
	ON_BN_CLICKED(IDC_PlayFarEndSound, &CMFCEchoCancelDlg::OnBnClickedPlayfarendsound)
	ON_BN_CLICKED(IDC_StopPlaySound, &CMFCEchoCancelDlg::OnBnClickedStopplaysound)
	ON_BN_CLICKED(IDC_PlayNearEndSound, &CMFCEchoCancelDlg::OnBnClickedPlaynearendsound)
	ON_BN_CLICKED(IDC_PlayMixSound, &CMFCEchoCancelDlg::OnBnClickedPlaymixsound)
	ON_EN_CHANGE(IDC_DelaySetMs, &CMFCEchoCancelDlg::OnEnChangeDelaysetms)
	ON_BN_CLICKED(IDC_StaticDelay, &CMFCEchoCancelDlg::OnBnClickedStaticdelay)
	ON_BN_CLICKED(IDC_RandonDelay, &CMFCEchoCancelDlg::OnBnClickedRandondelay)
	ON_EN_CHANGE(IDC_MinRandonDelay, &CMFCEchoCancelDlg::OnEnChangeMinrandondelay)
	ON_EN_CHANGE(IDC_MaxRandonDelay, &CMFCEchoCancelDlg::OnEnChangeMaxrandondelay)
	ON_BN_CLICKED(IDC_PlayProcessedSound, &CMFCEchoCancelDlg::OnBnClickedPlayprocessedsound)
END_MESSAGE_MAP()


// CMFCEchoCancelDlg ��Ϣ�������

BOOL CMFCEchoCancelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������..."�˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCEchoCancelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCEchoCancelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCEchoCancelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMFCEchoCancelDlg::OnBnClickedPickfarendfile()
{
	//��ȡԶ����Ƶ·��
	BOOL isOpen  = TRUE;     //�Ƿ��(����Ϊ����)  
	CString defaultDir  = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\��������";   //Ĭ�ϴ򿪵��ļ�·��  
	CString fileName  = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\��������\\3710993.mp3";         //Ĭ�ϴ򿪵��ļ���  
	CString filter  = L"�ļ� (*.wav)|*.wav||";   //�ļ����ǵ�����  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result  = openFileDlg.DoModal();
	CString filePath = defaultDir;
	if (result  == IDOK) {
		filePath  = openFileDlg.GetPathName();
	}
	CWnd::SetDlgItemTextW(IDC_FarEndFilePath, filePath);
	CWnd::SetDlgItemTextW(IDC_Status, L"�ļ��Ѽ���");

}

void CMFCEchoCancelDlg::OnBnClickedPicknearendfile()
{
	//��ȡ������Ƶ·��
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
	CString defaultDir = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\��������";   //Ĭ�ϴ򿪵��ļ�·��  
	CString fileName = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\��������\\3710993.mp3";         //Ĭ�ϴ򿪵��ļ���  
	CString filter = L"�ļ� (*.wav)|*.wav||";   //�ļ����ǵ�����  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	CWnd::SetDlgItemTextW(IDC_NearEndFilePath, filePath);
	CWnd::SetDlgItemTextW(IDC_Status, L"�ļ��Ѽ���");
	
}


void CMFCEchoCancelDlg::OnBnClickedSelectlms()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::LMS;
	CWnd::SetDlgItemTextW(IDC_Status, L"ѡ��LMS�㷨");
}


void CMFCEchoCancelDlg::OnBnClickedSelectnlms()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::NLMS;
	CWnd::SetDlgItemTextW(IDC_Status, L"ѡ��NLMS�㷨");
}


void CMFCEchoCancelDlg::OnBnClickedSelectrls()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::RLS;
	CWnd::SetDlgItemTextW(IDC_Status, L"ѡ��RLS�㷨");
}


void CMFCEchoCancelDlg::OnBnClickedPlayfarendsound()
{
	//����Զ����Ƶ
	CString fileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, fileName);
	if (fileName == L"")
	{
		MessageBox(L"��������", L"Error");
		return;
	}
	MyAudioClient::Play(fileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"���ڲ���Զ����Ƶ");
}


void CMFCEchoCancelDlg::OnBnClickedStopplaysound()
{
	//ֹͣ����
	MyAudioClient::Stop();
	CWnd::SetDlgItemTextW(IDC_Status, L"��ֹͣ");
}


void CMFCEchoCancelDlg::OnBnClickedPlaynearendsound()
{
	//���Ž�����Ƶ
	CString fileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, fileName);
	if (fileName == L"")
	{
		MessageBox(L"��������", L"Error");
		return;
	}
	MyAudioClient::Play(fileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"���ڲ��Ž�����Ƶ");
}


void CMFCEchoCancelDlg::OnBnClickedPlaymixsound()
{
	CString NearFileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, NearFileName);
	CString FarFileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, FarFileName);
	if (NearFileName == L"" || FarFileName == L"")
	{
		MessageBox(L"��������", L"Error");
		return;
	}
	MyAudioClient::Play(NearFileName, MyAudioClient::SoundType::mix, FarFileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"���ڲ��Ż����Ƶ");
}


void CMFCEchoCancelDlg::OnEnChangeDelaysetms()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_DelaySetMs, szDelayMs);
	MyAudioClient::delayMs = _wtoi(szDelayMs);
	CWnd::SetDlgItemTextW(IDC_Status, L"�ӳٱ�����Ϊ" + szDelayMs + L"ms");
}


void CMFCEchoCancelDlg::OnBnClickedStaticdelay()
{
	MyAudioClient::randonDelayFlag = false;
	CWnd::SetDlgItemTextW(IDC_Status, L"����Ϊ�̶��ӳ�");
	
}


void CMFCEchoCancelDlg::OnBnClickedRandondelay()
{
	MyAudioClient::randonDelayFlag = true;
	CWnd::SetDlgItemTextW(IDC_Status, L"����Ϊ����ӳ�");
}




void CMFCEchoCancelDlg::OnEnChangeMinrandondelay()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_MinRandonDelay, szDelayMs);
	MyAudioClient::minRandonDelayMs = _wtoi(szDelayMs);
	WCHAR szMaxDelay[10];
	_itow(MyAudioClient::maxRandonDelayMs, szMaxDelay, 10);
	CWnd::SetDlgItemTextW(IDC_Status, L"����ӳٱ�����Ϊ" + szDelayMs + "-" + szMaxDelay + L"ms");
}


void CMFCEchoCancelDlg::OnEnChangeMaxrandondelay()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_MaxRandonDelay, szDelayMs);
	MyAudioClient::maxRandonDelayMs = _wtoi(szDelayMs);
	WCHAR szMimDelay[10];
	_itow(MyAudioClient::minRandonDelayMs, szMimDelay, 10);
	CString szTmp(szMimDelay);
	CWnd::SetDlgItemTextW(IDC_Status, L"����ӳٱ�����Ϊ" + szTmp + "-" + szDelayMs + L"ms");
}


void CMFCEchoCancelDlg::OnBnClickedPlayprocessedsound()
{
	CString NearFileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, NearFileName);
	CString FarFileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, FarFileName);

	if (NearFileName == L"" || FarFileName == L"")
	{
		MessageBox(L"��������", L"Error");
		return;
	}
	
	int delay = MyAudioClient::Play(NearFileName, MyAudioClient::SoundType::processed, FarFileName);
	delay = delay / MyAudioClient::audioLength1ms;
	WCHAR szDelay[10];
	_itow(delay, szDelay, 10);
	CWnd::SetDlgItemTextW(IDC_DelayTimeMS, szDelay);
	CWnd::SetDlgItemTextW(IDC_Status, L"���ڲ��Ŵ�������Ƶ");
}
