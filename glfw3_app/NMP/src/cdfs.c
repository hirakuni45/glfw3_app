#include <stdio.h>
#include <string.h>
#include <stdint.h>
// #include <sega_sys.h>
// #include <sega_gfs.h>
// #define BUP_START_ADDR	(SATURN_CRAM_TOP+0xF0000)
// #include <sega_bup.h>
#include "saturn.h"
#include "ballsim.h"
#include "option.h"
#include "task.h"
#include "top.h"
#include "cdfs.h"

#define SMCD_MAX_OPEN  4
#define ROOT_FILES_MAX   256
#define MAIN_FILES_MAX   24

extern volatile unsigned short padLevel[], padTriggerN[];
extern char soundDrive;
extern char optionMenuPad[];
extern char multiPlayerEnable;
extern char trialEnable, geEnable;
extern char flashSegaEnable;
extern char alertTestEnable, blockTestEnable;
extern char cdTrackStyle;
extern NECRONOMICON necroPad[];

extern void ExecReturn(void);
extern void FreezeScoreCode(int *dst, char *src);
extern void ExpandScoreCode(char *dst, int *src);
extern char convertTrackTables3[];

static void CreatePinBallDir(void);
static void CreateTestDir(int);
static void CreatePinBallData(void);
static int CreateNECRONOMICON(void);

void DumpMemory(char *, int);
void ServiceMultiPlayer(void);

static char cdfsEnable = 0;
static char cdTrack[2];
static short cdMode, playCount;
// static CdcPly cdPlayPad;
// static CdcStat cdPlayStat;
// static GfsCdbMng gfcdWork;
static uint32_t backupBuffer[2048];
static unsigned char pinballData[256];
// static BupDir pinballDir;
static char convertTrackTables[] = {  -1, -1, 6, 2, 3, 4, 5, 6, 4, 5, -1 };
static char convertTrackTables2[] = {  -1, -1, 11, 12, 12, 12, 12, 12, 12, 12, -1 };
static unsigned char pinballFN[] = { 'N', 'E', 'C', 'R', 'O', '_', 'V', 'E', 'R', '9', '6', 0 };
static unsigned char pinballCM[] = {
/* ・ピンボールス・ */
	0xA5, 0xCB, 0xDF, 0xDD, 0xCE, 0xDE, 0xB0, 0xD9, 0xBD, 0xA5, 0 };
static unsigned char testFN[] = { 'T', 'E', 'S', 'T', 'O', '_', 'V', 'E', 'R', '9', '6', 0 };
static unsigned char testCM[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0 };

// static Uint8		gfsWork[GFS_WORK_SIZE(SMCD_MAX_OPEN)];
// static GfsDirTbl	rootDirTbl;
// static GfsDirName	rootDirName[ROOT_FILES_MAX];
// static GfsDirTbl	mainDirTbl;
// static GfsDirName	mainDirName[MAIN_FILES_MAX];

char errMsg[256];
int cdStatus;
short cdFadeLevel = 7;

static uint8_t saturn_dram_top_[1024 * 1024];

uint8_t *get_SATURN_DRAM_TOP()
{
	return saturn_dram_top_;
}

/*------------------------------------------------------/
/	ファイル・システム関連				/
/------------------------------------------------------*/
void InitFileSystem(void)
{
#if 0
	int	i;
	Sint32	id;

	cdfsEnable = 0;

	GFS_DIRTBL_TYPE(&rootDirTbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&rootDirTbl) = rootDirName;
	GFS_DIRTBL_NDIR(&rootDirTbl) = ROOT_FILES_MAX;

	for(i=0; i<10; ++i) {
	if(GFS_Init(SMCD_MAX_OPEN, &gfsWork, &rootDirTbl) > 0) break;
	}
	if(i) {
	if(multiPlayerEnable==0) {
		sprintf(&errMsg[0],
		"(%d)Stop.\nFalse !\nCan't mount 'File-System: ISO9660'.\n", i);
		ToBlackScreen();
		LaunchDebugger(&errMsg[0]);
		}
	}

	if(flashSegaEnable==0) {
	if((id = GFS_NameToId("NECRO")) < 0) {
	if(multiPlayerEnable==0) {
		sprintf(&errMsg[0],
		"(%d)Stop.\nFalse !\nName to ID.\n", i);
		ToBlackScreen();
		LaunchDebugger(&errMsg[0]);
		}
	}

	GFS_DIRTBL_TYPE(&mainDirTbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&mainDirTbl) = mainDirName;
	GFS_DIRTBL_NDIR(&mainDirTbl) = MAIN_FILES_MAX;
	GFS_LoadDir(id, &mainDirTbl);
	GFS_SetDir(&mainDirTbl);
	}
	cdfsEnable = 1;
#endif
}

