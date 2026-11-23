#include <stdio.h>
#include "saturn.h"
#include "mobj.h"
#include "sound.h"
#include "option.h"
#include "score.h"
#include "top.h"
#include "pinball.h"
#include "cdfs.h"
#include "task.h"
#include "genr.h"
#include "info.h"
#include "dex.h"
#include "top.h"
#include "flags.h"
#include "fnc.h"
#include "etc.h"

#define LENZ_X_SIZE	56
#define LENZ_Y_SIZE	56
#define CIRCLE_MAX	58
#define MAN_COLOR_MAX	240
#define MAN_MIRROR_TOP	0x06080000

extern volatile unsigned int clutTransMask;
extern volatile unsigned short padLevel[], padTriggerP[], padTriggerN[], padStatus[];
extern volatile unsigned short padOnlyLevel[], padOnlyTriggerP[], padOnlyTriggerN[];
extern volatile short mouseDx, mouseDy, mouseBl, mouseEna;
extern unsigned int clutTransBit, clutBuff[];
extern int clutIdxMOBJ;
extern ANIME fontAnime, infoAnime, ifonAnime, info2Anime;
extern short infoNumber, infoAnimeMax;
extern int frameRate;
extern char gamePauseEnable;
extern unsigned int clutSave[], clutLupe[];
extern char trialEnable, geEnable;
extern char flashSegaEnable;
extern char staffRollDebug;
extern char *scoreConst[];
extern short tvFrame;
extern short cdFadeLevel;
extern unsigned short gamePadLevel, gamePadTrigger, gamePadRelese;
extern char playerNo;

static uint8_t man_mirror_top_[1024 * 512];
static uint8_t *get_MAN_MIRROR_TOP() { return man_mirror_top_; }

static void ServiceOptions(void);
static void MobjOptionsText(int x, int y, const char *t, int);
static int TakeOptionTextSize(const char *t);
static void EntryColorBars(void);
static void ServiceManual(void);
static void InstallLenzObject(ANIME *);
static void RefreshLenzImage(MOBJ *, int, int);
static void EraseRing(int, int);
static void ExtractBits(int x, int y, const uint8_t *src);
static void EtcCopyImage(int, int, int, int, int);
static void RefreshImage(int x, int y, int w, int h, int);
static void ServiceTopExploer(void);
static void ManualText(int x, int y, int c);
static void MainStaffRoll(void);
static int ScanTextWidth(const char *txt);
static void ScanTextHeight(const char *txt, int *);
static int ConvertCode(int c);
static int MobjStaffTexts(int x, int y, const char *s);
static void PositioningText(int t, const char *s, int *);
static int GetKernOffset(int new, int old, const char *kern);

char optionMenuPad[OPTION_PAD_MAX];
NECRONOMICON necroPad[3];
ANIME lupeAnime;
	
