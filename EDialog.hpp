#ifndef __E_DIALOG_H___
#define __E_DIALOG_H___
#pragma once
#include <commdlg.h>
#include <list>

class ESaveFileDlg
{
public:
	ESaveFileDlg(LPCTSTR lpFileName, LPCTSTR lpTitle=NULL, HWND hWndParent=NULL)
	{
		memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
		
		if (NULL != lpTitle)
			lstrcpy(m_szFileTitle, lpTitle);
		else
			m_szFileTitle[0] = '\0';

		if (NULL != lpFileName)
			lstrcpy(m_szFilePath, lpFileName);
		
		m_ofn.lStructSize = sizeof(m_ofn);
		m_ofn.hwndOwner = hWndParent;

		m_ofn.lpstrFile = m_szFilePath;
		m_ofn.nMaxFile = _MAX_PATH;
		m_ofn.lpstrDefExt = nullptr;
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _MAX_FNAME;
		m_ofn.lpstrFilter = _T("All Files\0 * .*\0\0");


		m_ofn.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if (m_ofn.hwndOwner == NULL)   // set only if not specified before
			m_ofn.hwndOwner = hWndParent;

		return ::GetSaveFileName(&m_ofn);
	}

	LPCTSTR GetFilePath()
	{
		return m_szFilePath;
	}
protected:
	OPENFILENAME				m_ofn;
	TCHAR							m_szFileTitle[_MAX_FNAME];												//TCHAR							m_szFileName[_MAX_PATH];		// contains full path name after return
	TCHAR							m_szFilePath[_MAX_PATH];
};

class EOpenFileDlg
{
public:
	EOpenFileDlg(bool bMultiSelect=false, HWND hWndParent = NULL)
		: m_nFileNameLeng(_MAX_PATH)
	{
		memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
		m_szFileTitle[0] = '\0';
		
		if (bMultiSelect)
			m_nFileNameLeng = 100 * _MAX_PATH;

		m_lpFileName = new TCHAR[m_nFileNameLeng];
		memset(m_lpFileName, 0, m_nFileNameLeng);

		m_ofn.lStructSize = sizeof(m_ofn);
		m_ofn.hwndOwner = hWndParent;

		m_ofn.lpstrFile = m_lpFileName;
		m_ofn.nMaxFile = m_nFileNameLeng;
		m_ofn.lpstrDefExt = nullptr;
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _MAX_FNAME;
		m_ofn.lpstrFilter = _T("All Files\0 * .*\0\0");
		

		m_ofn.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (bMultiSelect)
			m_ofn.Flags |= OFN_ALLOWMULTISELECT;
		

		// setup initial file name
		//if(lpszFileName != NULL)
		//	_tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);
	}
	~EOpenFileDlg()
	{
		if(nullptr != m_lpFileName)
		{
			delete[] m_lpFileName;
			m_lpFileName = nullptr;
			m_nFileNameLeng = 0;
		}
	}
	void SetFlags(DWORD dwFlags)
	{

	}
	void SetTitle(LPCTSTR lpTitle)
	{
		m_ofn.lpstrTitle = lpTitle;
	}
	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if(m_ofn.hwndOwner == NULL)   // set only if not specified before
			m_ofn.hwndOwner = hWndParent;
			
		return ::GetOpenFileName(&m_ofn);
	}

	LPCTSTR GetFilePath()
	{
		return m_lpFileName;
	}

	bool GetFileList(std::list<std::wstring>& fileList)
	{
		std::wstring sPath;
		bool bFirst = true;
		int nPos = 1;
		for (int i=1; i<m_nFileNameLeng; ++i)
		{
			if('\0' == m_lpFileName[i])
			{
				if (bFirst)
				{
					sPath = std::wstring(m_lpFileName, i);
					bFirst = false;
				}
				else
				{					
					std::wstring sFile(m_lpFileName + nPos, i - nPos);
					if(sFile.empty())
						break;
					fileList.push_back(sPath + L"\\"+ sFile);
				}
				nPos = i + 1;
			}
		}
		
		if (0 == fileList.size() && !sPath.empty())
			fileList.push_back(sPath);

		return true;
	}
public:
	OPENFILENAME				m_ofn;
protected:
	
	TCHAR							m_szFileTitle[_MAX_FNAME];		// contains file title after return
	//TCHAR							m_szFileName[_MAX_PATH];		// contains full path name after return
	TCHAR*							m_lpFileName;
	int									m_nFileNameLeng;
};

class EDirDialog
{
public:
	EDirDialog(LPCTSTR lpTitle=nullptr, HWND hwndOwner=NULL)
	{
		memset(m_szDir, 0, MAX_PATH);
	
		m_BrowserInfo.hwndOwner = hwndOwner;
		m_BrowserInfo.pidlRoot = NULL;
		m_BrowserInfo.pszDisplayName = m_szDir;
		m_BrowserInfo.lpszTitle = lpTitle;
		m_BrowserInfo.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
		m_BrowserInfo.lpfn = NULL;
		m_BrowserInfo.lParam = 0;
		m_BrowserInfo.iImage = 0;

	}

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		if (NULL == m_BrowserInfo.hwndOwner)   // set only if not specified before
			m_BrowserInfo.hwndOwner = hWndParent;

		ITEMIDLIST* pidl = SHBrowseForFolder(&m_BrowserInfo);
		if (pidl == NULL)//
		{
			return 0;	//Ã»ÓÐÑ¡s
		}
		if (!SHGetPathFromIDList(pidl, m_szDir))
		{
			return 0;
		}

		return 1;
	}
	LPCTSTR GetDirPath()
	{
		return m_szDir;
	}
protected:
	BROWSEINFO				m_BrowserInfo;
	TCHAR						m_szDir[MAX_PATH];
};


#endif