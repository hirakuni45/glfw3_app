/*------------------------------------------------------/
/														/
/	ボール・シュミレーター装置依存部分（ＴＯＰ）			/
/														/
/	高解像画面モード（６４０Ｘ４４８）仕様					/
/	ＢＧ：２５６色、ＭＯＢＪ：２５６						/
/														/
/------------------------------------------------------*/
// #include <sega_xpt.h>
// #include <sega_def.h>
// #include <sega_spr.h>
// #include <sega_scl.h>
// #include <sega_int.h> 
// #include <sega_tim.h> 
// #include <sega_snd.h>
#include <stdio.h>
#include "saturn.h"
#include "win.h"
#include "cdfs.h"
#include "task.h"
#include "top.h"
#include "mobj.h"

#define CLUT_MAX	512
#define SMPC_RECIVE_MAX	128

extern void InitFunctions(void);
extern void EntryMainMenu(void);
extern void InitTimerTask(void);
extern void EntryTimerTask(void (*)(void));
extern void ExitTimerTask(void);
extern void MainPlasmaTask(void);
extern void ReciveValues(int);

static void SetupUserBreak(void);
static void LaunchUserBreak(void);

static void InstallInterruptTask(void);
static void InitSMPC(void);
static void EntrySMPC(void);
static void ServiceSMPC(void);

static int TakePeriId(unsigned char *, int, unsigned char *);
static void ServiceInputMethod(uint32_t padbits);

static const unsigned short vramCycle[] = {
	0xef44,0xffff,			/* V-RAM A0 */
	0xffff,0xffff,			/* V-RAM A1 */
	0xfe44,0xffff,			/* V-RAM B0 */
	0xffff,0xffff };		/* V-RAM B1 */
static short smpcPtr = 0;
static volatile char turnSMPC, flagSMPC = 0;
static unsigned char timeData[16];
/// static unsigned char portData[SMPC_RECIVE_MAX], portLevel[SMPC_RECIVE_MAX];
/// static const unsigned short smpcIdTbl[] = {
///	SS_PEIDPL_T1, SS_PEIDPL_T2, SS_PEIDPL_T3, SS_PEIDPL_T4,
///	SS_PEIDPL_T5, SS_PEIDPL_T6, 0, 0,
///	0, 0, 0, 0,
///	0, 0, 0, 0 };
static short mouseAcl = 0, mouseDbc = 0, mouseOld = 0;
static short msX, msY, msC;
static short volWait = 0;
static const char *soundGroupPtr;
static char soundAreaNo;
static uint32_t padLevelMaster;

volatile uint32_t clutTransBit = 0;
volatile uint32_t clutTransMask = 1;
volatile unsigned short padLevel[2], padTriggerP[2], padTriggerN[2], padStatus[2];
volatile unsigned short padOnlyLevel[2], padOnlyTriggerP[2], padOnlyTriggerN[2];
volatile unsigned short mouseLevel, mouseTriggerP, mouseTriggerN;
volatile unsigned short analogEna[2], analogX[2], analogY[2], analogL[2], analogR[2];
volatile short mouseDx, mouseDy, mouseBl, mouseEna;
volatile short mouseX, mouseY, mouseB, mouseT;
volatile uint32_t vsyncCounter;
volatile char slaveSync;

char soundDrive = 0;
char soundSeq = 0;
// short mxFlag = 0;
short tvFrame;

uint32_t clutBuff[CLUT_MAX];
uint32_t clutHard[CLUT_MAX];
const uint32_t *GetCLUT() { return clutHard; }

void main_nmp(int no)
{
	tvFrame = SCREEN_NTSC_FRAME;
	padLevelMaster = 0;

	ReciveValues(no);
	SetupScreenStyle(tvFrame);
	InitFunctions();
	InitFileSystem();
	DoMainGameOnly(no);
}

