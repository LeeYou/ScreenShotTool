#include "StdAfx.h"
#include "MainWnd.h"
//#include "EFile.hpp"
#include "EDialog.hpp"

CMainWnd::CMainWnd()
	: SHostWnd(_T("layout:wnd_main"))//���ﶨ����������Ҫʹ�õĲ����ļ� ��uires.idx�ļ��ж����
{
	//if (::OpenClipboard(NULL))
	//{
	//	//��ռ�����
	//	::EmptyClipboard();
	//	//����Ļ����ճ������������,
	//	//hBitmap Ϊ�ղŵ���Ļλͼ���
	//	::SetClipboardData(CF_BITMAP, hBitmap);
	//	//�رռ�����
	//	::CloseClipboard();
	//}
}


CMainWnd::~CMainWnd(void)
{
}

BOOL CMainWnd::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	InitWnd(m_pCapture, L"rectcapture");

	m_pCapture->GetEventSet()->subscribeEvent(&CMainWnd::OnEventRectCapturing, this);
	m_pCapture->GetEventSet()->subscribeEvent(&CMainWnd::OnEventRectMoving, this);
	m_pCapture->GetEventSet()->subscribeEvent(&CMainWnd::OnEventRectCaptured, this);
	m_pCapture->GetEventSet()->subscribeEvent(&CMainWnd::OnEventCtxMenu, this);

	InitWnd(m_pTextTitle, L"text_title");
	InitWnd(m_pLayCursor, L"lay_cursor");
	m_pLayCursor->SetVisible(FALSE, TRUE);

	InitWnd(m_pImgCursor, L"img_cursor");
	InitWnd(m_pTextRGB, L"text_rgb");
	InitWnd(m_pTextRect, L"text_rect");

	InitWnd(m_pLayTools, L"lay_tools");
	m_pLayTools->SetVisible(FALSE, TRUE);

	//ModifyStyle(0, WS_POPUP);
	//��ȡ��Ļ�ֱ���
	int xScreen = GetSystemMetrics(SM_CXSCREEN);
	int yScreen = GetSystemMetrics(SM_CYSCREEN);
		

	//Ϊ��Ļ�����豸������
	HDC hScreenDC = CreateDCW(_T("DISPLAY"), NULL, NULL, NULL);

	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	HDC hMemDC = CreateCompatibleDC(hScreenDC);

	// ����һ������Ļ�豸��������ݵ�λͼ
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, xScreen, yScreen);
	// ����λͼѡ���ڴ��豸��������
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// ����Ļ�豸�����������ڴ��豸��������

	BitBlt(hMemDC, 0, 0, xScreen, yScreen, hScreenDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);

	::ReleaseDC(NULL, hScreenDC);
	
	// ���� bitmap �ļ�
	size_t nHeaderSize = sizeof(BITMAPINFOHEADER) + 3 * sizeof(RGBQUAD);
	BYTE* pHeader = new BYTE[nHeaderSize];
	LPBITMAPINFO pBmi = (LPBITMAPINFO)pHeader;
	memset(pHeader, 0, nHeaderSize);
	pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmi->bmiHeader.biBitCount = 0;
	int nRet = GetDIBits(hMemDC, hBitmap, 0, 0, NULL, pBmi, DIB_RGB_COLORS);
	
	BITMAPFILEHEADER bmf;
	if (pBmi->bmiHeader.biSizeImage <= 0)
		pBmi->bmiHeader.biSizeImage = pBmi->bmiHeader.biWidth * abs(pBmi->bmiHeader.biHeight) * (pBmi->bmiHeader.biBitCount + 7) / 8;

	int nBitFileHeaderSize = sizeof(BITMAPFILEHEADER);

	BYTE* pFileBuf = new BYTE[nBitFileHeaderSize + nHeaderSize + pBmi->bmiHeader.biSizeImage];
	
	bmf.bfType = 0x4D42;
	bmf.bfReserved1 = bmf.bfReserved2 = 0;
	bmf.bfSize = nBitFileHeaderSize + nHeaderSize + pBmi->bmiHeader.biSizeImage;
	bmf.bfOffBits = nBitFileHeaderSize + nHeaderSize;
	
	nRet = GetDIBits(hMemDC, hBitmap, 0, abs(pBmi->bmiHeader.biHeight), pFileBuf+ nBitFileHeaderSize + nHeaderSize, pBmi, DIB_RGB_COLORS);

	memcpy_s(pFileBuf, nBitFileHeaderSize, &bmf, nBitFileHeaderSize);
	memcpy_s(pFileBuf + nBitFileHeaderSize, nHeaderSize, pBmi, nHeaderSize);

	auto p = SResLoadFromMemory::LoadImage(pFileBuf, nBitFileHeaderSize + nHeaderSize + pBmi->bmiHeader.biSizeImage);

	delete[] pHeader;
	delete[] pFileBuf;

	DeleteObject(hBitmap);
	DeleteDC(hMemDC);

	
	m_pCapture->SetImage(p);


	// ���� ������ʾ�Ĵ��� λ��
	/*BOOL CALLBACK OnEnumWinFun(HWND hWnd, LPARAM lParam)
	{
		CRect rc;
		::GetWindowRect(hWnd, &rc);

		return TRUE;
	}*/
	
	auto fun = [](HWND hWnd, LPARAM lParam) -> BOOL
	{
		if (NULL == hWnd || !::IsWindowVisible(hWnd))
			return TRUE;
		
		/*TCHAR lpTitle[255];
		::GetWindowText(hWnd, lpTitle, 255);
		WINDOWINFO info;
		::GetWindowInfo(hWnd, &info);*/

		CRect rc;
		::GetWindowRect(hWnd, &rc);
		if (!rc.IsRectEmpty() && rc.bottom>0 && rc.right >0)
		{
			auto ls = (std::list<CRect>*)lParam;
			/*for (auto& r : *ls)
			{
				if (r == rc)
					break;

				if(r.rcin)
			}*/
			if(ls->end() == std::find(ls->begin(), ls->end(), rc))
				ls->push_back(rc);
		}

		return TRUE;
	};

	// ö�� ��ʾ�Ĵ���λ�� �����ݽ�ͼ
	::EnumWindows(fun, (LPARAM)&m_RectList);