void ReadFile(const char *fname, void *buf)
{
	FILE *fp;
	char fn[32];

	strcpy(fn, "./RES/");
	strcat(fn, fname);

printf("Read file: '%s'\n", fn);

	fp = fopen(fn, "rb");
	if(fp == NULL) {
		return;
	}
	long sz;
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	size_t rs = fread(buf, 1, sz, fp);

	fclose(fp);

	if(rs != sz) {
		sprintf(&errMsg[0],
			"Stop.\nCan't open input file as '%s'.\n", fname);
		ToBlackScreen();
		LaunchDebugger(&errMsg[0]);
	}

printf("Read resource: %ld bytes\n", sz);

#if 0
	int	i, id;
	char	*src, *dst, c, cna[16];

	if((id = GFS_NameToId(fname)) < 0) {
	if(multiPlayerEnable==0) {
	sprintf(&errMsg[0],
		"(%d)Stop.\nFalse ! 'can't find file as '%s'\n", i, fname);
		ToBlackScreen();
		LaunchDebugger(&errMsg[0]);
		}
	}

	for( ; ; )
	{
	if((i = GFS_Load(id, 0, buf, GFS_BUFSIZ_INF)) < 0) {
	if(multiPlayerEnable==0) {
	sprintf(&errMsg[0],
		"(%d)Stop.\nCan't open input file as '%s'.\n", i, fname);
		ToBlackScreen();
		LaunchDebugger(&errMsg[0]);
			}
		}
	else break;
	}
#endif
}

void LaunchResource(const char *name)
{
	ReadFile(name, (void *)get_SATURN_DRAM_TOP());
	InitResource();
}

/*------------------------------------------------------/
/	オーディオ関連					/
/------------------------------------------------------*/
void ServiceAudioCD(void)
{
#if 0
	int	i, n;

	CDC_GetPeriStat(&cdPlayStat);
	n = cdPlayStat.status & 15;
	if(n==6 && multiPlayerEnable!=0) SYS_EXECDMP();
	if(cdfsEnable==0) return;

	cdStatus = n;

	if(soundDrive)
	{
/*	i = cdFadeLevel - cdFadeLevel / 7; */
	i = cdFadeLevel;
	SND_SetCdDaLev(i, i);
	SND_SetCdDaPan(0, 0);
	}

	switch(cdMode)
	{
	case 0:
		break;
	case 1:
		i = cdTrack[0];
		if(i < 0) cdMode = 3;
		else	{
			if(flashSegaEnable) i = GFS_ConvTno(i);	
			CDC_PLY_STYPE(&cdPlayPad) = CDC_PTYPE_TNO;
			CDC_PLY_STNO(&cdPlayPad) = i;
			CDC_PLY_SIDX(&cdPlayPad) = 1;
			CDC_PLY_ETYPE(&cdPlayPad) = CDC_PTYPE_TNO;
			CDC_PLY_ETNO(&cdPlayPad) = i;
			CDC_PLY_EIDX(&cdPlayPad) = 99;
			CDC_PLY_PMODE(&cdPlayPad) = CDC_PM_DFL;
			CDC_CdPlay(&cdPlayPad);
			cdMode++;
			playCount = 6;
			}
		break;
	case 2:
		if(n==3 && playCount > 0) playCount--;
		if(playCount==0 && (n==1 || n==2)) cdMode = 3;
		break;
	case 3:
		i = cdTrack[1];
		if(i < 0) cdMode = 0;
		else	{
			if(flashSegaEnable) i = GFS_ConvTno(i);	
			CDC_PLY_STYPE(&cdPlayPad) = CDC_PTYPE_TNO;
			CDC_PLY_STNO(&cdPlayPad) = i;
			CDC_PLY_SIDX(&cdPlayPad) = 1;
			CDC_PLY_ETYPE(&cdPlayPad) = CDC_PTYPE_TNO;
			CDC_PLY_ETNO(&cdPlayPad) = i;
			CDC_PLY_EIDX(&cdPlayPad) = 99;
			CDC_PLY_PMODE(&cdPlayPad) = CDC_PM_DFL;
			CDC_CdPlay(&cdPlayPad);
			cdMode++;
			playCount = 6;
			}
		break;
	case 4:
		if(n==3 && playCount > 0) playCount--;
		if(playCount==0 && (n==1 || n==2)) cdMode = 3;
		break;
	default:
		break;
	}
#endif
}