/*----------------------------------------------/
/	Screen style intializations		/
/----------------------------------------------*/
void SetupScreenStyle(int type)
{
	char	c;
///	SclConfig	scfg;
///	SclVramConfig	tp;
///	Uint8  *VRAM;

	smpcPtr = 0;

///	SCL_Vdp2Init();
///	SCL_SetDisplayMode(SCL_DOUBLE_INTER, SCL_224LINE, SCL_HIRESO_A);
///	SCL_SetColRamMode(SCL_CRM24_1024);
///	SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|
///					SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7, 7);
///	SCL_SetSpriteMode(SCL_TYPEC, SCL_PALETTE, SCL_SP_WINDOW);
///	SPR_Initial(&VRAM);

///	SCL_SetCycleTable(vramCycle);

///	SCL_InitConfigTb(&scfg);
///	scfg.dispenbl = ON;
///	scfg.charsize = 0;
///	scfg.pnamesize = 0;
///	scfg.platesize = 0;
///	scfg.bmpsize = SCL_BMP_SIZE_1024X512;
///	scfg.coltype = SCL_COL_TYPE_256;
///	scfg.datatype = SCL_BITMAP;
///	SCL_SetConfig(SCL_NBG0, &scfg);

/*	SCL_SetCycleTable(vramCycle); */

///	SCL_AllocColRam(SCL_NBG0, 256, ON);

///	SCL_SetColRamOffset(SCL_NBG0, 0, ON);
///	SCL_SetColRamOffset(SCL_SPR, 1, ON);

///	SCL_SetPriority(SCL_SPR, 7);
///	SCL_SetPriority(SCL_NBG0, 6);

///	SCL_SetFrameInterval(1);

///	InitSMPC();
///	InstallInterruptTask();

	ToBlackScreen();
	clutTransMask = 1;

///	SPR_SetTvMode(SPR_TV_HIRESO, SPR_TV_640X224, ON);
///	SPR_SetEraseData(0x0000, 0, 0, 640-1, 448-1);

///	SCL_Open(SCL_NBG0);
///		SCL_MoveTo(FIXED(0), FIXED(0),0);
///		SCL_Scale(FIXED(1.0), FIXED(1.0));
///	SCL_Close();

	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
}

/*----------------------------------------------/
/						/
/	ＢＧ側（ＶＤＰ２）ＶＲＡＭクリア	/
/						/
/----------------------------------------------*/
void ClearBackGroundScreen(void)
{
	int	i, *p;

	p = (int *)get_VDP2_VRAM_TOP();
	for(i=0; i<((1024 * 512) / 4); ++i) *p++ = 0;
}

/*----------------------------------------------/
/						/
/	カラー・ルック・アップ・テーブル	/
/	関連関数				/
/						/
/----------------------------------------------*/
void ToBlackScreen(void)
{
	clutTransMask = 1;
	ClearCLUT();
	ClearBackGroundScreen();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	clutTransMask = 0;
}

void ClearCLUT(void)
{
	int	i;

	for(i=0; i<CLUT_MAX; ++i) clutBuff[i] = 0;
	clutTransBit = 0xffffffff;
}

void SetCLUT(int i, int r, int g, int b)
{
	if(i>=CLUT_MAX) return;
	clutBuff[i] = (b << 16) | (g << 8) | r;
	clutTransBit |= 1 << (i >> 4);
}

void SetCLUTi(int i, int c)
{
	if(i>=CLUT_MAX) return;
	clutBuff[i] = c;
	clutTransBit |= 1 << (i >> 4);
}

void SetCLUTs(int t, int s, const uint8_t *p)
{
	int	i, j, k, *c, r, g, b;

	j = t + s;
	k = 0;
	c = (int *)&clutBuff[t];
	for(i=t; i<j; ++i)
	{
	if(i >= CLUT_MAX) break;
	r = *p++;
	g = *p++;
	b = *p++;
	*c++ = (b << 16) | (g << 8) | r;
	k |= 1 << (i >> 4);
	}
	clutTransBit |= k;
}

void BrightColorTables(int t, int s, int l)
{
	int	r, g, b, i, j, k;

	j = t + s;
	k = 0;
	uint8_t *ptr = (uint8_t *)&clutBuff[t];
	for(i = t; i < j; ++i) {
		b = *ptr++; g = *ptr++; r = *ptr++;
		ptr -= 3;
		*ptr++ = (b * l) >> 8;
		*ptr++ = (g * l) >> 8;
		*ptr++ = (r * l) >> 8;
		ptr++;
		k |= 1 << (i >> 4);
	}
	clutTransBit |= k;
}