static const char initialScoreNameTables[] = {
	'A', 'R', 'K',
	'C', 'L', 'T',
	'D', 'R', 'M',
	'A', 'K', 'I',
	'M', 'I', 'E',
	'O', 'G', 'U',
	'K', 'O', 'B',
	'F', 'L', 'T'
};
static const short topExpPosTables[] = {
	208-15,
	208+13*0-2, 208+13*1-2, 208+13*2-2,
	208+13*3+1, 208+13*4+1, 208+13*5+1,
	208+13*6+4, 208+13*7+4, 208+13*8+4,
	208+13*9+7, 208+13*10+7, 208+13*11+7
};
static unsigned short manLevel, manTrigger;
static short	manTimer;
static short	mouseAcl;
static short	lenzPosX = 640 / 2;
static short	lenzPosY = 448 / 2;
static unsigned short lineBuff[LENZ_X_SIZE];
static short	circleTables[CIRCLE_MAX];
static ANIME ballsAnime, lenzAnime;
static ANIME fon0Anime, fon1Anime, log0Anime, log1Anime, log2Anime, log3Anime;
static const short optionMenuTables[] = {
	320, 86, 320, 104,
	320, 130, 320, 150,
	320, 294, 320, 312,
	320, 336, 320, 354,
	320, 377, -1, -1,
	320, 401, -1, -1,
};
static const short optionTypeTables[] = {
	181, 169, 462, 169,
	157, 244, 484, 244
};
static const char optionMenuIndex[] = {
	13, 13, 14, 14,
	14, 14, 13, 13,
	14, 14, 13, 14,

	6, 7, 6, 7,
	8, 9, 10, 8,
	11, 12, 11, 12,
	11, 12, 11, 12
};
static const char *optionMenuTextTables[] = {
	"PLAYERS",
	"CONTROLLER",
	"INFORMATION ARROW",
	"BACKGROUND MUSIC",
	"COLOR BARS",
	"EXIT",
/* 6 */	"ONE PLAYER",
/* 7 */	"TWO PLAYERS",
/* 8 */	"TYPE A",
/* 9 */	"TYPE B",
/*10 */	"TYPE C",
/*11 */	"OFF",
/*12 */	"ON",
/*13 */	"NUDGE",
/*14 */	"FLIPPER"
};
static const char optionFontSizeTables[] = {
	11, 9, 11, 12, 7, 7, 11,
	12, 5, 5, 11, 8, 14, 12,
	12, 8, 12, 10, 6, 10, 11,
	11, 16, 11, 11, 9,
	8, 6, 8, 7, 9, 7, 8, 8, 8, 8
};
static const char *stringTables[] = {
	"Arkham",
	"Cult of the Bloody Tangue",
	"Dreamlands",
	"Exit"
};
static char	optionMenuPos;
static const char *resourceTables[] = { "SNDA", "SNDB", "SNDD" };
static const char *soundNameTables[] = {
/* 0 */	"Flipper",
/* 1 */	"Slingshot",
/* 2 */	"Ball Hit",
/* 3 */	"Ball Load",
/* 4 */	"Ball Lock",
/* 5 */	"Ball Eject",
/* 6 */	"Plunger Shot",
/* 7 */	"Vertical Up Kicker",
/* 8 */	"Jet Bumper",
/* 9 */	"Super Jet Bumper",
/*10 */	"Jet Bumper Ambient",
/*11 */	"Sacrificial Drums",
/*12 */	"Psycho Save",
/*13 */	"Time Back",
/*14 */	"Top Lane",
/*15 */	"Spot Target",
/*16 */	"Drop Target",
/*17 */	"Drop Target Reset",
/*18 */	"Deathwatch Award",
/*19 */	"MAZE Award",
/*20 */	"Kickback",
/*21 */	"Kickback Comeback",
/*22 */	"Round Jackpot",
/*23 */	"Soul Award",
/*24 */	"Extraball Lit",
/*25 */	"Extraball Award",
/*26 */	"Out Lane",
/*27 */	"Flipper Down-sound",
/*28 */	"Jackpot!!",
/*29 */	"Million Grail",
/*30 */	"None",
/*31 */	"None",

/* Arkham */
/* 0 */	"Ramp",
/* 1 */	"None",
/* 2 */	"Round Start",
/* 3 */	"Lane",
/* 4 */	"None",
/* 5 */	"Witchcraft Lit",
/* 6 */	"Witchcraft Award",
/* 7 */	"Bumper Plus",
/* 8 */	"Slingshot Plus",
/* 9 */	"None",
/*10 */	"None",
/*11 */	"Normal Jackpot",
/*12 */	"Super Jackpot",
/*13 */	"Spirits Jackpot",
/*14 */	"Dark Knowledge Jackpot",
/*15 */	"Gate Open",
/*16 */	"None",
/*17 */	"None",
/*18 */	"None",
/*19 */	"None",
/*20 */	"Asylum Award",
/*21 */	"Cemetery Yes Award",
/*22 */	"Cemetery No Award",
/*23 */	"Deathwalker Award",
/*24 */	"Creation Award",
/*25 */	"Creation Restart",
/*26 */	"None",
/*27 */	"None",
/*28 */	"None",
/*29 */	"None",
/*30 */	"None",
/*31 */	"None",

/* Cult of the Bloody Tangue */
/* 0 */	"Ramp",
/* 1 */	"None",
/* 2 */	"Round Start",
/* 3 */	"Lane",
/* 4 */	"None",
/* 5 */	"Ancient Lit",
/* 6 */	"Ancient Award",
/* 7 */	"Bumper Plus",
/* 8 */	"Slingshot Plus",
/* 9 */	"None",
/*10 */	"None",
/*11 */	"Normal Jackpot",
/*12 */	"Super Jackpot",
/*13 */	"Power Jackpot",
/*14 */	"Machine Awaken Jackpot",
/*15 */	"Full Charge",
/*16 */	"None",
/*17 */	"None",
/*18 */	"None",
/*19 */	"None",
/*20 */	"Machine Lunar Award",
/*21 */	"Machine Solar Award",
/*22 */	"Eclipse Award",
/*23 */	"Dark Angels Award",
/*24 */	"Destruction Award",
/*25 */	"Destruction Add Award",
/*26 */	"None",
/*27 */	"None",
/*28 */	"None",
/*29 */	"None",
/*30 */	"None",
/*31 */	"None",

/* Dreamlands */
/* 0 */	"Ramp",
/* 1 */	"None",
/* 2 */	"Round Start",
/* 3 */	"Lane",
/* 4 */	"None",
/* 5 */	"Dreamer Lit",
/* 6 */	"Dreamer Award",
/* 7 */	"Bumper Plus",
/* 8 */	"Slingshot Puls",
/* 9 */	"None",
/*10 */	"None",
/*11 */	"Normal Jackpot",
/*12 */	"Nightmare Jackpot",
/*13 */	"Nightmare Award",
/*14 */	"Nightmare Lock Ready",
/*15 */	"Nightmare Locked",
/*16 */	"None",
/*17 */	"None",
/*18 */	"None",
/*19 */	"None",
/*20 */	"Moonbeast Award",
/*21 */	"Silver Gate Award",
/*22 */	"Enchanted Wood Award",
/*23 */	"NECRONOMICON Award",
/*24 */	"NECRONOMICON Add Award",
/*25 */	"None",
/*26 */	"Firespitter",
/*27 */	"Magna Lock",
/*28 */	"None",
/*29 */	"None",
/*30 */	"None",
/*31 */	"None"
};
static const char scoreTest[] = { 0,1,2,3,4,5,6,7,8,9 };
static const char *manTopicTables[] = {
	"TAK0",	"TAK1",	"TAK2",	"TAK3",
	"TBD0",	"TBD1",	"TBD2",	"TBD3",
	"TDR0",	"TDR1",	"TDR2",	"TDR3"
};
static char manPlayEnable;
static char manPageMax[4 * 3];
static char manPagePos[4 * 3];
static short manMode, manTopicPos, manFieldPos;
static unsigned short brightTimer;
static const char font0SizeTables[] = {
	11, 8, 10, 10, 8, 8, 11,	/* ABCDEFG */
	10, 4, 5, 9, 8, 11, 10,		/* HIJKLMN */
	12, 8, 11, 9, 8, 11, 10,	/* OPQRSTU */
	10, 16, 11, 10, 10, 7, 8,	/* VWXYZab */
	7, 8, 8, 5, 8, 7, 4, 3,		/* cdefghij */
	7, 3, 13, 8, 8, 8, 7, 6,	/* klmnopqr */
	6, 6, 8, 7, 11, 8, 7, 7,	/* stuvwxyz */
	3, 3, 3, 2
};
static const char font0KernTables[] = {
	-1, -2, -2, -1, 2, 1, 0, -2, -1,
	-1, 0, 0, 0, -1, 0, -1, -2, -2
};
static const char font1SizeTables[] = {
	15, 11, 15, 17, 10, 10, 15,
	15, 6, 7, 12, 11, 19, 16,
	17, 11, 18, 12, 9, 14, 15,
	16, 21, 15, 14, 13,
	15, 11, 15, 17, 10, 10, 15,
	15, 6, 7, 12, 11, 19, 16,
	17, 11, 18, 12, 9, 14, 15,
	16, 21, 15, 14, 13,
	5, 5, 5, 5
};
static const char font1KernTables[] = {
	-4, -5, -3, -3, -1, -2, -1, -1, -1, -1, 
	-3, -3, -3, -3, -3, -5, -3, -3
};
static const char alphaTables[] = {
	'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K',
	'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T',
	'V', 'W', 'X', 'Y', 'Z', '0', '1', '2',
	'3', '4', '5', '6', '7', '8', '9', '?',
	'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
	'l', 'm', 'n', 'p', 'q', 'r', 's', 't',
	'v', 'w', 'x', 'y', 'z', '!', '#', '-',
	'+', '=', '*', '/', '&', '%', '$', '@'
};
static char *staffRollText;
static int staffRollPos, staffRollDiff, staffRollTime, staffRollHeight;
static short staffDelay, staffInten;

