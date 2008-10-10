; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
Class1=CMod_showmapApp
LastClass=CCharInfoDialog
NewFileInclude2=#include "mod_showmap.h"
ResourceCount=5
NewFileInclude1=#include "stdafx.h"
Class2=CCharInfoDialog
LastTemplate=CDialog
Resource1=IDD_TOOL_MAPSHOW
Resource2=IDR_SHOWMAP_MENU
Resource3=IDD_TOOL_MAPSHOW (Polish)
Resource4=IDD_TOOL_MAPSHOW_CONFIG
Resource5=IDR_SHOWMAP_MENU (Polish)

[CLS:CMod_showmapApp]
Type=0
HeaderFile=mod_showmap.h
ImplementationFile=mod_showmap.cpp
Filter=N

[CLS:CCharInfoDialog]
Type=0
HeaderFile=CharInfoDialog.h
ImplementationFile=CharInfoDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CCharInfoDialog
VirtualFilter=dWC

[DLG:IDD_TOOL_MAPSHOW]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_TOOL_MAPSHOW_RESEARCH,button,1342242819
Control3=IDC_TOOL_MAPSHOW_CLEAR,button,1342242816
Control4=IDC_TILE_INFO,static,1350696960
Control5=IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH,button,1342242819

[MNU:IDR_SHOWMAP_MENU]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_ROPE
Command4=ID_SHOWMAPOPTIONS_LADDER
Command5=ID_SHOWMAPOPTIONS_STAIRS
Command6=ID_SHOWMAPOPTIONS_MAGICROPE
Command7=ID_SHOWMAPOPTIONS_OPENHOLE
Command8=ID_SHOWMAPOPTIONS_CLOSEDHOLE
Command9=ID_SHOWMAPOPTIONS_CRATE
Command10=ID_SHOWMAPOPTIONS_DEPOT
CommandCount=10

[MNU:IDR_SHOWMAP_MENU (Polish)]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_DEPOT
Command4=ID_SHOWMAPOPTIONS_ROPE
Command5=ID_SHOWMAPOPTIONS_LADDER
Command6=ID_SHOWMAPOPTIONS_STAIRS
Command7=ID_SHOWMAPOPTIONS_MAGICROPE
Command8=ID_SHOWMAPOPTIONS_OPENHOLE
Command9=ID_SHOWMAPOPTIONS_CLOSEDHOLE
Command10=ID_SHOWMAPOPTIONS_CRATE
Command11=IDD_TOOL_MAPSHOW_CONFIG
CommandCount=11

[DLG:IDD_TOOL_MAPSHOW (Polish)]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_TOOL_MAPSHOW_RESEARCH,button,1342242819
Control3=IDC_TOOL_MAPSHOW_CLEAR,button,1342242816
Control4=IDC_TILE_INFO,static,1350696960
Control5=IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH,button,1342242819

[DLG:IDD_TOOL_MAPSHOW_CONFIG]
Type=1
Class=?
ControlCount=25
Control1=IDC_COMMIT,button,1342242817
Control2=IDC_GROUND,button,1342242819
Control3=IDC_CONTAINER,button,1342242819
Control4=IDC_DEPOT,button,1342242819
Control5=IDC_SHOVEL,button,1342308361
Control6=IDC_ROPE,button,1342177289
Control7=IDC_USE_TILE,button,1342177289
Control8=IDC_MOVABLE,button,1342243075
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,button,1342177287
Control12=IDC_GO_UP,button,1342308361
Control13=IDC_GO_DOWN,button,1342177289
Control14=IDC_STATIC,button,1342177287
Control15=IDC_BLOCKING,button,1342251267
Control16=IDC_WALKABLE,button,1342251267
Control17=IDC_ALWAYS_ON_TOP,button,1342251267
Control18=IDC_STACKABLE,button,1342243075
Control19=IDC_MORE_ALWAYS_ON_TOP,button,1342251267
Control20=IDC_TILE_ID,edit,1350641792
Control21=IDC_STATIC,static,1342309120
Control22=IDC_SPEED,edit,1350639744
Control23=IDC_STATIC,static,1342308864
Control24=IDC_PREVIOUS_CONFIG_TILE,button,1342242816
Control25=IDC_NEXT_CONFIG_TILE,button,1342242816