/*----------------------------------------------/
/						/
/	新しい割り込みベクターの登録		/
/						/
/----------------------------------------------*/
static void InstallInterruptTask(void)
{
///	INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_SYS | INT_MSK_SPR);
///	INT_SetScuFunc(INT_SCU_VBLK_IN, EntryVsync);
///	INT_SetScuFunc(INT_SCU_VBLK_OUT, ExitVsync);
///	INT_SetScuFunc(INT_SCU_SYS, EntrySMPC);
///	INT_SetScuFunc(INT_SCU_SPR, NULL);
///	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_SYS | INT_MSK_SPR, INT_MSK_NULL);
}

/*--------------------------------------------------/
/
/	ＳＭＰＣ制御関連ルーチン（System Management & Peripheral Control）
/
/--------------------------------------------------*/
static void InitSMPC(void)
{
	int	i;

///	*(unsigned char *)SATURN_PORT_PSEL = 0;
///	*(unsigned char *)SATURN_PORT_EXLE = 0;
///	*(unsigned char *)SATURN_PORT_DDR1 = 0;
///	*(unsigned char *)SATURN_PORT_DDR2 = 0;
///	for(i=0; i<SMPC_RECIVE_MAX; ++i) portData[i] = portLevel[i] = 0xff;
	turnSMPC = -1;
	flagSMPC = 1;
	smpcPtr = 0;
}

static void EntrySMPC(void)
{
///	int	i, j;
///	unsigned char *get, *put;

///	if(turnSMPC < 0) return;
///	else if(turnSMPC==0)
///	{
///	get = (unsigned char *)SATURN_SMPC_OREG;
///	put = &timeData[0];
///	for(i=0; i<16; ++i) { *put++ = *get++; get++; }
///	*(unsigned char *)SATURN_SMPC_IREG = 0x81;
///	if(*(unsigned char *)SATURN_SMPC_SF & 1) { turnSMPC++; smpcPtr = 0; }
///	else turnSMPC = -1;
///	}
///	else
///	{
///	get = (unsigned char *)SATURN_SMPC_OREG;
///	put = &portData[smpcPtr];
///	for(i=0; i<32; ++i) { *put++ = *get; get += 2; }
///	smpcPtr += 32;
///	if(smpcPtr > SMPC_RECIVE_MAX) { *(unsigned char *)SATURN_SMPC_IREG = 0x40; turnSMPC = -1; smpcPtr = 0; }
///	else if(*(unsigned char *)SATURN_SMPC_SR & 0x20) *(unsigned char *)SATURN_SMPC_IREG ^= 0x80;
///	else turnSMPC = -1;
///	}
}

static void ServiceSMPC(void)
{
///	int	i;
///	volatile unsigned char *get, *put;

///	if(flagSMPC)
///	{
///	get = (volatile unsigned char *)SATURN_SMPC_SF;
///	while(*get & 1) ;

///	put = (unsigned char *)SATURN_SMPC_IREG;
///	*put++ = 0x01; put++;
///	*put++ = 0x08; put++;
///	*put   = 0xf0;

///	for(i=0; i<SMPC_RECIVE_MAX; ++i) portLevel[i] = portData[i];

///	smpcPtr = 0;
///	turnSMPC = 0;
///	*get = 0x01;
///	*(unsigned char *)SATURN_SMPC_CMD = 0x10;	/* Go INTBACK ! */
///	}
}

void GetDateTime(DateTime *ptr)
{
///	unsigned short	a, b;

///	a = timeData[1] >> 4; a *= 1000;
///	b = timeData[1] & 15; b *= 100;
///	ptr->year = a + b + ((timeData[2] >> 4) * 10) + (timeData[2] & 15);
///	ptr->month = timeData[3] & 15;
///	ptr->week = timeData[3] >> 4;
///	ptr->day = (timeData[4] >> 4) * 10 + (timeData[4] & 15);
///	ptr->time = (timeData[5] >> 4) * 10 + (timeData[5] & 15);
///	ptr->min = (timeData[6] >> 4) * 10 + (timeData[6] & 15);
///	ptr->sec = (timeData[7] >> 4) * 10 + (timeData[7] & 15);
	ptr->year = 0;
	ptr->month = 0;
	ptr->week = 0;
	ptr->day = 0;
	ptr->time = 0;
	ptr->min = 0;
	ptr->sec = 0;
}

