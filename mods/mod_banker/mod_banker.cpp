/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/


// mod_banker.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_banker.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "TibiaMapProxy.h"
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_bankerApp

BEGIN_MESSAGE_MAP(CMod_bankerApp, CWinApp)
//{{AFX_MSG_MAP(CMod_bankerApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

int findBanker(CConfigData *);
int moveToBanker(CConfigData *);
int depositGold();
int isDepositing();
int isCavebotOn();
int countAllItemsOfType(int);
int shouldBank(CConfigData *);

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	CMemReaderProxy reader;
//	CPackSenderProxy sender;
//	CTibiaItemProxy itemProxy;
//	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	reader.setGlobalVariable("cavebot_depositing", "false");
	while (!toolThreadShouldStop) {
		Sleep(200);
		int attackedCreature = reader.getAttackedCreature();
		//if (isDepositing() || !isCavebotOn() || shouldBank(config) && !attackedCreature) {
		if (shouldBank(config) && !attackedCreature) {
			if (findBanker(config)) {
				reader.setGlobalVariable("caveboot_halfsleep","true");
				if (moveToBanker(config)) {
					//AfxMessageBox("Yup, found the banker!");
					if (depositGold())
						reader.setGlobalVariable("caveboot_halfsleep","false");
				}
			}
		}
		else 
			reader.setGlobalVariable("caveboot_halfsleep","false");
	}
	reader.setGlobalVariable("caveboot_halfsleep","false");
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_bankerApp construction

CMod_bankerApp::CMod_bankerApp() {
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_bankerApp::~CMod_bankerApp() {
	if (m_configDialog)
		delete m_configDialog;
	delete m_configData;	
}

char * CMod_bankerApp::getName() {
	return "Auto banker";
}

int CMod_bankerApp::isStarted() {
	return m_started;
}

void CMod_bankerApp::start() {	
	superStart();
	if (m_configDialog) 	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}
	
	DWORD threadId;
	
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_bankerApp::stop() {
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_bankerApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	
	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_bankerApp::configToControls() {
	if (m_configDialog)		
		m_configDialog->configToControls(m_configData);
}

void CMod_bankerApp::controlsToConfig() {
	if (m_configDialog)	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_bankerApp::disableControls() {
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_bankerApp::enableControls() {
	if (m_configDialog)
		m_configDialog->enableControls();
}


char *CMod_bankerApp::getVersion() {
	return "1.0";
}

int CMod_bankerApp::validateConfig(int showAlerts) {	
	return 1;
}

void CMod_bankerApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_bankerApp::loadConfigParam(char *paramName,char *paramValue) {
}

char *CMod_bankerApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]=0;
	
	return buf;
}

char *CMod_bankerApp::getConfigParamName(int nr) {
	switch (nr) {
	default:
		return NULL;
	}
}

int findBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (config->targetX == self->x && config->targetY == self->y && config->targetZ == self->z) return 1; 
	for (int x = 0; x < 10; x++) {
		struct point nearestBank = CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->bankerX[x], config->bankerY[x], config->bankerZ[x], 0, config->path);
		if (nearestBank.x && nearestBank.y && nearestBank.z) {
			config->targetX = nearestBank.x;
			config->targetY = nearestBank.y;
			config->targetZ = nearestBank.z;
			delete self;
			return 1;
		}
		else if (x == 9) {
			config->targetX = config->targetY = config->targetZ = 0;
			delete self;
			return 0;
		}
	}
	return -1;
}

int moveToBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();

	CModuleUtil::executeWalk(self->x,self->y,self->z,config->path);
	self = reader.readSelfCharacter();
	if (self->x == config->targetX && self->y == config->targetY && self->z == config->targetZ) {
		delete self;
		return 1;
	}
	else {
		delete self;
		return 0;
	}
}

int depositGold() {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CTibiaContainer *cont;
	int objectId = itemProxy.getValueForConst("GP");
	int foundInBag = 0;
	for (int contNr = 0; contNr < 16; contNr++) {
		cont = reader.readContainer(contNr);
		int count = cont->itemsInside;
		for (int slotNr = count - 1; slotNr >= 0; slotNr--) {
			CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
			if (item->objectId == objectId) {
				foundInBag = contNr;
				contNr = 16;
				slotNr = 0;
			}
		}
	}

	sender.say("Hi");
	Sleep (500);
	sender.sayNPC("deposit all");
	Sleep (500);
	sender.sayNPC("yes");
	if (CModuleUtil::waitForItemsInsideChange(foundInBag, cont->itemsInside)) {
		sender.sayNPC("balance");
		delete cont;
		return 1;
	}
	delete cont;	
	return 0;
}

int isCavebotOn() {
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);
		
		Module32First(hSnap,&lpModule);
		do {
			if (strcmpi(lpModule.szModule,"mod_cavebot.dll") == 0){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted) {
					if (isStarted())
						return 1;
					else
						return 0;
				}
			}
		}while (Module32Next(hSnap,&lpModule));		
		CloseHandle(hSnap);
	}
	return -1;
}

int isDepositing() {
	CMemReaderProxy reader;
	char *var=reader.getGlobalVariable("cavebot_depositing");
	if (var==NULL||strcmp(var,"true")) {
		delete var;
		return 0;
	}
	else {
		delete var;
		return 1;
	}
}

int countAllItemsOfType(int objectId) {
	CMemReaderProxy reader;
	int contNr;
	int ret=0;
	for (contNr = 0; contNr < 8; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		delete cont;
	}
	return ret;
}

int shouldBank(CConfigData *config) {
	CTibiaItemProxy itemProxy;
	int objectId = itemProxy.getValueForConst("GP");
	if (countAllItemsOfType(objectId) >= config->minimumGoldToBank)
		return 1;
	else 
		return 0;
}

