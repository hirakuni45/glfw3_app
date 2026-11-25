//=========================================================================//
/*! @file
	@brief  pinball 関係メイン処理 Necronomicon
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdio.h>
// #include <sega_sys.h>
#include "ballsim.h"
#include "mobj.h"
#include "info.h"
#include "saturn.h"
#include "sound.h"
#include "option.h"
#include "fnc.h"
#include "cdfs.h"
#include "flags.h"
#include "task.h"
#include "top.h"
#include "tracks.h"
#include "sim.h"
#include "etc.h"
#include "genr.h"
#include "pinball.h"

#define DEMO_PLAY_DELAY		30
#define MASK_TABLES_MAX		12000
#define LOGO_FADE_FRAME		60
#define LOGO_DISPLAY_FRAME	(60 * 3)
#define DEMO_PLAY_TIME		40
#define PAD_COMMAND_MAX		16

extern short exitBallCount;

extern int frameRate;
extern short tvFrame;
extern volatile unsigned int clutTransMask, clutTransBit;
extern volatile unsigned int vsyncCounter;
extern volatile unsigned short padLevel[], padTriggerP[], padTriggerN[], padStatus[];
extern volatile unsigned short padOnlyLevel[], padOnlyTriggerP[], padOnlyTriggerN[];
extern volatile short mouseDx, mouseDy, mouseBl, mouseEna;
extern volatile unsigned short mouseLevel, mouseTriggerP, mouseTriggerN;
/// extern volatile short analogEna[], analogX[], analogY[];
extern uint32_t clutBuff[];
extern char flipperMask;
extern int screenWidth, screenHeight;
extern ANIME fontAnime, infoAnime, ifonAnime, info2Anime, lupeAnime;
extern char gameMode;
extern char playerNo;
extern volatile short slaveTaskNo;
extern short cdFadeLevel;
extern char startMaskEnable, demoPlayIndex;
extern char mainMenuPos, gameMenuPos;
extern char optionMenuPad[];
extern int clutIdxMOBJ;
extern char infoPause;
extern char titleMovieIndex;

static void ServiceSecretCommand(void);
static void FlashPadCommand(void);
static void ServicePadCommand(void);
static int ScanPadCommand(const uint16_t *cmd);
static void ServiceStartABC(void);
static void EntryMovie(void);
static void EntryTestBackup(void);
static void EntryGameInitBefore(void);
static void EntryGameInit(void);
static void MainGame3D(void);
static int GameDemo3D(void);
static int Game3D(void);
static void MaskSub(int, int, int, int);

uint32_t clutSave[256];
uint32_t clutLupe[16];
uint16_t gamePadLevel, gamePadTrigger, gamePadRelese;
short gameAnalogX, gameAnalogY, gameAnalogEna;
short vibrX, vibrY, vibrXo, vibrYo;
short outTimer;
short demoPlayEnable;
char debugFlag = 0;
char gamePauseEnable;
char vibrEnable;
char debugPauseEnable;

static ANIME flipperAnime, ballsAnime, dropAnime, slingAnime, jetsAnime;
static ANIME objsAnime, cursAnime;
static const char titleMovieTables[] = { 0, 5, 0, 0 };
static const char *logoReName = { "LGOALGOBLGODLGOC" };
static const char fontSizeTables[] = {
	4, 4, 7, 9, 10, 7, 10, 3, 6, 6, 10, 8, 3, 8, 3, 7,
	7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 8, 7, 8, 8,
	9, 7, 7, 7, 7, 7, 7, 7, 7, 4, 7, 7, 7, 10, 8, 7,
	7, 7, 7, 7, 8, 7, 7, 10, 8, 8, 7, 6, 7, 6, 7, 7 };
static short *maskPtr[4];
static short mseX, mseY;
static short wposX, wposY;
static short maskTables[MASK_TABLES_MAX];
static const short menuObjsPos[] = { 52, 302,   283, 242,   495, 175 };
static const short cursObjsPos[] = { 143, 302-52,   361, 242-52,   530, 175-52 };
static const short gameObjsPos[] = { 334, 109,  470, 134,   306, 210,   293, 150 };
static short movieTaskNo;
static unsigned short padCommand[PAD_COMMAND_MAX];
static short padCommandPtr;
static const uint16_t staffDebugCommand[] = {
	SATURN_DIR_RIGHT,
	SATURN_DIR_RIGHT,
	SATURN_PAD_X,
	SATURN_PAD_Z,
	SATURN_PAD_Y,
	SATURN_PAD_C,
	SATURN_PAD_B,
	SATURN_PAD_A,
	SATURN_PAD_A,
	SATURN_PAD_B,
	SATURN_PAD_C,
	0xffff };
static const uint16_t debugEnableCommand[] = {
	SATURN_PAD_Z,
	SATURN_PAD_Y,
	SATURN_PAD_X,
	SATURN_DIR_UP,
	SATURN_DIR_UP,
	SATURN_PAD_C,
	SATURN_PAD_B,
	SATURN_PAD_A,
	SATURN_DIR_DOWN,
	SATURN_DIR_DOWN,
	SATURN_DIR_DOWN,
	SATURN_DIR_DOWN,
	0xffff };

static void ServiceSecretCommand(void)
{
	if(ScanPadCommand(staffDebugCommand)) staffRollDebug = 1;
	else if(ScanPadCommand(debugEnableCommand)) debugEnable = 1;
	ServicePadCommand();
}

static void FlashPadCommand(void)
{
	int	i;

	for(i=0; i<PAD_COMMAND_MAX; ++i) padCommand[i] = 0;
	padCommandPtr = 0;
}

static void ServicePadCommand(void)
{
	unsigned short	p;

	p = padTriggerP[0];
	if(p & (SATURN_DIR_ALL | SATURN_PAD_ABCXYZ | SATURN_PAD_RIGHT | SATURN_PAD_LEFT))
		{ padCommand[padCommandPtr] = p;
		padCommandPtr++;
		if(padCommandPtr >= PAD_COMMAND_MAX) padCommandPtr = 0; }
}

static int ScanPadCommand(const uint16_t *cmd)
{
	int	i;
	uint16_t p;

	if(cmd==NULL) return 0;
	i = padCommandPtr;
	while((p = *cmd++)!=0xffff)
	{ i--;
	if(i < 0) i = PAD_COMMAND_MAX - 1;
	if(p!=padCommand[i]) return 0;
	}
	return 1;
}

static int logoTaskIdx;
static int logoFadeFrame;
static int logoFadeInten;

static void TaskLogoInit(void);
static void TaskLogoFadeIn(void);
static void TaskLogoDisplay(void);
static void TaskLogoFadeOut(void);

void EntryLogo(void)
{
// printf("EntryLogo\n");
	demoPlayIndex = 0;
	startMaskEnable = 1;
	mainMenuPos = 1;
	gameMenuPos = 1;
	movieTaskNo = 0;
	titleMovieIndex = 0;

	if(debugEnable) {
		KillTask(EntryLogo);
		CreateTask(EntryMovie);
		return;
	}

	LaunchResource("LOGO.RES");
	logoTaskIdx = 0;
	KillTask(EntryLogo);

///	CreateTask(TaskLogoInit);
	CreateTask(EntryTitle);
}

static void TaskLogoInit(void)
{
// printf("TaskLogoInit: %d\n", logoTaskIdx);
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	ExtractBitImage(logoReName + logoTaskIdx * 4, 0, 0);
	for(int j = 0; j < 256; ++j) {
		clutSave[j] = clutBuff[j];
		clutBuff[j] = 0;
	}
	clutTransMask = 1;

	logoFadeFrame = 0;
	logoFadeInten = 0;
	KillTask(TaskLogoInit);
	CreateTask(TaskLogoFadeIn);
}

static void TaskLogoFadeIn(void)
{
// if(logoFadeFrame == 0) printf("TaskLogoFadeIn\n");
	if(logoFadeFrame < LOGO_FADE_FRAME) {
		SyncBlanking();
		ServiceStartABC();
		for(int j = 0; j < 256; ++j) clutBuff[j] = clutSave[j];
		BrightColorTables(0, 64, logoFadeInten);
		logoFadeInten += 5;
		if(logoFadeInten > 255) logoFadeInten = 256;
		logoFadeFrame++;
	} else {
		logoFadeFrame = 0;
		KillTask(TaskLogoFadeIn);
		CreateTask(TaskLogoDisplay);
	}
}

static void TaskLogoDisplay(void)
{
// if(logoFadeFrame == 0) printf("TaskLogoDisplay\n");
	if(logoFadeFrame < LOGO_DISPLAY_FRAME) {
		SyncBlanking();
		ServiceStartABC();
		logoFadeFrame++;
	} else {
		logoFadeFrame = 0;
		logoFadeInten = 256;
		KillTask(TaskLogoDisplay);
		CreateTask(TaskLogoFadeOut);
	}
}

static void TaskLogoFadeOut(void)
{
// if(logoFadeFrame == 0) printf("TaskLogoFadeOut\n");
	if(logoFadeFrame < LOGO_FADE_FRAME) {
		SyncBlanking();
		ServiceStartABC();
		for(int j = 0; j < 256; ++j) clutBuff[j] = clutSave[j];
		BrightColorTables(0, 64, logoFadeInten);
		logoFadeInten -= 5;
		if(logoFadeInten < 0) logoFadeInten = 0;
		logoFadeFrame++;
	} else {
		logoTaskIdx++;
		KillTask(TaskLogoFadeOut);
		if(logoTaskIdx < 4) {
			CreateTask(TaskLogoInit);
		} else {
///			CreateTask(EntryMovie);
			CreateTask(EntryTitle);
		}
	}
}

static void ServiceStartABC(void)
{
	unsigned short t, l;

	t = padOnlyTriggerP[0] | padOnlyTriggerP[1];
	if(t & SATURN_PAD_START) l = padOnlyLevel[0] | padOnlyLevel[1];
	else l = 0;
	if((l & SATURN_PAD_ABC)==SATURN_PAD_ABC) {
	if(geEnable) LaunchGameWare();
	else if(flashSegaEnable) SYS_Exit(0);
	else SYS_EXECDMP();
	}
}

static void EntryMovie(void)
{
///	void (*taskAVI)(int);

	PlayCD(-1, -1);

	if(flashSegaEnable | geEnable) {
		KillTask(EntryMovie);
		CreateTask(EntryTitle);
		return;
	}

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

///	ReadFile("TMOTION.SH2", SATURN_DRAM_TOP);
	cdFadeLevel = 7;
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	TransmitValues();
	ServiceCloseInt();

///	taskAVI = (void *)SATURN_DRAM_TOP;
///	(*taskAVI)(movieTaskNo);
}

static void LoopTitle(void);
static uint32_t tTitle = 0;
static uint32_t sTitle = 0;

void EntryTitle(void)
{
	PlayCD(-1, -1);
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	LaunchResource("MENU.RES");
	LaunchSound("SNDG", "SNDM");
	SetupSoundGroup(0);
	if(trialEnable | geEnable) ExtractBitImage("TLRV", 0, 0);
	else ExtractBitImage("TITL", 0, 0);
	InstallMotionObject("MOBJ", &objsAnime);

	for(int i = 0; i < 10; ++i) {
		clutSave[i] = clutBuff[i + 54];
		SetCLUTi(54+i, 0);
	}
	clutTransMask = 1;

	FlashPadCommand();

	KillTask(EntryTitle);
	CreateTask(LoopTitle);
	tTitle = 0;
	sTitle = 0;
}

static void LoopTitle(void)
{
	int start = 0;
	SyncBlanking();
	ServiceSecretCommand();
	Rand();
	OpenMotionObject(0, 0);
	gamePadLevel = padOnlyLevel[0] | padOnlyLevel[1];
	gamePadTrigger = padOnlyTriggerP[0] | padOnlyTriggerP[1];
	sTitle++;
	if(sTitle > (tvFrame * 2)) startMaskEnable = 0;
	if(startMaskEnable == 0) {
		if(gamePadTrigger & SATURN_PAD_START) start = 1;
		ServiceBrightClut(54, 10);
	}
	if(gamePadLevel) tTitle = 0;
	tTitle++;
	if(tTitle >= (30 * tvFrame) && flashSegaEnable!=0) SYS_Exit(0);
	if(sTitle >= (tvFrame * DEMO_PLAY_DELAY)) {
		realmsEnable = 0;
		demoPlayEnable = DEMO_PLAY_TIME * tvFrame;
		gameMenuPos = demoPlayIndex + 1;
		demoPlayIndex++;
		if(demoPlayIndex >= 3) demoPlayIndex = 0;
		if(flashSegaEnable) demoPlayIndex = 0;
		CloseMotionObject();
		KillTask(LoopTitle);
///		CreateTask(EntryGameInit);
	CreateTask(EntryMainMenu);
		return;
	}
	CloseMotionObject();
	
	if(start) {
		if((gamePadLevel & SATURN_PAD_ABC) == SATURN_PAD_ABC) {
			if(geEnable) LaunchGameWare();
			else if(flashSegaEnable) SYS_Exit(0);
			else SYS_EXECDMP();
		}

		KillTask(LoopTitle);
		uint16_t i = SATURN_PAD_RIGHT | SATURN_PAD_LEFT;
		if((gamePadLevel & i)==i && debugEnable!=0) CreateTask(EntryStaffRoll);
		else CreateTask(EntryMainMenu);
	}
}

static void LoopMainMenu(void);
static uint32_t tMainMenu;

void EntryMainMenu(void)
{
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	ExtractBitImage("MAIN", 0, 0);
	InstallMotionObject("MOBJ", &objsAnime);
	InstallMotionObject("CURS", &cursAnime);
	clutTransMask = 1;

	cdFadeLevel = 7;

	PlayCD(-1, CDA_MENU);

	tMainMenu = 0;

	KillTask(EntryMainMenu);
	CreateTask(LoopMainMenu);
}

static void LoopMainMenu(void)
{
	int start = 0;
	SyncBlanking();
	Rand();
	OpenMotionObject(0, 0);
	gamePadLevel = padOnlyLevel[0] | padOnlyLevel[1];
	gamePadTrigger = padOnlyTriggerP[0] | padOnlyTriggerP[1];
	if(gamePadLevel) tMainMenu = 0;
	int i = gamePadTrigger & SATURN_PAD_START;
	if(i) start = 1;
	int k = mainMenuPos;
	if(gamePadTrigger & SATURN_DIR_LEFT) mainMenuPos--;
	if(gamePadTrigger & SATURN_DIR_RIGHT) mainMenuPos++;
	if((trialEnable | geEnable)!=0 && mainMenuPos > 1) mainMenuPos = 1;

	if(mainMenuPos < 0) mainMenuPos = 0;
	else if(mainMenuPos >= 2) mainMenuPos = 2;
	if(k != mainMenuPos) RingSfx(SFX_MENU_SELECT);

	k = mainMenuPos * 2;
	MOBJ *mobj = objsAnime.mobj + mainMenuPos;
	mobj->x = menuObjsPos[k++];
	mobj->y = menuObjsPos[k];
	if((vsyncCounter & 7) <= 5) SetupObject(mobj);
	k = mainMenuPos * 2;
	mobj = cursAnime.mobj;
	mobj->x = cursObjsPos[k++] - 44;
	mobj->y = cursObjsPos[k];
	SetupObject(mobj);
	CloseMotionObject();
	tMainMenu++;
	if(tMainMenu >= (30 * tvFrame) && flashSegaEnable != 0) SYS_Exit(0);

	if(start) {
		RingSfx(SFX_SACRIFICIAL_DRUMS);

		KillTask(LoopMainMenu);

		i = SATURN_PAD_RIGHT | SATURN_PAD_LEFT;
		if((gamePadLevel & SATURN_PAD_ABC)==SATURN_PAD_ABC && (gamePadTrigger & SATURN_PAD_START) != 0) {
			CreateTask(EntryTitle);
		} else if(debugEnable != 0 && (gamePadLevel & i) == i) {
			CreateTask(EntrySoundTester);
		} else if(mainMenuPos == 0) {
			CreateTask(EntryManual);
		} else if(mainMenuPos == 1) {
			CreateTask(EntryGameMenu);
		} else if(mainMenuPos == 2) {
			CreateTask(EntryOptions);
		} else {
			CreateTask(EntryMainMenu);
		}
	}
}

static void LoopGameMenu(void);
static uint32_t tGameMenu;

void EntryGameMenu(void)
{
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	ExtractBitImage("GAME", 0, 0);
	InstallMotionObject("GOBJ", &objsAnime);
	clutTransMask = 1;

	KillTask(EntryGameMenu);
	CreateTask(LoopGameMenu);

	tGameMenu = 0;
}

static void LoopGameMenu(void)
{
	int start = 0;
	SyncBlanking();
	Rand();
	OpenMotionObject(0, 0);
	gamePadLevel = padOnlyLevel[0] | padOnlyLevel[1];
	gamePadTrigger = padOnlyTriggerP[0] | padOnlyTriggerP[1];
	if(gamePadLevel) tGameMenu = 0;
	if((gamePadTrigger & SATURN_PAD_START) != 0) {
		start = 1;
	}

	int k = gameMenuPos;
	if(gamePadTrigger & SATURN_DIR_LEFT) gameMenuPos--;
	if(gamePadTrigger & SATURN_DIR_RIGHT) gameMenuPos++;
	if(trialEnable) gameMenuPos = 1;
	if(geEnable) {
		if(gameMenuPos > 2) gameMenuPos = 1;
		else if(gameMenuPos < 1) gameMenuPos = 2;
	}
	gameMenuPos &= 3;

	if(optionMenuPad[OPTION_NO_PLAYER]!=0 && gameMenuPos==0) {
		gameMenuPos = k;
		if(gameMenuPos==0) gameMenuPos = 1;
	}

	if(k != gameMenuPos) RingSfx(SFX_MENU_SELECT);
	k = gameMenuPos * 2;
	MOBJ *mobj = objsAnime.mobj + gameMenuPos;
	mobj->x = gameObjsPos[k++];
	mobj->y = gameObjsPos[k];
	if((vsyncCounter & 7) <= 5) SetupObject(mobj);
	CloseMotionObject();
	tGameMenu++;
	if(tGameMenu >= (30 * tvFrame) && flashSegaEnable != 0) SYS_Exit(0);

	if(start) {
		KillTask(LoopGameMenu);
		if((gamePadLevel & SATURN_PAD_ABC)==SATURN_PAD_ABC && (gamePadTrigger & SATURN_PAD_START) != 0) {
			CreateTask(EntryTitle);
		} else {
			PlayCD(-1, -1);
			if(gameMenuPos==0) RingSfx(SFX_SACRIFICIAL_DRUMS);
			else if(gameMenuPos==1) RingSfx(SFX_MENU_ARKHAM);
			else if(gameMenuPos==2) RingSfx(SFX_MENU_BLOODY);
			else RingSfx(SFX_MENU_DLANDS);

			int i = SATURN_PAD_RIGHT | SATURN_PAD_LEFT;
			if(debugEnable) {
				if((gamePadLevel & i)==SATURN_PAD_RIGHT) debugFlag = 1;
				if((gamePadLevel & i)==i) CreateTask(EntryInfoTester);
				else CreateTask(EntryTestBackup);
			} else {
///				CreateTask(EntryTestBackup);
				CreateTask(EntryGameInit);
			}
			realmsEnable = 0;
			if(gameMenuPos==0) { realmsEnable = 1; gameMenuPos = 1; }
		}
	}
}

static void EntryTestBackup(void)
{
	if(ProbeBuckupMemory()) {
		ToBlackScreen();
		ResetMotionObject();
		OpenMotionObject(0, 0);
		CloseMotionObject();
		ResetBackground();
		ExtractBitImage("ALRT", 0, 0);
		clutTransMask = 1;
		for( ; ; )	{
			SyncBlanking();
			if((padTriggerP[0] | padTriggerP[1]) & SATURN_PAD_START) break;
		}
	}
	demoPlayEnable = 0;
	KillTask(EntryTestBackup);
	if(realmsEnable) CreateTask(EntryGameInitBefore);
	else CreateTask(EntryGameInit);
}

static void EntryGameInitBefore(void)
{
	KillTask(EntryGameInitBefore);
	CreateTask(EntryMovie);
	movieTaskNo = gameMenuPos;
}

static void EntryGameInit(void)
{
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

	if(gameMenuPos==1) LaunchResource("ARKHAM2.RES");
	else if(gameMenuPos==2) LaunchResource("BLOODY2.RES");
	else if(gameMenuPos==3) LaunchResource("DLANDS2.RES");

	InstallMotionObject("BALL", &ballsAnime);
	InstallMotionObject("FLIP", &flipperAnime);
	if(gameMenuPos==1) InstallMotionObject("DROP", &dropAnime);
	InstallMotionObject("SHOT", &slingAnime);
	InstallMotionObject("JETS", &jetsAnime);
	InstallMotionObject("PR16", &infoAnime);
	InstallMotionObject("PLFO", &ifonAnime);
	Install16ColorObject(&info2Anime);
	clutIdxMOBJ = 240 + 256;
	InstallMotionObject("LUPE", &lupeAnime);
	for(int i = 0; i < 16; ++i) clutLupe[i] = clutBuff[256 + 240 + i];

	int k = 0;
	for(int j = 0; j < 4; ++j) {
		const uint8_t *src = GetResource("MASK", j);
		int i = TakeMiddle(src + 1) >> 1;
		src += 4;
		maskPtr[j] = &maskTables[k];
		while(i) { maskTables[k] = *(short *)src; src += 2; i--; k++; }
	}
	if(gameMenuPos==1) {
		LaunchSound("SNDG", "SNDA");
		LaunchResource("ARKHAM1.RES");
	} else if(gameMenuPos==2) {
		LaunchSound("SNDG", "SNDB");
		LaunchResource("BLOODY1.RES");
	} else if(gameMenuPos==3) {
		LaunchSound("SNDG", "SNDD");
		LaunchResource("DLANDS1.RES");
	}

	StartMusic(gameMenuPos - 1);

	ExtractBitImage("PASE", 0, 0);
	clutBuff[0] = 0;
	for(int i = 0; i < 256; ++i) {
		clutSave[i] = clutBuff[i];
		SetCLUTi(i + 256, clutBuff[i]);
	}

	InitBallSimulator((FIELD *)GetResource("PARA", 0), (short *)GetResource("HITS", 0), NULL);

	flipperMask = 0;
	cdFadeLevel = 7;
	gamePauseEnable = 1;
	debugPauseEnable = 0;

	InitGeneralRule(gameMenuPos - 1);
///	InstallSlaveFunction(ServiceSlave);

	clutTransMask = 1;

	vibrEnable = 0;

	vibrX = vibrY = vibrXo = vibrYo = 0;
	outTimer = 0;
	playerNo = 0;

	KillTask(EntryGameInit);
	CreateTask(MainGame3D);
}

static void MainGame3D(void)
{
	// タスク管理から除いたので付けたし（パッド関係のサービス）
	SyncBlanking();

	if(padStatus[0] & 5) {
		if((padOnlyTriggerN[1] & SATURN_PAD_START) != 0 && debugEnable!=0) debugPauseEnable ^= 1;
		MakeGameInput(playerNo);
		if((padOnlyLevel[0] & SATURN_PAD_ABC)==SATURN_PAD_ABC) {
			if(padOnlyTriggerN[0] & SATURN_PAD_START) {
				slaveTaskNo = 0;
/*				InstallSlaveFunction(ServiceNull); */
				PlayCD(-1, -1);
				KillTask(MainGame3D);
				CreateTask(EntryTitle);
				return;
			}
		}
	}

	int st = 0;
	if(demoPlayEnable) {
		demoPlayEnable--;
		st = GameDemo3D();
		if(st < 0) {
			slaveTaskNo = 0;
/*			InstallSlaveFunction(ServiceNull); */
			PresetBGposition(0, 0);
			ToBlackScreen();
			ResetMotionObject();
			ResetBackground();
			KillTask(MainGame3D);
			CreateTask(EntryTitle);
			PlayCD(-1, -1);
			return;
		} else if(demoPlayEnable==0) {
			slaveTaskNo = 0;
/*			InstallSlaveFunction(ServiceNull); */
			PresetBGposition(0, 0);
			ToBlackScreen();
			ResetMotionObject();
			ResetBackground();
			PlayCD(-1, -1);
			if(optionMenuPad[OPTION_NO_REALMS_COMP]!=0 && demoPlayIndex==0) {
				movieTaskNo = 4;
			} else {
				titleMovieIndex++;
				movieTaskNo = titleMovieTables[titleMovieIndex];
				if(demoMovieMax==0) movieTaskNo = 0;
				else if(titleMovieIndex >= demoMovieMax) titleMovieIndex = 0;
			}
			KillTask(MainGame3D);
			CreateTask(EntryMovie);
			return;
		}
	} else {
		if((gamePadTrigger & SATURN_PAD_START)!=0 && gameMode!=0) {
			gamePauseEnable ^= 1;
			if(gamePauseEnable==0) { TurnIntoDark(80); cdFadeLevel = 3; }
			else {
				cdFadeLevel = 7;
				for(int i = 0; i < 256; ++i) {
					SetCLUTi(i, clutSave[i]);
					SetCLUTi(i+256, clutSave[i]);
				}
			}
		}
///		st = Game3D();
	}

	// SH2 スレーブ CPU のタスク