#ifdef _DEBUG
	SetWindowPos(NULL, 0, 0, xScreen, yScreen, SWP_SHOWWINDOW);
#else
	SetWindowPos(HWND_TOPMOST, 0, 0, xScreen, yScreen, SWP_SHOWWINDOW);
#endif //

//	CRect rc = HitWnd({ 1100,300});
	//m_pCapture->ShowCaptureRect(rc);

	return TRUE;
}

void CMainWnd::OnPaint(HDC dc)
{
	__super::OnPaint(dc);
		
	/*CRect rc = GetClientRect();
	HDC hMemDC = CreateCompatibleDC(dc);
	HGDIOBJ hOldBmp = SelectObject(hMemDC, m_hBitmap);
	BitBlt(dc, 0, 0, rc.Width(), rc.Height(), hMemDC, 0, 0, SRCCOPY);
	::SelectObject(hMemDC, hOldBmp);
	::DeleteDC(hMemDC);*/
}

CRect CMainWnd::HitWnd(const CPoint& pt)
{
	for (auto& rc : m_RectList)
	{
		if (rc.PtInRect(pt))
		{
			return rc;
		}
	}

	return CRect();
}

void CMainWnd::OnMouseMove(UINT nFlags, CPoint pt)
{
	SHostWnd::OnMouseMove(nFlags, pt);
	CRect rcCa = m_pCapture->GetCaptureRect();
	if (rcCa.IsRectEmpty() && !m_pCapture->IsCapturing())
	{
		//HWND h = ::WindowFromPoint(pt);		
		CRect rc = HitWnd(pt);
		if(rc.IsRectEmpty())	return;

		m_pCapture->ShowCaptureRect(rc);

		SStringT sT;
		sT.Format(_T("%d��%d"), rc.Width(), rc.Height());
		ShowLayCursor(pt, sT);
		
		return;
	}
}

void CMainWnd::OnBtnClose()
{
	__super::DestroyWindow();
}

void CMainWnd::OnBtnSave()
{
	ESaveFileDlg dlg(_T("��������ͼƬ.png"));
	if (1 != dlg.DoModal())
		return;
	m_pCapture->SavePng(dlg.GetFilePath());
	OnBtnClose();
}

bool CMainWnd::OnEventRectCapturing(EventRectCapturing* pEvt)
{
	if (m_pLayTools->IsVisible())			// ������ �� �϶�ʱ ����
		m_pLayTools->SetVisible(FALSE, TRUE);

	if (!m_pTextTitle->IsVisible())
		m_pTextTitle->SetVisible(TRUE, FALSE);

	CRect rcCa = m_pCapture->GetCaptureRect();
	SStringT sT;
	sT.Format(_T("%d��%d"), rcCa.Width(), rcCa.Height());

	ShowLayCursor(pEvt->pt, sT);
	m_pTextTitle->SetWindowText(sT);
	ShowTitle();
	return true;
}

bool CMainWnd::OnEventRectMoving(EventRectMoving* pEvt)
{
	ShowTitle();
	ShowTool();
	return true;
}