void SetCommandSMPC(int cmd)
{
	volatile unsigned char *ptr;

	flagSMPC = 0;
///	while(turnSMPC >= 0) SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
///	ptr = (volatile unsigned char *)SATURN_SMPC_SF;
///	while((*ptr & 1)==1) ;
///	*ptr = 1;
///	*(volatile unsigned char *)SATURN_SMPC_CMD = cmd;
///	while((*ptr & 1)==1) ;
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	flagSMPC = 1;
}

int TakePeriData(unsigned char *dst, int id, unsigned char *src)
{
#if 0
	int	i, j, k, f, p;
	unsigned char c, *top;

	f = -1;
	top = src;
	for(p=0; p<2; ++p)
	{
	c = *src++;
	if(c==0xf1)	/* Direct conection */
		{
		if((id & SS_PEIDPL_1)!=0 && p==0) k = TakePeriId(dst, id, src);
		else if((id & SS_PEIDPL_2)!=0 && p==1) k = TakePeriId(dst, id, src);
		else k = -1;
		if(k >= 0) { src += k; f = (src - top) | (p << 8); }
		else src += (*src & 15) + 1;
		}
	else if(c==0x04 || c==0x16)
		{ j = c & 15;
		for(i=0; i<j; ++i)
			{ c = *src;
			if(c==0xff) src++;
			else	{
				if(id & smpcIdTbl[i])
					{
					if((id & SS_PEIDPL_1)!=0 && p==0) k = TakePeriId(dst, id, src);
					else if((id & SS_PEIDPL_2)!=0 && p==1) k = TakePeriId(dst, id, src);
					else k = -1;
					}
				else k = -1;
				if(k >= 0) { src += k; f = (src - top) | (p << 8) | (i << 10) | 0x100; }
				else src += (*src & 15) + 1;
				}
			if(f >= 0) break;
			}
		}
	if(f >= 0) break;
	}
	return f;
#endif
return 0;
}

static int TakePeriId(unsigned char *dst, int id, unsigned char *src)
{
///	int	i, s, c;

///	c = *src++;
///	if((c & 0xff)!=(id & 0xff)) return -1;
///	s = c & 15;
///	if(s==0) return -1;
///	if(dst!=NULL) { for(i=0; i<s; ++i) *dst++ = *src++; }
///	return s + 1;
return 1;
}

