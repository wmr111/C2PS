#pragma once
#include "StdAfx.h"
#include "CtoPsPrj.h"

#include "CCAcroPDDoc.h"
#include "CCAcroRect.h"

#include "CArtLayer.h"
#include "CArtLayers.h"

#include "CPDFOpenOptions.h"
#include "CRGBColor.h"
#include "CSelection.h"
#include "CSolidColor.h"

BOOL CCtoPsPrj::CreateNewPrj()
{
	ads_point point1, point2;
	acedInitGet(RSG_NONULL, NULL);
	int rt;
	rt = acedGetPoint(NULL, _T("\n选取对角点"), point1);
	if (rt != RTNORM)
		return FALSE;
	acedInitGet(RSG_NONULL, NULL);
	rt = acedGetCorner(point1, _T("\n选取对角点"), point2);
	if (rt != RTNORM)
		return FALSE;

	m_LBPn.x = point2[0]>point1[0]?point1[0]:point2[0];
	m_RTPn.x = point2[0]+point1[0]-m_LBPn.x;
	m_LBPn.y = point2[1]>point1[1]?point1[1]:point2[1];
	m_RTPn.y = point2[1]+point1[1]-m_LBPn.y;

	m_Scale = 1;
	m_strPath = _T("D:\\test.pdf");

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbDatabase* pDataBase = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable* pLayerTable;
	pDataBase->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (!pLayerTable->has(_T("*WXC2PS")))
	{
		AcDbLayerTableRecord* pLayerTableRecord = new AcDbLayerTableRecord;
		pLayerTableRecord->setName(_T("*WXC2PS"));
		pLayerTableRecord->setIsLocked(TRUE);
		pLayerTableRecord->setIsPlottable(FALSE);
		pLayerTable->add(pLayerTableRecord);
		pLayerTableRecord->close();
	}
	if (!pLayerTable->has(_T("*WXC2PSEdit")))
	{
		AcDbLayerTableRecord* pLayerTableRecord = new AcDbLayerTableRecord;
		pLayerTableRecord->setName(_T("*WXC2PSEdit"));
		pLayerTableRecord->setIsLocked(TRUE);
		pLayerTableRecord->setIsPlottable(FALSE);
		pLayerTable->add(pLayerTableRecord);
		pLayerTableRecord->close();
	}
	pLayerTable->close();
	acDocManager->unlockDocument(acDocManager->curDocument());
	m_ObjIdArray.removeAll();
	m_ObjIdArray.append(AcDbObjectId::kNull);
	m_ColorArray.removeAll();
	m_ColorArray.append(RGB(255, 255, 255));
	return TRUE;
}

void CCtoPsPrj::PrintPDF()
{
	acDocManager->lockDocument(acDocManager->curDocument());
	resbuf rb;
	ads_getvar(_T("BACKGROUNDPLOT"), &rb);
	short BackGroundPlotValue = rb.resval.rint;
	rb.resval.rint = 0;
	ads_setvar(_T("BACKGROUNDPLOT"), &rb);
	//
	AcDbLayoutManager *pLM = acdbHostApplicationServices()->layoutManager();
	AcDbPlotSettingsValidator *pPSV = acdbHostApplicationServices()->plotSettingsValidator();
	AcDbLayout *pLayout = pLM->findLayoutNamed(pLM->findActiveLayout(TRUE), TRUE);
	pPSV->refreshLists(pLayout);
	pPSV->setPlotCfgName(pLayout, _T("DWG To PDF.pc3"));
	pPSV->setCurrentStyleSheet(pLayout, _T("acad.ctb"));
	AcArray<const ACHAR *> mediaList;
	CStringArray strMediaList;
	int index;
	
	pPSV->canonicalMediaNameList(pLayout, mediaList);
	for (int m=0;m<mediaList.length();m++)
	{
		CString MediaName = mediaList[m];
		strMediaList.Add(MediaName);
	}
	for (int m=0;m<strMediaList.GetSize();m++)
	{
		const ACHAR *pName;
		pPSV->getLocaleMediaName(pLayout, m, pName);
		if (_tcscmp(pName, _T("3000x1000"))==0)
		{
			index = m;
			break;
		}
	}
	pPSV->setCanonicalMediaName(pLayout, strMediaList[index]);
	pPSV->setPlotPaperUnits(pLayout, AcDbPlotSettings::kMillimeters);	//Canonical之后
	pPSV->setPlotWindowArea(pLayout, m_LBPn.x, m_LBPn.y, m_RTPn.x, m_RTPn.y);
	pPSV->setPlotType(pLayout, AcDbPlotSettings::kWindow);	//PlotWindowArea之后
	pPSV->setPlotCentered(pLayout, FALSE);
	pPSV->setPlotOrigin(pLayout, 0, 0);
	pPSV->setUseStandardScale(pLayout, FALSE);
	pPSV->setCustomPrintScale(pLayout, 1, m_Scale);
	pPSV->setPlotRotation(pLayout, AcDbPlotSettings::k0degrees);

	AcPlPlotInfo PlotInfo;
	PlotInfo.setLayout(pLayout->objectId());
	PlotInfo.setOverrideSettings(pLayout);
	pLayout->close();
	AcPlPlotInfoValidator plotInfoValidator;
	plotInfoValidator.validate(PlotInfo);
	//
	AcPlPlotPageInfo PageInfo;
	AcPlPlotEngine* pEngine;
	AcPlPlotFactory::createPublishEngine(pEngine);
	pEngine->beginPlot(NULL);
	pEngine->beginDocument(PlotInfo, _T("AutoCADtoPhotoShop"), NULL, 1, TRUE, m_strPath);
	pEngine->beginPage(PageInfo, PlotInfo, TRUE);
	pEngine->beginGenerateGraphics();
	pEngine->endGenerateGraphics();
	pEngine->endPage();
	pEngine->endDocument();
	pEngine->endPlot();
	pEngine->destroy();
	//
	rb.resval.rint = BackGroundPlotValue;
	ads_setvar(_T("BACKGROUNDPLOT"), &rb);
	acDocManager->unlockDocument(acDocManager->curDocument());
}

