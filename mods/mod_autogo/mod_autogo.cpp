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


// mod_autogo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_autogo.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "TibiaMapProxy.h"
#include "ModuleUtil.h"
#include <MMSystem.h>
#include <Tlhelp32.h>

#include "IPCBackPipeProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EWX_FORCEIFHUNG      0x00000010


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp

BEGIN_MESSAGE_MAP(CMod_autogoApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_autogoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;
char suspendedModules[20][64];
int suspendedCount = 0;


int actionPos2ID(int pos){
	switch (pos){
		case ACTION_NONE_POS:		return ACTION_NONE;
		case ACTION_SUSPEND_POS:	return ACTION_SUSPEND;
		case ACTION_LOGOUT_POS:		return ACTION_LOGOUT;
		case ACTION_KILL_POS:		return ACTION_KILL;
		case ACTION_SHUTDOWN_POS:	return ACTION_SHUTDOWN;
		case ACTION_RUNAWAY_POS:	return ACTION_RUNAWAY;
		default:					return 0;
	}

}

void actionTerminate(){
	CMemReaderProxy reader;
	HANDLE hTibiaProc;

	hTibiaProc = OpenProcess(PROCESS_TERMINATE,true,reader.getProcessId());

	TerminateProcess(hTibiaProc,0);

	CloseHandle(hTibiaProc);
}

int actionShutdownSystem(){
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	//T4: Privilages are needed for XP & 2000
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)){
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		
        tkp.PrivilegeCount = 1; 
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)){
			ExitWindowsEx(EWX_SHUTDOWN|EWX_POWEROFF|EWX_FORCEIFHUNG,0);
			return 1;
		}
	}
	return 0;
}

int actionSuspend(int lSuspend){
	if (lSuspend){
		HANDLE hSnap;
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		if (hSnap){
			MODULEENTRY32 lpModule;
			lpModule.dwSize = sizeof(MODULEENTRY32);

			Module32First(hSnap,&lpModule);
			do {
				if (strcmpi(lpModule.szModule,"mod_autogo.dll")!=0){
					FARPROC isStarted;
					isStarted = GetProcAddress(lpModule.hModule,"isStarted");
					if (isStarted){
						if (isStarted()){
							/*FARPROC stop;
							stop = GetProcAddress(lpModule.hModule,"stop");
							stop();*/
							GetProcAddress(lpModule.hModule,"stop")();
							lstrcpyn(suspendedModules[suspendedCount++],lpModule.szModule,63);
						}
					}
				}
			}while (Module32Next(hSnap,&lpModule));

			CloseHandle(hSnap);
		}
	}else{
		int i;
		for (i=0;i<suspendedCount;i++){
			HMODULE hModule;
			hModule = GetModuleHandle(suspendedModules[i]);
			if (hModule){
				FARPROC start;
				start = GetProcAddress(hModule,"start");
				start();
			}
		}
		suspendedCount=0;
	}
	return 0;
}

int OnList(char whiteList[][32],char name[]){
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])){
		if (!strcmpi(whiteList[i],name)){
			return 1;
		}
		i++;
	}

	return 0;
}

int triggerBattleList(int options, char whiteList[][32]){
	CPackSenderProxy sender;
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int creatureNr;	


	// special handling of battle list auto log/go detection
	if ((options&BATTLELIST_BATTLELIST))
	{
		if(reader.readBattleListMax()>=0||reader.readBattleListMin()>=0) {			
			delete self;
			return 1;
		}
	}

	for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++){
		CTibiaCharacter *ch=reader.readVisibleCreature(creatureNr);
		
		if (ch->visible){		
			if (ch->tibiaId!=self->tibiaId &&  !OnList(whiteList,(char*)ch->name))
			{				
				if (ch->z==self->z||(options&BATTLELIST_PARANOIAM))
				{					
					if ((options&BATTLELIST_GM)&&ch->name[0]=='G'&&ch->name[1]=='M')
					{
						// this is GM						
						delete ch;
						delete self;						
						return 1;
					}
					if ((options&BATTLELIST_GM)&&ch->name[0]=='C'&&ch->name[1]=='M')
					{
						// this is CM						
						delete ch;
						delete self;						
						return 1;
					}
					
					if ((options&BATTLELIST_PLAYER))
					{
						// this is other player
						if (ch->tibiaId < 0x40000000)
						{
							delete ch;
							delete self;							
							return 1;
						}
					}
					
					if ((options&BATTLELIST_MONSTER))
					{
						// this is monster/npc
						if (ch->tibiaId >= 0x40000000)
						{
							delete ch;
							delete self;							
							return 1;
						}
					}
				}
			}
			
		}
		delete ch;
	};
	delete self;

	return 0;
}