/*----------------------------------------------/
/						/
/	V-SYNC interrupt routines.		/
/						/
/----------------------------------------------*/
void SyncBlanking(void)
{
///	unsigned char dt[16];

/* SetSpeedLevel(620, 0);
*/
///	i = vsyncCounter;
///	SCL_DisplayFrame();
///	while(i == (volatile) vsyncCounter) ;

	mouseEna = 0;
	analogEna[0] = analogEna[1] = 0;
	uint16_t s, n;
	for(int i = 0; i < 2; ++i)
	{
		int j;
		if(i) j = SS_PEIDPL_2; else j = SS_PEIDPL_1;
		s = n = 0;
///		for(int k=0; k<6; ++k) {
///			if(analogEna[i]==0 && (TakePeriData(&dt[0],
///				smpcIdTbl[k] | j | SS_PEID_MULCON, &portLevel[0]))>=0) {
///				n |= ((dt[0] << 8) | dt[1]) ^ 0xffff;
///				analogX[i] = dt[2];
///				analogY[i] = dt[3];
///				analogR[i] = dt[4];
///				analogL[i] = dt[5];
///				analogEna[i] = 1;
///				s |= 4;
///			} else if((TakePeriData(&dt[0], smpcIdTbl[k] | j | SS_PEID_6B, &portLevel[0]))>=0) {
///				n |= ((dt[0] << 8) | dt[1]) ^ 0xffff;
///				s |= 1;
///			}
///		}
		n = padLevelMaster;
///	if(mouseEna==0 && (TakePeriData(&dt[0], SS_PEIDPL_AT | j | SS_PEID_MOUSE, &portLevel[0]))>=0) {
		s = 0;
		if(mouseEna==0) {
			uint16_t m = 0;
			mouseEna |= 1;
///			mouseBl = dt[0];
///			mouseDx = dt[1];
			mouseBl = 0;
			mouseDx = 0;
			if(mouseBl & 0x10) mouseDx |= 0xff00;
///			mouseDy = dt[2];
			mouseDy = 0;
			if(mouseBl & 0x20) mouseDy |= 0xff00;
			if(mouseBl & SATURN_MS_A) m |= SATURN_PAD_A;
			if(mouseBl & SATURN_MS_B) m |= SATURN_PAD_B;
			if(mouseBl & SATURN_MS_C) m |= SATURN_PAD_C;
			if(mouseBl & SATURN_MS_START) m |= SATURN_PAD_START;
			uint16_t o = mouseLevel;
			mouseLevel = m;
			mouseTriggerP = m & (o ^ 0xffff);
			mouseTriggerN = o & (m ^ 0xffff);
			s |= 2;
		}
		uint16_t o = padOnlyLevel[i];
		padOnlyLevel[i] = n;
		padOnlyTriggerP[i] = n & (o ^ 0xffff);
		padOnlyTriggerN[i] = o & (n ^ 0xffff);
		padStatus[i] = s;
		padLevel[i] = padOnlyLevel[i];
		padTriggerP[i] = padOnlyTriggerP[i];
		padTriggerN[i] = padOnlyTriggerN[i];
	}

	if(mouseEna) {
		padLevel[0] |= mouseLevel;
		padTriggerP[0] |= mouseTriggerP;
		padTriggerN[0] |= mouseTriggerN;
	} else {
		mouseLevel = mouseTriggerP = mouseTriggerN = 0;
	}

	if((padStatus[0] & 7)==0) {
		mouseEna = 0;
		mouseLevel = mouseTriggerP = mouseTriggerN = 0;
		for(int i = 0; i < 2; ++i) {
			analogEna[i] = 0;
			analogX[i] = analogY[i] = analogL[i] = analogR[i] = 0;
			padOnlyLevel[i] = padOnlyTriggerP[i] = padOnlyTriggerN[i] = 0;
			padLevel[i] = padTriggerP[i] = padTriggerN[i] = 0;
		}
	}
/* SetSpeedLevel(620, 31);
*/
}

void EntryVsync(uint32_t padbits)
{
///	SCL_VblankStart();
	padLevelMaster = padbits;
/* Color Luck Up Service */
	if(clutTransMask != 0) {
		int j = 6;
		uint32_t b = 1;
		const uint32_t *clut = &clutBuff[0];
		uint32_t ofs = 0;
		for(int i = 0; i < 512; i += 16) {
			if(clutTransBit & b) {
///				SCL_SetColRam(SCL_NBG0, i, i + 16, clut);
				for(int l = 0; l < 16; ++l) clutHard[ofs + l] = clut[l];
				clutTransBit ^= b;
				j--;
			}
			if(j == 0) break;
			clut += 16;
			b += b;
			ofs += 16;
		}
	}

	ServiceInputMethod(padbits);
	ServiceSMPC();
}

void ExitVsync(void)
{
///	int	i;
///	volatile char	*ptr;

///	SCL_VblankEnd();
	vsyncCounter++;
///	ptr = &slaveSync;
///	ptr += 0x20000000;
///	( *ptr )++;
///	ServiceAudioCD();
///	if(volWait!=0 && soundDrive!=0 && soundSeq!=0)
///	{
///	volWait--;
///	if(volWait==0) SND_SetTlVl(15);
///	}
}

void PresetBGposition(int x, int y)
{
///	SCL_Open(SCL_NBG0);
///		SCL_MoveTo(FIXED(x), FIXED(y),0);
///	SCL_Close();
}