/*	第２トラックからＣＤオーディオ・トラックが開始する	*/
void PlayCD(int tno1, int tno2)
{
#if 0
	if(cdTrackStyle==0) ;
	else if(cdTrackStyle==1) {
	if(tno1 > 9 || tno1 < 0) tno1 = -1;
	else tno1 = convertTrackTables[tno1];
	if(tno2 > 9 || tno2 < 0) tno2 = -1;
	else tno2 = convertTrackTables[tno2];
	}
	else if(cdTrackStyle==2) {
	if(tno1 > 9 || tno1 < 0) tno1 = -1;
	else tno1 = convertTrackTables2[tno1];
	if(tno2 > 9 || tno2 < 0) tno2 = -1;
	else tno2 = convertTrackTables2[tno2];
	}
	else if(cdTrackStyle==3) {
	if(tno1 > 48 || tno1 < 0) tno1 = -1;
	else tno1 = convertTrackTables3[tno1];
	if(tno2 > 48 || tno2 < 0) tno2 = -1;
	else tno2 = convertTrackTables3[tno2];
	}

	cdTrack[0] = tno1;
	cdTrack[1] = tno2;
	cdMode = 1;
#endif
}

/*------------------------------------------------------/
/	バックアップ関係				/
/------------------------------------------------------*/
int ProbeBuckupMemory(void)
{
#if 0
	BupConfig	cf[3];
	BupStat		st;
	BupDir		dir;
	int	i, f;
	char	*sptr;

	if(alertTestEnable) return 1;
	if(trialEnable | geEnable) return 0;

	f = 0;

	SetCommandSMPC(SMPC_RESET_DISABLE);
	BUP_Init((Uint32 *)BUP_START_ADDR, (Uint32 *)backupBuffer, &cf[0]);

	if(blockTestEnable) {
	i = BUP_Stat(0, 64, &st);
	if(i==BUP_UNFORMAT) {
		BUP_Format(0);
		i = BUP_Stat(0, 64, &st);
		}
	i = st.freesize / 64;
	CreateTestDir((i - 3) * 64);
	BUP_Write(0, &pinballDir, (Uint8 *)pinballData, (Uint8)OFF);
	}

	if(BUP_Stat(0, 193, &st)==BUP_UNFORMAT) {
	BUP_Format(0);
	}

	i = BUP_Dir(0, pinballFN, 1, &dir);
	if(i==0) {
		BUP_Stat(0, 193, &st);
		if(st.freesize < (64 * 4)) f = 1;
		if(st.datanum < 1) f = 1;
	}
	else if(i==1) ;
	else f = 1;

	SetCommandSMPC(SMPC_RESET_ENABLE);

/*	if(backupStatusEnable) {
	ToBlackScreen();
	sptr = (char *)BUP_START_ADDR;
	sptr += sprintf(sptr, "Match count: %d\n", i);
	sptr += sprintf(sptr, "Total free size: %d [Byte(s)]\n", st.totalsize);
	sptr += sprintf(sptr, "Total free block: %d [Block(s)]\n", st.totalblock);
	sptr += sprintf(sptr, "Block size: %d [Byte(s)]\n", st.blocksize);
	sptr += sprintf(sptr, "Free size: %d [Byte(s)]\n", st.freesize);
	sptr += sprintf(sptr, "Free block: %d [Block(s)]\n", st.freeblock);
	sptr += sprintf(sptr, "Data num: %d Data[(s)]\n", st.datanum);
	sptr = (char *)BUP_START_ADDR;
	LaunchDebugger(sptr);
	}
*/
	return f;
#endif
return 0;
}

void ReadBackupSystem(void)
{
#if 0
	BupConfig	cf[3];
	int	i, j, k, s;
	char	*dst;

	if(trialEnable | geEnable) return;
	SetCommandSMPC(SMPC_RESET_DISABLE);
	BUP_Init((Uint32 *)BUP_START_ADDR, (Uint32 *)backupBuffer, &cf[0]);
	i = BUP_Read(0, &pinballFN[0], &pinballData[0]);
	if(i==0) { if(CreateNECRONOMICON()) InitNECRONOMICON(); }
	SetCommandSMPC(SMPC_RESET_ENABLE);
#endif
}

void WriteBackupSystem(void)
{
#if 0
	BupConfig	cf[3];
	int	i, k, s;
	char	*dst;

	if(trialEnable | geEnable) return;
	SetCommandSMPC(SMPC_RESET_DISABLE);
	BUP_Init((Uint32 *)BUP_START_ADDR, (Uint32 *)backupBuffer, &cf[0]);
	CreatePinBallData();
	CreatePinBallDir();
	i = BUP_Write(0, &pinballDir, (Uint8 *)&pinballData[0], (Uint8)OFF);
	if(i==BUP_UNFORMAT)
	{
	BUP_Format(0);
	BUP_Write(0, &pinballDir, (Uint8 *)&pinballData[0], (Uint8)OFF);
	}
	SetCommandSMPC(SMPC_RESET_ENABLE);
#endif
}