/*----------------------------------------------/
/						/
/	Sound Tester				/
/						/
/----------------------------------------------*/
void EntrySoundTester(void)
{
	int	i, j, k, l;
	MOBJ	*mobj;

	PlayCD(-1, -1);
	LaunchResource("SOUND.RES");

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	InstallMotionObject("FONT", &fontAnime);
	clutTransMask = 1;

	for( ; ; ) {
	l = 0;
	while((padOnlyTriggerP[0] & SATURN_PAD_START)==0) {
		SyncBlanking();
		OpenMotionObject(0, 0);

		MobjStringC(320, 100, "CHOOSE TABLE");

		if(padOnlyTriggerP[0] & SATURN_DIR_UP) l++;
		if(padOnlyTriggerP[0] & SATURN_DIR_DOWN) l--;
		if(l < 0) l = 0;
		if(l > 3) l = 3;
		MobjStringC(320, 300, stringTables[l]);
		CloseMotionObject();
		}

	if(l==3) {
		KillTask(EntrySoundTester);
		CreateTask(EntryMainMenu);
		LaunchResource("MENU.RES");
		LaunchSound("SNDG", "SNDM");
		return;
		}

	LaunchSound("SNDG", resourceTables[l]);
	SetupSoundGroup(l);

	k = 0;
	while((padOnlyTriggerP[0] & SATURN_PAD_START)==0) {
		SyncBlanking();
		OpenMotionObject(0, 0);

		MobjStringC(320, 100, "0 to 31:GENERAL, 32 to 63:SPECIAL");
		MobjStringC(320, 150, stringTables[l]);

		if(padOnlyTriggerP[0] & SATURN_DIR_UP) k++;
		if(padOnlyTriggerP[0] & SATURN_DIR_DOWN) k--;
		if(k < 0) k = 0;
		if(k > 63) k = 63;
		if(padOnlyTriggerP[0] & SATURN_PAD_ABCXYZ) {
///			RingSfx(k, 0);
			RingSfx(k);
		}
		if(k >= 32) i = l * 32;
		else i = 0;
		MobjStringC(320, 200, soundNameTables[k + i]);
		MobjNumber(320, 300, k);

		CloseMotionObject();
		}
	SyncBlanking();
	}
}

/*----------------------------------------------/
/						/
/	Infomation Tester			/
/						/
/----------------------------------------------*/
void EntryInfoTester(void)
{
	int	i, j, k, l, m;
	MOBJ	*mobj;

	for( ; ; ) {

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	InstallMotionObject("FONT", &fontAnime);
	clutTransMask = 1;

	gamePauseEnable = 1;

	l = 0;
	while((padOnlyTriggerP[0] & SATURN_PAD_START)==0) {
		SyncBlanking();
		OpenMotionObject(0, 0);

		MobjStringC(320, 100, "CHOOSE TABLE");

		if(padOnlyTriggerP[0] & SATURN_DIR_UP) l++;
		if(padOnlyTriggerP[0] & SATURN_DIR_DOWN) l--;
		if(l < 0) l = 0;
		if(l > 3) l = 3;
		MobjStringC(320, 300, stringTables[l]);
		CloseMotionObject();
		}

	if(l==3) {
		KillTask(EntryInfoTester);
		CreateTask(EntryMainMenu);
		return;
		}

	if(l==0) LaunchResource("ARKHAM.RES");
	else if(l==1) LaunchResource("BLOODY.RES");
	else if(l==2) LaunchResource("DLANDS.RES");

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	InstallMotionObject("BALL", &ballsAnime);
	InstallMotionObject("PR16", &infoAnime);
	InstallMotionObject("PLFO", &ifonAnime);
	clutTransMask = 1;

	Install16ColorObject(&info2Anime);

	InstallSlaveFunction(ServiceSlave);
	InitInfomations(l);

	CreateInfoScore(scoreTest);
	k = m = 0;
	while((padOnlyTriggerP[0] & SATURN_PAD_START)==0) {
	SyncBlanking();
	OpenMotionObject(0, 0);

	if(padOnlyTriggerP[0] & SATURN_DIR_UP) k++;
	if(padOnlyTriggerP[0] & SATURN_DIR_DOWN) k--;
	if(k < 0) k = 0;
	if(k > infoAnimeMax) k = infoAnimeMax;
	if(padOnlyTriggerP[0] & SATURN_PAD_A) SetInfomations(k, 0);
	MobjDecimal(320, 360, k);

	ServiceInfomations();

	CloseMotionObject();
	m++;
	infoNumber = m / tvFrame;
	if(m >= (tvFrame * 99)) m = 0;
		}
	LaunchResource("MENU.RES");
	}
}

/*----------------------------------------------/
/						/
/	Options					/
/						/
/----------------------------------------------*/
void InitNECRONOMICON(void)
{
	int	i, j, k;
	char	base[12];
	const char *nam;

	InitConstScore();
	optionMenuPad[OPTION_NO_PLAYER] = 0;
	optionMenuPad[OPTION_NO_CONTROLLER] = 0;
	optionMenuPad[OPTION_NO_ARROW] = 1;
	optionMenuPad[OPTION_NO_BGM] = 1;
	optionMenuPad[OPTION_NO_REALMS_COMP] = 0;

	for(j=0; j<3; ++j) {
	PresetScore(base, scoreConst[SCORE_ARK_BASE + j + j]);
	nam = initialScoreNameTables;
	for(i=0; i<8; ++i) {
		PresetScore(&necroPad[j].topScore[i * 12], base);
		AddScore(base, scoreConst[SCORE_ARK_BASE + j + j + 1]);
		for(k=0; k<3; ++k) necroPad[j].topName[i * 3 + k] = *nam++;
		}
	}
}

void EntryOptions(void)
{
	int	i;

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	ExtractBitImage("OPTO", 0, 0);
	InstallMotionObject("OPTF", &fontAnime);
	clutTransMask = 1;

	for(i=0; i<16; ++i) clutSave[i] = clutBuff[256 + i];

	optionMenuPos = 0;
	brightTimer = 0;

	KillTask(EntryOptions);
	CreateTask(MainOptions);
}

void MainOptions(void)
{
	int	f;

	manLevel = padLevel[0] | padLevel[1];
	manTrigger = padTriggerP[0] | padTriggerP[1];

	ServiceOptions();
	f = manTrigger & SATURN_PAD_START; 
	f |= manTrigger & SATURN_PAD_ABCXYZ;
	if(f) {
	if((manLevel & SATURN_PAD_ABC)==SATURN_PAD_ABC) {
		KillTask(MainOptions);
		CreateTask(EntryMainMenu);
		}
	else if(optionMenuPos>=4) {
		KillTask(MainOptions);
		if(optionMenuPos==4) {
			RingSfx(SFX_TIME_BACK);
			CreateTask(EntryColorBars);
			}
		else 	{
			RingSfx(SFX_DEATHWATCH_AWARD);
			CreateTask(EntryMainMenu);
			ToBlackScreen();
			ResetMotionObject();
			ResetBackground();
			OpenMotionObject(0, 0);
			CloseMotionObject();
			WriteBackupSystem();
			}
		}
	}
}