bool CMainWnd::OnEventRectCaptured(EventRectCaptured* pEvt)
{
	if (!m_pLayTools->IsVisible())						// ������ �� �϶�ʱ ����
		m_pLayTools->SetVisible(TRUE, TRUE);

	if (m_pLayCursor->IsVisible())
		m_pLayCursor->SetVisible(FALSE, TRUE);

	if (!m_pTextTitle->IsVisible())
		m_pTextTitle->SetVisible(TRUE, TRUE);

	if (m_pTextTitle->GetWindowText().IsEmpty())
	{
		CRect rcCap = m_pCapture->GetCaptureRect();
		m_pTextTitle->SetWindowText(SStringT().Format(_T("%d��%d"), rcCap.Width(), rcCap.Height()));
	}

	ShowTitle();
	ShowTool();
	return true;
}
void CMainWnd::ShowLayCursor(const CPoint& pt, LPCTSTR lpRect)
{
	if (!m_pLayCursor->IsVisible())
		m_pLayCursor->SetVisible(TRUE, TRUE);

	// ��ʾ 
	IBitmap* pCurBitmap = (IBitmap*)m_memRT->GetCurrentObject(OT_BITMAP);
	if (nullptr != pCurBitmap)
	{
		auto crCursor = m_memRT->GetPixel(pt.x, pt.y);
		m_pTextRGB->SetWindowText(SStringT().Format(_T("RGB:(%d,%d,%d)"), GetRValue(crCursor), GetGValue(crCursor), GetGValue(crCursor)));


		CAutoRefPtr<IRenderTarget> pMemRT;
		GETRENDERFACTORY->CreateRenderTarget(&pMemRT, 32, 24);

		CRect rcDest(0, 0, 32, 24);
		pMemRT->DrawBitmapEx(rcDest, pCurBitmap, CRect(pt.x - 16, pt.y - 12, pt.x + 16, pt.y + 12), 0);

		IBitmap* pCache = (IBitmap*)pMemRT->GetCurrentObject(OT_BITMAP);

		m_pImgCursor->SetImage(pCache);
	}
	m_pTextRect->SetWindowText(lpRect);
	CRect rcLay = m_pLayCursor->GetWindowRect();
	CRect rcWnd = GetWindowRect();

	CPoint ptLay;
	if ((rcWnd.bottom - pt.y - 20) > rcLay.Height())
	{
		ptLay.y = pt.y + 20;
	}
	else
	{
		ptLay.y = pt.y - rcLay.Height() - 20;
	}
	if ((rcWnd.right - pt.x - 10) > rcLay.Width())
	{
		ptLay.x = pt.x + 10;
	}
	else
	{
		ptLay.x = pt.x - rcLay.Width() - 20;
	}
	
	

	SStringT sLayPos;
	sLayPos.Format(_T("%d,%d"), ptLay.x, ptLay.y);

	m_pLayCursor->SetAttribute(L"pos", sLayPos);
}

void CMainWnd::ShowTitle()
{
	CRect rcText = m_pTextTitle->GetWindowRect();
	CRect rcWnd = GetWindowRect();
	CRect rcCap = m_pCapture->GetCaptureRect();
	
	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top ;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringT sLayPos;
	sLayPos.Format(_T("%d,%d"), nX, nY);
	m_pTextTitle->SetAttribute(L"pos", sLayPos);
}

void CMainWnd::ShowTool()
{
	CRect rcLay = m_pLayTools->GetWindowRect();
	CRect rcWnd = GetWindowRect();
	CRect rcCap = m_pCapture->GetCaptureRect();

	int nX = rcCap.right - rcLay.Width();
	if (nX < rcWnd.left)
		nX = rcWnd.left;
	
	int nY = 0;	
	if ((rcWnd.bottom - rcCap.bottom - 10) > rcLay.Height())			// bottom
	{
		nY = rcCap.bottom + 10;
	}
	else if ((rcCap.top - rcWnd.top - 10) > rcLay.Height())				// top  �� �ռ�
	{
		nY = rcCap.top - rcLay.Height() - 10;
	}
	else // ���Ͻ� �� ��ʾ
	{
		nY = rcCap.top + 10;
	}

	SStringT sLayPos;
	sLayPos.Format(_T("%d,%d"), nX, nY);

	m_pLayTools->SetAttribute(L"pos", sLayPos);
}

bool CMainWnd::OnEventCtxMenu(EventCtxMenu* pEvt)
{
	if (nullptr == pEvt) return true;

	CRect rcCa = m_pCapture->GetCaptureRect();
	if (rcCa.IsRectEmpty())
	{
		SHostWnd::DestroyWindow();
		return true;
	}

	if (rcCa.PtInRect(pEvt->pt))
	{
		// ��ʾ �˵�
		SMenuEx menu;
		if (!menu.LoadMenu(_T("layout:menu_catchscreen")))
		{
			return true;
		}

		CPoint pt;
		GetCursorPos(&pt);
		UINT uCmd = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
		switch (uCmd)
		{
			case 1:				// �Ự�ö�
				break;
			case 2:				// �رջỰ
				break;
			case 3:				// �ر�ȫ���Ự
				break;
			case 4:				// �鿴����
				break;
			case 5:				// �趨��ע
				break;

			default:
				break;
		}
		return true;
	}

	m_pCapture->Reset();
	m_pTextTitle->SetVisible(FALSE, TRUE);
	m_pLayTools->SetVisible(FALSE, TRUE);
	m_pLayCursor->SetVisible(FALSE, TRUE);

	return true;
}