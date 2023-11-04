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
//----- CtoPsDlg.h : Declaration of the CCtoPsDlg
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "LayerListBox.h"
#include "CtoPsPrj.h"

//-----------------------------------------------------------------------------
class CCtoPsDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (CCtoPsDlg)

public:
	CCtoPsDlg (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_CTOPS} ;
protected:
	CCtoPsPrj m_Project;
	CLayerListBox m_ListBox;
protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedNewProject();
	afx_msg LRESULT OnMoveItem(WPARAM, LPARAM);
	afx_msg LRESULT OnDelItem(WPARAM, LPARAM);
	afx_msg LRESULT OnSetCurItem(WPARAM, LPARAM);
	afx_msg LRESULT OnCreateItem(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeName(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeVisible(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeColor(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeLock(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
protected:
	
public:
	afx_msg void OnClickedNewlayer();
	afx_msg void OnClickedNewbound();
	afx_msg void OnClickedModifylayer();
	afx_msg void OnClickedDellayer();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

} ;