static void ServiceOptions(void)
{
	int	i, j, k, l, m, n;

	i = optionMenuPos;
	if(manTrigger & SATURN_DIR_UP) optionMenuPos--;
	if(manTrigger & SATURN_DIR_DOWN) optionMenuPos++;
	if(i!=optionMenuPos) RingSfx(SFX_PSYCHO_SAVE);

	if(optionMenuPos < 0) optionMenuPos = 0;
	if(optionMenuPos > 5) optionMenuPos = 5;
	j = SATURN_PAD_ABCXYZ | SATURN_DIR_RIGHT;
	j |= SATURN_DIR_LEFT;
	if(manTrigger & j) {
	if(optionMenuPos < 4) RingSfx(SFX_FLIPPER);
	if(optionMenuPos==1) {
		optionMenuPad[optionMenuPos]++;
		if(optionMenuPad[optionMenuPos] > 2)
			optionMenuPad[optionMenuPos] = 0;
		}
	else optionMenuPad[optionMenuPos] ^= 1;
	}

	OpenMotionObject(0, 0);

	for(i=0; i<6; ++i) {
	j = optionMenuTables[i * 4];
	k = optionMenuTables[i * 4 + 1];
	if(i==optionMenuPos) l = 16; else l = 0;
	MobjOptionsText(j, k - 12, optionMenuTextTables[i], l);
	if(i < 4) {
		j = optionMenuTables[i * 4 + 2];
		k = optionMenuTables[i * 4 + 3];
		m = optionMenuIndex[12 + optionMenuPad[i] + i * 4];
		MobjOptionsText(j, k - 12, optionMenuTextTables[m], 0);
		}
	if(i==1) {
		for(m=0; m<4; ++m) {
		j = optionTypeTables[m + m];
		k = optionTypeTables[m + m + 1];
		n = optionMenuIndex[optionMenuPad[1] * 4 + m];
		MobjOptionsText(j, k - 12, optionMenuTextTables[n], 0);
			}
		}
	}
	CloseMotionObject();

	ServiceBrightClut(256 + 16, 16);
}

static void MobjOptionsText(int x, int y, const char *t, int p)
{
	int	s, m, n, c;
	MOBJ	*mobj;

	s = TakeOptionTextSize(t);
	x -= s >> 1;
	n = *t;
	while((m = *t++)!=0) {
	if(m >= 'a' && m <= 'z') m -= 0x20;
	if(m >= '0' && m <= '9') c = m - '0' + 26;
	else if(m >= 'A' && m <= 'Z') c = m - 'A';
	else if(m == 0x20) c = 26 + 10;

	if(n=='A' && m=='Y') x -= 3;
	else if(n=='A' && m=='T') x -= 3;
	else if(n=='R' && m=='O') x -= 1;
	else if(n=='K' && m=='G') x -= 2;

	if(c < 36) {
		mobj = fontAnime.mobj + c;
		mobj->x = x;
		mobj->y = y;
		mobj->color = p;
		SetupObject(mobj);
		x += optionFontSizeTables[c];
		}
	else x += 3;
	x += 1;
	n = m;
	}
}

static int TakeOptionTextSize(const char *t)
{
	int	m, n, s;

	s = 0;
	n = *t;
	while((m = *t++)!=0) {
	if(m >= 'a' && m <= 'z') m -= 0x20;
	if(m >= '0' && m <= '9') s += optionFontSizeTables[m - '0' + 26];
	else if(m >= 'A' && m <= 'Z') s += optionFontSizeTables[m - 'A'];
	else if(m == 0x20) s += 6;
	if(n=='A' && m=='Y') s -= 3;
	else if(n=='A' && m=='T') s -= 3;
	else if(n=='R' && m=='O') s -= 1;
	else if(n=='K' && m=='G') s -= 2;
	n = m;
	}
	return s;
}

static void EntryColorBars(void)
{
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	ExtractBitImage("COLR", 0, 0);
	clutTransMask = 1;

	manTrigger = 0;
	while((manTrigger & SATURN_PAD_START)==0) {
	manTrigger = padTriggerP[0] | padTriggerP[1];
	SyncBlanking();
	if(manTrigger & SATURN_PAD_ABCXYZ) break;
	}
	RingSfx(SFX_OUT_LANE);
	KillTask(EntryColorBars);
	CreateTask(EntryOptions);
}

void ServiceBrightClut(int top, int cnt)
{
	int	i;

	for(i=0; i<cnt; ++i) {
	clutBuff[top + i] = clutSave[i];
	}
	if(brightTimer < 256) i = brightTimer ^ 255;
	else i = brightTimer;
	BrightColorTables(top, cnt, i & 255);
	brightTimer += tvFrame / 5;
	brightTimer &= 511;
}

/*----------------------------------------------/
/						/
/	Instruction Manual			/
/						/
/----------------------------------------------*/
void EntryManual(void)
{
	int	i, j, k, l;
	MOBJ	*mobj;

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

	LaunchResource("MANUAL.RES");

	ExtractBitImage("BOOK", 0, 0);

	clutIdxMOBJ = MAN_COLOR_MAX + 256;
	InstallMotionObject("LUPE", &lupeAnime);
	for(i=0; i<MAN_COLOR_MAX; ++i) clutBuff[i + 256] = clutBuff[i];
	for(i=0; i<16; ++i) clutSave[i] = clutBuff[128 + i];
	clutBuff[225] = 0;
	clutBuff[225+256] = 0;

	clutTransBit |= 0xffff0000;
	SetCLUT(0, 0, 0, 0);
	clutTransMask = 1;

	InstallLenzObject(&lenzAnime);

	EtcCopyImage(0, 0, 640, 448, 0);

	for(i=0; i<12; ++i) {
	j = TakeResourcePage(manTopicTables[i]);
	manPageMax[i] = j / 3;
	if(manPageMax[i]) manPageMax[i]--;
	manPagePos[i] = 0;
	}

	manMode = 0;
	mouseAcl = 0;

	manTopicPos = -1;
	manFieldPos = -1;
	manPlayEnable = -1;

	manTimer = 0;

	KillTask(EntryManual);
	CreateTask(MainManual);
}

void MainManual(void)
{
	manLevel = padLevel[0] | padLevel[1];
	manTrigger = padTriggerP[0] | padTriggerP[1];

	manTimer++;
	if(manLevel) manTimer = 0;
	if(manTimer >= (30 * tvFrame) && flashSegaEnable!=0) SYS_Exit(0);

	ServiceManual();
	if(manTrigger & SATURN_PAD_START) {
	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();
	LaunchResource("MENU.RES");
	KillTask(MainManual);
	CreateTask(EntryMainMenu);
	}
}

void InitPlayingManual(int no)
{
	int	i, j, k, l, x;
	uint16_t *dst;
	MOBJ	*mobj;

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

	ExtractBitImage("BOOK", 0, 0);

	for(i=0; i<16; ++i) clutBuff[256+240+i] = clutLupe[i];
	for(i=0; i<MAN_COLOR_MAX; ++i) clutBuff[i + 256] = clutBuff[i];
	for(i=0; i<16; ++i) clutSave[i] = clutBuff[128 + i];
	clutBuff[225] = 0;
	clutBuff[225+256] = 0;

	clutTransBit |= 0xffff0000;
	SetCLUT(0, 0, 0, 0);
	clutTransMask = 1;

/* Emulation  LENZ objects */
	j = CIRCLE_MAX * CIRCLE_MAX;
	for(i=0; i<CIRCLE_MAX; ++i) {
	circleTables[i] = Isqrt(j - (CIRCLE_MAX - i) * (CIRCLE_MAX - i));
	}
	lenzAnime = info2Anime;
	mobj = lenzAnime.mobj;
	mobj->color = 0;
	mobj->mode = VDP1_MODE_BANK256;
	x = LENZ_X_SIZE;
	if(x & 7) i = 1; else i = 0;
	x >>= 3;
	x += i;
	mobj->chasiz = (x << 8) + (LENZ_Y_SIZE & 0xff);
	dst = (uint16_t *)get_VDP1_VRAM_TOP();
	dst += ((uint16_t)mobj->chaptr) << 2;
	for(i=0; i<((LENZ_X_SIZE * LENZ_Y_SIZE) >> 1); ++i) *dst++ = 0;

	EtcCopyImage(0, 0, 640, 448, 0);

	for(i=0; i<12; ++i) {
	if(no==(i / 4)) j = TakeResourcePage(manTopicTables[i]);
	else j = 0;
	manPageMax[i] = j / 3;
	if(manPageMax[i]) manPageMax[i]--;
	manPagePos[i] = 0;
	}

	manMode = 0;
	mouseAcl = 0;

	manTopicPos = -1;
	manFieldPos = -1;
	manPlayEnable = no;

	manTimer = 0;
}