void CCtoPsPrj::CropPDF()
{
	const double dpmm = 72/25.4;
	int nWidth = (int)((m_RTPn.x-m_LBPn.x)/m_Scale*dpmm);
	int nHeight = (int)((m_RTPn.y-m_LBPn.y)/m_Scale*dpmm);
	CCAcroRect CropRect;
	CropRect.CreateDispatch(_T("AcroExch.Rect"));
	CropRect.put_Left(0);
	CropRect.put_bottom(0);
	CropRect.put_right(nWidth);
	CropRect.put_Top(nHeight);

	CCAcroPDDoc PDFDoc;
	PDFDoc.CreateDispatch(_T("AcroExch.PDDoc"));
	PDFDoc.Open(m_strPath);
	PDFDoc.CropPages(0, 0, 0, CropRect);
	m_strPath.Left(m_strPath.GetLength()-4);
	m_strPath.Append(_T("_0.pdf"));
	PDFDoc.Save(1, m_strPath);
}

void CCtoPsPrj::CreatePSD()
{
	COleVariant covOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);

	m_PSApp.CreateDispatch(_T("Photoshop.Application"));
	m_PSApp.put_Visible(TRUE);
	m_PSApp.put_DisplayDialogs(3);

	CPDFOpenOptions PDFOop;
	PDFOop.CreateDispatch(_T("Photoshop.PDFOpenOptions"));
	PDFOop.put_CropPage(5);
	PDFOop.put_Resolution(200);

	VARIANT VarDisp;
	VariantInit(&VarDisp);
	VarDisp.vt = VT_DISPATCH;
	VarDisp.pdispVal = PDFOop;

	m_PSDoc = m_PSApp.Open(m_strPath, VarDisp, covOpt);

	CArtLayer ArtLayer;
	ArtLayer = m_PSDoc.get_ActiveLayer();
	ArtLayer.put_Name(_T("图元层"));
}

void CCtoPsPrj::CreateItem(CLayerListBox::LayerData* pData, int nIndex)
{
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.Add();
	ArtLayer.Move(ArtLayers.get_Item(COleVariant((LONG)nIndex+1)), 4);
	ArtLayer.put_Name(pData->LayerText);
	ArtLayer.put_Visible(pData->LayerVisible);
	ArtLayer.put_AllLocked(pData->Locked);
}