int triggerMessage(){
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1003)){
		int infoType;
		int nickLen;
		char nickBuf[32];

		memset(nickBuf,0,32);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		
		CTibiaCharacter *temp = reader.readSelfCharacter();
		//T4: Name in temp structure is empty, bug?
		CTibiaCharacter *self = reader.getCharacterByTibiaId(temp->tibiaId);
		delete temp;

		if (strcmpi(nickBuf,self->name)!=0 && strcmpi(nickBuf,"Tibia Auto")!=0){
			delete self;
			return infoType;
		}
		delete self;
	}
	return -1;
}

int triggerBlank(){
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	int ret=0;
	int pos;
	for (pos=0;pos<10;pos++){
		int itemNr;
		CTibiaContainer *container = reader.readContainer(pos);
		//T4: Update missing OnOff flag checking
		for (itemNr=0;itemNr<container->itemsInside && container->flagOnOff;itemNr++){
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
			if (item->objectId==itemProxy.getValueForConst("runeBlank")){
				// blank rune found!
				ret++;
			};
		};								
		delete container;
	};
	return ret;
}

int triggerNoSpace(){
	CMemReaderProxy reader;
	int ret=1;
	int pos;
	for (pos=0;pos<10;pos++){
		CTibiaContainer *container = reader.readContainer(pos);
		if (container->flagOnOff && container->itemsInside < container->size){
			ret = 0;
			break;
		}
		delete container;
	};
	return ret;
}

int triggerOutOf(int options){
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CTibiaItem *Item = NULL;
	CUIntArray itemArray;
			
	itemArray.Add(itemProxy.getValueForConst("worms"));
	int pos;
	int ret=0;	//Bitfield
	
	for (pos=0;pos<10&&!ret;pos++){
		//T4: Missing check if container is opened
		CTibiaContainer *container = reader.readContainer(pos);
		if (container->flagOnOff){
			if (options&OUTOF_FOOD && !(ret&OUTOF_FOOD)){
				Item = CModuleUtil::lookupItem(pos,itemProxy.getItemsFoodArray());
				if (Item != NULL){
					ret = ret|OUTOF_FOOD;
					delete Item;
				}
			}
		
			if (options&OUTOF_SPACE && !(ret&OUTOF_SPACE)){
				if (container->itemsInside < container->size)
					ret = ret|OUTOF_SPACE;
			}
		}
		delete container;
	}
	//T4: As in code ret saves is certain thing in backpack, at the end I revers bits
	return (ret^options);
}