void MainPlayingManual(void)
{
	int	i, f, x;
	MOBJ	*mobj;

	f = 1;
	while(f) {
	SyncBlanking();

	if(padStatus[playerNo] & 5) i = playerNo; else i = 0; 
	manLevel = padLevel[i];
	manTrigger = padTriggerP[i];

	ServiceManual();
	if(manTrigger & SATURN_PAD_START) f = 0;
	}

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

	mobj = info2Anime.mobj;
	mobj->color = 0;
	mobj->mode = VDP1_MODE_BANK16;
	x = 256;
	if(x & 7) i = 1; else i = 0;
	x >>= 3;
	x += i;
	mobj->chasiz = (x << 8) + (96 & 0xff);

	ExtractBitImage("PASE", 0, 0);
	clutBuff[0] = 0;
	for(i=0; i<256; ++i) {
	clutSave[i] = clutBuff[i];
	SetCLUTi(i+256, clutBuff[i]);
	}

	TurnIntoDark(80);

	clutTransMask = 1;
}

static void ServiceManual(void)
{
	int	i, j, k, m, n, a, b, c;
	MOBJ	*mobj;
	const uint8_t *txt;
	const uint8_t *l;

	OpenMotionObject(0, 0);

	if(manFieldPos < 0) { a = -1;
		if(manPlayEnable >= 0) manFieldPos = manPlayEnable;
		else manFieldPos = 0;
		}
	else a = manFieldPos;

	if(manTopicPos < 0) { b = -1; manTopicPos = 0; }
	else b = manTopicPos;
	c = 0;

	if(manTopicPos > 0 && manTopicPos < 5) {
	i = manFieldPos * 4 + (manTopicPos - 1);
	c = manPagePos[i];
	}

	switch(manMode)
	{
	case 0:
		m = SATURN_PAD_RIGHT | SATURN_DIR_RIGHT;
		n = SATURN_PAD_LEFT  | SATURN_DIR_LEFT;

		if(manTrigger & (SATURN_PAD_B | SATURN_PAD_Y)) manMode = 1;

		j = manTopicPos;
		if(manTrigger & SATURN_DIR_UP) manTopicPos--;
		else if(manTrigger & SATURN_DIR_DOWN) manTopicPos++;
		if(manTopicPos < 0) manTopicPos = 0;
		else if(manTopicPos > 5) manTopicPos = 5;
		if(j!=manTopicPos && j >= 0) {
		j <<= 3;
		j += 128;
		for(i=0; i<8; ++i) {
			SetCLUTi(i+j, clutSave[i]);
			SetCLUTi(i+j+256, clutSave[i]);
			}
		}
		break;
	case 1:
		m = SATURN_PAD_RIGHT;
		n = SATURN_PAD_LEFT;

		if(manTrigger & (SATURN_PAD_B | SATURN_PAD_Y)) {
			manMode = 0;
			}
		if(mouseEna) {
			lenzPosX += mouseDx;
			lenzPosY -= mouseDy;
			}
		if(manLevel & SATURN_DIR_ALL) mouseAcl++;
		else mouseAcl = 0;
		if(mouseAcl > 64) mouseAcl = 64;
		i = mouseAcl / 8;
		i++;
		if(manLevel & SATURN_DIR_RIGHT) lenzPosX += i;
		else if(manLevel & SATURN_DIR_LEFT) lenzPosX -= i;
		if(manLevel & SATURN_DIR_DOWN) lenzPosY += i;
		else if(manLevel & SATURN_DIR_UP) lenzPosY -= i;

		if(lenzPosX < 16) lenzPosX = 16;
		else if(lenzPosX > (640-16)) lenzPosX = 640-16;
		j = 448;
		if(lenzPosY < 20) lenzPosY = 20;
		else if(lenzPosY > (j - 24)) lenzPosY = j - 24;

		mobj = lenzAnime.mobj;
		mobj->x = lenzPosX - LENZ_X_SIZE;
		mobj->y = lenzPosY - LENZ_Y_SIZE;
		mobj->color = 0x0000;
		RefreshLenzImage(mobj, lenzPosX, lenzPosY);
		SetupObjectBox(mobj->x, mobj->y, mobj->x + LENZ_X_SIZE*2, mobj->y + LENZ_Y_SIZE*2, 1);
		SetupResizeObject(mobj, LENZ_X_SIZE * 2 - 1, LENZ_Y_SIZE * 2 - 1);
		EraseRing(lenzPosX, lenzPosY);
		mobj = lupeAnime.mobj;
		mobj->x = lenzPosX - 64;
		mobj->y = lenzPosY - 64;
		SetupObject(mobj);
		break;
	default:
		break;
	}

	j = manTopicPos * 8 + 128;
	ServiceBrightClut(j, 8);
	for(i=0; i<8; ++i) SetCLUTi(j+i+256, clutBuff[j+i]);

	if(manTopicPos==0) {
	if(trialEnable) manFieldPos = 0;
	else if(manPlayEnable >= 0) manFieldPos = manPlayEnable;
	else	{
		if(manTrigger & m) manFieldPos++;
		else if(manTrigger & n) manFieldPos--;
		if(geEnable) i = 1; else i = 2;
		if(manFieldPos < 0) manFieldPos = 0;
		else if(manFieldPos > i) manFieldPos = i;
		}

	if(a!=manFieldPos || b!=manTopicPos) {
		txt = GetResource("BASE", manFieldPos * 5 + 0);
		ExtractBitImageOfs(txt, 130, 16, 176, -1);
		txt = GetResource("BASE", manFieldPos * 5 + 1);
		ExtractBitImageOfs(txt, 176, 104, 0, -1);
		EtcCopyImage(169, 92, 216, 301, 0);
		txt = GetResource("BASE", manFieldPos * 5 + 2);
		RefreshImage(400, 30, 200, 16, 0);
		ExtractBitImageOfs(txt, 400, 30, 192, 15);
		txt = GetResource("BASE", manFieldPos * 5 + 3);
		ExtractBitImageOfs(txt, 199, 110, 208, 0);
		l = GetResource("BASE", manFieldPos * 5 + 4);
		ExtractBits(395, 99, l);

		RefreshImage(145, 413, 69, 37, 0);
		RefreshImage(554, 413, 69, 37, 0);
		}
	}
	else if(manTopicPos==5) {
/*	if(manTrigger & m) manFieldPos++;
	else if(manTrigger & n) manFieldPos--;
	if(manFieldPos < 0) manFieldPos = 0;
	else if(manFieldPos > 2) manFieldPos = 2;
	if(trialEnable) manFieldPos = 0;
*/
	if(a!=manFieldPos || b!=manTopicPos) {
		txt = GetResource("BOOK", 1);
		ExtractBitImageOfs(txt, 0, 0, 0, -1);
		txt = GetResource("BASE", manFieldPos * 5 + 0);
		ExtractBitImageOfs(txt, 130, 16, 176, -1);
		ExtractBitImageOfs(GetResource("TOPS", 0), 400, 30, 192, 15);
		ServiceTopExploer();
		}
	}
	else {
	if(trialEnable) manFieldPos = 0;
	else if(manPlayEnable >= 0) manFieldPos = manPlayEnable;
	i = manFieldPos * 4 + (manTopicPos - 1);
	if(b!=manTopicPos && manTopicPos==1) {
		txt = GetResource("BASE", manFieldPos * 5 + 1);
		ExtractBitImageOfs(txt, 176, 104, 0, -1);
		EtcCopyImage(169, 92, 216, 301, 0);
		}

	if(manTrigger & m) manPagePos[i]++;
	else if(manTrigger & n) manPagePos[i]--;
	if(manPagePos[i] < 0) manPagePos[i] = 0;
	else if(manPagePos[i] > manPageMax[i]) manPagePos[i] = manPageMax[i];

	if(a!=manFieldPos || b!=manTopicPos || c!=manPagePos[i]) {
		RefreshImage(400, 30, 200, 16, 0);
		ExtractBitImageOfs(GetResource(manTopicTables[i], manPagePos[i] * 3),
			400, 30, 192, 15);
		RefreshImage(169, 92, 216, 301, 0);
		l = GetResource((const char *)txt, manPagePos[i] * 3 + 2);
		ExtractBitImageOfs(l, 199, 110, 208, 0);
		l = GetResource((const char *)txt, manPagePos[i] * 3 + 1);
		ExtractBits(395, 99, l);

	if(manPagePos[i])
		ExtractBitImageOfs(GetResource("ARRW", 0), 145, 413, 176, 0);
	else	RefreshImage(145, 413, 69, 37, 0);
	if(manPagePos[i] < manPageMax[i])
		ExtractBitImageOfs(GetResource("ARRW", 1), 554, 413, 176, 0);
	else	RefreshImage(554, 413, 69, 37, 0);

		}
	}

	CloseMotionObject();
}

