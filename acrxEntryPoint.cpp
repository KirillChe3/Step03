//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "Utilities.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep03App : public AcRxArxApp {

public:
	CStep03App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents() {
	}


	static void ADSKStep03CREATE() {
		try {
			AcDbObjectId layerId;
			if (createLayer(L"USER", layerId) != Acad::eOk) {
				acutPrintf(L"\nCannot create new layer record");
				return;
			}

			applyCurDwgLayerTableChanges();
			acutPrintf(L"\nNew Layer created successfully");

			if (createBlockRecord(L"EMPLOYEE") != Acad::eOk) {
				acutPrintf(L"\nCannot create block record");
			}
			else
			{
				acutPrintf(L"\nBlock record EMPLOYEE created successfully");
			}
		}
		catch (...)
		{
		}
	}

	static void ADSKStep03SETLAYER() {
		try {
			Acad::ErrorStatus errorStatus;
			AcDbBlockTable* pBlockTable;
			AcDbBlockTableRecord* pModelSpace;

			if ((errorStatus = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead)) != Acad::eOk) {
				acutPrintf(L"\nCannot open block table");
				return;
			}

			if ((errorStatus = pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite)) != Acad::eOk) {
				acutPrintf(L"\nCannot get Model Space. Check the drawing");
				return;
			}

			pBlockTable->close();

			AcDbBlockTableRecordIterator* pBTRIter;
			if ((errorStatus = pModelSpace->newIterator(pBTRIter)) != Acad::eOk) {
				acutPrintf(L"\nCannot create Model Space iterator");
				pModelSpace->close();
				return;
			}

			ACHAR* blockName;
			AcDbEntity* pEntity;
			AcDbBlockTableRecord* pCurrEntityBlock;
			AcDbObjectId blockId;

			for (pBTRIter->start(); !pBTRIter->done(); pBTRIter->step()) {
				if ((errorStatus = pBTRIter->getEntity(pEntity, AcDb::kForRead)) != Acad::eOk) {
					acutPrintf(L"Cannot open Entity");
					continue;
				}

				if (pEntity->isA() != AcDbBlockReference::desc()) {
					pEntity->close();
					continue;
				}

				blockId = (AcDbBlockReference::cast(pEntity))->blockTableRecord();

				if (acdbOpenObject((AcDbObject*&)pCurrEntityBlock, blockId, AcDb::kForRead) == Acad::eOk) {
					pCurrEntityBlock->getName(blockName);
					if (wcscmp(blockName, L"EMPLOYEE")==0) {
						if (pEntity->upgradeOpen() == Acad::eOk) {
							pEntity->setLayer(L"USER");
						}
					}
					pCurrEntityBlock->close();
					acdbFree(blockName);
				}
				pEntity->close();
			}
			pModelSpace->close();
		}
		catch (...) {
		}
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep03App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep03App, ADSKStep03, CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep03App, ADSKStep03, SETLAYER, SETLAYER, ACRX_CMD_TRANSPARENT, NULL)