void alarmSound(int alarmId){
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
		

	OFSTRUCT lpOpen;
	char wavFile[1024];

	switch (alarmId){
		case TRIGGER_BATTLELIST:sprintf(wavFile,"%s\\mods\\sound\\battlelist.wav",installPath);break;
		case TRIGGER_SIGN:		sprintf(wavFile,"%s\\mods\\sound\\sign.wav",installPath);break;
		case TRIGGER_MESSAGE:	sprintf(wavFile,"%s\\mods\\sound\\message.wav",installPath);break;
		case TRIGGER_MOVE:		sprintf(wavFile,"%s\\mods\\sound\\move.wav",installPath);break;
		case TRIGGER_HPLOSS:	sprintf(wavFile,"%s\\mods\\sound\\hploss.wav",installPath);break;
		case TRIGGER_HPBELOW:	sprintf(wavFile,"%s\\mods\\sound\\hpbelow.wav",installPath);break;
		case TRIGGER_SOULPOINT_BELOW:	sprintf(wavFile,"%s\\mods\\sound\\soulpointbelow.wav",installPath);break;
		case TRIGGER_BLANK:		sprintf(wavFile,"%s\\mods\\sound\\blank.wav",installPath);break;
		case TRIGGER_CAPACITY:	sprintf(wavFile,"%s\\mods\\sound\\capacity.wav",installPath);break;
		case TRIGGER_OUTOF:		sprintf(wavFile,"%s\\mods\\sound\\outof.wav",installPath);break;
		default:				sprintf(wavFile,"%s\\mods\\sound\\alarm.wav",installPath);break;
	}

	if (OpenFile(wavFile,&lpOpen,OF_EXIST) != HFILE_ERROR){
		PlaySound(wavFile,NULL,SND_FILENAME|SND_ASYNC);
		// isn't some CloseFile() needed? -- vanitas
	} else {
		sprintf(wavFile,"%s\\mods\\sound\\alarm.wav",installPath);
		if(OpenFile(wavFile,&lpOpen,OF_EXIST) != HFILE_ERROR){
			PlaySound(wavFile,NULL,SND_FILENAME|SND_ASYNC);
			// isn't some wait needed? cause multiple async plays will kill CPU --vanitas
		} else{
			MessageBeep(MB_OK);
		}	
	}
}
char *alarmStatus(int alarmId){	
	if (alarmId&TRIGGER_BATTLELIST) return "BattleList alarm";
	if (alarmId&TRIGGER_SIGN)		return "A sign appeared";
	if (alarmId&TRIGGER_MESSAGE)	return "New message";
	if (alarmId&TRIGGER_MOVE)		return "Your moving";
	if (alarmId&TRIGGER_HPLOSS)		return "You have lost HP";
	if (alarmId&TRIGGER_HPBELOW)	return "Your HP is below certain value";
	if (alarmId&TRIGGER_SOULPOINT_BELOW)	return "Too few soul points";
	if (alarmId&TRIGGER_BLANK)		return "Too few blank runes";
	if (alarmId&TRIGGER_CAPACITY)	return "Capacity is too small";
	if (alarmId&TRIGGER_OUTOF)		return "You run out of food/worms/space";
	return "";
}

