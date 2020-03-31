
// HashCalculatorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HashCalculator.h"
#include "HashCalculatorDlg.h"
#include "afxdialogex.h"
#include "sha256.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SHA256_OUTPUT_LENGTH	(32 * 2 + 1)
extern int sha256_encode(const char* in_buf, const int inbuf_len, char* out_buf, int& outbuf_len);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHashCalculatorDlg 对话框



CHashCalculatorDlg::CHashCalculatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHashCalculatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHashCalculatorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_IN, m_editInput);
}

BEGIN_MESSAGE_MAP(CHashCalculatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CHashCalculatorDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHashCalculatorDlg 消息处理程序

BOOL CHashCalculatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

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

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHashCalculatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHashCalculatorDlg::OnPaint()
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
HCURSOR CHashCalculatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHashCalculatorDlg::OnBnClickedOk()
{
    /*
    std::string text;
    char result_buf[SHA256_OUTPUT_LENGTH];
    int len = SHA256_OUTPUT_LENGTH;
    int ret = sha256_encode(text.c_str(), text.length(), result_buf, len);
    if (0 == ret)
        std::cout << "out:" << result_buf << std::endl;
    else
        std::cout << "sha256 encode error, err_id:" << ret << std::endl;
    */

    int rowCnt = 0;
    int realRowCnt = 0;
    rowCnt = m_editInput.GetLineCount();
    CString hashResult;
    for (int i = 0; i < rowCnt; i++) {
        int len = m_editInput.LineLength(m_editInput.LineIndex(i));
        CString sLineContent;
        m_editInput.GetLine(i, sLineContent.GetBuffer(len), len);
        sLineContent.ReleaseBuffer(len);
        if (!sLineContent.IsEmpty()) {
            std::string lineCtx;
            lineCtx = CT2A(sLineContent.GetString());
            char result_buf[SHA256_OUTPUT_LENGTH];
            int len = SHA256_OUTPUT_LENGTH;
            int ret = sha256_encode(lineCtx.c_str(), lineCtx.length(), result_buf, len);
            if (0 == ret) {
                CString sh256Res(result_buf);
                hashResult += sh256Res;
                realRowCnt++;
            }
        }
    }

    if (realRowCnt > 1)
    {
        std::string hx;
        hx = CT2A(hashResult.GetString());
        hashResult.Empty();
        char result_buf[SHA256_OUTPUT_LENGTH];
        int len = SHA256_OUTPUT_LENGTH;
        int ret = sha256_encode(hx.c_str(), hx.length(), result_buf, len);
        if (0 == ret)
            hashResult = result_buf;
    }

    CString sRowCnt;
    sRowCnt.Format(_T("%d"), realRowCnt);
    SetDlgItemTextW(IDC_EDIT_TOTAL, sRowCnt);
    SetDlgItemTextW(IDC_EDIT_RESULT, hashResult);
    //CDialogEx::OnOK();
}
