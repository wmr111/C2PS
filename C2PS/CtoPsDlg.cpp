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
//----- CtoPsDlg.cpp : Implementation of CCtoPsDlg
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "CtoPsDlg.h"


//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CCtoPsDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(CCtoPsDlg, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
	ON_MESSAGE(WM_MOVEITEM, &CCtoPsDlg::OnMoveItem)
	ON_MESSAGE(WM_DELITEM, &CCtoPsDlg::OnDelItem)
	ON_MESSAGE(WM_SETCURITEM, &CCtoPsDlg::OnSetCurItem)
	ON_MESSAGE(WM_CREATEITEM, &CCtoPsDlg::OnCreateItem)
	ON_MESSAGE(WM_CHANGENAME, &CCtoPsDlg::OnChangeName)
	ON_MESSAGE(WM_CHANGEVISIBLE, &CCtoPsDlg::OnChangeVisible)
	ON_MESSAGE(WM_CHANGECOLOR, &CCtoPsDlg::OnChangeColor)
	ON_MESSAGE(WM_CHANGELOCK, &CCtoPsDlg::OnChangeLock)
	ON_BN_CLICKED(IDC_NEWPROJECT, &CCtoPsDlg::OnClickedNewProject)
	ON_BN_CLICKED(IDC_NEWLAYER, &CCtoPsDlg::OnClickedNewlayer)
	ON_BN_CLICKED(IDC_NEWBOUND, &CCtoPsDlg::OnClickedNewbound)
	ON_BN_CLICKED(IDC_MODIFYLAYER, &CCtoPsDlg::OnClickedModifylayer)
	ON_BN_CLICKED(IDC_DELLAYER, &CCtoPsDlg::OnClickedDellayer)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CCtoPsDlg::CCtoPsDlg (CWnd *pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog (CCtoPsDlg::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void CCtoPsDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange (pDX) ;
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT CCtoPsDlg::OnAcadKeepFocus (WPARAM, LPARAM) {
	return (TRUE) ;
}

void CCtoPsDlg::OnClickedNewProject()
{
	if (!m_Project.CreateNewPrj())
		return;
	m_Project.PrintPDF();
	m_Project.CropPDF();
	m_Project.CreatePSD();
	m_ListBox.AddStringNoMsg(_T("图元层"));
	m_ListBox.AddString(_T("底图层"));
	m_ListBox.MoveItem(1, 0);
	m_ListBox.SetLockNoMsg(0, TRUE);
	//
	GetDlgItem(IDC_NEWLAYER)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELLAYER)->EnableWindow(TRUE);
	GetDlgItem(IDC_MODIFYLAYER)->EnableWindow(TRUE);
	GetDlgItem(IDC_SHOWPS)->EnableWindow(TRUE);
	
}

void CCtoPsDlg::OnClickedNewlayer()
{
	m_ListBox.AddString(_T("新图层"));

}


void CCtoPsDlg::OnClickedNewbound()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CCtoPsDlg::OnClickedModifylayer()
{
	static BOOL s_bInOperation = FALSE;
	if (s_bInOperation)
	{
		if (!m_Project.EndModifylayerInCAD())
			return;
		m_Project.EndModifylayer();
		m_ListBox.EnableWindow(TRUE);
		s_bInOperation = FALSE;
		GetDlgItem(IDC_MODIFYLAYER)->SetWindowText(_T("修改图层"));
	}
	else
	{
		if (!m_Project.ModifylayerInCAD())
			return;
		m_ListBox.EnableWindow(FALSE);
		s_bInOperation = TRUE;
		GetDlgItem(IDC_MODIFYLAYER)->SetWindowText(_T("结束修改"));
		
	}
	GetDlgItem(IDC_NEWBOUND)->EnableWindow(s_bInOperation);
	GetDlgItem(IDC_SELECTBOUND)->EnableWindow(s_bInOperation);
}


void CCtoPsDlg::OnClickedDellayer()
{
	m_ListBox.DeleteString();
}

LRESULT CCtoPsDlg::OnMoveItem(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
	return 0;
	m_Project.SwapItemInCAD((int)wParam, (int)lParam);
	m_Project.SwapItem((int)wParam, (int)lParam);*/
	return 0;
}

LRESULT CCtoPsDlg::OnDelItem(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
	return 0;
	m_Project.DelItem((int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnSetCurItem(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
		return 0;
	m_Project.SetCurItem((int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnCreateItem(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
		return 0;
	m_Project.CreateItemInCAD((CLayerListBox::LayerData*) wParam, (int)lParam);
	m_Project.CreateItem((CLayerListBox::LayerData*) wParam, (int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnChangeName(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
		return 0;
	m_Project.ChangeName((LPTSTR)wParam, (int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnChangeVisible(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
		return 0;
	m_Project.ChangeVisible((BOOL)wParam, (int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
		return 0;
	m_Project.ChangeColor((COLORREF)wParam, (int)lParam);*/
	return 0;
}
LRESULT CCtoPsDlg::OnChangeLock(WPARAM wParam, LPARAM lParam)
{
	/*if (!m_bExistPrj)
	return 0;
	m_Project.ChangeLock((BOOL)wParam, (int)lParam);*/
	return 0;
}

BOOL CCtoPsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rect;
	GetDlgItem(IDC_PLACEHOLDER)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_ListBox.Create(WS_CHILD|WS_VISIBLE, rect, this, IDC_LAYERLIST);
	m_ListBox.LoadIcon(IDI_OPEN, IDI_CLOSE, IDI_LOCK);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}




void CCtoPsDlg::OnOK()
{
	//CAcUiDialog::OnOK();
}


void CCtoPsDlg::OnCancel()
{
	//CAcUiDialog::OnCancel();
}


BOOL CCtoPsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN && IsChild(GetFocus()))
	{
		::TranslateMessage(pMsg);
		GetFocus()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}
	return CAcUiDialog::PreTranslateMessage(pMsg);
}