void alarmAction(int alarmId, CConfigData *config){
	CMemReaderProxy reader;
	CPackSenderProxy sender;

	strcpy(config->status,alarmStatus(alarmId));

	//sender.sendTAMessage(statusInfo(alarmId));
	/*if (config->sound&alarmId){
		playSound(alarmId);
	}*/

	int iAction=0;
	
	if (alarmId&TRIGGER_SIGN)		{iAction|=actionPos2ID(config->actionSign);}
	if (alarmId&TRIGGER_MESSAGE)	{iAction|=actionPos2ID(config->actionMessage);}
	if (alarmId&TRIGGER_MOVE)		{iAction|=actionPos2ID(config->actionMove);}
	if (alarmId&TRIGGER_HPLOSS)		{iAction|=actionPos2ID(config->actionHpLoss);}
	if (alarmId&TRIGGER_HPBELOW)	{iAction|=actionPos2ID(config->actionHpBelow);}
	if (alarmId&TRIGGER_SOULPOINT_BELOW)	{iAction|=actionPos2ID(config->actionSoulPointBelow);}
	if (alarmId&TRIGGER_BLANK)		{iAction|=actionPos2ID(config->actionBlank);}
	if (alarmId&TRIGGER_CAPACITY)	{iAction|=actionPos2ID(config->actionCapacity);}
	

	//if (iAction&ACTION_SUSPEND){
		actionSuspend(iAction&ACTION_SUSPEND);
	//}
	if (iAction&ACTION_LOGOUT){				
		//sender.sendTAMessage("Logout");
		sender.logout();
	}
	if (iAction&ACTION_KILL){
		//sender.sendTAMessage("Kill");
		sender.logout();
		Sleep(1500);

		actionTerminate();
	}
	if (iAction&ACTION_SHUTDOWN){
		sender.logout();
		Sleep(1500);			
		if (!actionShutdownSystem()){
			actionTerminate();
		};
		Sleep(2000);
			
	}
	if (iAction&ACTION_RUNAWAY){
		CTibiaCharacter *self = reader.readSelfCharacter();
		CMemConstData memConstData = reader.getMemConstData();
		CTibiaMapProxy tibiaMap;
		
		int path[10000];
		delete self;
		self = reader.readSelfCharacter();

		if (self->x!=config->runawayX || self->y!=config->runawayY || self->z!=config->runawayZ){
		
			// proceed with path searching									
			CModuleUtil::findPathOnMap(self->x,self->y,self->z,config->runawayX,config->runawayY,config->runawayZ,0,path);
			int pathSize;
			for (pathSize=0;pathSize<10000&&path[pathSize];pathSize++){}										
			if (pathSize){
				CModuleUtil::executeWalk(self->x,self->y,self->z,path);
				CModuleUtil::sleepWithStop(500,&toolThreadShouldStop);
			}
		}
		delete self;
	}else if (config->action&ACTION_RUNAWAY){
		CTibiaCharacter *self = reader.readSelfCharacter();
		CMemConstData memConstData = reader.getMemConstData();
		CTibiaMapProxy tibiaMap;

		int path[10000];
		delete self;
		self = reader.readSelfCharacter();

		if (self->x!=config->actX || self->y!=config->actY || self->z!=config->actZ){

			CModuleUtil::sleepWithStop(2000,&toolThreadShouldStop);
			
			// proceed with path searching									
			CModuleUtil::findPathOnMap(self->x,self->y,self->z,config->actX,config->actY,config->actZ,0,path);
			int pathSize;
			for (pathSize=0;pathSize<10000&&path[pathSize];pathSize++){}										
			if (pathSize){
				CModuleUtil::executeWalk(self->x,self->y,self->z,path);											
			}
		}else{
			if (config->actDirection){
				CPackSenderProxy sender;
				if (config->actDirection==DIR_LEFT){
					sender.turnLeft();
				}else if(config->actDirection==DIR_RIGHT){
					sender.turnRight();
				}else if(config->actDirection==DIR_UP){
					sender.turnUp();
				}else if(config->actDirection==DIR_DOWN){
					sender.turnDown();
				}
			}
		}
		delete self;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;

	CTibiaCharacter *self = reader.readSelfCharacter();

	int lastX = self->x;
	int lastY = self->y;
	int lastZ = self->z;
	int lastMoved = 0;
	int lastHp = self->hp;

	int iAlarm;
	int alarm;

	delete self;	

	PlaySound(0, 0, 0);

	//while (triggerMessage()>0){};//Remove all msg in queue
	
	while (!toolThreadShouldStop)
	{			
		Sleep(100);	
		alarm = 0;
		
		if ((config->trigger&TRIGGER_BATTLELIST)){
			iAlarm = triggerBattleList(config->optionsBattleList,config->whiteList);
			if (iAlarm){
				alarm |= TRIGGER_BATTLELIST;
				if (config->sound&TRIGGER_BATTLELIST)
					alarmSound(TRIGGER_BATTLELIST);
			}
		}

		if (config->trigger&TRIGGER_SIGN){
			int flags = reader.getSelfEventFlags()&config->optionsSign;
			if (flags){
				alarm |= TRIGGER_SIGN;
				if (config->sound&TRIGGER_SIGN)
					alarmSound(TRIGGER_SIGN);
			}
			//0x00000001 - poison;
			//0x00000002 - fire;
			//0x00000004 - energy;
			//0x00000008 - drunk;
			//0x00000010 - shield;
			//0x00000020 - paralize;
			//0x00000040 - haste
			//0x00000080 - attack;
			/*char buffer[1024];
			sprintf(buffer,"0x%x",reader.getSelfEventFlags());
			sender.sendTAMessage(buffer);
			Sleep(1000);*/
		}
		if (config->trigger&TRIGGER_MESSAGE){
			int iType = triggerMessage();
			if (config->optionsMessage&MESSAGE_PUBLIC && (iType == 1 || iType == 2) /*|| iType == 3*/){
				alarm |= TRIGGER_MESSAGE;
				if (config->sound&TRIGGER_MESSAGE)
					alarmSound(TRIGGER_MESSAGE);
			}
			if (config->optionsMessage&MESSAGE_PRIVATE && iType == 4){
				alarm |= TRIGGER_MESSAGE;
				if (config->sound&TRIGGER_MESSAGE)
					alarmSound(TRIGGER_MESSAGE);
			}

		}
		if (config->trigger&TRIGGER_MOVE){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (lastX != self->x || lastY != self->y || lastZ != self->z){
				alarm |= TRIGGER_MOVE;
				if (config->sound&TRIGGER_MOVE)
					alarmSound(TRIGGER_MOVE);
				lastMoved=1;
			} else {
				if (lastMoved)
				{
					alarm |= TRIGGER_MOVE;
					if ((config->sound&TRIGGER_MOVE))
						alarmSound(TRIGGER_MOVE);
				}

			}
			lastX = self->x;
			lastY = self->y;
			lastZ = self->z;			
			delete self;
		}
		if (config->trigger&TRIGGER_HPLOSS){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (lastHp > self->hp ){
				alarm |= TRIGGER_HPLOSS;
				if (config->sound&TRIGGER_HPLOSS)
					alarmSound(TRIGGER_HPLOSS);
			}
			lastHp = self->hp;
			delete self;
		}
		if (config->trigger&TRIGGER_HPBELOW){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->hp < config->optionsHpBelow){
				alarm |= TRIGGER_HPBELOW;
				if (config->sound&TRIGGER_HPBELOW)
					alarmSound(TRIGGER_HPBELOW);
			}
			lastHp = self->hp;
			delete self;
		}
		if (config->trigger&TRIGGER_SOULPOINT_BELOW){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->soulPoints < config->optionsSoulPointBelow){
				alarm |= TRIGGER_SOULPOINT_BELOW;
				if (config->sound&TRIGGER_SOULPOINT_BELOW)
					alarmSound(TRIGGER_SOULPOINT_BELOW);
			}
			delete self;
		}
		if (config->trigger&TRIGGER_BLANK){
			if (triggerBlank() < config->optionsBlank){
				alarm |= TRIGGER_BLANK;
				if (config->sound&TRIGGER_BLANK)
					alarmSound(TRIGGER_BLANK);
			}
		}
		if (config->trigger&TRIGGER_CAPACITY){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->cap < config->optionsCapacity){
				alarm |= TRIGGER_CAPACITY;
				if (config->sound&TRIGGER_CAPACITY)
					alarmSound(TRIGGER_CAPACITY);
			}
			delete self;
		}
	

		alarmAction(alarm,config);
	}
	// clear current status
	config->status[0]='\0';

	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp construction

