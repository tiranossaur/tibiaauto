// mod_trademon.h : main header file for the MOD_TRADEMON DLL
//

#if !defined(AFX_MOD_TRADEMON_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_TRADEMON_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ConfigDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMod_trademonApp
// See mod_trademon.cpp for the implementation of this class
//

class CMod_trademonApp : public IModuleInterface
{
public:
	CMod_trademonApp();
	~CMod_trademonApp();
public:
	void getNewSkin(CSkin);
	int validateConfig(int showAlerts);
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void start();
	void stop();
	void showConfigDialog();
	void configToControls();
	void controlsToConfig();
	void disableControls();
	void enableControls();
	char *getVersion();
	void resetConfig();
	void loadConfigParam(const char *paramName, char *paramValue);
	char *saveConfigParam(const char *paramName);
	const char **getConfigParamNames();
private:
	int m_started;
	CConfigDialog * m_configDialog;
	CConfigData *m_configData;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_TRADEMON_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