void CCtoPsPrj::CreateItemInCAD(CLayerListBox::LayerData* pData, int nIndex)
{
	//acedCommand(RTSTR, _T("UCS"), RTPOINT, point1, RTSTR, PAUSE, RTSTR, PAUSE, RTSTR, _T("PLAN"), RTSTR, _T(""), RTNONE);
	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbDatabase* pDataBase = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBlockTable;
	AcDbObjectId ObjIdRecord;

	pDataBase->getBlockTable(pBlockTable, AcDb::kForWrite);
	AcDbBlockTableRecord* pNewBlock = new AcDbBlockTableRecord, *pCurSpace;
	pBlockTable->add(pNewBlock);
	ObjIdRecord = pNewBlock->objectId();
	AcDbPoint* pNewPoint = new AcDbPoint(AcGePoint3d(0, 0, 0));
	pNewBlock->appendAcDbEntity(pNewPoint);
	pNewPoint->setVisibility(AcDb::kInvisible);
	pNewPoint->close();
	pNewBlock->close();
	pBlockTable->close();

	AcDbLayerTable* pLayerTable;
	AcDbObjectId LayerObjId;
	pDataBase->getLayerTable(pLayerTable, AcDb::kForRead);
	pLayerTable->getAt(_T("WX_CtoPS"), LayerObjId);
	pLayerTable->close();

	AcDbObjectId ObjId = pDataBase->currentSpaceId();
	acdbOpenAcDbObject((AcDbObject*&)pCurSpace, ObjId, AcDb::kForWrite);
	AcDbBlockReference *pBlockReference = new AcDbBlockReference();
	pBlockReference->setBlockTableRecord(ObjIdRecord);
	pBlockReference->setPosition(AcGePoint3d(0, 0, 0));
	pBlockReference->setLayer(LayerObjId);
	pCurSpace->appendAcDbEntity(ObjId ,pBlockReference);
	pBlockReference->close();
	pCurSpace->close();
	acDocManager->unlockDocument(acDocManager->curDocument());
	m_ObjIdArray.insertAt(nIndex, ObjIdRecord);
	m_ColorArray.insertAt(nIndex, pData->LayerColor);
}

void CCtoPsPrj::ChangeLock(BOOL bLock, int nIndex)
{
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)nIndex+1));
	ArtLayer.put_AllLocked(bLock);
}

void CCtoPsPrj::ChangeName(LPTSTR pChar, int nIndex)
{
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)nIndex+1));
	ArtLayer.put_Name(pChar);
}

void CCtoPsPrj::ChangeVisible(BOOL nVisible, int nIndex)
{
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)nIndex+1));
	ArtLayer.put_Visible(nVisible);
}

void CCtoPsPrj::ChangeColor(COLORREF color, int nIndex)
{
	m_ColorArray[nIndex] = color;
	EndModifylayer();
}

void CCtoPsPrj::SetCurItem(int nIndex)
{
	m_CurSel = nIndex;
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)nIndex+1));
	m_PSDoc.put_ActiveLayer(ArtLayer);
}

void CCtoPsPrj::SwapItem(int nFrom, int nTo)
{
	int l, m;
	l=nFrom<nTo?nFrom:nTo;
	m=nFrom<nTo?nTo:nFrom;
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)m+1));
	ArtLayer.Move(ArtLayers.get_Item(COleVariant((LONG)l+1)), 4);
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)l+1));
	ArtLayer.Move(ArtLayers.get_Item(COleVariant((LONG)m+1)), 4);
}

void CCtoPsPrj::SwapItemInCAD(int nFrom, int nTo)
{
	AcDbObjectId ObjId = m_ObjIdArray[nFrom];
	m_ObjIdArray[nFrom] = m_ObjIdArray[nTo];
	m_ObjIdArray[nTo] = ObjId;
	COLORREF color = m_ColorArray[nFrom];
	m_ColorArray[nFrom] = m_ColorArray[nTo];
	m_ColorArray[nTo] = color;
}

void CCtoPsPrj::DelItem(int nIndex)
{
	CArtLayers ArtLayers;
	ArtLayers = m_PSDoc.get_ArtLayers();
	CArtLayer ArtLayer;
	ArtLayer = ArtLayers.get_Item(COleVariant((LONG)nIndex+1));
	ArtLayer.Delete();
}