///	ServiceSlave();

	if(st < 0) {
		slaveTaskNo = 0;
/*		InstallSlaveFunction(ServiceNull); */
		PresetBGposition(0, 0);
		ToBlackScreen();
		ResetMotionObject();
		ResetBackground();
		KillTask(MainGame3D);
		if(flashSegaEnable) SYS_Exit(0);
		else CreateTask(EntryTitle);
		if(debugEnable==0) WriteBackupSystem();
	} else if(st != 0 && realmsEnable != 0) {
		slaveTaskNo = 0;
/*		InstallSlaveFunction(ServiceNull); */
		PresetBGposition(0, 0);
		ToBlackScreen();
		ResetMotionObject();
		ResetBackground();
		gameMenuPos++;
		KillTask(MainGame3D);
		CreateTask(EntryGameInitBefore);
	}
}

static int GameDemo3D(void)
{
	if(gamePadTrigger) return -1;
	gamePadLevel = gamePadTrigger = gamePadRelese = 0;
	Game3D();
	AutoFlipper();
	return 0;
}

static int Game3D(void)
{
	int f = 0;
	OpenMotionObject(0, 0);

	ServicePlayerSelector();
	BallSimulatorMotionObjects();

	if(gamePauseEnable) {
		if(debugPauseEnable == 0) {
			if(debugEnable) MoveTestBall(padOnlyLevel[1]);
			MainBallSimulator();
			if(vibrEnable) ServiceVibration();
			f = ServiceGeneralRule();
			infoPause = 1;
		} else {
			infoPause = 0;
		}
		if(demoPlayEnable == 0) {
			ServiceInfomations();
		} else {
			if((vsyncCounter & 31) < 12) {
				MobjInfoTexts(320, 190, 1, "EXPLORERS/PRESS START BUTTON");
			}
		}
	} else {
		ServiceStatus();
		if(gamePadLevel) outTimer = 0;
		else outTimer++;
		if(outTimer >= (30 * tvFrame) && flashSegaEnable != 0) f = -1;
	}

	PresetBGposition(vibrXo, vibrYo);
	vibrXo = vibrX;
	vibrYo = vibrY;

/* MobjDecimal(570, 50, exitBallCount); */

	CloseMotionObject();
	return f;
}