/*----------------------------------------------/
/						/
/	マウス／ＰＡＤ・エミュレーション	/
/						/
/----------------------------------------------*/
static void ServiceInputMethod(uint32_t padbits)
{
///	int	i;
///	unsigned char data[16];

//	if(mxFlag)
	{
		mouseB = 0;
#if 0
	if((TakePeriData(&data[0], SS_PEIDPL_AT|SS_PEIDPL_AP|SS_PEID_MOUSE, &portLevel[0]))>=0) {
		p = data[0];
		x = data[1];
		y = data[2];
		if(p & 0x10) x |= 0xff00;
		mouseX += x;
		if(p & 0x20) y |= 0xff00;
		mouseY -= y;
		if(p & SATURN_MS_A) mouseB |= MOUSE_LEFT_LEVEL;
		if(p & SATURN_MS_B) mouseB |= MOUSE_MIDDLE_LEVEL;
		if(p & SATURN_MS_C) mouseB |= MOUSE_RIGHT_LEVEL;
	}
#endif
		uint16_t p = padbits;
///		for(i=0; i<6; ++i) {
///			if((TakePeriData(&data[0], smpcIdTbl[i]|SS_PEIDPL_AP|SS_PEID_6B, &portLevel[0]))>=0) {
///				p |= ((data[0] << 8) + data[1]) ^ 0xffff;
///			}
///		}
		short x;
		short y;
		if(p) {
			if(p & SATURN_DIR_ALL) mouseAcl++;
			else mouseAcl = 0;
			if(mouseAcl > 64) mouseAcl = 64;
			x = mouseAcl / 8;
			x++;
			if(p & SATURN_DIR_RIGHT) mouseX += x;
			else if(p & SATURN_DIR_LEFT) mouseX -= x;
			if(p & SATURN_DIR_DOWN) mouseY += x;
			else if(p & SATURN_DIR_UP) mouseY -= x;
			if(p & (SATURN_PAD_A|SATURN_PAD_X)) mouseB |= MOUSE_LEFT_LEVEL;
			if(p & (SATURN_PAD_B|SATURN_PAD_Y)) mouseB |= MOUSE_MIDDLE_LEVEL;
			if(p & (SATURN_PAD_C|SATURN_PAD_Z)) mouseB |= MOUSE_RIGHT_LEVEL;
		}
		if((mouseOld & MOUSE_LEVEL)==0 && (mouseB & MOUSE_LEVEL)!=0) {
			if(mouseDbc < 30) mouseB |= MOUSE_LAUNCH;
		}
		mouseOld = mouseB;
		if(mouseB & MOUSE_LEVEL) mouseDbc = 0;
		else if(mouseDbc < 300) mouseDbc++;

		if(mouseX < 0) mouseX = 0;
		else if(mouseX >= 640) mouseX = 640-1;
		if(mouseY < 0) mouseY = 0;
		else if(mouseY >= 448) mouseY = 448-1;

		mouseT++;
	}
}

int TakeDirection(void)
{
	int	i, dx, dy;

	i = padTriggerP[0] | padTriggerP[1];
	if(mouseEna)
	{
	msX += mouseDx;
	msY += mouseDy;
	if(msX >= 66) { i |= SATURN_DIR_RIGHT; msX = 0; }
	else if(msX <= -66) { i |= SATURN_DIR_LEFT; msX = 0; }
	if(msY >= 66) { i |= SATURN_DIR_UP; msY = 0; }
	else if(msY <= -66) { i |= SATURN_DIR_DOWN; msY = 0; }
	}
	return i;
}
/*----------------------------------------------/
/						/
/	スレーブＣＰＵ実行関連			/
/						/
/----------------------------------------------*/
void InstallSlaveFunction(void (*task)(void)) 
{
#if 0
	volatile unsigned char *ptr;

	flagSMPC = 0;
	while(turnSMPC >= 0) SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	ptr = (volatile unsigned char *)SATURN_SMPC_SF;
	while((*ptr & 1)==1) ;
	*ptr = 1;
	*(volatile unsigned char *)SATURN_SMPC_CMD = SMPC_SLAVE_OFF;
	while((*ptr & 1)==1) ;

	if(task!=NULL)
	{
	SYS_SETSINT(0x94, task);
	ptr = (volatile unsigned char *)SATURN_SMPC_SF;
	while((*ptr & 1)==1) ;
	*ptr = 1;
	*(volatile unsigned char *)SATURN_SMPC_CMD = SMPC_SLAVE_ON;
	while((*ptr & 1)==1) ;
	}
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	flagSMPC = 1;
#endif
}

#if 0
char GetCashThruChar(char *ptr)
{
//	return *(char *)(ptr + 0x20000000);
	return *(char *)(ptr);
}