BOOL CCtoPsPrj::ModifylayerInCAD()
{
	AcDbObjectId ObjId = m_ObjIdArray[m_CurSel];
	if (ObjId == AcDbObjectId::kNull)
	{
		AfxMessageBox(_T("不可修改"));
		return FALSE;
	}
	acDocManager->lockDocument(acDocManager->curDocument());

	AcDbDatabase* pDataBase = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTableRecord* pBTRecord;
	acdbOpenAcDbObject((AcDbObject*&)pBTRecord, ObjId, AcDb::kForWrite);
	AcDbBlockTableRecordIterator* pBTRIterator;
	pBTRecord->newIterator(pBTRIterator);
	AcArray<AcDbObjectId> ObjIdArray;
	for (;!pBTRIterator->done();pBTRIterator->step())
	{
		AcDbObjectId EntityId;
		pBTRIterator->getEntityId(EntityId);
		ObjIdArray.append(EntityId);
	}
	delete pBTRIterator;
	pBTRecord->close();

	AcDbBlockTableRecord* pCurSpace;
	AcDbObjectId CurSpaceId = pDataBase->currentSpaceId();
	acdbOpenAcDbObject((AcDbObject*&)pCurSpace, CurSpaceId, AcDb::kForWrite);
	pCurSpace->assumeOwnershipOf(ObjIdArray);
	pCurSpace->close();

	m_LockedLayerName.RemoveAll();
	AcDbLayerTable* pLayerTable;

	pDataBase->getLayerTable(pLayerTable, AcDb::kForRead);

	AcDbLayerTableIterator* pIterator;
	pLayerTable->newIterator(pIterator);
	AcDbLayerTableRecord* pRecord;
	for (;!pIterator->done();pIterator->step())
	{
		pIterator->getRecord(pRecord, AcDb::kForWrite);
		if (!pRecord->isLocked())
		{
			const ACHAR* pChar;
			pRecord->getName(pChar);
			pRecord->setIsLocked(TRUE);
			CString Name = pChar;
			m_LockedLayerName.Add(pChar);
		}
		pRecord->close();
	}
	Acad::ErrorStatus es = pLayerTable->getAt(_T("WX_CtoPSEdit"), pRecord, AcDb::kForWrite);
	pRecord->setIsLocked(FALSE);
	m_CurLayerId = pDataBase->clayer();
	ObjId = pRecord->objectId();
	pRecord->close();
	pDataBase->setClayer(ObjId);
	delete pIterator;
	pLayerTable->close();
	acDocManager->unlockDocument(acDocManager->curDocument());

	CString cmd = _T("REGEN\n");
	acDocManager->sendStringToExecute(acDocManager->curDocument(), cmd);
	return TRUE;
}

BOOL CCtoPsPrj::EndModifylayerInCAD()
{
	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbDatabase* pDataBase = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable* pLayerTable;
	pDataBase->getLayerTable(pLayerTable, AcDb::kForRead);
	AcDbLayerTableRecord* pRecord;
	for (int m=0;m<m_LockedLayerName.GetCount();m++)
	{
		pLayerTable->getAt(m_LockedLayerName[m], pRecord, AcDb::kForWrite);
		pRecord->setIsLocked(FALSE);
		pRecord->close();
	}
	m_LockedLayerName.RemoveAll();
	pLayerTable->getAt(_T("WX_CtoPSEdit"), pRecord, AcDb::kForWrite);
	pRecord->setIsLocked(TRUE);
	pDataBase->setClayer(m_CurLayerId);
	pRecord->close();
	pLayerTable->close();

	
	AcDbObjectId ObjId = m_ObjIdArray[m_CurSel];
	AcDbBlockTableRecord* pBTRecord;
	acdbOpenAcDbObject((AcDbObject*&)pBTRecord, ObjId, AcDb::kForWrite);
	resbuf* pBuf = acutBuildList(8, _T("WX_CtoPSEdit"), RTNONE);
	ads_name ss;
	acedSSGet(_T("X"), NULL, NULL, pBuf, ss);
	acutRelRb(pBuf);
	long len;
	acedSSLength(ss, &len);
	AcArray<AcDbObjectId> ObjIdArray;
	for (int m=0;m<len;m++)
	{
		ads_name name;
		acedSSName(ss, m, name);
		acdbGetObjectId(ObjId, name);
		ObjIdArray.append(ObjId);
	}
	acedSSFree(ss);
	pBTRecord->assumeOwnershipOf(ObjIdArray);
	pBTRecord->close();
	acDocManager->unlockDocument(acDocManager->curDocument());
	CString cmd = _T("REGEN\n");
	acDocManager->sendStringToExecute(acDocManager->curDocument(), cmd);
	return TRUE;
}

VOID CCtoPsPrj::EndModifylayer()
{
	CSelection Sel;
	Sel = m_PSDoc.get_Selection();
	Sel.SelectAll();
	Sel.Clear();
	Sel.Deselect();

	AcDbObjectId ObjId = m_ObjIdArray[m_CurSel];
	AcDbBlockTableRecord* pBTRecord;
	acdbOpenAcDbObject((AcDbObject*&)pBTRecord, ObjId, AcDb::kForRead);
	AcDbBlockTableRecordIterator* pIterator;
	pBTRecord->newIterator(pIterator);
	for (;!pIterator->done();pIterator->step())
	{
		AcDbEntity* pEntity;
		pIterator->getEntity(pEntity, AcDb::kForRead);
		DrawEntityInPS(pEntity);
		pEntity->close();
	}
	delete pIterator;
	pBTRecord->close();
}

