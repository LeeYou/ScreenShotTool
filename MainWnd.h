#pragma once
#include "resource.h"
#include "ResetStateSkin.hpp"
#include "SRectCapture.h"
//#include "ui\SDrawBoard.h"

#include <list>

class CMainWnd : public SHostWnd
{
public:
	CMainWnd();
	~CMainWnd(void);
public:	
	void OnFinalMessage(HWND hWnd)
	{
		//delete this;
	}
	template<class T>
	inline bool InitWnd(T*& pWnd, LPCTSTR lpWndName)
	{
		pWnd = FindChildByName2<T>(lpWndName);
		//assert(pWnd);
		if (nullptr == pWnd)
		{
			SStringT sErrorText;
			sErrorText.Format(_T("没有name为 <%s> 的控件"), lpWndName);
			OutputDebugString(sErrorText);
		}
		return nullptr != pWnd;
	}
protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnPaint(HDC dc);
	void OnMouseMove(UINT nFlags, CPoint pt);
	//窗口消息处理映射表
	BEGIN_MSG_MAP_EX(CMainWnd)
		MSG_WM_INITDIALOG(OnInitDialog)
		//MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		CHAIN_MSG_MAP(SHostWnd)
	END_MSG_MAP()

	void OnBtnClose();
	void OnBtnSave();
	//事件处理映射表
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnBtnClose)
		EVENT_NAME_COMMAND(L"btn_save", OnBtnSave)
	EVENT_MAP_END()

protected:
	bool OnEventRectCapturing(EventRectCapturing* pEvt);
	bool OnEventRectMoving(EventRectMoving* pEvt);
	bool OnEventRectCaptured(EventRectCaptured* pEvt);
	bool OnEventCtxMenu(EventCtxMenu* pEvt);
protected:
	CRect HitWnd(const CPoint& pt);
	void ShowLayCursor(const CPoint& pt, LPCTSTR lpRect);
	void ShowTitle();
	void ShowTool();
protected:
	std::list<CRect>					m_RectList;
private:
	SRectCapture*					m_pCapture;
	SStatic*								m_pTextTitle;
	SWindow*							m_pLayCursor;
	SWindow*							m_pLayTools;

	SImageWnd*						m_pImgCursor;
	SStatic*								m_pTextRGB;
	SStatic*								m_pTextRect;

};