static void InstallLenzObject(ANIME *anm)
{
	MOBJ	*mobj;
	uint16_t *dst;
	int	i, j;

	j = CIRCLE_MAX * CIRCLE_MAX;
	for(i=0; i<CIRCLE_MAX; ++i) {
	circleTables[i] = Isqrt(j - (CIRCLE_MAX - i) * (CIRCLE_MAX - i));
	}

	anm->x = LENZ_X_SIZE;
	anm->y = LENZ_Y_SIZE;
	anm->count = 8;		/* 8 bits per pixel */
	InstallMotionObject(NULL, anm);

	mobj = anm->mobj;
	dst = (uint16_t *)get_VDP1_VRAM_TOP();
	dst += ((uint16_t)mobj->chaptr) << 2;
	for(i=0; i<((LENZ_X_SIZE * LENZ_Y_SIZE) >> 1); ++i) *dst++ = 0;
}

static void RefreshLenzImage(MOBJ *mobj, int x, int y)
{
	uint16_t *dst;
	uint8_t	*ptr, *wak;
	int	i, j, k, m, n, xc, yc;

	dst = (uint16_t *)get_VDP1_VRAM_TOP();
	dst += ((uint16_t)mobj->chaptr) << 2;

	k = 448;
	xc = x;
	yc = y;
	x -= LENZ_X_SIZE >> 1;
	y -= LENZ_Y_SIZE >> 1;
	m = y + LENZ_Y_SIZE;
	n = x + LENZ_X_SIZE;
	for(j=y; j<m; ++j)
	{
	if(j >= 0 && j < k)
		{
		ptr = get_VDP2_VRAM_TOP();
		ptr += j << 10;
		wak = (unsigned char *)&lineBuff[0];
		for(i=x; i<n; ++i)
			{
			if(i >= 0 && i < 640) *wak++ = *(ptr + i);
			else *wak++ = 0;
			}
		for(i=0; i<(LENZ_X_SIZE / 2); ++i) *dst++ = lineBuff[i];
		}
	else dst += (LENZ_X_SIZE >> 1);
	}
}

static void EraseRing(int x, int y)
{
	int	i;

	for(i=0; i<CIRCLE_MAX; ++i)
	{
	SetupObjectLine(x-CIRCLE_MAX, y-CIRCLE_MAX+i, x-circleTables[i], y-CIRCLE_MAX+i, 0);
	SetupObjectLine(x-CIRCLE_MAX, y+CIRCLE_MAX-i-1, x-circleTables[i], y+CIRCLE_MAX-i-1, 0);
	SetupObjectLine(x+circleTables[i], y-CIRCLE_MAX+i, x+CIRCLE_MAX, y-CIRCLE_MAX+i, 0);
	SetupObjectLine(x+circleTables[i], y+CIRCLE_MAX-i-1, x+CIRCLE_MAX, y+CIRCLE_MAX-i-1, 0);
	}
}

static uint32_t GetInt_(const uint8_t *src)
{
	uint32_t v;

	v  = (uint32_t)src[0] << 24;
	v |= (uint32_t)src[1] << 16;
	v |= (uint32_t)src[2] << 8;
	v |= (uint32_t)src[3];
	return v;
}

static void ExtractBits(int x, int y, const uint8_t *in)
{
	int	i, j;
	uint32_t w, h;
	unsigned int	a, b, c, d;
	uint8_t *dst;
	const uint8_t *org, *src;

	dst = get_VDP2_VRAM_TOP();
	dst += x + y * 1024;
	org = get_MAN_MIRROR_TOP();
	org += x + y * 1024;

	h = w = GetInt_(in); in += 4;
	w >>= 16;
	h &= 0xffff;
	in += 8;

	src = in;

	c = GetInt_(src);	/* OP Code 1 */
	c &= 0xff;
	src += 4;

	b = 0;
	d = 0;
	for(j=0; j<h; ++j) {
	for(i=0; i<w; ++i) {
		if(b==0) {
			if(d==0) a = *src++;
			if(a==c) { a = *src++; d = *src++; }
			if(d) d--;
			b = 0x80;
			}
		if(a & b) { *dst++ = 127; org++; } else *dst++ = *org++;
		b >>= 1;
		}
	dst += 1024 - w;
	org += 1024 - w;
	}
}

