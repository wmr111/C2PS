// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- LayerListBox.cpp : Implementation of CLayerListBox
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "LayerListBox.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CLayerListBox, CListBox)

BEGIN_MESSAGE_MAP(CLayerListBox, CListBox)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_EDITDONE, &CLayerListBox::OnEditDone)
	ON_MESSAGE(WM_EDITCANCEL, &CLayerListBox::OnEditCancel)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CLayerListBox::CLayerListBox () {
}

//-----------------------------------------------------------------------------
CLayerListBox::~CLayerListBox () {
}
//创建字体,创建文本框
void CLayerListBox::Initial()
{
	m_Font.CreatePointFont(100, _T("宋体"));
	m_HicoOpen = NULL;
	m_HicoClose = NULL;
	m_bEditText = FALSE;
	CClientDC dc(this);
	dc.SelectObject(m_Font);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_cyChar = tm.tmHeight;
	m_Edit.Create(WS_CHILD| ES_LEFT, CRect(0, 0, 0, 0), this, 100);
	m_Edit.SetFont(&m_Font);
	m_nFromItem = -1;
	m_nPrevItem = -1;
	m_bEditText = FALSE;
	m_bDraged = FALSE;
	m_bMoved = FALSE;
	m_cCursel = 0;
	m_hCursorMove = ::LoadCursor(NULL, IDC_HAND);
	m_hCursorStop = ::LoadCursor(NULL, IDC_NO);

}
//获取项目是否被选定
int CLayerListBox::GetSelect(int nIndex, BOOL& selected) const
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		return LB_ERR;
	selected = pData->Selected;
	return nIndex;
}
//设置项目被加选或删选,向父窗口发送消息,更新项目绘图
int CLayerListBox::SetSelect(int nIndex, BOOL selected)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (pData->Selected == selected)
		return nIndex;
	pData->Selected = selected;
	if (pData->Selected)
		m_cCursel++;
	else
		m_cCursel--;
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	GetParent()->SendMessage(WM_SETSELCHANGE, selected, nIndex);
	return nIndex;
}
//设置项目被单选,向父窗口发送消息,更新项目绘图;-1项表示什么都没选
int CLayerListBox::SetCurSel(int nIndex)
{
	if (SetCurSelNoMsg(nIndex) == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_SETCURITEM, 0, nIndex);
	return nIndex;
}
//设置项目被单选,不向父窗口发送消息,更新项目绘图;-1项表示什么都没选
int CLayerListBox::SetCurSelNoMsg(int nIndex)
{
	if (nIndex!=-1 && nIndex>=GetCount())
		return LB_ERR;
	CRect rect;
	if (m_cCursel != 0)
	{
		for (int n=0;n<GetCount();n++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(n);
			if (pData->Selected == TRUE)
			{
				pData->Selected = FALSE;
				GetItemRect(n, &rect);
				InvalidateRect(&rect);
			}
		}
		m_cCursel = 0;
	}
	if (nIndex!=-1)
	{
		m_cCursel++;
		LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
		pData->Selected = TRUE;
		GetItemRect(nIndex, &rect);
		InvalidateRect(&rect);
	}
	UpdateWindow();
	return nIndex;
}
//开关切换选择,并向父窗口发消息
int CLayerListBox::AlterSelect(int nIndex)
{
	BOOL selected;
	if (GetSelect(nIndex, selected) == LB_ERR)
		return LB_ERR;
	return SetSelect(nIndex, !selected);
}
//加载图标
void CLayerListBox::LoadIcon(UINT nIDopen, UINT nIDclose, UINT nIDlock)
{
	if (nIDopen)
		m_HicoOpen = ::LoadIcon(AfxGetModuleState()->m_hCurrentResourceHandle, MAKEINTRESOURCE(nIDopen));
	if (nIDclose)
		m_HicoClose = ::LoadIcon(AfxGetModuleState()->m_hCurrentResourceHandle, MAKEINTRESOURCE(nIDclose));
	if (nIDlock)
		m_HicoLock = ::LoadIcon(AfxGetModuleState()->m_hCurrentResourceHandle, MAKEINTRESOURCE(nIDlock));
}
//在默认选定条件下加入新项,向父窗口发送消息
int CLayerListBox::AddString(LPCTSTR lpszItem, COLORREF color, BOOL visible, BOOL bLocked)
{
	int nIndex;
	if (m_cCursel == 0)
	{
		nIndex = 0;
	}
	else
	{
		for (nIndex=0;nIndex<GetCount();nIndex++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected)
				break;
		}
	}
	InsertString(nIndex, lpszItem, color, visible, bLocked);
	return nIndex;
}
//在默认选定条件下加入新项,不向父窗口发送消息
int CLayerListBox::AddStringNoMsg(LPCTSTR lpszItem, COLORREF color, BOOL visible, BOOL bLocked)
{
	int nIndex;
	if (m_cCursel == 0)
	{
		nIndex = 0;
	}
	else
	{
		for (nIndex=0;nIndex<GetCount();nIndex++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected)
				break;
		}
	}
	InsertStringNoMsg(nIndex, lpszItem, color, visible, bLocked);
	return nIndex;
}
//在指定位置加入新项,向父窗口发送消息
int CLayerListBox::InsertString(int nIndex, LPCTSTR lpszItem, COLORREF color, BOOL visible, BOOL bLocked)
{
	if (InsertStringNoMsg(nIndex, lpszItem, color, visible, bLocked) == LB_ERR)
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	GetParent()->SendMessage(WM_CREATEITEM, (WPARAM)pData, nIndex);
	GetParent()->SendMessage(WM_SETCURITEM, 0, nIndex);
	return nIndex;
}
//在指定位置加入新项,不向父窗口发送消息
int CLayerListBox::InsertStringNoMsg(int nIndex, LPCTSTR lpszItem, COLORREF color, BOOL visible, BOOL bLocked)
{
	nIndex = CListBox::InsertString(nIndex, _T(""));
	if (nIndex==LB_ERRSPACE || nIndex==LB_ERR)
		return LB_ERR;
	SetLayerData(nIndex, lpszItem ,color, visible, bLocked);
	SetCurSelNoMsg(nIndex);
	return nIndex;
}
//设置项目数据,不向父窗口发消息
int CLayerListBox::SetLayerData(int nIndex, LPCTSTR lpszItem, COLORREF color, BOOL visible, BOOL bLocked)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		pData = new LayerData;
	pData->LayerText = lpszItem;
	pData->LayerColor = color;
	pData->LayerVisible = visible;
	pData->Locked = bLocked;
	pData->Selected = FALSE;
	SetItemDataPtr(nIndex, pData);
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	return nIndex;
}
//改变项目颜色,并向父窗口发送消息,更新项目绘图
int CLayerListBox::SetLayerColor(int nIndex, COLORREF color)
{
	if (SetLayerColorNoMsg(nIndex, color) == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_CHANGECOLOR, color, nIndex);
	return nIndex;
}
//改变项目颜色,不并向父窗口发送消息,更新项目绘图
int CLayerListBox::SetLayerColorNoMsg(int nIndex, COLORREF color)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	pData->LayerColor = color;
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	return nIndex;
}
//设置可见性,向父窗口发消息,更新项目绘图
int CLayerListBox::SetLayerVisible(int nIndex, BOOL visible)
{
	if (SetLayerVisibleNoMsg(nIndex, visible) == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_CHANGEVISIBLE, visible, nIndex);
	return nIndex;
}
//设置可见性,不向父窗口发消息,更新项目绘图
int CLayerListBox::SetLayerVisibleNoMsg(int nIndex, BOOL visible)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	pData->LayerVisible = visible;
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	return nIndex;
}
int CLayerListBox::SetLayerText(int nIndex, CString rText)
{
	if (SetLayerTextNoMsg(nIndex, rText) == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_CHANGENAME, (WPARAM)rText.GetBuffer(), nIndex);
	return nIndex;
}
int CLayerListBox::SetLayerTextNoMsg(int nIndex, CString rText)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	pData->LayerText = rText;
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	return nIndex;
}
//获取颜色
int CLayerListBox::GetLayerColor(int nIndex, COLORREF& color)
{

	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		return LB_ERR;
	color = pData->LayerColor;
	return nIndex;
}
//获得项目可见性
int CLayerListBox::GetLayerVisible(int nIndex, BOOL& visible)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		return LB_ERR;
	visible = pData->LayerVisible;
	return nIndex;
}
//获取项目文本
int CLayerListBox::GetLayerText(int nIndex, CString& rText)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		return LB_ERR;
	rText = pData->LayerText;
	return nIndex;
}
//GetLayerText的别名
inline int CLayerListBox::GetText(int nIndex, CString& rText)
{
	return GetLayerText(nIndex, rText);
}
//开关切换可见性,并向父窗口发消息
int CLayerListBox::AlterLayerVisible(int nIndex)
{
	BOOL visible;
	if (GetLayerVisible(nIndex, visible) == LB_ERR)
		return LB_ERR;
	return SetLayerVisible(nIndex, !visible);
}
//开关切换可见性,不并向父窗口发消息
int CLayerListBox::AlterLayerVisibleNoMsg(int nIndex)
{
	BOOL visible;
	if (GetLayerVisible(nIndex, visible) == LB_ERR)
		return LB_ERR;
	return SetLayerVisibleNoMsg(nIndex, !visible);
}
//设置锁定,向父窗口发消息,更新项目绘图
int CLayerListBox::SetLock(int nIndex, BOOL bLock)
{
	if (SetLockNoMsg(nIndex, bLock) == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_CHANGELOCK, bLock, nIndex);
	return nIndex;
}
int CLayerListBox::SetLockNoMsg(int nIndex, BOOL bLock)
{
	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	pData->Locked = bLock;
	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(&rect);
	UpdateWindow();
	return nIndex;
}
//获取项目锁定情况
int CLayerListBox::GetLock(int nIndex, BOOL& bLock)
{

	if (nIndex>=GetCount())
		return LB_ERR;
	LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
	if (!pData)
		return LB_ERR;
	bLock = pData->Locked;
	return nIndex;
}
//
void CLayerListBox::MoveItemNoMsg(int nFrom, int nTo)
{
	LayerData* pNewData = new LayerData;
	LayerData* pDelData = (LayerData*)GetItemDataPtr(nFrom);
	*pNewData = *pDelData;
	CListBox::DeleteString(nFrom);
	int nIndex = (nFrom<nTo)?(nTo-1):(nTo);
	CListBox::InsertString(nIndex, _T(""));
	SetItemDataPtr(nIndex, pNewData);
}
//
void CLayerListBox::MoveItem(int nFrom, int nTo)
{
	if (nFrom==nTo || nFrom+1==nTo)
		return;
	MoveItemNoMsg(nFrom, nTo);
	GetParent()->SendMessage(WM_MOVEITEM, nTo, nFrom);
}
//
void CLayerListBox::MoveMultiItems(int nTo)
{
	if (m_cCursel==0)
		return;
	if (m_cCursel==1)
	{
		int nIndex;
		for (nIndex=0;nIndex<GetCount();nIndex++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected == TRUE)
				break;
		}
		MoveItem(nIndex, nTo);
		return;
	}
	else
	{
		SendMessage(WM_SETREDRAW, FALSE, 0);
		int nIndex, nInsert = nTo;
		for (nIndex=0;nIndex<nInsert;)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected == TRUE)
			{
				MoveItemNoMsg(nIndex, nTo);
				nInsert--;
			}
			else
			{
				nIndex++;
			}
		}
		for (nIndex=nTo;nIndex<GetCount();nIndex++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected == FALSE)
				break;
		}
		nInsert = nIndex;
		nIndex++;
		for (;nIndex<GetCount();nIndex++)
		{
			LayerData* pData = (LayerData*)GetItemDataPtr(nIndex);
			if (pData->Selected == TRUE)
			{
				MoveItemNoMsg(nIndex, nInsert);
				nInsert++;
			}
		}
		SendMessage(WM_SETREDRAW, TRUE, 0);
		GetParent()->SendMessage(WM_MOVEITEM, nTo, -1);
	}
}