CMod_autogoApp::CMod_autogoApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_autogoApp::~CMod_autogoApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_autogoApp::getName()
{
	return "Auto go/logout";
}


int CMod_autogoApp::isStarted()
{
	if (!m_started)
	{
		// if not started then regularry consume 1003 messages from the queue
		CIPCBackPipeProxy backPipe;
		struct ipcMessage mess;	

		backPipe.readFromPipe(&mess,1003);
	}
	return m_started;
}


void CMod_autogoApp::start()
{	

	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_autogoApp::stop()
{
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)
	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_autogoApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_autogoApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_autogoApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_autogoApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_autogoApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_autogoApp::getVersion()
{	
	return "3.1";
}


int CMod_autogoApp::validateConfig(int showAlerts)
{	
	return 1;
}

void CMod_autogoApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_autogoApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"act/x"))						m_configData->actX					= atoi(paramValue);
	if (!strcmp(paramName,"act/y"))						m_configData->actY					= atoi(paramValue);
	if (!strcmp(paramName,"act/z"))						m_configData->actZ					= atoi(paramValue);
	if (!strcmp(paramName,"act/direction"))				m_configData->actDirection			= atoi(paramValue);
	if (!strcmp(paramName,"runaway/x"))					m_configData->runawayX				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/y"))					m_configData->runawayY				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/z"))					m_configData->runawayZ				= atoi(paramValue);
	if (!strcmp(paramName,"trigger"))					m_configData->trigger				= atoi(paramValue);
	if (!strcmp(paramName,"action/BattleListGM"))		m_configData->actionBattleListGM	= atoi(paramValue);
	if (!strcmp(paramName,"action/BattleListPlayer"))	m_configData->actionBattleListPlayer	= atoi(paramValue);
	if (!strcmp(paramName,"action/BattleListList"))		m_configData->actionBattleListList	= atoi(paramValue);
	if (!strcmp(paramName,"action/BattleListMonster"))	m_configData->actionBattleListMonster	= atoi(paramValue);
	if (!strcmp(paramName,"action/Sign"))				m_configData->actionSign			= atoi(paramValue);
	if (!strcmp(paramName,"action/Message"))			m_configData->actionMessage			= atoi(paramValue);
	if (!strcmp(paramName,"action/HpLoss"))				m_configData->actionHpLoss			= atoi(paramValue);
	if (!strcmp(paramName,"action/HpBelow"))			m_configData->actionHpBelow			= atoi(paramValue);
	if (!strcmp(paramName,"action/HpAbove"))			m_configData->actionHpAbove			= atoi(paramValue);
	if (!strcmp(paramName,"action/ManaBelow"))			m_configData->actionManaBelow		= atoi(paramValue);
	if (!strcmp(paramName,"action/ManaAbove"))			m_configData->actionManaAbove		= atoi(paramValue);
	if (!strcmp(paramName,"action/Move"))				m_configData->actionMove			= atoi(paramValue);
	if (!strcmp(paramName,"action/SoulPointBelow"))		m_configData->actionSoulPointBelow	= atoi(paramValue);
	if (!strcmp(paramName,"action/SoulPointAbove"))		m_configData->actionSoulPointAbove	= atoi(paramValue);
	if (!strcmp(paramName,"action/Blank"))				m_configData->actionBlank			= atoi(paramValue);
	if (!strcmp(paramName,"action/Capacity"))			m_configData->actionCapacity		= atoi(paramValue);
	if (!strcmp(paramName,"action/OutOfFood"))			m_configData->actionOutOfFood		= atoi(paramValue);
	if (!strcmp(paramName,"action/OutOfCustom"))		m_configData->actionOutOfCustom		= atoi(paramValue);
	if (!strcmp(paramName,"action/OutOfSpace"))			m_configData->actionOutOfSpace		= atoi(paramValue);
	if (!strcmp(paramName,"action/RunawayReached"))		m_configData->actionRunawayReached		= atoi(paramValue);
	if (!strcmp(paramName,"options/BattleList"))		m_configData->optionsBattleList		= atoi(paramValue);
	if (!strcmp(paramName,"options/Sign"))				m_configData->optionsSign			= atoi(paramValue);
	if (!strcmp(paramName,"options/Message"))			m_configData->optionsMessage		= atoi(paramValue);
	if (!strcmp(paramName,"options/HpBelow"))			m_configData->optionsHpBelow		= atoi(paramValue);
	if (!strcmp(paramName,"options/HpAbove"))			m_configData->optionsHpAbove		= atoi(paramValue);
	if (!strcmp(paramName,"options/ManaBelow"))			m_configData->optionsManaBelow		= atoi(paramValue);
	if (!strcmp(paramName,"options/ManaAbove"))			m_configData->optionsManaAbove		= atoi(paramValue);
	if (!strcmp(paramName,"options/SoulPointBelow"))	m_configData->optionsSoulPointBelow	= atoi(paramValue);
	if (!strcmp(paramName,"options/SoulPointAbove"))	m_configData->optionsSoulPointAbove	= atoi(paramValue);
	if (!strcmp(paramName,"options/Blank"))				m_configData->optionsBlank			= atoi(paramValue);
	if (!strcmp(paramName,"options/Capacity"))			m_configData->optionsCapacity		= atoi(paramValue);
	if (!strcmp(paramName,"options/OutOfCustomItem"))	m_configData->optionsOutOfCustomItem= atoi(paramValue);
	if (!strcmp(paramName,"options/RunawayReached"))	m_configData->optionsRunawayReached= atoi(paramValue);
	if (!strcmp(paramName,"sound"))						m_configData->sound					= atoi(paramValue);
	if (!strcmp(paramName,"whiteList")){
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->whiteList[currentPos++],paramValue,32);
	}
}

