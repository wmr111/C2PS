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
//----- LayerListBox.h : Declaration of the CLayerListBox
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "LayerEdit.h"
#define WM_MOVEITEM WM_USER+101
#define WM_DELITEM WM_USER+102
#define WM_SETCURITEM WM_USER+103
#define WM_SETSELCHANGE WM_USER+104
#define WM_CREATEITEM WM_USER+105
#define WM_CHANGENAME WM_USER+106
#define WM_CHANGEVISIBLE WM_USER+107
#define WM_CHANGECOLOR WM_USER+108
#define WM_CHANGELOCK WM_USER+109

//-----------------------------------------------------------------------------
class CLayerListBox : public CListBox {
	DECLARE_DYNAMIC (CLayerListBox)
public:
	class LayerData
	{
	public:
		CString LayerText;
		BOOL LayerVisible;
		COLORREF LayerColor;
		BOOL Locked;
		BOOL Selected;
	public:
		LayerData(){};
		LayerData(const LayerData& CopyFrom)
		{
			LayerColor = CopyFrom.LayerColor;
			LayerText = CopyFrom.LayerText;
			LayerVisible = CopyFrom.LayerVisible;
			Locked = CopyFrom.Locked;
			Selected = CopyFrom.Selected;
		};
		LayerData& operator=(const LayerData& CopyFrom)
		{
			LayerColor = CopyFrom.LayerColor;
			LayerText = CopyFrom.LayerText;
			LayerVisible = CopyFrom.LayerVisible;
			Locked = CopyFrom.Locked;
			Selected = CopyFrom.Selected;
			return *this;
		}
	};
public:
	CLayerListBox () ;
	virtual ~CLayerListBox () ;
	void Initial();
	int GetSelect(int nIndex, BOOL& selected) const;
	int SetSelect(int nIndex, BOOL selected);
	int SetCurSel(int nIndex);
	int SetCurSelNoMsg(int nIndex);
	int AlterSelect(int nIndex);

	void LoadIcon(UINT nIDopen, UINT nIDclose, UINT nIDlock);
	int AddString(LPCTSTR lpszItem, COLORREF color = RGB(255, 255, 255), BOOL visible = TRUE, BOOL bLocked = FALSE);
	int AddStringNoMsg(LPCTSTR lpszItem, COLORREF color = RGB(255, 255, 255), BOOL visible = TRUE, BOOL bLocked = FALSE);
	int InsertString(int nIndex, LPCTSTR lpszItem, COLORREF color = RGB(255, 255, 255), BOOL visible = TRUE, BOOL bLocked = FALSE);
	int InsertStringNoMsg(int nIndex, LPCTSTR lpszItem, COLORREF color = RGB(255, 255, 255), BOOL visible = TRUE, BOOL bLocked = FALSE);
	int SetLayerData(int nIndex, LPCTSTR lpszItem, COLORREF color = RGB(255, 255, 255), BOOL visible = TRUE, BOOL bLocked = FALSE);
	int SetLayerColor(int nIndex, COLORREF color);
	int SetLayerColorNoMsg(int nIndex, COLORREF color);
	int SetLayerVisible(int nIndex, BOOL visible);
	int SetLayerVisibleNoMsg(int nIndex, BOOL visible);
	int SetLayerText(int nIndex, CString rText);
	int SetLayerTextNoMsg(int nIndex, CString rText);
	int GetLayerColor(int nIndex, COLORREF& color);
	int GetLayerVisible(int nIndex, BOOL& visible);
	int GetLayerText(int nIndex, CString& rText);
	int GetText(int nIndex, CString& rText);
	int AlterLayerVisible(int nIndex);
	int AlterLayerVisibleNoMsg(int nIndex);
	int SetLock(int nIndex, BOOL bLock);
	int SetLockNoMsg(int nIndex, BOOL bLock);
	int GetLock(int nIndex, BOOL& bLock);
	int DeleteString();
	int DeleteStringNoMsg();
	void MoveItemNoMsg(int nFrom, int nTo);
	void MoveItem(int nFrom, int nTo);
	void MoveMultiItems(int nTo);
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnEditDone(WPARAM, LPARAM);
	afx_msg LRESULT OnEditCancel(WPARAM, LPARAM);

	BOOL OnBeginDrag(CPoint pt);
	void OnCancelDrag(CPoint pt);
	UINT OnDragging(CPoint pt);
	void OnDropped(CPoint pt);
	void DrawDragLine(int nIndex);
protected:
	//当前已选择项总数目
	int m_cCursel;
	//内部字体
	CFont m_Font;
	//文字高度
	int m_cyChar;
	//开眼图标
	HICON m_HicoOpen;
	//闭眼图标
	HICON m_HicoClose;
	//锁定图标
	HICON m_HicoLock;
	//是否在编辑文字
	BOOL m_bEditText;
	//在编辑时,所指的项目指标
	int m_nEditIndex;
	//在拖放时,最初点击的项目指标
	UINT m_nFromItem;
	//在拖放时,上次所在的项目指标
	UINT m_nPrevItem;
	//是否在拖放
	BOOL m_bDraged;
	//是否移动过
	BOOL m_bMoved;
	//编辑框
	CLayerEdit m_Edit;
	//移动图像
	HCURSOR m_hCursorMove;
	//阻止图像
	HCURSOR m_hCursorStop;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
} ;