void MakeGameInput(int i)
{
	if(padStatus[0] & 5) {
		if((padStatus[i] & 5) == 0) i = 0;
		gamePadLevel = padOnlyLevel[i];
		gamePadTrigger = padOnlyTriggerP[i];
		gamePadRelese = padOnlyTriggerN[i];
		gameAnalogX = analogX[i];
		gameAnalogY = analogY[i];
		gameAnalogEna = analogEna[i];

		if(mouseEna) {
			if(mouseLevel & SATURN_PAD_START) gamePadLevel |= SATURN_PAD_START;
			if(mouseTriggerP & SATURN_PAD_START) gamePadTrigger |= SATURN_PAD_START;
			if(mouseLevel & SATURN_PAD_A) gamePadLevel |= SATURN_DIR_LEFT;
			if(mouseTriggerP & SATURN_PAD_A) gamePadTrigger |= SATURN_DIR_LEFT;
			if(mouseLevel & SATURN_PAD_C) gamePadLevel |= SATURN_PAD_C;
			if(mouseTriggerP & SATURN_PAD_C) gamePadTrigger |= SATURN_PAD_C;

			mseX += mouseDx;
			mseY += mouseDy;
			if(mseX >= 58) {
				gamePadLevel |= SATURN_PAD_RIGHT;
				gamePadTrigger |= SATURN_PAD_RIGHT;
				mseX = 0;
			} else if(mseX <= -58) {
				gamePadLevel |= SATURN_PAD_LEFT;
				gamePadTrigger |= SATURN_PAD_LEFT;
				mseX = 0;
			}
			if(mseY >= 60) {
				gamePadLevel |= SATURN_PAD_LEFT | SATURN_PAD_RIGHT;
				gamePadTrigger |= SATURN_PAD_LEFT | SATURN_PAD_RIGHT;
				mseY = 0;
			} else if(mseY <= -60) {
				mseY = 0;
			}
		}
	} else {
		gamePadLevel = gamePadTrigger = gamePadRelese = 0;
	}
}