char *CMod_autogoApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"act/x"))						sprintf(buf,"%d",m_configData->actX);
	if (!strcmp(paramName,"act/y"))						sprintf(buf,"%d",m_configData->actY);
	if (!strcmp(paramName,"act/z"))						sprintf(buf,"%d",m_configData->actZ);
	if (!strcmp(paramName,"act/direction"))				sprintf(buf,"%d",m_configData->actDirection);
	if (!strcmp(paramName,"runaway/x"))					sprintf(buf,"%d",m_configData->runawayX);
	if (!strcmp(paramName,"runaway/y"))					sprintf(buf,"%d",m_configData->runawayY);
	if (!strcmp(paramName,"runaway/z"))					sprintf(buf,"%d",m_configData->runawayZ);
	if (!strcmp(paramName,"trigger"))					sprintf(buf,"%d",m_configData->trigger);
	if (!strcmp(paramName,"action/BattleListGM"))		sprintf(buf,"%d",m_configData->actionBattleListGM);
	if (!strcmp(paramName,"action/BattleListList"))		sprintf(buf,"%d",m_configData->actionBattleListList);
	if (!strcmp(paramName,"action/BattleListPlayer"))	sprintf(buf,"%d",m_configData->actionBattleListPlayer);
	if (!strcmp(paramName,"action/BattleListMonster"))	sprintf(buf,"%d",m_configData->actionBattleListMonster);
	if (!strcmp(paramName,"action/Sign"))				sprintf(buf,"%d",m_configData->actionSign);
	if (!strcmp(paramName,"action/Message"))			sprintf(buf,"%d",m_configData->actionMessage);
	if (!strcmp(paramName,"action/HpLoss"))				sprintf(buf,"%d",m_configData->actionHpLoss);
	if (!strcmp(paramName,"action/HpBelow"))			sprintf(buf,"%d",m_configData->actionHpBelow);
	if (!strcmp(paramName,"action/HpAbove"))			sprintf(buf,"%d",m_configData->actionHpAbove);
	if (!strcmp(paramName,"action/ManaBelow"))			sprintf(buf,"%d",m_configData->actionManaBelow);
	if (!strcmp(paramName,"action/ManaAbove"))			sprintf(buf,"%d",m_configData->actionManaAbove);
	if (!strcmp(paramName,"action/Move"))				sprintf(buf,"%d",m_configData->actionMove);
	if (!strcmp(paramName,"action/SoulPointBelow"))		sprintf(buf,"%d",m_configData->actionSoulPointBelow);
	if (!strcmp(paramName,"action/SoulPointAbove"))		sprintf(buf,"%d",m_configData->actionSoulPointAbove);
	if (!strcmp(paramName,"action/Blank"))				sprintf(buf,"%d",m_configData->actionBlank);
	if (!strcmp(paramName,"action/Capacity"))			sprintf(buf,"%d",m_configData->actionCapacity);
	if (!strcmp(paramName,"action/OutOfFood"))			sprintf(buf,"%d",m_configData->actionOutOfFood);
	if (!strcmp(paramName,"action/OutOfCustom"))		sprintf(buf,"%d",m_configData->actionOutOfCustom);
	if (!strcmp(paramName,"action/OutOfSpace"))			sprintf(buf,"%d",m_configData->actionOutOfSpace);
	if (!strcmp(paramName,"action/RunawayReached"))		sprintf(buf,"%d",m_configData->actionRunawayReached);
	if (!strcmp(paramName,"options/BattleList"))		sprintf(buf,"%d",m_configData->optionsBattleList);
	if (!strcmp(paramName,"options/Sign"))				sprintf(buf,"%d",m_configData->optionsSign);
	if (!strcmp(paramName,"options/Message"))			sprintf(buf,"%d",m_configData->optionsMessage);
	if (!strcmp(paramName,"options/HpAbove"))			sprintf(buf,"%d",m_configData->optionsHpAbove);
	if (!strcmp(paramName,"options/HpBelow"))			sprintf(buf,"%d",m_configData->optionsHpBelow);
	if (!strcmp(paramName,"options/ManaAbove"))			sprintf(buf,"%d",m_configData->optionsManaAbove);
	if (!strcmp(paramName,"options/ManaBelow"))			sprintf(buf,"%d",m_configData->optionsManaBelow);
	if (!strcmp(paramName,"options/SoulPointBelow"))	sprintf(buf,"%d",m_configData->optionsSoulPointBelow);
	if (!strcmp(paramName,"options/SoulPointAbove"))	sprintf(buf,"%d",m_configData->optionsSoulPointAbove);
	if (!strcmp(paramName,"options/Blank"))				sprintf(buf,"%d",m_configData->optionsBlank);
	if (!strcmp(paramName,"options/Capacity"))			sprintf(buf,"%d",m_configData->optionsCapacity);
	if (!strcmp(paramName,"options/OutOfCustomItem"))	sprintf(buf,"%d",m_configData->optionsOutOfCustomItem);
	if (!strcmp(paramName,"options/RunawayReached"))	sprintf(buf,"%d",m_configData->optionsRunawayReached);
	if (!strcmp(paramName,"sound"))						sprintf(buf,"%d",m_configData->sound);
	if (!strcmp(paramName,"whiteList")){		
		if (currentPos<100){				
			if (IsCharAlphaNumeric(m_configData->whiteList[currentPos][0])){				
				lstrcpyn(buf,m_configData->whiteList[currentPos++],32);
			}
		}		
	}

	return buf;
}