static void CreatePinBallDir(void)
{
#if 0
	int	i;
	BupDate	t;
	DateTime	st;

	if(trialEnable | geEnable) return;
	for(i=0; i<12; ++i) pinballDir.filename[i] = pinballFN[i];
	for(i=0; i<11; ++i) pinballDir.comment[i] = pinballCM[i];
	pinballDir.language = BUP_JAPANESE;
	GetDateTime(&st);
	t.year = st.year - 1980;
	t.month = st.month;
	t.day = st.day;
	t.time = st.time;
	t.min = st.min;
	t.week = st.week;
	pinballDir.date = BUP_SetDate(&t);
	pinballDir.datasize = 193;
#endif
}

static void CreateTestDir(int size)
{
#if 0
	int	i;
	BupDate	t;
	DateTime	st;

	for(i=0; i<12; ++i) pinballDir.filename[i] = testFN[i];
	for(i=0; i<11; ++i) pinballDir.comment[i] = testCM[i];
	pinballDir.language = BUP_JAPANESE;
	GetDateTime(&st);
	t.year = st.year - 1980;
	t.month = st.month;
	t.day = st.day;
	t.time = st.time;
	t.min = st.min;
	t.week = st.week;
	pinballDir.date = BUP_SetDate(&t);
	pinballDir.datasize = size;
#endif
}

static void CreatePinBallData(void)
{
#if 0
	int	i, j, k, s, a[2];
	char	*txt;
	unsigned char *data;

	s = 9218;
	data = &pinballData[1];
	for(i=0; i<OPTION_PAD_MAX; ++i) {
	*data++ = optionMenuPad[i] & 255;
	s += optionMenuPad[i];
	}
	for(j=0; j<3; ++j) {
	for(i=0; i<5; ++i) {
	txt = &necroPad[j].topScore[(7 - i) * 12];
	FreezeScoreCode(a, txt);
	for(k=0; k<3; ++k) { s += a[0] & 255; *data++ = a[0]; a[0] >>= 8; }
	for(k=0; k<3; ++k) { s += a[1] & 255; *data++ = a[1]; a[1] >>= 8; }
	txt = &necroPad[j].topName[(7 - i) * 3];
	for(k=0; k<3; ++k) { s += *(unsigned char *)txt; *data++ = *txt++; }
		}
	pinballData[0] = s;
	}
#endif
}

static int CreateNECRONOMICON(void)
{
#if 0
	int	i, j, k, s, a[2];
	char	*txt;
	unsigned char *data;

	s = 9218;
	data = &pinballData[1];
	for(i=0; i<OPTION_PAD_MAX; ++i) {
	optionMenuPad[i] = *data++;
	s += (unsigned char)optionMenuPad[i];
	}
	for(j=0; j<3; ++j) {
		for(i=0; i<5; ++i) {
		for(k=0; k<2; ++k) {
			s += *(data+2);
			a[k] = *(data+2);
			a[k] <<= 8;
			s += *(data+1);
			a[k] |= *(data+1);
			a[k] <<= 8;
			s += *data;
			a[k] |= *data;
			data += 3;
			}
		txt = &necroPad[j].topScore[(7 - i) * 12];
		ExpandScoreCode(txt, a);
		txt = &necroPad[j].topName[(7 - i) * 3];
		for(k=0; k<3; ++k) { s += *data; *txt++ = *data++; }
		}
	}
	s &= 255;
	if(s==pinballData[0]) return 0;
	else return 1;
#endif
	return 0;
}

/*------------------------------------------------------/
/	メモリー・ダンプ関係				/
/------------------------------------------------------*/
/*
void DumpMemory(char *ptr, int cnt)
{
	int	i, j;
	char	*str;

	str = (char *)(SATURN_CRAM_TOP+0xFF000);
	for(i=0; i<cnt; ++i)
	{
	j = (unsigned int)ptr;
	if((i & 7)==0) {
	sprintf(str, "%08X-", j); str += 9;
	}

	j = *(unsigned char *)ptr++;
	sprintf(str, " %02X", j);
	str += 3;
	if((i & 7)==7) *str++ = '\n';
	}
	*str++ = 0;
	LaunchDebugger((char *)(SATURN_CRAM_TOP+0xFF000));
}
*/