void ServiceRealmsMode(int no)
{
	if(no >= 5) {
		CreateTask(EntryStaffRoll);
	} else {
		CreateTask(EntryGameInit);
	}
}

void TurnIntoDark(int v)
{
	for(int i = 0; i < 256; ++i) clutBuff[i] = clutSave[i];
	BrightColorTables(0, 255, v);
	for(int i = 16; i < 256; ++i) SetCLUTi(i + 256, clutBuff[i]);
}

int MobjScore(int x, int y, const char *ptr)
{
	int f = 0;
	for(int i = 0; i < 12; ++i) {
		if( ptr[i] ) f |= 1;
		f <<= 1;
	}

	int j = 0x0fff;
	for(int i = 0; i < 12; ++i) {
		int k = *ptr;
		ptr++;
		x -= 16;
		if(y >= 0) MobjInfoText(x, y, 2, k + 30);
		if(i >= 1 && (f & j)==0) break;
		if(i == 2 || i == 5 || i == 8) {
			x -= 8;
			if(y >= 0) MobjInfoText(x, y, 2, 26);
		}
		j >>= 1;
	}
	return x;
}



void MobjBall(int x, int y, int size, int maskno, int no)
{
	short	h, v, *msk, *tbl, *ptr;
	int	i, j, k, l, m, n, f;
	int	xt, yt, xb, yb;
	MOBJ	*mobj;

	if(size > 31) size = 31;
	else if(size < 9) size = 9;

	h = screenWidth >> 1;
	v = screenHeight >> 1;
	l = 31 - size;

	mobj = ballsAnime.mobj + l;
	mobj->x = h + x - wposX - vibrX - (size >> 1);
	mobj->y = v - y - wposY - vibrY - (mobj->v >> 1);
	mobj->color = 0x10;
	SetupObject(mobj);

	h = mobj->h >> 1;
	v = mobj->v >> 1;
	x = mobj->x;
	y = mobj->y;
	j = 440;
	if(mobj->y >= j) {
	j = mobj->y;
	for(i=0; i<32; ++i) SetupObjectLine(mobj->x, j+i, mobj->x+32, j+i, 0);
	}

	size++;
	MaskSub(maskno, x, y, size);
	MaskSub(maskno+2, x, y, size);
}

