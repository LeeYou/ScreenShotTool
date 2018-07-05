#include "stdafx.h"
#include "SRectCapture.h"

namespace SOUI
{	

//DEFINE_GUID(ImageFormatPNG1, 0xb96b3caf, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e);//copy from gdi+


SRectCapture::SRectCapture()
	: m_bmpBg(nullptr)
	, m_bCapturing(false)
	, m_eDraging(EcPosType::Null)
{
	m_crMask = RGBA(125, 125, 125, 125);
	m_crBorder = RGBA(22, 119, 189, 255);

	m_evtSet.addEvent(EVENTID(EventRectCapturing));
	m_evtSet.addEvent(EVENTID(EventRectMoving));
	m_evtSet.addEvent(EVENTID(EventRectCaptured));

	m_ImageFormatPNG1.Data1 = 0xb96b3caf;
	m_ImageFormatPNG1.Data2 = 0x0728;
	m_ImageFormatPNG1.Data3 = 0x11d3;
	m_ImageFormatPNG1.Data4[0] = 0x9d;
	m_ImageFormatPNG1.Data4[1] = 0x7b;
	m_ImageFormatPNG1.Data4[2] = 0x00;
	m_ImageFormatPNG1.Data4[3] = 0x00;
	m_ImageFormatPNG1.Data4[4] = 0xf8;
	m_ImageFormatPNG1.Data4[5] = 0x1e;
	m_ImageFormatPNG1.Data4[6] = 0xf3;
	m_ImageFormatPNG1.Data4[7] = 0x2e;
	
		
		
}

SRectCapture::~SRectCapture()
{
		
}
void SRectCapture::SetImage(IBitmap* p)
{
	m_bmpBg = p;
	Invalidate();
}
void SRectCapture::Reset()
{
	m_bCapturing = false;
	m_eDraging = EcPosType::Null;
	m_rcShow.SetRectEmpty();
	m_rcCapture.SetRectEmpty();
	Invalidate();
}

void SRectCapture::ShowCaptureRect(const CRect& rc)
{
	if (m_rcShow == rc) return;

	m_bCapturing = false;
	m_eDraging = EcPosType::Null;
	m_rcShow = rc;
	Invalidate();
}

void SRectCapture::SetCaptureRect(const CRect& rc)
{
	m_bCapturing = false;
	m_eDraging = EcPosType::SelectRect;
	m_rcCapture = rc;
	Invalidate();
}

CRect SRectCapture::GetCaptureRect()
{
	return m_rcCapture;
}

bool SRectCapture::SavePng(LPCTSTR lpPngFile)
{
	CAutoRefPtr<IRenderTarget> pMemRT;
	GETRENDERFACTORY->CreateRenderTarget(&pMemRT, m_rcCapture.Width(), m_rcCapture.Height());

	CRect rcDest(0, 0, m_rcCapture.Width(), m_rcCapture.Height());
	pMemRT->DrawBitmapEx(rcDest, m_bmpBg, m_rcCapture, 0);
		
	IBitmap* pCache = (IBitmap*)pMemRT->GetCurrentObject(OT_BITMAP);
	return S_OK == pCache->Save(lpPngFile, (const LPVOID)&m_ImageFormatPNG1);
#if 0
	
#endif

	
	//pMemRT->FillSolidEllipse(rcDest, GETCOLOR(L"#1F1F1F00"));
	//pMemRT->FillSolidEllipse(rcDest, GETCOLOR(L"#FFFFFF"));
	
	
}

void SRectCapture::OnPaint(IRenderTarget* pRT)
{
	if (nullptr == m_bmpBg)
	{
		SWindow::OnPaint(pRT);
		return;
	}

	CRect rcWnd = GetWindowRect();
	pRT->DrawBitmap(rcWnd, m_bmpBg, 0, 0);

	CRect rcSel;
	bool bShowM = !m_rcShow.IsRectEmpty();
	if (bShowM)
	{
		rcSel = m_rcShow;
		//nPenSize = 5;
	}
	else if (!m_rcCapture.IsRectNull())
	{
		rcSel = m_rcCapture;
	}
	else
	{
		pRT->FillSolidRect(rcWnd, m_crMask);
		return;
	}
	
	// 重点 来了 
	// 分 4 次 来 画 这个 未 选中 的 区域 
	if (rcSel.left > 0)
	{
		CRect rcLeft(0, 0, rcSel.left, rcWnd.bottom);
		pRT->FillSolidRect(rcLeft, m_crMask);
	}	
	if (rcSel.top > 0)
	{
		CRect rcTop(rcSel.left, 0, rcSel.right, rcSel.top);
		pRT->FillSolidRect(rcTop, m_crMask);
	}	
	if (rcSel.right < rcWnd.right)
	{
		CRect rcRight(rcSel.right, 0, rcWnd.right, rcWnd.bottom);
		pRT->FillSolidRect(rcRight, m_crMask);
	}	
	if (rcSel.bottom < rcWnd.bottom)
	{
		CRect rcBottom(rcSel.left, rcSel.bottom, rcSel.right, rcWnd.bottom);
		pRT->FillSolidRect(rcBottom, m_crMask);
	}

	if (bShowM)
	{
#ifdef _DEBUG
		//debug 下 因为 skia 的 DrawRectangle 绘制6 size 的pen 耗时 太长 应该是bug 
		//  等以后这个bug 修复了 再 改回来 
		// 现在 使用 4次 FillSolidRect 来 模拟 
		CRect rcFill = {rcSel.left - 3, rcSel.top - 3, rcSel.right + 3, rcSel.top + 3};
		pRT->FillSolidRect(rcFill, m_crBorder);

		rcFill = {rcSel.left - 3, rcSel.bottom - 3, rcSel.right + 3, rcSel.bottom + 3};
		pRT->FillSolidRect(rcFill, m_crBorder);

		rcFill = {rcSel.left - 3, rcSel.top, rcSel.left + 3, rcSel.bottom};
		pRT->FillSolidRect(rcFill, m_crBorder);

		rcFill = {rcSel.right - 3, rcSel.top, rcSel.right + 3, rcSel.bottom};
		pRT->FillSolidRect(rcFill, m_crBorder);
#else
		CRect rcLine(rcSel);
		CAutoRefPtr<IPen> curPen, oldPen;
		pRT->CreatePen(PS_SOLID, m_crBorder, 5, &curPen);
		pRT->SelectObject(curPen, (IRenderObj**)&oldPen);
		pRT->DrawRectangle(rcLine);
		pRT->SelectObject(oldPen);
#endif // _DEBUG

	}
	else
	{// 画 选中 框
		CRect rcLine(rcSel);
		rcLine.InflateRect(1, 1);
		CAutoRefPtr<IPen> curPen, oldPen;
		pRT->CreatePen(PS_SOLID, m_crBorder, 1, &curPen);
		pRT->SelectObject(curPen, (IRenderObj**)&oldPen);
		pRT->DrawRectangle(rcLine);
		pRT->SelectObject(oldPen);
	}
	
	if (bShowM) return;

	for (int i = 0; i < 8; ++i)
	{
		pRT->FillSolidRect(m_rcPos[i], m_crBorder);
	}
}

void SRectCapture::OnLButtonDown(UINT nFlags, CPoint pt)
{
	if (nullptr == m_bmpBg)
		return;

	SetCapture();
	m_ptClick = pt;
	if (m_rcCapture.IsRectNull())
	{		
		m_bCapturing = true;
		//::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
		return;
	}

	auto dwCursor = IDC_ARROW;
	
	m_eDraging = HitPos(pt);
	switch (m_eDraging)
	{
		case EcPosType::TopLeft:
		case EcPosType::BottomRight:
			dwCursor = IDC_SIZENWSE;
			break;
		case EcPosType::TopCenter:
		case EcPosType::BottomCenter:
			dwCursor = IDC_SIZENS;
			break;
		case EcPosType::TopRight:
		case EcPosType::BottomLeft:
			dwCursor = IDC_SIZENESW;
			break;
		case EcPosType::LeftCenter:
		case EcPosType::RightCenter:
			dwCursor = IDC_SIZEWE;
			break;
		case EcPosType::SelectRect:
			dwCursor = IDC_SIZEALL;
			//m_ptOld = m_rcCapture.TopLeft();
			m_ptClick = pt - m_rcCapture.TopLeft();
			break;
		case EcPosType::Null:
		default:
			break;
	}
	::SetCursor(::LoadCursor(NULL, dwCursor));
}

void SRectCapture::OnLButtonUp(UINT nFlags, CPoint pt)
{
	if (nullptr == m_bmpBg) return;

	m_bCapturing = false;
	m_eDraging = EcPosType::Null;

	if (!m_rcShow.IsRectEmpty())
	{
		m_rcCapture = m_rcShow;
		m_rcShow.SetRectEmpty();
		CalcPos();
		Invalidate();
	}
	
	
	ReleaseCapture();
	EventRectCaptured evt(this);
	evt.pt = pt;
	FireEvent(evt);
}

void SRectCapture::OnMouseMove(UINT nFlags, CPoint pt)
{
	// 正在捕获 框选
	if (m_bCapturing)
	{
		if(!m_rcShow.IsRectEmpty())
			m_rcShow.SetRectEmpty();

		if (m_ptClick == pt)
			return;

		m_rcCapture.SetRect(m_ptClick, pt);
		m_rcCapture.NormalizeRect();
		
		CalcPos();
		Invalidate();
		EventRectCapturing evt(this);
		evt.pt = pt;
		FireEvent(evt);
		return;
	}
	
	// 正常情况
	if (EcPosType::Null == m_eDraging)		// 显示 光标
	{
		ShowCursor(HitPos(pt));
		return;
	}
	
	// 剩下的 就是在 移动 或 变更
	CRect rcWnd = GetWindowRect();
	switch (m_eDraging)
	{
		case EcPosType::Null:

			break;
		case EcPosType::TopLeft:			
			if (pt.x > m_rcCapture.right && pt.y > m_rcCapture.bottom)
			{
				m_eDraging = EcPosType::BottomRight;

				m_rcCapture.left = m_rcCapture.right;
				m_rcCapture.right = pt.x;

				m_rcCapture.top = m_rcCapture.bottom;
				m_rcCapture.bottom = pt.y;
			}
			else if (pt.x > m_rcCapture.right)
			{
				m_eDraging = EcPosType::TopRight;
				m_rcCapture.left = m_rcCapture.right;
				m_rcCapture.right = pt.x;
			}
			else if (pt.y > m_rcCapture.bottom)
			{
				m_eDraging = EcPosType::BottomLeft;
				m_rcCapture.top = m_rcCapture.bottom;
				m_rcCapture.bottom = pt.y;
			}
			else
			{
				m_rcCapture.left = (pt.x > rcWnd.left) ? pt.x : rcWnd.left;
				m_rcCapture.top = (pt.y > rcWnd.top) ? pt.y : rcWnd.top;
			}
			break;
		case EcPosType::TopCenter:
			if (pt.y > m_rcCapture.bottom)
			{
				m_eDraging = EcPosType::BottomCenter;
				m_rcCapture.top = m_rcCapture.bottom;
				m_rcCapture.bottom = pt.y;
			}
			else
			{
				m_rcCapture.top = (pt.y > rcWnd.top) ? pt.y : rcWnd.top;
			}
			break;
		case EcPosType::TopRight:
			if (pt.x < m_rcCapture.left && pt.y > m_rcCapture.bottom)
			{
				m_eDraging = EcPosType::BottomLeft;

				m_rcCapture.right = m_rcCapture.left;
				m_rcCapture.left = pt.x;

				m_rcCapture.top = m_rcCapture.bottom;
				m_rcCapture.bottom = pt.y;
			}
			else if (pt.x < m_rcCapture.left)
			{
				m_eDraging = EcPosType::TopLeft;

				m_rcCapture.right = m_rcCapture.left;
				m_rcCapture.left = pt.x;
			}
			else if (pt.y > m_rcCapture.bottom)
			{
				m_eDraging = EcPosType::BottomRight;

				m_rcCapture.top = m_rcCapture.bottom;
				m_rcCapture.bottom = pt.y;
			}
			else
			{
				m_rcCapture.top = (pt.y > rcWnd.top) ? pt.y : rcWnd.top;
				m_rcCapture.right = (pt.x < rcWnd.right) ? pt.x : rcWnd.right;
			}
			break;
		case EcPosType::RightCenter:
			if (pt.x < m_rcCapture.left)
			{// 如果 过线了 就 换成 反的
				m_eDraging = EcPosType::LeftCenter;
				m_rcCapture.right = m_rcCapture.left;
				m_rcCapture.left = pt.x;
			}
			else
			{
				m_rcCapture.right = (pt.x < rcWnd.right) ? pt.x : rcWnd.right;
			}
			break;
		case EcPosType::BottomRight:
			if (pt.x < m_rcCapture.left && pt.y < m_rcCapture.top)
			{
				m_eDraging = EcPosType::TopLeft;

				m_rcCapture.right = m_rcCapture.left;
				m_rcCapture.left = pt.x;

				m_rcCapture.bottom = m_rcCapture.top;
				m_rcCapture.top = pt.y;
			}
			else if (pt.x < m_rcCapture.left)
			{
				m_eDraging = EcPosType::BottomLeft;

				m_rcCapture.right = m_rcCapture.left;
				m_rcCapture.left = pt.x;
			}
			else if (pt.y < m_rcCapture.top)
			{
				m_eDraging = EcPosType::TopRight;

				m_rcCapture.bottom = m_rcCapture.top;
				m_rcCapture.top = pt.y;
			}
			else
			{
				m_rcCapture.bottom = (pt.y < rcWnd.bottom) ? pt.y : rcWnd.bottom;
				m_rcCapture.right = (pt.x < rcWnd.right) ? pt.x : rcWnd.right;
			}
			break;	
		case EcPosType::BottomCenter:
			if (pt.y < m_rcCapture.top)
			{
				m_eDraging = EcPosType::TopCenter;
				m_rcCapture.bottom = m_rcCapture.top;
				m_rcCapture.top = pt.y;
			}
			else
			{
				m_rcCapture.bottom = (pt.y < rcWnd.bottom) ? pt.y : rcWnd.bottom;
			}
			break;		
		case EcPosType::BottomLeft:
			if (pt.x > m_rcCapture.right && pt.y < m_rcCapture.top)
			{
				m_eDraging = EcPosType::TopRight;

				m_rcCapture.bottom = m_rcCapture.top;
				m_rcCapture.top = pt.y;

				m_rcCapture.left = m_rcCapture.right;
				m_rcCapture.right = pt.x;
			}
			else if (pt.x > m_rcCapture.right)
			{
				m_eDraging = EcPosType::BottomRight;
				m_rcCapture.left = m_rcCapture.right;
				m_rcCapture.right = pt.x;
			}
			else if (pt.y < m_rcCapture.top)
			{
				m_eDraging = EcPosType::TopLeft;
				m_rcCapture.bottom = m_rcCapture.top;
				m_rcCapture.top = pt.y;
			}
			else
			{
				m_rcCapture.bottom = (pt.y < rcWnd.bottom) ? pt.y : rcWnd.bottom;
				m_rcCapture.left = (pt.x > rcWnd.left) ? pt.x : rcWnd.left;
			}
			break;
		case EcPosType::LeftCenter:
			if (pt.x > m_rcCapture.right)
			{
				m_eDraging = EcPosType::RightCenter;
				m_rcCapture.left = m_rcCapture.right;
				m_rcCapture.right = pt.x;
			}
			else
			{
				m_rcCapture.left = (pt.x > rcWnd.left) ? pt.x : rcWnd.left;
			}			
			break;		
		case EcPosType::SelectRect:
		{			
			CPoint ptLT = pt - m_ptClick;			// 相对 鼠标点击 时  的 偏移量  也就是 移动 的 值 
			if (ptLT.x < rcWnd.left)
				ptLT.x = rcWnd.left;
			else if (ptLT.x > rcWnd.right - m_rcCapture.Width())
				ptLT.x = rcWnd.right - m_rcCapture.Width();
			if (ptLT.y < rcWnd.top)
				ptLT.y = rcWnd.top;
			else if (ptLT.y > rcWnd.bottom - m_rcCapture.Height())
				ptLT.y = rcWnd.bottom - m_rcCapture.Height();
			m_rcCapture.MoveToXY(ptLT);
			break;
		}
		default:
			break;		
	}
	ShowCursor(m_eDraging);
	CalcPos();
	Invalidate();
	if (EcPosType::SelectRect == m_eDraging)
	{
		EventRectMoving evt(this);
		evt.pt = pt;
		FireEvent(evt);
	}
	else
	{
		EventRectCapturing evt(this);
		evt.pt = pt;
		FireEvent(evt);
	}	
}

void SRectCapture::OnRButtonDown(UINT nFlags, CPoint pt)
{
}

void SRectCapture::OnRButtonUp(UINT nFlags, CPoint pt)
{
	FireCtxMenu(pt);
}

void SRectCapture::ShowCursor(EcPosType ePos)
{
	auto dwCursor = IDC_ARROW;
	switch (ePos)
	{
		case EcPosType::Null:
			dwCursor = IDC_ARROW;
			break;
		case EcPosType::TopLeft:
		case EcPosType::BottomRight:
			dwCursor = IDC_SIZENWSE;

			break;
		case EcPosType::TopCenter:
		case EcPosType::BottomCenter:
			dwCursor = IDC_SIZENS;
			break;
		case EcPosType::TopRight:
		case EcPosType::BottomLeft:
			dwCursor = IDC_SIZENESW;
			break;
		case EcPosType::LeftCenter:
		case EcPosType::RightCenter:
			dwCursor = IDC_SIZEWE;
			break;
		case EcPosType::SelectRect:
			dwCursor = IDC_SIZEALL;
			break;
		default:
			break;
	}
	::SetCursor(::LoadCursor(NULL, dwCursor));
}

void SRectCapture::CalcPos()
{
	CRect rcLine(m_rcCapture);
	rcLine.InflateRect(1, 1);
	CAutoRefPtr<IPen> curPen, oldPen;

	CPoint center = rcLine.CenterPoint();

	// 上左 方块
	m_rcPos[(int)EcPosType::TopLeft].SetRect(rcLine.left - 1, rcLine.top - 1, rcLine.left + 3, rcLine.top + 3);
	
	// 上中 方块
	m_rcPos[(int)EcPosType::TopCenter].SetRect(center.x - 2, rcLine.top - 1, center.x + 2, rcLine.top + 3);
	
	// 上右 方块
	m_rcPos[(int)EcPosType::TopRight].SetRect(rcLine.right - 3, rcLine.top - 1, rcLine.right + 1, rcLine.top + 3);
	
	// 右中 方块
	m_rcPos[(int)EcPosType::RightCenter].SetRect(rcLine.right - 3, center.y - 2, rcLine.right + 1, center.y + 2);

	// 下右 方块
	m_rcPos[(int)EcPosType::BottomRight].SetRect(rcLine.right - 3, rcLine.bottom - 3, rcLine.right + 1, rcLine.bottom + 1);

	// 下中 方块
	m_rcPos[(int)EcPosType::BottomCenter].SetRect(center.x - 2, rcLine.bottom - 3, center.x + 2, rcLine.bottom + 1);

	// 下左 方块
	m_rcPos[(int)EcPosType::BottomLeft].SetRect(rcLine.left - 1, rcLine.bottom - 3, rcLine.left + 3, rcLine.bottom + 1);

	// 左中 方块
	m_rcPos[(int)EcPosType::LeftCenter].SetRect(rcLine.left - 1, center.y - 2, rcLine.left + 3, center.y + 2);
}

SRectCapture::EcPosType SRectCapture::HitPos(CPoint& pt)
{
	for (int i = 0; i < 8; ++i)
	{
		if (m_rcPos[i].PtInRect(pt))
			return EcPosType(i);
	}

	if (m_rcCapture.PtInRect(pt))
		return EcPosType::SelectRect;

	return EcPosType::Null;
}

}