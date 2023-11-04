#pragma once
#include "StdAfx.h"
#include "CApplication.h"
#include "CDocument0.h"
#include "LayerListBox.h"
class CCtoPsPrj
{
public:
	BOOL CreateNewPrj();
	void PrintPDF();
	void CropPDF();
	void CreatePSD();
	void CreateItem(CLayerListBox::LayerData*, int);
	void CreateItemInCAD(CLayerListBox::LayerData*, int);

	void ChangeLock(BOOL, int);
	void ChangeName(LPTSTR, int);
	void ChangeVisible(BOOL, int);
	void ChangeColor(COLORREF, int);

	void SetCurItem(int);
	void SwapItem(int, int);
	void SwapItemInCAD(int, int);
	void DelItem(int);

	
	BOOL ModifylayerInCAD();
	void EndModifylayer();
	BOOL EndModifylayerInCAD();
	void DrawEntityInPS(AcDbEntity*);


	static void RegisterCommand();
	static void PickFirst();
protected:
	AcGePoint2d m_LBPn;
	AcGePoint2d m_RTPn;
	double m_Scale;
	CString m_strPath;
	CApplication m_PSApp;
	CDocument0 m_PSDoc;

	AcDbObjectIdArray m_ObjIdArray;
	AcArray<COLORREF> m_ColorArray;
	int m_CurSel;

	CStringArray m_LockedLayerName;
	AcDbObjectId m_CurLayerId;
};