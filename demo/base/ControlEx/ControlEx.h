#pragma once

#define SAFE_FIND_CONTROL(ctrl, name)	ctrl = static_cast<decltype(ctrl)>(m_pm.FindControl(name)); ASSERT(ctrl != nullptr);
#define SAFE_DELETE(p)					{ if(p){delete(p); (p)=NULL;} }
#define SAFE_DELETE_ARRAY(p)			{ if(p){delete[](p); (p)=NULL;} }

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public: 
	  
	CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI *pm) override
	{
		if (_tcscmp(pstrClass, _T("Cef")) == 0) return new CCefUI();
		return NULL;
	}
};