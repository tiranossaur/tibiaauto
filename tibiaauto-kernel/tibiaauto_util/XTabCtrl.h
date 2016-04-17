#pragma once
#include "tibiaauto_util.h"

#include "Skin.h"
class CConfigData;

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl window
#include <afxtempl.h>

class TIBIAAUTOUTIL_API CXTabCtrl : public CTabCtrl
{
// Construction
public:
	CXTabCtrl();

// Attributes
public:

// Operations
public:
	void AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage = 0);
	void EnableTab(int iIndex, BOOL bEnable = TRUE);
	BOOL SelectTab(int iIndex);
	void DeleteAllTabs();
	void DeleteTab(int iIndex);
	void SetTopLeftCorner(CPoint pt);
	BOOL IsTabEnabled(int iIndex);

	void SetDisabledColor(COLORREF cr);
	void SetSelectedColor(COLORREF cr);
	void SetNormalColor(COLORREF cr);
	void SetMouseOverColor(COLORREF cr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTabCtrl)
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTabColor(COLORREF);
	BOOL SelectNextTab(BOOL bForward);
	BOOL SelectTabByName(LPCTSTR lpszTabCaption);
	void ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage);
	virtual ~CXTabCtrl();

	// Generated message map functions
protected:
	CArray<BOOL, BOOL> m_arrayStatusTab; //** enabled Y\N
	CBrush foreBrush;
	int m_iSelectedTab;
	POINT m_ptTabs;
	COLORREF m_crTab;
	COLORREF m_crSelected;
	COLORREF m_crDisabled;
	COLORREF m_crNormal;
	COLORREF m_crMouseOver;

	int m_iIndexMouseOver;

	bool m_bMouseOver;
	bool m_bColorMouseOver;
	bool m_bColorNormal;
	bool m_bColorDisabled;
	bool m_bColorSelected;
	bool m_bColorTab;

	//{{AFX_MSG(CXTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