int CLayerListBox::DeleteString()
{

	int nIndex = DeleteStringNoMsg();
	if (nIndex == LB_ERR)
		return LB_ERR;
	GetParent()->SendMessage(WM_DELITEM, 0, 0);
	GetParent()->SendMessage(WM_SETCURITEM, 0, nIndex);
	return nIndex;
}

int CLayerListBox::DeleteStringNoMsg()
{
	if (m_cCursel==0)
		return LB_ERR;
	CArray<int> SelectArray;
	CRect rect;
	for (int n=GetCount()-1;n>=0;n--)
	{
		LayerData* pData = (LayerData*)GetItemDataPtr(n);
		if (pData->Selected == TRUE)
		{
			if (pData->Locked == TRUE)
				return LB_ERR;
			SelectArray.Add(n);
		}
	}
	for (int n=0;n<SelectArray.GetCount();n++)
	{
		CListBox::DeleteString(SelectArray.GetAt(n));	
	}
	m_cCursel=0;
	int nSelect = SelectArray.GetAt(SelectArray.GetCount()-1);
	if (nSelect >= GetCount())
		nSelect = GetCount()-1;
	SetCurSelNoMsg(nSelect);
	return nSelect;
}

// CLayerListBox 消息处理程序



//设置样式,设定最小宽度
BOOL CLayerListBox::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LBS_OWNERDRAWVARIABLE| LBS_SORT);
	cs.style |= LBS_OWNERDRAWFIXED| LBS_HASSTRINGS| LBS_NOTIFY| LBS_DISABLENOSCROLL| LBS_MULTIPLESEL| WS_BORDER| WS_VSCROLL | WS_CHILD| WS_VISIBLE;
	if (cs.cx<250)
		cs.cx = 250;
	return CListBox::PreCreateWindow(cs);
}
//设置项目高度
void CLayerListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = 40;
}
//绘制项目
void CLayerListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (lpDIS->itemID == -1)
		return;
	LayerData* pData = (LayerData*)GetItemDataPtr(lpDIS->itemID);
	if (!pData)
		return;
	CDC dc;
	dc.Attach(lpDIS->hDC);
	dc.SaveDC();

	CString ItemText;
	ItemText = pData->LayerText;
	CRect rect = lpDIS->rcItem;

	CBrush BrushBG;
	BrushBG.CreateSolidBrush(pData->Selected?RGB(191, 191, 255):RGB(255, 255, 255));
	dc.SelectObject(BrushBG);
	rect.left = 40;
	dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	rect.top += 5;
	rect.bottom -= 5;
	rect.left = 5;
	rect.right = 35;

	if (m_HicoOpen && pData->LayerVisible)
	{
		dc.DrawIcon(rect.left, rect.top, m_HicoOpen);
	}
	else if (m_HicoClose && !pData->LayerVisible)
	{
		dc.DrawIcon(rect.left, rect.top, m_HicoClose);
	}
	rect.left = 45;
	rect.right = 75;
	CBrush BrushLC;
	BrushLC.CreateSolidBrush(pData->LayerColor);
	dc.SelectObject(BrushLC);
	dc.Rectangle(rect);

	rect.left = 85;
	rect.right = lpDIS->rcItem.right;
	dc.SelectObject(m_Font);
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(ItemText, rect, DT_SINGLELINE|DT_VCENTER);

	if (m_HicoLock && pData->Locked)
	{
		rect.left = rect.right-40;
		dc.DrawIcon(rect.left, rect.top, m_HicoLock);
	}
	dc.RestoreDC(-1);
	dc.Detach();
}
//删除项目
void CLayerListBox::DeleteItem(LPDELETEITEMSTRUCT lpDIS)
{
	LayerData* pData = (LayerData*)GetItemDataPtr(lpDIS->itemID);
	if (pData)
		delete pData;
	CListBox::DeleteItem(lpDIS);
}
//初始化
int CLayerListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	Initial();
	return 0;
}
//双击消息
void CLayerListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint pt;
	::GetCursorPos(&pt);
	int nIndex = AfxLBItemFromPt(m_hWnd, pt, FALSE);
	if (nIndex==-1 || GetCount()==0)
		return;

	ScreenToClient(&pt);
	CRect rect;
	GetItemRect(nIndex, &rect);
	int nX = pt.x-rect.left;
	if (nX>5 && nX<35)
	{
		AlterLayerVisible(nIndex);
		return;
	}
	if (nX>45 && nX<75)
	{
		CColorDialog ColorDlg;
		if (ColorDlg.DoModal() == IDOK)
		{
			COLORREF color = ColorDlg.GetColor();
			SetLayerColor(nIndex, color);
		}
		return;
	}
	if (nX>85 && !m_bEditText)
	{
		CString strText;
		GetText(nIndex, strText);
		CRect rect;
		GetItemRect(nIndex, &rect);
		rect.top = (rect.top+rect.bottom-m_cyChar)/2;
		rect.left = 85;
		rect.right -= 40;
		rect.bottom = rect.top+m_cyChar;
		m_Edit.SetWindowText(strText);
		m_Edit.MoveWindow(&rect);
		m_Edit.ShowWindow(SW_SHOW);
		m_Edit.SetFocus();
		m_bEditText = TRUE;
		m_nEditIndex = nIndex;
		return;
	}
	CListBox::OnLButtonDblClk(nFlags, point);
}