void CCtoPsPrj::DrawEntityInPS(AcDbEntity* pEntity)
{
	COleVariant covOpt(DISP_E_PARAMNOTFOUND, VT_ERROR);
	AcDbPolyline* pPolyLine;
	pPolyLine = AcDbPolyline::cast(pEntity);
	if (pPolyLine)
	{
		SAFEARRAYBOUND bound, boundPn;
		bound.lLbound = 0;
		bound.cElements = pPolyLine->numVerts();
		boundPn.lLbound = 0;
		boundPn.cElements = 2;

		SAFEARRAY* pPointArray = SafeArrayCreate(VT_VARIANT, 1, &bound);
		SafeArrayLock(pPointArray);
		VARIANT* pPointData = (VARIANT*)pPointArray->pvData;

		for (uint m=0;m<bound.cElements;m++)
		{
			AcGePoint2d point2D;
			pPolyLine->getPointAt(m, point2D);

			SAFEARRAY* pXYArray = SafeArrayCreate(VT_VARIANT, 1, &boundPn);
			SafeArrayLock(pXYArray);
			VARIANT* pXYData = (VARIANT*)pXYArray->pvData;
			pXYData[0] = COleVariant((double)(point2D.x-m_LBPn.x)/m_Scale/25.4*200);
			pXYData[1] = COleVariant((double)(m_RTPn.y-point2D.y)/m_Scale/25.4*200);
			SafeArrayUnlock(pXYArray);

			::VariantClear(&pPointData[m]);
			pPointData[m].vt = VT_ARRAY | VT_VARIANT;
			pPointData[m].parray = pXYArray;
		}
		SafeArrayUnlock(pPointArray);

		VARIANT varPointArray;
		VariantInit(&varPointArray);
		varPointArray.vt = VT_ARRAY | VT_VARIANT;
		varPointArray.parray = pPointArray;

		CSelection Sel;
		Sel = m_PSDoc.get_Selection();
		Sel.Select(varPointArray, COleVariant((LONG)1), covOpt, covOpt);
		SafeArrayDestroy(pPointArray);

		CRGBColor RGBcolor;
		RGBcolor.CreateDispatch(_T("Photoshop.RGBColor"));
		CSolidColor SColor;
		SColor.CreateDispatch(_T("Photoshop.SolidColor"));
		RGBcolor.put_Red(GetRValue(m_ColorArray[m_CurSel]));
		RGBcolor.put_Green(GetGValue(m_ColorArray[m_CurSel]));
		RGBcolor.put_Blue(GetBValue(m_ColorArray[m_CurSel]));
		SColor.put_RGB(RGBcolor);
		VARIANT varColor;
		::VariantInit(&varColor);
		varColor.vt = VT_DISPATCH;
		varColor.pdispVal = SColor;
		Sel.Fill(varColor, covOpt, covOpt, covOpt);
		Sel.Deselect();
	}
}

void CCtoPsPrj::RegisterCommand()
{
	acedRegCmds->addCommand(_T("WX_AutoCADtoPhotoShop"), _T("WX_PICKFIRST"), _T("WX_PICKFIRST"), ACRX_CMD_MODAL|ACRX_CMD_USEPICKSET|ACRX_CMD_REDRAW, PickFirst);
}
void CCtoPsPrj::PickFirst()
{
	//m_LockedLayerName.RemoveAll();
	AcDbDatabase* pDataBase = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable* pLayerTable;
	pDataBase->getLayerTable(pLayerTable, AcDb::kForRead);

	AcDbLayerTableIterator* pIterator;
	pLayerTable->newIterator(pIterator);
	AcDbLayerTableRecord* pRecord;
	for (;!pIterator->done();pIterator->step())
	{
		pIterator->getRecord(pRecord, AcDb::kForWrite);
		if (!pRecord->isLocked())
		{
			const ACHAR* pChar;
			pRecord->getName(pChar);
			pRecord->setIsLocked(TRUE);
			CString Name = pChar;
			//m_LockedLayerName.Add(pChar);
		}
		pRecord->close();
	}
	Acad::ErrorStatus es = pLayerTable->getAt(_T("WX_CtoPSEdit"), pRecord, AcDb::kForWrite);
	pRecord->setIsLocked(FALSE);
	//m_CurLayerId = pDataBase->clayer();
	AcDbObjectId ObjId = pRecord->objectId();
	pRecord->close();
	pDataBase->setClayer(ObjId);
	delete pIterator;
	pLayerTable->close();
}