char *CMod_autogoApp::getConfigParamName(int nr)
{

	switch (nr)
	{
	case 0: return "act/x";
	case 1: return "act/y";
	case 2: return "act/z";
	case 3: return "act/direction";
	case 4: return "runaway/x";
	case 5: return "runaway/y";
	case 6: return "runaway/z";
	case 7: return "trigger";
	case 8: return "action/BattleListGM";
	case 9: return "action/BattleListPlayer";
	case 10: return "action/BattleListMonster";
	case 11: return "action/BattleListList";
	case 12: return "action/Sign";
	case 13: return "action/Message";
	case 14: return "action/HpLoss";
	case 15: return "action/HpAbove";
	case 16: return "action/HpBelow";	
	case 17: return "action/ManaAbove";
	case 18: return "action/ManaBelow";
	case 19: return "action/Move";
	case 20: return "action/SoulPointBelow";
	case 21: return "action/SoulPointAbove";
	case 22: return "action/Blank";
	case 23: return "action/Capacity";	
	case 24: return "action/OutOfCustom";
	case 25: return "action/OutOfFood";
	case 26: return "action/OutOfSpace";
	case 27: return "action/RunawayReached";
	case 28: return "options/BattleList";
	case 29: return "options/Sign";
	case 30: return "options/Message";
	case 31: return "options/HpAbove";
	case 32: return "options/HpBelow";
	case 33: return "options/ManaAbove";
	case 34: return "options/ManaBelow";
	case 35: return "options/SoulPointBelow";
	case 36: return "options/SoulPointAbove";
	case 37: return "options/RunawayReached";
	case 38: return "options/Blank";
	case 39: return "options/Capacity";
	case 40: return "options/OutOfCustomItem";
	case 41: return "sound";
	case 42: return "whiteList";
	default:
		return NULL;
	}
}
int CMod_autogoApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"whiteList")) return 1;
	return 0;
}

void CMod_autogoApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"whiteList")) currentPos=0;
}
