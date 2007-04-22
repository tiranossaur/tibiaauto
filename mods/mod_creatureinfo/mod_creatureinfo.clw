; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=5
Class1=CMod_creatureinfoApp
LastClass=CSendStats
NewFileInclude2=#include "mod_creatureinfo.h"
ResourceCount=4
NewFileInclude1=#include "stdafx.h"
Class2=CConfigDialog
LastTemplate=CDialog
<<<<<<< mod_creatureinfo.clw
Resource1=IDD_CONFIG (English (U.S.))
=======
Resource1=IDD_CONFIG (English (U.S.))
>>>>>>> 1.7
Class3=CNameChanger
Resource2=IDD_INFO (English (U.S.))
Class4=CKnownInfo
Resource3=IDD_NAME (English (U.S.))
Class5=CSendStats
Resource4=IDD_SENDSTATS

[CLS:CMod_creatureinfoApp]
Type=0
HeaderFile=mod_creatureinfo.h
ImplementationFile=mod_creatureinfo.cpp
Filter=N
LastObject=CMod_creatureinfoApp

[CLS:CConfigDialog]
Type=0
HeaderFile=ConfigDialog.h
ImplementationFile=ConfigDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CConfigDialog
VirtualFilter=dWC

[CLS:CNameChanger]
Type=0
HeaderFile=NameChanger.h
ImplementationFile=NameChanger.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_TOOLCREATURINFO_LIST
VirtualFilter=dWC

[DLG:IDD_CONFIG (English (U.S.))]
Type=1
Class=CConfigDialog
ControlCount=36
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,button,1342177287
Control4=IDC_TOOLCREATURINFO_PLAYER,button,1342242819
Control5=IDC_TOOLCREATURINFO_PLAYER_1LINE,edit,1350631552
Control6=IDC_TOOLCREATURINFO_PLAYER_2LINE,edit,1350631552
Control7=IDC_TOOLCREATURINFO_MONSTER,button,1342242819
Control8=IDC_TOOLCREATURINFO_MONSTER_1LINE,edit,1350631552
Control9=IDC_TOOLCREATURINFO_MONSTER_2LINE,edit,1350631552
Control10=IDC_TOOLCREATURINFO_MONSTER_UNIQUEMONSTERNAMES,button,1342242819
Control11=IDC_TOOLCREATURINFO_SELF,button,1342242819
Control12=IDC_TOOLCREATURINFO_SELF_1LINE,edit,1350631552
Control13=IDC_TOOLCREATURINFO_SELF_2LINE,edit,1350631552
Control14=IDC_TOOLCREATURINFO_ALLINFO,button,1342242819
Control15=IDC_TOOLCREATURINFO_NAMECHANGER,button,1342242816
Control16=IDC_TOOLCREATURINFO_KNOWNINFO,button,1476460544
Control17=IDC_ENABLE,button,1342246915
Control18=IDOK,button,1342242817
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_TOOLCREATURINFO_ADDITIONALREQUEST,button,1342242819
Control26=IDC_COLLECT_STATS,button,1476460547
Control27=IDC_SHOW_CREATURES_IN_AREA,button,1073807363
Control28=IDC_STATIC,button,1342177287
Control29=IDC_STATIC,static,1342308864
Control30=IDC_STATIC,static,1342308864
Control31=IDC_RANGE_XY,edit,1350631552
Control32=IDC_RANGE_Z,edit,1350631552
Control33=IDC_STATIC,static,1342308352
Control34=IDC_TOOLCREATURINFO_SELF_WINDOW,edit,1350631552
Control35=IDC_STATIC,static,1342308352
Control36=IDC_TOOLCREATURINFO_SELF_TRAY,edit,1350631552

[DLG:IDD_NAME (English (U.S.))]
Type=1
Class=CNameChanger
ControlCount=6
Control1=IDC_TOOLCREATURINFO_LIST,SysListView32,1350631941
Control2=IDOK,button,1342242816
Control3=IDC_TOOLCREATURINFO_REFRESH,button,1342246915
Control4=IDC_TOOLCREATURINFO_RELPOS,button,1342246915
Control5=IDC_TOOLCREATURINFO_ADVANCED,button,1342246915
Control6=IDC_TOOLCREATURINFO_BATTLEONLY,button,1342246915

[DLG:IDD_INFO (English (U.S.))]
Type=1
Class=CKnownInfo
ControlCount=24
Control1=IDC_TOOLCREATURINFO_INFO_MONSTERS,button,1342259209
Control2=IDC_TOOLCREATURINFO_INFO_PLAYERS,button,1342259209
Control3=IDC_TOOLCREATURINFO_INFO_NAME,edit,1350633600
Control4=IDC_TOOLCREATURINFO_INFO_TYPE,edit,1350633600
Control5=IDC_TOOLCREATURINFO_INFO_HP,edit,1350633600
Control6=IDC_TOOLCREATURINFO_INFO_VOC,edit,1350633600
Control7=IDC_TOOLCREATURINFO_INFO_LEVEL,edit,1350633600
Control8=IDC_TOOLCREATURINFO_INFO_LAST,edit,1350633600
Control9=IDC_TOOLCREATURINFO_INFO_GUILDNAME,edit,1350633600
Control10=IDC_TOOLCREATURINFO_INFO_GUILDRANK,edit,1350633600
Control11=IDC_TOOLCREATURINFO_INFO_GUILDDESCRIPTION,edit,1350633600
Control12=IDOK,button,1342242817
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_TOOLCREATURINFO_INFO_TXTVOC,static,1342308352
Control18=IDC_TOOLCREATURINFO_INFO_TXTLEVEL,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,button,1342177287
Control24=IDC_TOOLCREATURINFO_INFO_COMBO,combobox,1344340227

[CLS:CKnownInfo]
Type=0
HeaderFile=KnownInfo.h
ImplementationFile=KnownInfo.cpp
BaseClass=CDialog
Filter=D
LastObject=CKnownInfo
VirtualFilter=dWC

[DLG:IDD_SENDSTATS]
Type=1
Class=CSendStats
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1350696961

[CLS:CSendStats]
Type=0
HeaderFile=SendStats.h
ImplementationFile=SendStats.cpp
BaseClass=CDialog
Filter=D