static void EtcCopyImage(int x, int y, int w, int h, int ox)
{
	int	i, j;
	uint8_t	*src, *dst;

	src = get_VDP2_VRAM_TOP();
	dst = get_MAN_MIRROR_TOP();
	src += x + y * 1024;
	dst += ox + x + y * 1024;
	for(i=0; i<h; ++i) {
	for(j=0; j<w; ++j) *dst++ = *src++;
	dst += 1024 - w;
	src += 1024 - w;
	}
}

static void RefreshImage(int x, int y, int w, int h, int ox)
{
	int	i, j;
	uint8_t *src, *dst;

	src = get_MAN_MIRROR_TOP();
	dst = get_VDP2_VRAM_TOP();
	src += ox + x + y * 1024;
	dst += x + y * 1024;
	for(i=0; i<h; ++i) {
	for(j=0; j<w; ++j) *dst++ = *src++;
	dst += 1024 - w;
	src += 1024 - w;
	}
}

static void ServiceTopExploer(void)
{
	int	i, j, k, l;
	uint32_t a[2];
	const char *txt;
	char c, tmp[16];

	txt = &necroPad[manFieldPos].topScore[7 * 12];
	FreezeScoreCode(a, txt);
	for(j=0; j<4; ++j) {
		tmp[j]   = alphaTables[a[0] & 31];
		tmp[j+4] = alphaTables[a[1] & 31];
		a[0] >>= 5;
		a[1] >>= 5;
		}
	tmp[8] = 0;
	txt = fdex(tmp, manFieldPos);
	for(j=0; j<13; ++j) {
		c = *txt++;
		ManualText(topExpPosTables[j] + 210, 115, alphaTables[c]);
		}

	for(i=0; i<5; ++i) {
	k = 7 - i;
	ManualText(174,      115 + i * 52, '#');
	ManualText(174 + 13, 115 + i * 52, i + 0x31);
	txt = &necroPad[manFieldPos].topScore[k * 12];
	for(j=0; j<12; ++j) {
		c = *txt++;
		ManualText(topExpPosTables[(11-j)+1], 115 + i * 52, c + 0x30);
		}
	txt = &necroPad[manFieldPos].topName[k * 3];
	for(j=0; j<3; ++j) {
		c = *txt++;
		ManualText(topExpPosTables[j+1], 115 + i * 52 + 24, c);
		}
	}
	l = 0;
	for(j=0; j<8; ++j)
	for(k=0; k<8; ++k) {
	ManualText(topExpPosTables[0] + 210 + k * 23, 115 + 52 + j * 26,
							alphaTables[l]);
	l++;
	}
}

static void ManualText(int x, int y, int c)
{
	char a;
	const uint8_t *txt;

	if(c >= '0' && c <= '9') a = c - '0' + 26 + 26;
	else if(c >= 'A' && c <= 'Z') a = c - 'A';
	else if(c >= 'a' && c <= 'z') a = c - 'a' + 26;
	else if(c=='#') a = 26 + 26 + 10;
	else if(c=='!') a = 26 + 26 + 10 + 1;
	else if(c=='?') a = 26 + 26 + 10 + 2;
	else if(c=='-') a = 26 + 26 + 10 + 3;
	else if(c=='+') a = 26 + 26 + 10 + 4;
	else if(c=='=') a = 26 + 26 + 10 + 5;
	else if(c=='*') a = 26 + 26 + 10 + 6;
	else if(c=='/') a = 26 + 26 + 10 + 7;
	else if(c=='&') a = 26 + 26 + 10 + 8;
	else if(c=='%') a = 26 + 26 + 10 + 9;
	else if(c=='$') a = 26 + 26 + 10 + 10;
	else if(c=='@') a = 26 + 26 + 10 + 11;
	else a = 26 + 26 + 10 + 11;
	txt = GetResource("FONT", a);
	ExtractBitImageOfs(txt, x, y, 224, -1);
}

/*----------------------------------------------/
/						/
/	Staff Roll for NECRONOMICON		/
/						/
/----------------------------------------------*/
void EntryStaffRoll(void)
{
	int	v[2], a, i;
	char	*p;

	ToBlackScreen();
	ResetMotionObject();
	ResetBackground();
	OpenMotionObject(0, 0);
	CloseMotionObject();

	LaunchResource("STAFF.RES");

	LaunchSound("SNDG", "SNDM");
	SetupSoundGroup(0);

	InstallMotionObject("FON0", &fon0Anime);
	InstallMotionObject("FON1", &fon1Anime);
	InstallMotionObject("LOG0", &log0Anime);
	InstallMotionObject("LOG1", &log1Anime);
	InstallMotionObject("LOG2", &log2Anime);
	InstallMotionObject("LOG3", &log3Anime);

	ExtractBitImage("STF0", 0, 0);

	for(i=0; i<64; ++i) clutSave[i] = clutBuff[i];
	for(i=0; i<96; ++i) clutSave[i+64] = clutBuff[256+i];

	clutTransMask = 1;

	staffRollText = (char *)GetResource("TEXJ", 0);
	staffRollPos = 0;
	staffRollTime = (155 + 10) * tvFrame;
	a = 0;
	p = staffRollText;
	for( ; ; ) {
	ScanTextHeight(p, v);
	a += v[0];
	if(v[1] < 0) break;
	p += v[1];
	}
	staffRollHeight = a << 16;
	staffRollDiff = staffRollHeight / staffRollTime;

	staffDelay = 0;
	staffInten = 0;

	cdFadeLevel = 7;

	PlayCD(CDA_ARK_MULTI_READY, CDA_BLD_MULTI_READY);

	KillTask(EntryStaffRoll);
	CreateTask(MainStaffRoll);
}

static void MainStaffRoll(void)
{
	int	i, j, w, v[2];
	char	*ptr;

	OpenMotionObject(0, 0);
	PositioningText(staffRollPos >> 16, staffRollText, v);
	ptr = v[1] + staffRollText;
	i = v[0] - (staffRollPos >> 16);
	while(i < 448) {
/* SetupObjectLine(0, 200, 639, 200, 31);  */
	w = ScanTextWidth(ptr);
	w >>= 1;
	j = MobjStaffTexts(185 - w, i, ptr);

	ScanTextHeight(ptr, v);
	i += v[0];
	if((staffRollPos + (i << 16)) >= staffRollHeight) break;
	ptr += j;
	}
	CloseMotionObject();

	if(staffRollDebug) {
	if(mouseEna) {
		i = mouseDy;
		staffRollPos -= i << 16;
		}
	if(padLevel[0] & SATURN_PAD_ABC) staffRollPos += 0x10000;
	if(padLevel[0] & SATURN_PAD_XYZ) staffRollPos -= 0x10000;
	if(staffRollPos < 0) staffRollPos = 0;
	else if(staffRollPos > (staffRollHeight - (448 << 16)))
			staffRollPos = staffRollHeight - (448 << 16) - 1;
	}
	else {
	staffRollPos += staffRollDiff;
	i = padTriggerP[0] | padTriggerP[1];
	if(optionMenuPad[OPTION_NO_REALMS_COMP]!=0 && i!=0) {
		PlayCD(-1, -1);
		KillTask(MainStaffRoll);
		CreateTask(EntryTitle);
		}
	else if(staffDelay) {
		i = tvFrame >> 1;
		if((staffDelay % i)==0 && cdFadeLevel!=0) cdFadeLevel--;
		if(staffDelay & 1) BrightColorTables(0, 64, staffInten);
		else BrightColorTables(256, 96, staffInten);
		if(staffDelay < (4 * tvFrame) && staffInten!=0) staffInten--;
		staffDelay--;
		if(staffDelay==0) {
			optionMenuPad[OPTION_NO_REALMS_COMP] = 1;
			WriteBackupSystem();
			KillTask(MainStaffRoll);
			CreateTask(EntryTitle);
			}
		}
	else if((staffRollHeight - (448 << 16)) < staffRollPos && staffDelay==0) {
		staffRollDiff = 0;
		staffInten = 256;
		staffDelay = 8 * tvFrame;
		}
	}
}

