#pragma once
#include <core/swnd.h>

namespace SOUI
{	

class EventRectCapturing : public TplEventArgs<EventRectCapturing>
{
	SOUI_CLASS_NAME(EventRectCapturing, L"on_rectcapturing")
public:
	EventRectCapturing(SObject *pSender)
		: TplEventArgs<EventRectCapturing>(pSender)
	{ }
	enum { EventID = EVT_EXTERNAL_BEGIN+101 };
	CPoint pt;
};

class EventRectMoving : public TplEventArgs<EventRectMoving>
{
	SOUI_CLASS_NAME(EventRectMoving, L"on_rectmoving")
public:
	EventRectMoving(SObject *pSender)
		: TplEventArgs<EventRectMoving>(pSender)
	{
	}
	enum { EventID = EVT_EXTERNAL_BEGIN + 102 };
	CPoint pt;
};

class EventRectCaptured : public TplEventArgs<EventRectCaptured>
{
	SOUI_CLASS_NAME(EventRectCaptured, L"on_rectcaptured")
public:
	EventRectCaptured(SObject *pSender)
		: TplEventArgs<EventRectCaptured>(pSender)
	{
	}
	enum { EventID = EVT_EXTERNAL_BEGIN + 103};
	CPoint pt;
};

class SRectCapture : public SWindow
{
   SOUI_CLASS_NAME(SRectCapture, L"rectcapture")
public:
	SRectCapture();
	~SRectCapture();
	void SetImage(IBitmap* p);
	void Reset();
	void SetCaptureRect(const CRect& rc);
	void ShowCaptureRect(const CRect& rc);
	CRect GetCaptureRect();
	bool SavePng(LPCTSTR lpPngFile);
	bool IsCapturing(){	return m_bCapturing;}
protected:
	void OnPaint(IRenderTarget* pRT);
	void OnMouseMove(UINT nFlags, CPoint pt);
	void OnLButtonDown(UINT nFlags, CPoint pt);
	void OnLButtonUp(UINT nFlags, CPoint pt);
	void OnRButtonDown(UINT nFlags, CPoint pt);
	void OnRButtonUp(UINT nFlags, CPoint pt);

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
	SOUI_MSG_MAP_END()
		
	SOUI_ATTRS_BEGIN()
		//ATTR_SKIN(L"headSkin", m_pBgSkin, TRUE)
	SOUI_ATTRS_END()

protected:
	enum class EcPosType
	{
		Null = -1,
		TopLeft,
		TopCenter,
		TopRight,
		RightCenter,
		BottomRight,
		BottomCenter,
		BottomLeft,
		LeftCenter,

		SelectRect,
	};
	void ShowCursor(EcPosType ePos);
	EcPosType HitPos(CPoint& pt);
	void CalcPos();
private:
	EcPosType							m_eDraging;			// 拖动中 的 方位
	bool m_bCapturing;				// 捕获

	CPoint m_ptClick;				// 当前 点击 坐标
protected:
	CAutoRefPtr<IBitmap>			m_bmpBg;
	//CAutoRefPtr<IPen>				m_penShow;
	//CAutoRefPtr<IPen>				m_penSelect;
	COLORREF							m_crMask;
	COLORREF							m_crBorder;

	CRect									m_rcShow;
	CRect									m_rcCapture;
	CRect									m_rcPos[8];			// 8 个 
	//SIZE									m_siHead;

	GUID FAR m_ImageFormatPNG1;
};

}