static void MaskSub(int no, int x, int y, int size)
{
	short	*msk, *tbl, *ptr;
	int	i, j, k, n, m, l;
	int	xt, yt, xb, yb;

	msk = maskPtr[no];
	i = *msk++;
	xt = *msk++; yt = *msk++;
	xb = *msk++; yb = *msk++;
	tbl = msk + (y - yt);
	msk += (yb - yt) + 1;
	if(xt <= x && xb > x && (yt-size) <= y && yb > y) {
	for(j=0; j<size; ++j)
		{
		l = y + j;
		if(l>=yb) break;
		m = *tbl++;
		n = *tbl;
		if(l >= 0) {
			l -= wposY;
			ptr = msk + m + m;
			k = n - m;
			while( k ) {
				m = *ptr++;
				n = *ptr++;
				if(m < x) m = x;
				if(n > (x+size)) n = x+size;
				if(m < n) {
	if(no < 2) SetupObjectLine(m-vibrX, l-vibrY, n-vibrX, l-vibrY, 0);
	else SetupObjectLineWithMesh(m-vibrX, l-vibrY, n-vibrX, l-vibrY, 0);
					}
				k--;
				}
			}
		}
	}
}

void MobjFlipper(int x, int y, int no)
{
	MOBJ	*mobj;

	x -= wposX + vibrX;
	y -= wposY + vibrY;
	mobj = flipperAnime.mobj + (no & 15);
	if(no & 0x10) mobj->flip = VDP1_CMD_H_FLIP;
	else mobj->flip = 0;
	mobj->x = x;
	mobj->y = y;
	mobj->color = 0x20;
	SetupObject(mobj);
}

