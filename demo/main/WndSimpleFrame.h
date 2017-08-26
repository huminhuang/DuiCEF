#pragma once

/************************************************************************/
/*
	SimpleFrame窗口
*/
/************************************************************************/

class CWndSimpleFrame
	: public CWindowWnd
	, public INotifyUI
{
public:
	CWndSimpleFrame(void);
	virtual ~CWndSimpleFrame(void);
	  
	LPCTSTR GetWindowClassName() const { return _T("UISimpleFrame"); }
	UINT GetClassStyle() const{ return UI_CLASSSTYLE_DIALOG; }
	void OnFinalMessage(HWND /*hWnd*/);

	void Init();
	void OnPrepare();
	void Notify(TNotifyUI& msg);

	void OnBtnMin();
	void OnBtnClose();

	//窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	//消息集成处理函数
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CPaintManagerUI	m_pm; 
};

	