LRESULT CLayerListBox::OnEditDone(WPARAM wParam, LPARAM lParam)
{
	m_bEditText = FALSE;
	CString rStr = (TCHAR*)lParam;
	SetLayerText(m_nEditIndex, rStr);
	return 0;
}

LRESULT CLayerListBox::OnEditCancel(WPARAM wParam, LPARAM lParam)
{
	m_bEditText = FALSE;
	return 0;
}

void CLayerListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

//拖放时绘制虚线
void CLayerListBox::DrawDragLine(int nIndex)
{
	CClientDC dc(this);
	CPen Pen(PS_SOLID, 3 , RGB(255, 255, 255));
	dc.SelectObject(&Pen);
	dc.SetROP2(R2_XORPEN);
	dc.SetBkMode(TRANSPARENT);
	CRect rect;
	if (nIndex<GetCount())
	{
		GetItemRect(nIndex, &rect);
		rect.left = 42;
		rect.right -= 2;
		dc.MoveTo(rect.left, rect.top);
		dc.LineTo(rect.right, rect.top);
	}
	else
	{
		GetItemRect(GetCount()-1, &rect);
		rect.left = 42;
		rect.right -= 2;
		dc.MoveTo(rect.left, rect.bottom);
		dc.LineTo(rect.right, rect.bottom);
	}
}
//单击
void  CLayerListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (GetCount()==0)
		return;
	CPoint pt;
	::GetCursorPos(&pt);
	int nIndex = AfxLBItemFromPt(m_hWnd, pt, FALSE);
	if (nIndex==-1)
	{
		SetCurSel(-1);
		return;
	}
	ScreenToClient(&pt);
	CRect rect;
	GetItemRect(nIndex, &rect);
	int nX = pt.x-rect.left;
	if (nX>5 && nX<35)
	{
		AlterLayerVisible(nIndex);
		return;
	}
	if (nFlags & MK_CONTROL)
	{
		AlterSelect(nIndex);
		return;
	}
	BOOL selected;
	GetSelect(nIndex, selected);
	if (!selected)
	{
		SetCurSel(nIndex);
		return;
	}
	else
	{
		m_bDraged = TRUE;
		m_nFromItem = nIndex;
		SetCapture();
		if (pt.y>(rect.top+rect.bottom)/2)
			nIndex++;
		DrawDragLine(nIndex);
		m_nPrevItem = nIndex;
		return;
	}
}
//移动
void CLayerListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDraged)
		return;
	m_bMoved = TRUE;
	CPoint pt;
	::GetCursorPos(&pt);
	int nIndex = AfxLBItemFromPt(m_hWnd, pt, FALSE);
	if (nIndex == -1)
	{
		if (m_nPrevItem == -1)
			return;
		DrawDragLine(m_nPrevItem);
		m_nPrevItem = -1;
		::SetCursor(m_hCursorStop);
		return;
	}
	ScreenToClient(&pt);
	CRect rect;
	GetItemRect(nIndex, &rect);
	if (pt.y>(rect.top+rect.bottom)/2)
		nIndex++;
	if (m_nPrevItem == nIndex)
		return;
	if (m_nPrevItem != -1)
		DrawDragLine(m_nPrevItem);
	DrawDragLine(nIndex);
	m_nPrevItem = nIndex;
	::SetCursor(m_hCursorMove);
	return;
}
//弹起
void CLayerListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDraged)
		return;
	CPoint pt;
	::GetCursorPos(&pt);
	int nIndex = AfxLBItemFromPt(m_hWnd, pt, FALSE);
	if (nIndex != -1)
	{
		ScreenToClient(&pt);
		CRect rect;
		GetItemRect(nIndex, &rect);
		if (pt.y>(rect.top+rect.bottom)/2)
			nIndex++;
	}

	if (m_nPrevItem != -1)
		DrawDragLine(m_nPrevItem);
	if (nIndex != -1)
	{
		if (!m_bMoved)
		{
			SetCurSel(m_nFromItem);
		}
		else
		{
			MoveMultiItems(nIndex);
		}
	}
	m_bDraged = FALSE;
	m_bMoved = FALSE;
	m_nFromItem = -1;
	m_nPrevItem = -1;
	ReleaseCapture();
	return;
}


BOOL CLayerListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_nFromItem != -1)
	{
		return TRUE;
	}

	return CListBox::OnMouseWheel(nFlags, zDelta, pt);
}


void CLayerListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_DELETE)
	{
		DeleteString();
		return;
	}
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}



