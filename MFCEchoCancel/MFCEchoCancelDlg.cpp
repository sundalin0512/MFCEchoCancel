
// MFCEchoCancelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCEchoCancel.h"
#include "MFCEchoCancelDlg.h"
#include "afxdialogex.h"
#include "AudioClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMFCEchoCancelDlg 对话框



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


// CMFCEchoCancelDlg 消息处理程序

BOOL CMFCEchoCancelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于..."菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCEchoCancelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCEchoCancelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMFCEchoCancelDlg::OnBnClickedPickfarendfile()
{
	//获取远端音频路径
	BOOL isOpen  = TRUE;     //是否打开(否则为保存)  
	CString defaultDir  = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\有声读物";   //默认打开的文件路径  
	CString fileName  = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\有声读物\\3710993.mp3";         //默认打开的文件名  
	CString filter  = L"文件 (*.wav)|*.wav||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result  = openFileDlg.DoModal();
	CString filePath = defaultDir;
	if (result  == IDOK) {
		filePath  = openFileDlg.GetPathName();
	}
	CWnd::SetDlgItemTextW(IDC_FarEndFilePath, filePath);
	CWnd::SetDlgItemTextW(IDC_Status, L"文件已加载");

}

void CMFCEchoCancelDlg::OnBnClickedPicknearendfile()
{
	//获取近端音频路径
	BOOL isOpen = TRUE;     //是否打开(否则为保存)  
	CString defaultDir = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\有声读物";   //默认打开的文件路径  
	CString fileName = L"C:\\Users\\sunda\\Documents\\Visual Studio 2017\\Projects\\有声读物\\3710993.mp3";         //默认打开的文件名  
	CString filter = L"文件 (*.wav)|*.wav||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	CWnd::SetDlgItemTextW(IDC_NearEndFilePath, filePath);
	CWnd::SetDlgItemTextW(IDC_Status, L"文件已加载");
	
}


void CMFCEchoCancelDlg::OnBnClickedSelectlms()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::LMS;
	CWnd::SetDlgItemTextW(IDC_Status, L"选择LMS算法");
}


void CMFCEchoCancelDlg::OnBnClickedSelectnlms()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::NLMS;
	CWnd::SetDlgItemTextW(IDC_Status, L"选择NLMS算法");
}


void CMFCEchoCancelDlg::OnBnClickedSelectrls()
{
	MyAudioClient::selectAlgorithm = MyAudioClient::AlgorithmEnum::RLS;
	CWnd::SetDlgItemTextW(IDC_Status, L"选择RLS算法");
}


void CMFCEchoCancelDlg::OnBnClickedPlayfarendsound()
{
	//播放远端音频
	CString fileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, fileName);
	if (fileName == L"")
	{
		MessageBox(L"出错啦！", L"Error");
		return;
	}
	MyAudioClient::Play(fileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"正在播放远端音频");
}


void CMFCEchoCancelDlg::OnBnClickedStopplaysound()
{
	//停止播放
	MyAudioClient::Stop();
	CWnd::SetDlgItemTextW(IDC_Status, L"已停止");
}


void CMFCEchoCancelDlg::OnBnClickedPlaynearendsound()
{
	//播放近端音频
	CString fileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, fileName);
	if (fileName == L"")
	{
		MessageBox(L"出错啦！", L"Error");
		return;
	}
	MyAudioClient::Play(fileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"正在播放近端音频");
}


void CMFCEchoCancelDlg::OnBnClickedPlaymixsound()
{
	CString NearFileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, NearFileName);
	CString FarFileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, FarFileName);
	if (NearFileName == L"" || FarFileName == L"")
	{
		MessageBox(L"出错啦！", L"Error");
		return;
	}
	MyAudioClient::Play(NearFileName, MyAudioClient::SoundType::mix, FarFileName);
	CWnd::SetDlgItemTextW(IDC_Status, L"正在播放混合音频");
}


void CMFCEchoCancelDlg::OnEnChangeDelaysetms()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_DelaySetMs, szDelayMs);
	MyAudioClient::delayMs = _wtoi(szDelayMs);
	CWnd::SetDlgItemTextW(IDC_Status, L"延迟被设置为" + szDelayMs + L"ms");
}


void CMFCEchoCancelDlg::OnBnClickedStaticdelay()
{
	MyAudioClient::randonDelayFlag = false;
	CWnd::SetDlgItemTextW(IDC_Status, L"设置为固定延迟");
	
}


void CMFCEchoCancelDlg::OnBnClickedRandondelay()
{
	MyAudioClient::randonDelayFlag = true;
	CWnd::SetDlgItemTextW(IDC_Status, L"设置为随机延迟");
}




void CMFCEchoCancelDlg::OnEnChangeMinrandondelay()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_MinRandonDelay, szDelayMs);
	MyAudioClient::minRandonDelayMs = _wtoi(szDelayMs);
	WCHAR szMaxDelay[10];
	_itow(MyAudioClient::maxRandonDelayMs, szMaxDelay, 10);
	CWnd::SetDlgItemTextW(IDC_Status, L"随机延迟被设置为" + szDelayMs + "-" + szMaxDelay + L"ms");
}


void CMFCEchoCancelDlg::OnEnChangeMaxrandondelay()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CString szDelayMs;
	CWnd::GetDlgItemTextW(IDC_MaxRandonDelay, szDelayMs);
	MyAudioClient::maxRandonDelayMs = _wtoi(szDelayMs);
	WCHAR szMimDelay[10];
	_itow(MyAudioClient::minRandonDelayMs, szMimDelay, 10);
	CString szTmp(szMimDelay);
	CWnd::SetDlgItemTextW(IDC_Status, L"随机延迟被设置为" + szTmp + "-" + szDelayMs + L"ms");
}


void CMFCEchoCancelDlg::OnBnClickedPlayprocessedsound()
{
	CString NearFileName;
	CWnd::GetDlgItemTextW(IDC_NearEndFilePath, NearFileName);
	CString FarFileName;
	CWnd::GetDlgItemTextW(IDC_FarEndFilePath, FarFileName);

	if (NearFileName == L"" || FarFileName == L"")
	{
		MessageBox(L"出错啦！", L"Error");
		return;
	}
	
	int delay = MyAudioClient::Play(NearFileName, MyAudioClient::SoundType::processed, FarFileName);
	delay = delay / MyAudioClient::audioLength1ms;
	WCHAR szDelay[10];
	_itow(delay, szDelay, 10);
	CWnd::SetDlgItemTextW(IDC_DelayTimeMS, szDelay);
	CWnd::SetDlgItemTextW(IDC_Status, L"正在播放处理后的音频");
}