static int ScanTextWidth(const char *txt)
{
	int	i, j, s, c, o;
	const char *ptr, *ker;

	ptr = font0SizeTables;
	ker = font0KernTables;
	j = 1;
	s = 0;
	o = 0;
	while((c = *txt++) != 0) {
	if(c==0x0d) return s;
	else if(c=='%') {
		c = *txt++;
		if(c=='.') return s;
		else if(c=='0') {
			ptr = font0SizeTables;
			ker = font0KernTables;
			j = 1;
			}
		else if(c=='1') {
			ptr = font1SizeTables;
			ker = font1KernTables;
			j = 2;
			}
		}
	else if(c >= 'A' && c <= 'z') {
		i = ConvertCode(c);
		s += ptr[i] + j + GetKernOffset(c, o, ker);
		}
	else if(c==0x20) {
		s += j;
		}
	o = c;
	}
	return s;
}

static void ScanTextHeight(const char *txt, int *val)
{
	int	c, f;
	const char *top;

	f = 16;
	val[0] = 0;
	top = txt;
	while((c = *txt++) != 0) {
	if(c==0x0d) {
		val[0] += f;
		val[1] = txt - top;
		return;
		}
	else if(c=='%') {
		c = *txt++;
		if(c=='.') {
			val[0] += f;
			val[1] = -1;
			return;
			}
		else if(c=='0') f = 16;
		else if(c=='1') f = 22;
		else if(c=='2') f = 50;
		else if(c=='3') f = 41;
		else if(c=='4') f = 44;
		else if(c=='5') f = 71;
		else if(c==0) {
			val[0] += f;
			val[1] = -1;
			return;
			}
		}
	}
	val[0] += f;
	val[1] = -1;
	return;
}

static int ConvertCode(int c)
{
	if(c >= 'A' && c <='Z') return c - 'A';
	else if(c >= 'a' && c <='z') return c - 'a' + 26;
	else if(c=='.') return 26 + 26;
	else if(c==',') return 26 + 26 + 1;
	else return 26 + 26 + 4;
}

static int MobjStaffTexts(int x, int y, const char *s)
{
	int	j, k, c, o, f;
	MOBJ	*mobj;
	const char *ptr, *top, *ker;

	f = 0;
	o = 0;
	top = s;
	ptr = font0SizeTables;
	ker = font0KernTables;
	k = 1;
	while((c = *s++)) {
	if(c >= 'A' && c <= 'z') {
		j = ConvertCode(c);
		if(f)	{
			mobj = fon1Anime.mobj + j;
			ptr = font1SizeTables;
			ker = font1KernTables;
			k = 2;
			}
		else 	{
			mobj = fon0Anime.mobj + j;
			ptr = font0SizeTables;
			ker = font0KernTables;
			k = 1;
			}
		x += GetKernOffset(c, o, ker);
		mobj->x = x;
		mobj->y = y;
		SetupObject(mobj);
		x += ptr[j] + k;
		}
	else if(c==0x20) {
		x += k + k;
		}
	else if(c=='%') {
		c = *s++;
		if(c=='0') f = 0;
		else if(c=='1') f = 1;
		else if(c=='2') {
			mobj = log2Anime.mobj;
			mobj->x = x - 125 / 2;
			mobj->y = y;
			SetupObject(mobj);
			}
		else if(c=='3') {
			mobj = log3Anime.mobj;
			mobj->x = x - 123 / 2;
			mobj->y = y;
			SetupObject(mobj);
			}
		else if(c=='4') {
			mobj = log1Anime.mobj;
			mobj->x = x - 141 / 2;
			mobj->y = y;
			SetupObject(mobj);
			}
		else if(c=='5') {
			mobj = log0Anime.mobj;
			mobj->x = x - 64 / 2;
			mobj->y = y;
			SetupObject(mobj);
			}
		}
	if(c==0x0d) break;
	o = c;
	}
	return s - top;
}

static void PositioningText(int t, const char *s, int *tt)
{
	int	a, b, v[2];

	b = a = 0;
	v[1] = v[0] = 0;
	while(a <= t) {
	tt[0] = a;
	tt[1] = b;
	ScanTextHeight(s, v);
	if(v[1] < 0) break;
	a += v[0];
	b += v[1];
	s += v[1];
	}
	return;
}

static int GetKernOffset(int new, int old, const char *kern)
{
	if(old=='A' && new=='T') return kern[0];
	else if(old=='A' && new=='V') return kern[1];
	else if(old=='A' && new=='W') return kern[2];
	else if(old=='A' && new=='Y') return kern[3];
	else if(old=='F' && new=='J') return kern[4];
	else if(old=='P' && new=='J') return kern[5];
	else if(old=='V' && new=='J') return kern[6];
	else if(old=='W' && new=='J') return kern[7];
	else if(old=='Y' && new=='J') return kern[8];
	else if(old=='T' && new=='J') return kern[9];
	else if(old=='L' && new=='T') return kern[10];
	else if(old=='L' && new=='V') return kern[11];
	else if(old=='L' && new=='W') return kern[12];
	else if(old=='L' && new=='Y') return kern[13];
	else if(old=='T' && new=='A') return kern[14];
	else if(old=='V' && new=='A') return kern[15];
	else if(old=='W' && new=='A') return kern[16];
	else if(old=='Y' && new=='A') return kern[17];
	else return 0;
}

void FreezeScoreCode(uint32_t *dst, const char *src)
{
	int	i, j;
	uint32_t s;

	for(j=0; j<2; ++j) {
		s = 0;
		for(i=0; i<6; ++i) {
			s *= 10;
			s += *src % 10;
			src++;
		}
		*dst++ = s;
	}
}

void ExpandScoreCode(char *dst, const int *src)
{
	int	i, j, s;

	for(j=0; j<2; ++j) {
	s = *src++;
	for(i=0; i<6; ++i) {
		*(dst + (5 - i)) = s % 10;
		s /= 10;
		}
	dst += 6;
	}
}