void MobjDropTarget(int x, int y, int no)
{
	MOBJ	*mobj;

	x -= wposX + vibrX;
	y -= wposY + vibrY;
	mobj = dropAnime.mobj + no;
	mobj->x = x;
	mobj->y = y;
	SetupObject(mobj);
}

void MobjJetBumper(int x, int y, int no)
{
	MOBJ	*mobj;

	x -= wposX + vibrX;
	y -= wposY + vibrY;
	mobj = jetsAnime.mobj + no;
	mobj->x = x;
	mobj->y = y;
	SetupObject(mobj);
}

void MobjSlingShot(int x, int y, int no)
{
	MOBJ	*mobj;

	x -= wposX + vibrX;
	y -= wposY + vibrY;
	mobj = slingAnime.mobj + no;
	mobj->x = x;
	mobj->y = y;
	SetupObject(mobj);	
}

int MobjFont(int x, int y, int n)
{
	int	i, j;
	MOBJ	*mobj;

	n -= 0x20;
	if(n >= 0x40) n -= 0x20;
	if(n > 0 && n < 64) {
	mobj = fontAnime.mobj + n - 1;
	mobj->x = x;
	mobj->y = y;
	SetupResizeObject(mobj, 32-1, 30-1);
	}
	return (fontSizeTables[n] << 1) + 2;
}

void MobjString(int x, int y, const char *ptr)
{
	int	n;

	while((n = *ptr++) != 0) {
	x += MobjFont(x, y, n);
	}
}

void MobjStringC(int x, int y, const char *ptr)
{
	int	n, m;
	const char *p;

	p = ptr;
	m = 0;
	while((n = *p++) != 0) {
	n -= 0x20;
	if(n >= 0x40) n -= 0x20;
	if(n > 0 && n < 64) m += (fontSizeTables[n] << 1) + 2;
	}
	x -= m >> 1;

	while((n = *ptr++ ) != 0) {
		x += MobjFont(x, y, n);
	}
}

void MobjNumber(int x, int y, int no)
{
	char	tt[4];

	tt[0] = (no / 10) + 0x30;
	tt[1] = (no % 10) + 0x30;
	tt[2] = 0;
	MobjStringC(x, y, tt);
}