short GetCashThruShort(char *ptr)
{
//	return *(short *)(ptr + 0x20000000);
	return *(short *)(ptr);
}

int GetCashThruInt(char *ptr)
{
//	return *(int *)(ptr + 0x20000000);
	return *(int *)(ptr);
}
#endif

/*----------------------------------------------/
/						/
/	サウンド・ドライバー関連初期化		/
/						/
/----------------------------------------------*/
void LaunchSound(const char *gen, const char *spl)
{
#if 0
	char	*ptr;
	int	i;
	SndIniDt soundSys;

	if(gen==NULL) return;

	flagSMPC = 0;
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();

	ptr = GetResource(gen, 0);
	soundSys.prg_adr = (Uint16 *)(ptr + 4);
	soundSys.prg_sz  = (Uint16) TakeMiddle(ptr + 1);
	if(spl!=NULL) ptr = GetResource(spl, 2);
	else ptr = GetResource(gen, 1);
	soundSys.ara_adr = (Uint16 *)(ptr + 4);
	soundSys.ara_sz  = (Uint16)(TakeMiddle(ptr + 1) >> 1);
	SND_Init(&soundSys);

	SND_ChgMap(0);
	ptr = GetResource(gen, 2);
	i = TakeMiddle(ptr + 1);
	SND_MoveData((Uint16 *)(ptr + 4), i, SND_KD_DSP_PRG, 0);
	ptr = GetResource(gen, 3);
	i = TakeMiddle(ptr + 1);
	SND_MoveData((Uint16 *)(ptr + 4), i, SND_KD_SEQ, 0);
	ptr = GetResource(gen, 4);
	i = TakeMiddle(ptr + 1);
	SND_MoveData((Uint16 *)(ptr + 4), i, SND_KD_TONE, 0);

	SND_SetTlVl(0);
	SND_ChgEfct(0);
	SND_ChgMix(0, 0);
	SND_ChgMixPrm(0, 7, 0);
	soundDrive = 1;
	soundSeq = 1;
	volWait = 45;

	if(spl!=NULL) {
	ptr = GetResource(spl, 0);
	i = TakeMiddle(ptr + 1);
	SND_MoveData((Uint16 *)(ptr + 4), i, SND_KD_SEQ, 1);
	ptr = GetResource(spl, 1);
	i = TakeMiddle(ptr + 1);
	SND_MoveData((Uint16 *)(ptr + 4), i, SND_KD_TONE, 1);
	}

	soundGroupPtr = NULL;

	SyncBlanking();
	flagSMPC = 1;
#endif
}

void SetSoundGroup(const char *ptr, int no)
{
	soundGroupPtr = ptr;
	soundAreaNo = no;
}

void RingSfx(int no)
{
#if 0
	int	b, sg;

	if(soundDrive!=0 && soundSeq!=0) {
	if(soundGroupPtr==NULL) sg = no;
	else if(no < 32) {
	sg = soundGroupPtr[no & 31];
	b = 0;
	}
	else	{
	sg = soundGroupPtr[32 + (no & 31) + (soundAreaNo * 32)];
	b = 1;
	}
	sg &= 7;
	no &= 31;
	SND_SetSeqVl(sg, 109, 0);
/*	SND_SetSeqVl(sg, 85, 0); */
	SND_StartSeq(sg, b, no, 0);
	}
#endif
}

void ServiceCloseInt(void)
{
#if 0
	flagSMPC = 0;
	while(turnSMPC >= 0) SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	SyncBlanking();
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_SYS | INT_MSK_SPR);
	INT_SetScuFunc(INT_SCU_VBLK_IN, NULL);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, NULL);
	INT_SetScuFunc(INT_SCU_SYS, NULL);
	INT_SetScuFunc(INT_SCU_SPR, NULL);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_SYS | INT_MSK_SPR, INT_MSK_NULL);
#endif
}

void SetSpeedLevel(int x, int c)
{
#if 0
	int	y;
	unsigned short *out;

	c |= c << 8;
	out = (unsigned short *)(VDP2_VRAM_TOP + (x & 0xfffe));
	for(y=0; y<448; ++y) { *out = c; out += 512; }
#endif
}
