#include <stdio.h>
#include <stdarg.h>
#include "saturn.h"
#include "window.h"
#include "fnc.h"
#include "mobj.h"
#include "task.h"
#include "top.h"
#include "mw.h"

extern volatile short mouseX, mouseY, mouseB, mouseT;
extern unsigned int clutTransMask;

#define WIN_WIDTH_MAX		640
#define WIN_HEIGHT_MAX		448

static void MxHorizontal(int xt, int yt, int xb, int c);
static void MxVertical(int xt, int yt, int yb, int c);
static void DrawIconImage(RECT *, char *);
static void LaunchApplicationDemo(RECT *, int);
static int GetSjisToKanjiAdr(char *);
static int PositioningText(SLIDER *, int, int);

/*----------------------------------------------/
/	Micro Window System include files	/
/----------------------------------------------*/
#include "gfont.h"
#include "parts.h"

/*----------------------------------------------/
/	Micro Window System static fileds	/
/----------------------------------------------*/
short mxFlag;
static short scsTime, scsCount;
static MOUSE mouse;
static MOBJ *msMobj = NULL;
static RECT sysCp, udrCp[WINDOW_MAX], textCp, *textCpPtr;
static short winWidth, winHeight, winMode, winModeOld;
static short winUse, winMax, udrMax;
static short relocateX, relocateY;
static short fontWidth = 8;
static WINDOW *winTop, *relocateWin, winBuff[WINDOW_MAX], *winTag[WINDOW_MAX];
static uint8_t *kanjiFont = NULL;
static const uint8_t winColVal[] = {
	0, 0, 0,
	30-3, 56-6, 85-9,
	60-6, 112-11, 170-17,
	90-9, 156-16, 255-26,
	200-20, 200-20, 255-26,
	72, 72, 72,
	255, 255, 255 };
static unsigned char winColIdx[WIN_COLOR_MAX];
static int iconPosX, iconPosY;
static void (*plotGo)(int, int, int);
static WINDOW *launchWin;

/*------------------------------------------------------/
/							/
/							/
/		Micro Window System.			/
/							/
/		      'VENUS'				/
/							/
/......................................................./
/							/
/	Copyright 1994(c) by K.Hiramatsu		/
/	October,01,1994 Version 0.01			/
/	October,05,1994 Version 0.20			/
/	October,16,1994 Version 0.61			/
/	October,17,1994 Version 0.69			/
/	October,18,1994 Version 0.70			/
/	October,21,1994 Version 1.00			/
/							/
/------------------------------------------------------*/
void MountKanjiFont(void)
{
	kanjiFont = (unsigned char *)GetResource("KANJ", 0);
}

void MxBaseInit(void)
{
	int	i, r, g, b;
	const uint8_t *c;

	winWidth = WIN_WIDTH_MAX;
	winHeight = WIN_HEIGHT_MAX;

	c = &winColVal[0];
	for(i=0; i<WIN_COLOR_MAX; ++i)
	{
	winColIdx[i] = i;
	r = *c++;
	g = *c++;
	b = *c++;
	SetCLUT(i, r, g, b);
	SetCLUT(i+256, r, g, b);
	}

	plotGo = MxPlot;

	textCp.xt = sysCp.xt = 0;
	textCp.yt = sysCp.yt = 0;
	textCp.xb = sysCp.xb = winWidth;
	textCp.yb = sysCp.yb = winHeight;
	textCpPtr = &textCp;

	iconPosX = 10;
	iconPosY = 10;

	scsTime = 90 * 60;
	scsCount = 0;

	PresetBGposition(0, 0);
	MxFilledRect(&sysCp, 0);
	clutTransMask = 1;
}

void MxInit(void)
{
	MxBaseInit();

	MxFilledRect(&sysCp, winColIdx[5]);
}

void MxInitWindow(void)
{
	int	i;
	WINDOW	*win;

	MxInit();

	mouseX = winWidth / 2;
	mouseY = winHeight / 2;
	mxFlag = 1;

	winMax = WINDOW_MAX;
	winTop = win = &winBuff[0];
	winUse = 0;
	for(i=0; i<WINDOW_MAX; ++i)
	{
	win->part = NULL;
	win->prio = -1;
	win++;
	}
	msMobj = PresetBitMapImage(&mousePart[0], 16, 16, winColIdx[6]);
	winModeOld = winMode = WIN_MODE_NORMAL;
}

WINDOW *MxOpenWindow(PART *pa)
{
	int	i, j, xt, yt, xs, ys;
	WINDOW	*win;

	xt = pa->rect.xt;
	yt = pa->rect.yt;
	xs = pa->rect.xb;
	ys = pa->rect.yb;
	if(yt==LOCATION_CENTER) yt = (winHeight - ys) / 2;
	else if(yt==LOCATION_AUTO && (pa->type==PART_ICON_TEXT || pa->type==PART_ICON_IMAGE))
		yt = iconPosY;
	if(xt==LOCATION_CENTER) xt = (winWidth - xs) / 2;
	else if(xt==LOCATION_AUTO && (pa->type==PART_ICON_TEXT || pa->type==PART_ICON_IMAGE))
		{ xt = iconPosX;
		iconPosX += xs + 10;
		if(iconPosX >= winHeight) { iconPosX = 10; iconPosY += 32+10; }
		}
	if(xs<=0 || ys<=0 || winUse>=(winMax-1)) return NULL;

	j = -1;
	win = winTop;
	for(i=0; i<winMax; ++i)
	{
	if(win->part!=NULL) win->prio++;
	else if(win->part==NULL && j<0) j = i;
	win++;
	}
	if(j < 0) return NULL;
	win = winTop + j;
	win->part = pa;
	win->prio = 0;
	win->org.xt = pa->rect.xt = xt;
	win->org.yt = pa->rect.yt = yt;
	win->org.xb = pa->rect.xb = xt + xs;
	win->org.yb = pa->rect.yb = yt + ys;
	DrawWinParts(win);
	winUse++;
	ReScanWindowTag();
	return win;
}

void MxRefreshWindow(void)
{
	int	i, r, g, b;
	const uint8_t *c;
	WINDOW	*win;

	c = &winColVal[0];
	for(i=0; i<WIN_COLOR_MAX; ++i)
	{
	winColIdx[i] = i;
	r = *c++;
	g = *c++;
	b = *c++;
	SetCLUT(i, r, g, b);
	SetCLUT(i+256, r, g, b);
	}
	MxFilledRect(&sysCp, winColIdx[5]);
	msMobj = PresetBitMapImage(&mousePart[0], 16, 16, winColIdx[6]);

	win = &winBuff[0];
	for(i=0; i<WINDOW_MAX; ++i)
	{
/*	if(win->part!=NULL && win!=launchWin) DrawWinParts(win); */
	if(win->part!=NULL) DrawWinParts(win);
	win++;
	}
}

void MxCloseWindow(WINDOW *win)
{
	int	i;

	EraseWindow(win);
	win->part = NULL;
	win->prio = -1;
	winUse--;
}

void ServiceWindowSystem(void)
{
	int	i, j, x, y, md, ml, f;
	RECT	*rct, box, org;
	WINDOW	*win;

	f = 0;
	ml = mouse.b & MOUSE_LEFT_LEVEL;
	md = mouse.b & MOUSE_LEFT_DOWN;
	if(winMode==WIN_MODE_RELOCATE)
	{
	rct = &relocateWin->part->rect;
	x = rct->xb - rct->xt;
	y = rct->yb - rct->yt;
	org.xt = rct->xt;
	org.yt = rct->yt;
	org.xb = rct->xb;
	org.yb = rct->yb;
	rct->xt = mouse.x - relocateX;
	rct->yt = mouse.y - relocateY;
	rct->xb = rct->xt + x;
	rct->yb = rct->yt + y;
	f = 1;
	}
	else if(winMode==WIN_MODE_RESIZE)
	{
	rct = &relocateWin->part->rect;
	org.xt = rct->xt;
	org.yt = rct->yt;
	org.xb = rct->xb;
	org.yb = rct->yb;
	rct->xb += mouse.x - relocateX;
	rct->yb += mouse.y - relocateY;
	if((rct->xt + WIN_MIN_X) > rct->xb) rct->xb = rct->xt + WIN_MIN_X;
	if((rct->yt + WIN_MIN_Y) > rct->yb) rct->yb = rct->yt + WIN_MIN_Y;
	relocateX = mouse.x;
	relocateY = mouse.y;
	f = 1;
	}
	else if(winMode==WIN_MODE_RELEASE && ml==0) winMode = WIN_MODE_NORMAL;

	if(f)
	{
/*	if(winMode!=winModeOld)
		{
		MakeupWindowArea(relocateWin->part->type, &org, &box);
		SaveWireFrames(&box);
		DrawWireFrames(&box);
		}
	else if(org.xt!=rct->xt || org.yt!=rct->yt ||
				org.xb!=rct->xb || org.yb!=rct->yb)
		{
		MakeupWindowArea(relocateWin->part->type, &org, &box);
		LoadWireFrames(&box);
		MakeupWindowArea(relocateWin->part->type, rct, &box);
		SaveWireFrames(&box);
		DrawWireFrames(&box);
		}
*/
	MakeupWindowArea(relocateWin->part->type, rct, &box);
	DrawWireFrames(&box);

	if(md)	{
		winMode = WIN_MODE_RELEASE;
		j = relocateWin->prio;
		if(j)	{
			win = winTop;
			for(i=0; i<winMax; ++i)
				{
				if(win->prio>=0 && win->prio < j) win->prio++;
				win++;
				}
			}
		relocateWin->prio = 0;
		DrawWinParts(relocateWin);
		ReScanWindowTag();
		}
	}
	winModeOld = winMode;
}

static void EraseWindow(WINDOW *win)
{
	int	i, j, xt, yt, xb, yb;
	WINDOW	*top, *tag[WINDOW_MAX];
	RECT	rect, zone;

	plotGo = MxPlot;
	MakeupWindowArea(win->part->type, &win->part->rect, &rect);
	if(rect.xt < 0) sysCp.xt = 0; else sysCp.xt = rect.xt;
	if(rect.yt < 0) sysCp.yt = 0; else sysCp.yt = rect.yt;
	if(rect.xb > winWidth) sysCp.xb = winWidth; else sysCp.xb = rect.xb;
	if(rect.yb > winHeight) sysCp.yb = winHeight; else sysCp.yb = rect.yb;
	MxFilledRect(&rect, winColIdx[5]);
	j = winUse;
	for(i=0; i<winUse; ++i)
	{
	top = winTag[--j];
	if(top->prio>=0 && top!=win)
		{
		MakeupWindowArea(top->part->type, &top->part->rect, &zone);
		MakeupSystemClips(&zone);
		xt = zone.xt - (rect.xb - rect.xt);
		yt = zone.yt - (rect.yb - rect.yt);
		xb = zone.xb;
		yb = zone.yb;
		if(xt <= rect.xt && xb > rect.xt &&
			yt <= rect.yt && yb > rect.yt) DrawWinPartsUser(top);
		}
	}
	sysCp.xt = 0;
	sysCp.yt = 0;
	sysCp.xb = winWidth;
	sysCp.yb = winHeight;
}

static void ReScanWindowTag(void)
{
	int	i, j;
	WINDOW	*win;

	for(j=0; j<winUse; ++j)
	{
	win = winTop;
	for(i=0; i<winMax; ++i)
		{
		if(j==win->prio) { winTag[j] = win; break; }
		win++;
		}
	}
}

void ServiceMouse(void)
{
	int	i, j, k;

	i = mouseT; while(i==mouseT) ;
	if(mouse.x!=mouseX || mouse.y!=mouseY || mouse.b!=mouseB) scsCount = 0;
	mouse.x = mouseX;
	mouse.y = mouseY;
	msMobj->x = mouse.x;
	msMobj->y = mouse.y;
	if(msMobj!=NULL) SetupObject(msMobj);
	i = mouse.b;
	mouse.b = mouseB;
	j = (0xffff ^ i) & mouse.b;
	k = i & (0xffff ^ mouse.b);
	mouse.b &= MOUSE_LEVEL | MOUSE_LAUNCH;
	if(j & MOUSE_LEFT_LEVEL) mouse.b |= MOUSE_LEFT_DOWN;
	if(k & MOUSE_LEFT_LEVEL) mouse.b |= MOUSE_LEFT_UP;
	if(j & MOUSE_MIDDLE_LEVEL) mouse.b |= MOUSE_MIDDLE_DOWN;
	if(k & MOUSE_MIDDLE_LEVEL) mouse.b |= MOUSE_MIDDLE_UP;
	if(j & MOUSE_RIGHT_LEVEL) mouse.b |= MOUSE_RIGHT_DOWN;
	if(k & MOUSE_RIGHT_LEVEL) mouse.b |= MOUSE_RIGHT_UP;
}

void ServiceScreenSaver(void)
{
	short	mx, my, mb;
	int	i, j, k, l, m;
	int	xn[256], yn[256], xs[256], ys[256], max;
	int	col, sin, cos;

	max = 256;
	if(scsCount >= scsTime)
	{
	mx = mouseX;
	my = mouseY;
	mb = mouseB;
	MxFilledRect(&sysCp, 0);
	col = 16;

	m = 0;
	for( ; ; )
		{
		CloseMotionObject();
		SyncBlanking();
		OpenMotionObject(0, 0);
	switch(m)
	{
	case 0:
		j = Rand() % (winWidth / 2);
		j = (winWidth / 4 + j) << 12;
		k = -((Rand() & 1023) + 10800);
		l = Rand() & 2047;
		for(i=0; i<max; ++i)
		{	
		xn[i] = j;
		yn[i] = winHeight << 12;
		xs[i] = l;
		ys[i] = k; }
		m++;
		break;
	case 1:
		SetCLUT(col+256, 255, 78, 0);
		SetCLUT(col+256+1, 0, 58, 255);
		m++;
		break;
	case 2:
		if(ys[0] >= 0) m++;
		break;
	case 3:
		sin = 3196; cos = 16069;
		for(k=0; k<max; ++k)
		{
		if((k & 31)==0)
			{ i = 330 * ((k >> 5) + 1);
			j = Rand() & 63; }
		if((k >> 5) < 3)
		{
		xs[k] += i + ((Rand() & 127) - 64) * ((k >> 5) + 1);
		ys[k] += j + ((Rand() & 127) - 64) * ((k >> 5) + 1);
		}
		else
		{
		xs[k] += i;
		ys[k] += j;
		}
		l = (i * cos + j * sin) >> 14;
		j = (j * cos - i * sin) >> 14;
		i = l;
		}
		l = 0;
		m++;
		k = 256;
		break;
	case 4:
		l++;
		if(l >= (4 * 60)) m = 0;
		if(k > 0) k-=2;
		else k=0;
		SetCLUT(col+256, (255*k)>>8, (78*k)>>8, 0);
		SetCLUT(col+256+1, 0, (58*k)>>8, (255*k)>>8);
		break;
	default:
		break;
	}
	for(i=0; i<max; ++i)
		{ if((i >> 5) < 3) j = col + 1;
		else j = col;
		SetupObjectLine(xn[i]>>12, yn[i]>>12, xn[i]>>12, (yn[i]>>12)+1, j);
		xn[i] += xs[i];
		yn[i] += ys[i];
		ys[i] += 45; }
		if(mx!=mouseX || my!=mouseY || mb!=mouseB) break;
		}
	MxRefreshWindow();
	scsCount = 0;
	}
	else scsCount++;
}

/*------------------------------------------------------/
/							/
/	Window Parts Reaction.				/
/							/
/------------------------------------------------------*/
int GetStickPosition(int a)
{
	int	i;

	for(i=0; i<32; ++i)
	{
	if(a & 1) return i;
	a >>= 1;
	}
	return -1;
}

static int GetStringCount(char *ptr)
{
	int	i, c;

	i = 0;
	while((c = *ptr++) != 0) { if(c=='\n') i++; }
	return i;
}

static int PositioningText(SLIDER *sld, int x, int y)
{
	char	c;
	int	i, j, f;

	f = 0;
	i = ((sld->sy - y) * sld->py) >> 16;
	i >>= 4;
	if(sld->oy!=i)
	{
	sld->pos = sld->top;
	sld->oy = i;
	while(i) { if(*sld->pos++=='\n') i--; }
	f = 1;
	}
	return f;
}

static void MakeupSliderStruct(SLIDER *sld, char *txt)
{
	if(txt==NULL)
		{ sld->px = sld->py = 0; return; }
	if(sld->pos!=NULL) return;
	else sld->pos = txt;
	ScanTextSize(txt, &sld->sx, &sld->sy);
	sld->top = txt;
}

static void MakeupWindowArea(int type, RECT *src, RECT *dst)
{
	switch(type)
	{
	case PART_HANDLE_BASE:
		dst->xt = src->xt - (2 + 3 + 2);
		dst->yt = src->yt - (2 + 3 + 2 + 16 + 2);
		dst->xb = src->xb + (2 + 3 + 2);
		dst->yb = src->yb + (2 + 3 + 2);
		break;
	case PART_FRAME_BASE:
		dst->xt = src->xt - (2 + 3 + 2);
		dst->yt = src->yt - (2 + 3 + 2);
		dst->xb = src->xb + (2 + 3 + 2);
		dst->yb = src->yb + (2 + 3 + 2);
		break;
	case PART_POSITIVE_BOX:
	case PART_NEGATIVE_BOX:
	case PART_IMAGE_BOX:
	case PART_FRAME_BOX:
	case PART_TOUCH_SWITCH:
	case PART_PUSH_SWITCH:
	case PART_ICON_TEXT:
	case PART_ICON_IMAGE:
		dst->xt = src->xt - 2;
		dst->yt = src->yt - 2;
		dst->xb = src->xb + 2;
		dst->yb = src->yb + 2;
		break;
	case PART_VERTICAL_SLIDER:
	case PART_HORIZONTAL_SLIDER:
		dst->xt = src->xt;
		dst->yt = src->yt;
		dst->xb = src->xb;
		dst->yb = src->yb;
		break;
	default:
		dst->xt = src->xt;
		dst->yt = src->yt;
		dst->xb = src->xb;
		dst->yb = src->yb;
		break;
	}
}

static void MakeupSystemClips(RECT *rtc)
{
	if(rtc->xt < 0) rtc->xt = 0;
	if(rtc->yt < 0) rtc->yt = 0;
	if(rtc->xb > winWidth) rtc->xb = winWidth;
	if(rtc->yb > winHeight) rtc->yb = winHeight;
}

void ServiceReactionWindow(void)
{
	int	fo, ml, mu, md;
	int	i, j, k, l, f, a, b, c;
	RECT	rtc, clp, org, rct;
	PART	*ptr, *pts;
	SLIDER	*sld;
	WINDOW	*tag[WINDOW_MAX], *win;

	if(winMode!=WIN_MODE_NORMAL) return;

	ReScanWindowTag();
	f = 0;
	ml = mouse.b & MOUSE_LEFT_LEVEL;
	mu = mouse.b & MOUSE_LEFT_UP;
	md = mouse.b & MOUSE_LEFT_DOWN;
	for(k=0; k<winUse; ++k)
	{
	win = winTag[k];
	ptr = win->part;
	rtc.xt = org.xt = ptr->rect.xt;
	rtc.yt = org.yt = ptr->rect.yt;
	rtc.xb = org.xb = ptr->rect.xb;
	rtc.yb = org.yb = ptr->rect.yb;
	MakeupWindowArea(ptr->type, &rtc, &sysCp);
	MakeupSystemClips(&sysCp);
	for( ; ; )
	    {
	a = ScanWindowZone(&rtc, ptr->type, mouse.x, mouse.y);
	f |= a;
	fo = ptr->flag;
	switch(ptr->type)
		{
	case PART_TEXT_BOX:
		break;
	case PART_IMAGE_BOX:
		if(a) if(ptr->run!=NULL) (ptr->run)();
		break;
	case PART_HANDLE_BASE:
	case PART_FRAME_BASE:
		if((a & 3)==1 && mu!=0)
			{
			if((ptr->flag & BASE_WITH_RESIZE)!=0 && (a & 0x10)!=0)
				{
				winMode = WIN_MODE_RESIZE;
				relocateX = mouse.x;
				relocateY = mouse.y;
				}
			else	{
				winMode = WIN_MODE_RELOCATE;
				relocateX = mouse.x - win->part->rect.xt;
				relocateY = mouse.y - win->part->rect.yt;
				}
			relocateWin = win;
			EraseWindow(win);
			}
		break;
	case PART_HORIZONTAL_SLIDER:
		if(a!=0 && ml!=0)
		{
		i = rtc.xb - rtc.xt;
		j = rtc.yb - rtc.yt;
		if(rtc.xt <= mouse.x && (rtc.xt+j) > mouse.x) l = -8;
		else if(rtc.xb > mouse.x && (rtc.xb-j) <= mouse.x) l = 8;
		else l = 0;
		sld = (SLIDER *)ptr->ext;
		if(l!=0 && sld!=NULL && sld->sx > i)
			{
			sld->px += (l << 16) / (sld->sx - i);
			if(sld->px > 65536) sld->px = 65536;
			else if(sld->px < 0) sld->px = 0;
			ptr->flag++;
/////			if(PositioningText(sld, i, j));
			PositioningText(sld, i, j);
				{
				pts = (PART *)ptr->image;
				MakeupPartRect(&org, &pts->rect, &rct);
				MakeupWindowArea(pts->type, &rct, &clp);
				MakeupSystemClips(&clp);
				MakeupUnderClips(&clp, k, -1);
				DrawOnlyPart(pts, &rct);
				}
			}
		}
		break;
	case PART_VERTICAL_SLIDER:
		if(a!=0 && ml!=0)
		{
		i = rtc.xb - rtc.xt;
		j = rtc.yb - rtc.yt;
		if(rtc.yt <= mouse.y && (rtc.yt+i) > mouse.y) l = -8;
		else if(rtc.yb > mouse.y && (rtc.yb-i) <= mouse.y) l = 8;
		else l = 0;
		sld = (SLIDER *)ptr->ext;
		if(l!=0 && sld!=NULL && sld->sy > j)
			{
			sld->py += (l << 16) / (sld->sy - j);
			if(sld->py > 65536) sld->py = 65536;
			else if(sld->py < 0) sld->py = 0;
			ptr->flag++;
/////			if(PositioningText(sld, i, j));
			PositioningText(sld, i, j);
				{
				pts = (PART *)ptr->image;
				MakeupPartRect(&org, &pts->rect, &rct);
				MakeupWindowArea(pts->type, &rct, &clp);
				MakeupSystemClips(&clp);
				MakeupUnderClips(&clp, k, -1);
				DrawOnlyPart(pts, &rct);
				}
			}
		}
		break;
	case PART_ICON_TEXT:
	case PART_ICON_IMAGE:
		if((a & 3)==1 && mouse.b & MOUSE_RIGHT_DOWN)
		{
		EraseWindow(win);
		launchWin = win;
		mxFlag = 0;
		LaunchApplicationDemo(&win->part->rect, 0);
		if(ptr->run!=NULL) (ptr->run)();
		LaunchApplicationDemo(&win->part->rect, 1);
		mxFlag = 1;
		DrawWinParts(win);
		}
		else if((a & 3)==1 && mu!=0)
		{
		winMode = WIN_MODE_RELOCATE;
		relocateX = mouse.x - win->part->rect.xt;
		relocateY = mouse.y - win->part->rect.yt;
		relocateWin = win;
		EraseWindow(win);
		}
		break;
	case PART_TEXT_CHOOSER:
		if(ptr->image!=NULL && a)
		{
		if(ml)	{
			sld = (SLIDER *)ptr->ext;
			if(sld!=NULL)
				{ j = rtc.yb - rtc.yt;
				if(sld->sy >= j)
					i = (sld->py * (sld->sy - j)) >> 16;
				else i = 0;
				i >>= 4; }
			else i = 0;
			ptr->flag = i + ((mouse.y - rtc.yt) >> 4);
			}
		else if(mu) ptr->action = ptr->flag;
/*		else if(md) if(ptr->run!=NULL) (ptr->run)(); */
		}
		break;
	case PART_PUSH_SWITCH:
	case PART_TOUCH_SWITCH:
		if(a)
		{
		if(ml) ptr->flag = 1; else ptr->flag = 0;
		if(md) if(ptr->run!=NULL) (ptr->run)();
		}
		else ptr->flag = 0;
		break;	
	default:
		break;
		}
	if(fo!=ptr->flag)
		{
		MakeupWindowArea(ptr->type, &rtc, &clp);
		MakeupSystemClips(&clp);
		MakeupUnderClips(&clp, k, -1);
		DrawOnlyPart(ptr, &rtc);
		}
	if((ptr = (void *)ptr->next)==NULL) break;
	MakeupPartRect(&org, &ptr->rect, &rtc);
	win = winTag[k];
	sysCp.xt = win->part->rect.xt;
	sysCp.yt = win->part->rect.yt;
	sysCp.xb = win->part->rect.xb;
	sysCp.yb = win->part->rect.yb;
	MakeupSystemClips(&sysCp);
		}
	if(f) break;
	}
	plotGo = MxPlot;
	sysCp.xt = 0;
	sysCp.yt = 0;
	sysCp.xb = winWidth;
	sysCp.yb = winHeight;
}

static void MakeupUnderClips(RECT *cp, int prio, int skip)
{
	int	i, j, m, n, u, v;
	WINDOW	*win;

	if(prio)
	{
	j = 0;
	for(i=0; i<prio; ++i)
		{
		if(i!=skip)
			{
		win = winTag[i];
		MakeupWindowArea(win->part->type, &win->part->rect, &udrCp[j]);
		MakeupSystemClips(&udrCp[j]);
		m = udrCp[j].xt - (cp->xb - cp->xt);
		n = udrCp[j].yt - (cp->yb - cp->yt);
		u = udrCp[j].xb;
		v = udrCp[j].yb;
		if(m <= cp->xt && u > cp->xt && n <= cp->yt && v > cp->yt) j++;
			}
		}
	if(j) { plotGo = MxPlotClip; udrMax = j; }
	else plotGo = MxPlot;
	}
	else plotGo = MxPlot;
}

static int ScanWindowZone(RECT *rtc, int type, int x, int y)
{
	int	c;
	RECT	z, a;

	MakeupWindowArea(type, rtc, &z);
	MakeupSystemClips(&z);
	if(z.xt <= x && z.xb > x && z.yt <= y && z.yb > y)
	{
	c = 1;
/*	a.xt = z.xt;
	a.yt = z.yt;
	a.xb = z.xt + 16 + 8;
	a.yb = z.yt + 16 + 8;
	if(a.xt <= x && a.xb > x && a.yt <= y && a.yb > y) c |= 0x10;
	a.xt = z.xb - 16 - 8;
	a.yt = z.yt;
	a.xb = z.xb;
	a.yb = z.yt + 16 + 8;
	if(a.xt <= x && a.xb > x && a.yt <= y && a.yb > y) c |= 0x14;
	a.xt = z.xt;
	a.yt = z.yb - 16 - 8;
	a.xb = z.xt + 16 + 8;
	a.yb = z.yb;
	if(a.xt <= x && a.xb > x && a.yt <= y && a.yb > y) c |= 0x18;*/
	a.xt = z.xb - 16 - 8;
	a.yt = z.yb - 16 - 8;
	a.xb = z.xb;
	a.yb = z.yb;
	if(a.xt <= x && a.xb > x && a.yt <= y && a.yb > y) c |= 0x1c;
	}
	else c = 0;
	if(type==PART_HANDLE_BASE || type==PART_FRAME_BASE)
	{
	if(rtc->xt <= x && rtc->xb > x && rtc->yt <= y && rtc->yb > y) c |= 2;
	}
	return c;
}

/*------------------------------------------------------/
/	Window Parts Rendering.				/
/------------------------------------------------------*/
static void MakeupPartRect(RECT *org, RECT *src, RECT *dst)
{
	int	xs, ys;

	xs = org->xb - org->xt;
	ys = org->yb - org->yt;
	if(src->xt==LOCATION_CENTER) dst->xt = ((xs - src->xb) / 2) + org->xt;
	else if(src->xt < 0) dst->xt = org->xb + src->xt;
	else dst->xt = org->xt + src->xt;
	if(src->yt==LOCATION_CENTER) dst->yt = ((ys - src->yb) / 2) + org->yt;
	else if(src->yt < 0) dst->yt = org->yb + src->yt;
	else dst->yt = org->yt + src->yt;
	if(src->xb < 0) dst->xb = org->xb + src->xb;
	else dst->xb = dst->xt + src->xb;
	if(src->yb < 0) dst->yb = org->yb + src->yb;
	else dst->yb = dst->yt + src->yb;
}

static void DrawWinParts(WINDOW *win)
{
	int	i;
	PART	*prt;
	RECT	rtc, org;

	prt = win->part;
	rtc.xt = org.xt = prt->rect.xt;
	rtc.yt = org.yt = prt->rect.yt;
	rtc.xb = org.xb = prt->rect.xb;
	rtc.yb = org.yb = prt->rect.yb;
	MakeupWindowArea(prt->type, &rtc, &sysCp);
	MakeupSystemClips(&sysCp);
	for( ; ; )
	{
	DrawOnlyPart(prt, &rtc);
	if((prt = (PART *)prt->next)==NULL) break;
	MakeupPartRect(&org, &prt->rect, &rtc);
	sysCp.xt = org.xt;
	sysCp.yt = org.yt;
	sysCp.xb = org.xb;
	sysCp.yb = org.yb;
	if(sysCp.xt < 0) sysCp.xt = 0;
	if(sysCp.yt < 0) sysCp.yt = 0;
	if(sysCp.xb > winWidth) sysCp.xb = winWidth;
	if(sysCp.yb > winHeight) sysCp.yb = winHeight;
	}
	sysCp.xt = 0;
	sysCp.yt = 0;
	sysCp.xb = winWidth;
	sysCp.yb = winHeight;
}

static void DrawWinPartsUser(WINDOW *win)
{
	PART	*prt;
	RECT	rtc, org;

	prt = win->part;
	rtc.xt = org.xt = prt->rect.xt;
	rtc.yt = org.yt = prt->rect.yt;
	rtc.xb = org.xb = prt->rect.xb;
	rtc.yb = org.yb = prt->rect.yb;
	for( ; ; )
	{
	DrawOnlyPart(prt, &rtc);
	if((prt = (PART *)prt->next)==NULL) break;
	MakeupPartRect(&org, &prt->rect, &rtc);
	}
}

static void DrawOnlyPart(PART *ptr, RECT *rtc)
{
	int	w, h, s, i, j;
	RECT	rect;
	char	*txt;
	SLIDER	*sld;
	PART	*pts;

	w = rtc->xb - rtc->xt;
	h = rtc->yb - rtc->yt;
	switch(ptr->type)
	{
	case PART_POSITIVE_BOX:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawPositiveFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		DrawWinText(rtc, (char *)ptr->image, ptr->flag, 0);
		break;
	case PART_NEGATIVE_BOX:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		DrawWinText(rtc, (char *)ptr->image, ptr->flag, 0);
		break;
	case PART_IMAGE_BOX:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		MxFilledRect(rtc, winColIdx[0]);
		break;
	case PART_FRAME_BOX:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawWinFrames(rtc);
		break;
	case PART_HANDLE_BASE:
		rtc->xt -= 2 + 3 + 2; rtc->yt -= 2 + 3 + 2 + 16 + 2;
		rtc->xb += 2 + 3 + 2; rtc->yb += 2 + 3 + 2;
		DrawPositiveFrame(rtc);
		rtc->xt += 5; rtc->yt += 5;
		rtc->xb -= 5; rtc->yb -= 5;
		DrawWinFrames(rtc);
		rtc->xt -= 3; rtc->yt -= 3;
		rtc->xb += 3; rtc->yb += 3;
		DrawBaseFrame(rtc, ptr->flag);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		rect.xt = rtc->xt;
		rect.yt = rtc->yt;
		rect.xb = rtc->xb;
		rect.yb = rect.yt + 16;
		DrawWinText(&rect, (char *)ptr->image, ptr->flag, 0);
		rtc->yt += 16;
		DrawWinSeparateH(rtc);
		rtc->yt += 2;
		MxFilledRect(rtc, winColIdx[2]);
		break;
	case PART_FRAME_BASE:
		rtc->xt -= 2 + 3 + 2; rtc->yt -= 2 + 3 + 2;
		rtc->xb += 2 + 3 + 2; rtc->yb += 2 + 3 + 2;
		DrawPositiveFrame(rtc);
		rtc->xt += 5; rtc->yt += 5;
		rtc->xb -= 5; rtc->yb -= 5;
		DrawWinFrames(rtc);
		rtc->xt -= 3; rtc->yt -= 3;
		rtc->xb += 3; rtc->yb += 3;
		DrawBaseFrame(rtc, ptr->flag);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		MxFilledRect(rtc, winColIdx[2]);
		break;
	case PART_PUSH_SWITCH:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		if(ptr->flag) DrawNegativeFrame(rtc);
		else DrawPositiveFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		DrawWinText(rtc, (char *)ptr->image, ptr->flag, 0);
		break;
	case PART_TOUCH_SWITCH:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawWinFrames(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		DrawWinText(rtc, (char *)ptr->image, ptr->flag, 0);
		break;
	case PART_TEXT_CHOOSER:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		sld = (SLIDER *)ptr->ext;
		if(sld!=NULL)
			{ MakeupSliderStruct(sld, ptr->image);
			txt = sld->pos;
			if(sld->sy >= h)
				i = (sld->py * (sld->sy - h)) >> 16;
			else i = 0;
			i >>= 4; }
		else txt = (char *)ptr->image;
		j = ptr->flag - i;
		if(j>=0 && j<(h >> 4)) i = 1 << j; else i = 0; 
		DrawWinText(rtc, txt, i, 0);
		break;
	case PART_TEXT_BOX:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		sld = (SLIDER *)ptr->ext;
		if(sld!=NULL)
			{ MakeupSliderStruct(sld, ptr->image);
			txt = sld->pos; }
		else txt = (char *)ptr->image;
		DrawWinText(rtc, txt, 0, 0);
		break;
	case PART_VERTICAL_SLIDER:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		h = rtc->yb - rtc->yt - 4;
		DrawSliderParts(rtc->xt+2, rtc->yt+2, SLIDER_UP_PART);
		DrawSliderParts(rtc->xt+2, rtc->yb-2-10, SLIDER_DOWN_PART);
		rtc->xt += 2; rtc->yt += 2 + w;
		rtc->xb -= 2; rtc->yb -= 2 + w;
		MxFilledRect(rtc, winColIdx[2]);
		rect.xt = rtc->xt;
		rect.xb = rtc->xb;
		s = rtc->yb - rtc->yt;
		i = 0;
		j = s;
		sld = (SLIDER *)ptr->ext;
		if(sld!=NULL)
		{
		pts = (PART *)ptr->image;
		MakeupSliderStruct(sld, (char *)pts->image);
		if(sld->sy > h)
			{
			j = s * h / sld->sy;
			i = (sld->py * (s - j)) >> 16;
			}
		}
		rect.yt = rtc->yt + i;
		rect.yb = rtc->yt + i + j;
		DrawPositiveFrame(&rect);
		rect.xt += 2; rect.yt += 2;
		rect.xb -= 2; rect.yb -= 2;
		MxFilledRect(&rect, winColIdx[2]);
		break;
	case PART_HORIZONTAL_SLIDER:
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawNegativeFrame(rtc);
		w = rtc->xb - rtc->xt - 4;
		DrawSliderParts(rtc->xt+2, rtc->yt+2, SLIDER_LEFT_PART);
		DrawSliderParts(rtc->xb-2-10, rtc->yt+2, SLIDER_RIGHT_PART);
		rtc->xt += 2 + h; rtc->yt += 2;
		rtc->xb -= 2 + h; rtc->yb -= 2;
		MxFilledRect(rtc, winColIdx[2]);
		rect.yt = rtc->yt;
		rect.yb = rtc->yb;
		s = rtc->xb - rtc->xt;
		i = 0;
		j = s;
		sld = (SLIDER *)ptr->ext;
		if(sld!=NULL)
		{
		pts = (PART *)ptr->image;
		MakeupSliderStruct(sld, (char *)pts->image);
		if(sld->sx > w)
			{
			j = s * w / sld->sx;
			i = (sld->px * (s - j)) >> 16;
			}
		}
		rect.xt = rtc->xt + i;
		rect.xb = rtc->xt + i + j;
		DrawPositiveFrame(&rect);
		rect.xt += 2; rect.yt += 2;
		rect.xb -= 2; rect.yb -= 2;
		MxFilledRect(&rect, winColIdx[2]);
		break;
	case PART_ICON_TEXT:
	case PART_ICON_IMAGE:
		if(ptr->next!=NULL) DrawIconImage(rtc, (char *)ptr->next);
		else {
		rtc->xt -= 2; rtc->yt -= 2;
		rtc->xb += 2; rtc->yb += 2;
		DrawPositiveFrame(rtc);
		rtc->xt += 2; rtc->yt += 2;
		rtc->xb -= 2; rtc->yb -= 2;
		MxFilledRect(rtc, winColIdx[2]);
		txt = (char *)ptr->image;
		rtc->xt += 8; rtc->yt += 8;
		if(txt!=NULL) { while( *txt ) { txt += DrawFont(rtc, txt, 0, 0);
		rtc->xt += fontWidth; } }
		}
		break;
	default:
		break;
	}
}

static void DrawIconImage(RECT *rtc, char *img)
{
	int	c, x, y, w, h;
	char	*ptr;

	w = *(unsigned char *)img++; w <<= 8;
	w |= *(unsigned char *)img++;
	h = *(unsigned char *)img++; h <<= 8;
	h |= *(unsigned char *)img++; 
	for(y=rtc->yt; y<rtc->yb; ++y)
	{
	if(y>=h) break;
	ptr = img;
	for(x=rtc->xt; x<rtc->xb; ++x)
		{
		c = *(unsigned char *)ptr++;
		if(c) (*plotGo)(x, y, c+WIN_COLOR_MAX);
		}
	img += w;
	}
}

static void DrawBaseFrame(RECT *rtc, int f)
{
	int	i;
	RECT	rect;

	for(i=0; i<3; ++i)
	{
	MxRect(rtc, winColIdx[2]);
	rtc->xt++; rtc->yt++;
	rtc->xb--; rtc->yb--;
	}
	if(f & BASE_WITH_RESIZE)
	{
	rect.xt = rtc->xt + 18; rect.yt = rtc->yt - 5;
	rect.xb = rtc->xt + 18; rect.yb = rtc->yt;
	DrawWinSeparateV(&rect);
	rect.xt = rtc->xb - 18; rect.yt = rtc->yt - 5;
	rect.xb = rtc->xb - 18; rect.yb = rtc->yt;
	DrawWinSeparateV(&rect);
	rect.xt = rtc->xt + 18; rect.yt = rtc->yb - 1;
	rect.xb = rtc->xt + 18; rect.yb = rtc->yb + 4;
	DrawWinSeparateV(&rect);
	rect.xt = rtc->xb - 18; rect.yt = rtc->yb - 1;
	rect.xb = rtc->xb - 18; rect.yb = rtc->yb + 4;
	DrawWinSeparateV(&rect);
	rect.xt = rtc->xt - 5; rect.yt = rtc->yt + 18;
	rect.xb = rtc->xt;     rect.yb = rtc->yt + 18;
	DrawWinSeparateH(&rect);
	rect.xt = rtc->xt - 5; rect.yt = rtc->yb - 20;
	rect.xb = rtc->xt;     rect.yb = rtc->yb - 20;
	DrawWinSeparateH(&rect);
	rect.xt = rtc->xb - 1; rect.yt = rtc->yt + 18;
	rect.xb = rtc->xb + 4; rect.yb = rtc->yt + 18;
	DrawWinSeparateH(&rect);
	rect.xt = rtc->xb - 1; rect.yt = rtc->yb - 20;
	rect.xb = rtc->xb + 4; rect.yb = rtc->yb - 20;
	DrawWinSeparateH(&rect);
	}
}

static char * ScanTextSize(char *ptr, int *x, int *y)
{
	unsigned char	c;
	int	xs, ys, xm;

	xs = 0;
	ys = 0;
	xm = 0;
	while((c = *(unsigned char *)ptr++) != 0)
	{
	if(c >= 0x20) xs += 8;
	else if(c=='\n') { ys += 16; xs = 0; }
	if(xm < xs) xm = xs;
	}
	*x = xm;
	*y = ys;
	return ptr;
}

static void DrawWinText(RECT *rt, char *img, int f, int xo)
{
	int	c, x, y, i, j;
	RECT	rtc, org;

	if(img==NULL) MxFilledRect(rt, winColIdx[2]);
	else
	{
	j = 0;
	rtc.xt = rt->xt;
	rtc.yt = rt->yt;
	rtc.xb = rt->xb;
	rtc.yb = rt->yt + 16;
	while((c = *(unsigned char *)img) != 0)
		{
		if(rt->yb <= rtc.yt) break;
		if(c < ' ')
			{
			img++;
			if(c==0x01) {
				ScanTextSize(img, &x, &y);
				rtc.xt = rt->xt + ((rt->xb - rt->xt - x) / 2);
				org.xt = rt->xt;
				org.yt = rtc.yt;
				org.xb = rtc.xt;
				org.yb = rtc.yb;
				if(f & 1) c = winColIdx[4];
				else c = winColIdx[2];
				if(org.xt < org.xb) MxFilledRect(&org, c);
				org.xt = rtc.xt + x;
				org.xb = rt->xb;
				if(org.xt < org.xb) MxFilledRect(&org, c);
				}
			else if(c=='\n') {
				if(rtc.xt >= rtc.xb || rtc.yt >= rtc.yb) ;
				else	{
					if(f & 1) c = winColIdx[4];
					else c = winColIdx[2];
					MxFilledRect(&rtc, c);
					}
				rtc.xt = rt->xt;
				rtc.yt += 16;
				rtc.yb = rtc.yt + 16;
				if(rtc.yb > rt->yb) rtc.yb = rt->yb;
				f >>= 1;
				j = 0;
				}
			}
		else if(rtc.xt < rtc.xb)
			{ img += DrawFont(&rtc, img, f, xo); rtc.xt += fontWidth; }
		else { if(GetSjisToKanjiAdr(img) < 0) { img++; rtc.xt += 8; }
						else { img += 2; rtc.xt += 16; }
			}
		}
	}
}

static int DrawFont(RECT *cp, char *ch, int f, int xo)
{
	int	i, j, k, x, y, w, h, r, p, o;
	unsigned int	m, *ptr;

	if(f & 1) { i = winColIdx[2]; j = winColIdx[4]; }
	else { i = winColIdx[4]; j = winColIdx[2]; }

	if((r=GetSjisToKanjiAdr(ch)) < 0)
	{
	fontWidth = 8;
	r = *(unsigned char *)ch;
	ptr = (unsigned int *)&fontMap[r << 4];
	r = 1;
	o = 4;
	}
	else
	{
	fontWidth = 16;
	ptr = (unsigned int *)(kanjiFont + (r << 5));
	r = 2;
	o = 2;
	}

	p = 0;
	k = 0x80000000;
	w = cp->xt + fontWidth;
	h = cp->yt + 16;
	for(y=cp->yt; y<h; ++y)
	{
	if(p==0) { m = *ptr++; p = o; }
	if(cp->yb > y) for(x=cp->xt; x<w; ++x)
		{
		if(cp->xb > x)
			{
			if(m & k) (*plotGo)(x, y, i);
			else (*plotGo)(x, y, j);
			}
		m += m;
		}
	p--;
	}
	return r;
}

static void DrawSliderParts(int xt, int yt, int dir)
{
	int	x, y, xb, yb;
	const uint8_t *p;

	xb = xt + 10;
	yb = yt + 10;
	p = &sliderParts[dir * 10 * 10];
	for(y=yt; y<yb; ++y)
		for(x=xt; x<xb; ++x)
			(*plotGo)(x, y, winColIdx[ *p++ ]);
}

static void DrawWireFrames(RECT *rt)
{
	int	i, c, x, y;

	c = winColIdx[4];
	x = (rt->xb - rt->xt) / 3;
	y = (rt->yb - rt->yt) / 3;
	SetupObjectLine(rt->xt, rt->yt+0, rt->xb-1, rt->yt+0, c);
	SetupObjectLine(rt->xt, rt->yt+1, rt->xb-1, rt->yt+1, c);
	SetupObjectLine(rt->xt, rt->yt+y, rt->xb-1, rt->yt+y, c);
	SetupObjectLine(rt->xt, rt->yt+y+1, rt->xb-1, rt->yt+y+1, c);
	SetupObjectLine(rt->xt, rt->yb-y, rt->xb-1, rt->yb-y, c);
	SetupObjectLine(rt->xt, rt->yb-y-1, rt->xb-1, rt->yb-y-1, c);
	SetupObjectLine(rt->xt, rt->yb-1, rt->xb-1, rt->yb-1, c);
	SetupObjectLine(rt->xt, rt->yb-2, rt->xb-1, rt->yb-2, c);
	SetupObjectLine(rt->xt+0, rt->yt, rt->xt+0, rt->yb-1, c);
	SetupObjectLine(rt->xt+1, rt->yt, rt->xt+1, rt->yb-1, c);
	SetupObjectLine(rt->xt+x, rt->yt, rt->xt+x, rt->yb-1, c);
	SetupObjectLine(rt->xt+x+1, rt->yt, rt->xt+x+1, rt->yb-1, c);
	SetupObjectLine(rt->xb-x, rt->yt, rt->xb-x, rt->yb-1, c);
	SetupObjectLine(rt->xb-x-1, rt->yt, rt->xb-x-1, rt->yb-1, c);
	SetupObjectLine(rt->xb-1, rt->yt, rt->xb-1, rt->yb-1, c);
	SetupObjectLine(rt->xb-2, rt->yt, rt->xb-2, rt->yb-1, c);
}

static void DrawPositiveFrame(RECT *rt)
{
	int	c1, c2;

	c1 = winColIdx[3];
	c2 = winColIdx[1];
	MxLine(rt->xt+1, rt->yt+0, rt->xb-1, rt->yt+0, c1);
	MxLine(rt->xb-1, rt->yt+1, rt->xb-1, rt->yb-1, c2);
	MxLine(rt->xt+0, rt->yb-1, rt->xb-2, rt->yb-1, c2);
	MxLine(rt->xt+0, rt->yt+0, rt->xt+0, rt->yb-2, c1);
	MxLine(rt->xt+2, rt->yt+1, rt->xb-2, rt->yt+1, c1);
	MxLine(rt->xb-2, rt->yt+2, rt->xb-2, rt->yb-2, c2);
	MxLine(rt->xt+1, rt->yb-2, rt->xb-3, rt->yb-2, c2);
	MxLine(rt->xt+1, rt->yt+1, rt->xt+1, rt->yb-3, c1);
}

static void DrawNegativeFrame(RECT *rt)
{
	int	c1, c2;

	c1 = winColIdx[1];
	c2 = winColIdx[3];
	MxLine(rt->xt+1, rt->yt+0, rt->xb-1, rt->yt+0, c1);
	MxLine(rt->xb-1, rt->yt+1, rt->xb-1, rt->yb-1, c2);
	MxLine(rt->xt+0, rt->yb-1, rt->xb-2, rt->yb-1, c2);
	MxLine(rt->xt+0, rt->yt+0, rt->xt+0, rt->yb-2, c1);
	MxLine(rt->xt+2, rt->yt+1, rt->xb-2, rt->yt+1, c1);
	MxLine(rt->xb-2, rt->yt+2, rt->xb-2, rt->yb-2, c2);
	MxLine(rt->xt+1, rt->yb-2, rt->xb-3, rt->yb-2, c2);
	MxLine(rt->xt+1, rt->yt+1, rt->xt+1, rt->yb-3, c1);
}

static void DrawWinFrames(RECT *rt)
{
	int	c1, c3;

	c1 = winColIdx[1];
	c3 = winColIdx[3];
	MxLine(rt->xt+0, rt->yt+0, rt->xb-1, rt->yt-0, c1);
	MxLine(rt->xt+1, rt->yt+1, rt->xb-2, rt->yt+1, c3);
	MxLine(rt->xb-1, rt->yt+1, rt->xb-1, rt->yb-2, c3);
	MxLine(rt->xb-2, rt->yt+2, rt->xb-2, rt->yb-2, c1);
	MxLine(rt->xt+0, rt->yb-1, rt->xb-1, rt->yb-1, c3);
	MxLine(rt->xt+2, rt->yb-2, rt->xb-3, rt->yb-2, c1);
	MxLine(rt->xt+0, rt->yt+1, rt->xt+0, rt->yb-1, c1);
	MxLine(rt->xt+1, rt->yt+1, rt->xt+1, rt->yb-2, c3);
}

static void DrawWinSeparateH(RECT *rt)
{
	MxLine(rt->xt+0, rt->yt+0, rt->xb-1, rt->yt-0, winColIdx[1]);
	MxLine(rt->xt+0, rt->yt+1, rt->xb-1, rt->yt+1, winColIdx[3]);
}

static void DrawWinSeparateV(RECT *rt)
{
	MxLine(rt->xt+0, rt->yt+0, rt->xt+0, rt->yb-1, winColIdx[1]);
	MxLine(rt->xt+1, rt->yt+0, rt->xt+1, rt->yb-1, winColIdx[3]);
}

void MxFilledRect(RECT *rt, int c)
{
	int	x, y, xt, xb, yt, yb;
	uint8_t *top, *out;

	xt = rt->xt;
	xb = rt->xb;
	yt = rt->yt;
	yb = rt->yb;
	if(plotGo!=MxPlot)
	{
	for(y=yt; y<yb; ++y)
		for(x=xt; x<xb; ++x) (*plotGo)(x, y, c);
	}
	else
	{
	if(sysCp.xt > xt) xt = sysCp.xt;
	if(sysCp.xb < xb) xb = sysCp.xb;
	if(xt >= xb) return;
	if(sysCp.yt > yt) yt = sysCp.yt;
	if(sysCp.yb < yb) yb = sysCp.yb;
	if(yt >= yb) return;
	top = (get_VDP2_VRAM_TOP() + xt + (yt << 10));
	for(y=yt; y<yb; ++y)
		{
		out = top;
		for(x=xt; x<xb; ++x) *out++ = c;
		top += 1024;
		}
	}
}

void MxRect(RECT *rt, int c)
{
	MxHorizontal(rt->xt, rt->yt, rt->xb, c);
	MxVertical(rt->xb-1, rt->yt, rt->yb, c);
	MxHorizontal(rt->xt, rt->yb-1, rt->xb, c);
	MxVertical(rt->xt, rt->yt, rt->yb, c);
}

void MxLine(int x1, int y1, int x2, int y2, int c)
{
	int dx, dy, sx, sy;
	int count;
	int brc, brmax;

	sx = sy = 1;
	dx = x2 - x1;
	dy = y2 - y1;
	if(dx < 0) { dx = -dx; sx = -sx; }
	if(dy < 0) { dy = -dy; sy = -sy; }
	if(dx > dy)
	{
	brmax = dx;
	brc = dx >> 1;
	(*plotGo)(x1, y1, c);
	for (count = dx; count; count--)
		{
		x1 += sx;
		brc += dy;
		if(brc > brmax) { brc -= dx; y1 += sy; }
		(*plotGo)(x1, y1, c);
			}
        }
        else
        {
	brmax = dy;
	brc = dy >> 1;
	(*plotGo)(x1, y1, c);
	for (count = dy; count; count--)
		{
		y1 += sy;
		brc += dx;
		if (brc > brmax) { brc -= dy; x1 += sx; }
		(*plotGo)(x1, y1, c);
		}
	}
}

void MxCircle(int x1, int y1, int r, int c)
{
	int	dx, dy, s;

	dx = r; dy = 0; s = r;
	while(dx >= dy)
	{
	(*plotGo)(x1 + dx, y1 + dy, c);
	(*plotGo)(x1 + dx, y1 - dy, c);
	(*plotGo)(x1 - dx, y1 + dy, c);
	(*plotGo)(x1 - dx, y1 - dy, c);
	(*plotGo)(x1 + dy, y1 + dx, c);
	(*plotGo)(x1 + dy, y1 - dx, c);
	(*plotGo)(x1 - dy, y1 + dx, c);
	(*plotGo)(x1 - dy, y1 - dx, c);
	s -= dy + dy - 1; dy++;
	if(s < 0) { s += dx + dx - 2; dx--; }
	}
}

static void MxHorizontal(int xt, int yt, int xb, int c)
{
	int	x;

	for(x=xt; x<xb; ++x) (*plotGo)(x, yt, c);
}

static void MxVertical(int xt, int yt, int yb, int c)
{
	int	y;

	for(y=yt; y<yb; ++y) (*plotGo)(xt, y, c);
}

void MxPlot(int x, int y, int c)
{
	if(sysCp.yt > y || sysCp.yb <= y ||
				sysCp.xt > x || sysCp.xb <= x) return;
	*(get_VDP2_VRAM_TOP() + x + (y << 10)) = c;
}

void MxPlotClip(int x, int y, int c)
{
	int	i;

	for(i=0; i<udrMax; ++i)
		if(udrCp[i].xt <= x && udrCp[i].xb > x &&
			udrCp[i].yt <= y && udrCp[i].yb > y) return;
	if(sysCp.yt > y || sysCp.yb <= y ||
				sysCp.xt > x || sysCp.xb <= x) return;
	*(get_VDP2_VRAM_TOP() + x + (y << 10)) = c;
}

int MxPixel(int x, int y)
{
	if(sysCp.yt > y || sysCp.yb <= y ||
				sysCp.xt > x || sysCp.xb <= x) return 0;
	return *(get_VDP2_VRAM_TOP() + x + (y << 10));
}
/*
void MxPrintf(char *fmt, ...)
{
	va_list ap;
	char *p, *pp, b, c, lb[128];
	unsigned int	u;

	va_start(ap, fmt);
	p = fmt;
	while(c = *p)
	{
	if(c!='%') { p += DrawFont(textCpPtr, p, 0, 0); textCpPtr->xt += fontWidth; }
	else	{
		p++;
		pp = &lb[0];
		*pp++ = c;
		while((c = *p++)!=0 && c!='u' && c!='U' && c!='d' && c!='D' &&
			c!='x' && c!='X' && c!='s' && c!='S' && c!='%' &&
			c!='h' && c!='H' && c!='v' && c!='V') *pp++ = c;
		*pp++ = c;
		*pp++ = 0;

		if(c=='h') textCpPtr->xt = va_arg(ap, unsigned int);
		else if(c=='H') sscanf(&lb[1], "%dH", &textCpPtr->xt);
		else if(c=='v') textCpPtr->yt = va_arg(ap, unsigned int);
		else if(c=='V') sscanf(&lb[1], "%dV", &textCpPtr->yt);
		else { u = va_arg(ap, unsigned int); sprintf(pp, &lb[0], u);
		while( *pp ) { pp += DrawFont(textCpPtr, pp, 0, 0); textCpPtr->xt += fontWidth; } }
		}
	if(c==0) break;
	}
	va_end(ap);
}
*/

static void LaunchApplicationDemo(RECT *base, int dir)
{
	int	i, j, k, l, m, s, dx, dy, f, u, v, x, y, si, co;
	int	cx[6], cy[6], sx[6], sy[6], sin[6], cos[6];
	POINT	pt[4];

	j = 26;
	sin[0] = 0;
	cos[0] = 16384;
	x = (base->xt + base->xb) / 2;
	y = (base->yt + base->yb) / 2;
	dx = winWidth / 2 - x;
	dy = winHeight / 2 - y;
	f = k = 0;
	for(i=0; i<j; ++i)
	{
	CloseMotionObject();
	SyncBlanking();
	OpenMotionObject(0, 0);

	m = k;
	k++;
	if(k >= 6) { k = 0; f = 6; }
	else if(f < 6) f = k;
	if(dir) s = (j-i-1) * (j-i-1) / j;
	else s = i * i / j;
	cx[m] = dx * s / j + x;
	cy[m] = dy * s / j + y;
	u = (winWidth + (winWidth >> 1)) / 2;
	v = (winHeight + (winHeight >> 1)) / 2;
	sx[m] = u * s / j + ((base->xb - base->xt) / 2);
	sy[m] = v * s / j + ((base->yb - base->yt) / 2);

	for(l=0; l<6; ++l)
		{
		if(l==f) break;
		si = sin[l];
		co = cos[l];
		u = sx[l];
		v = sy[l];
		pt[0].x = cx[l] + ((u * co + v * si) >> 14);
		pt[0].y = cy[l] + ((v * co - u * si) >> 14);
		u = -sx[l];
		v = sy[l];
		pt[1].x = cx[l] + ((u * co + v * si) >> 14);
		pt[1].y = cy[l] + ((v * co - u * si) >> 14);
		u = -sx[l];
		v = -sy[l];
		pt[2].x = cx[l] + ((u * co + v * si) >> 14);
		pt[2].y = cy[l] + ((v * co - u * si) >> 14);
		u = sx[l];
		v = -sy[l];
		pt[3].x = cx[l] + ((u * co + v * si) >> 14);
		pt[3].y = cy[l] + ((v * co - u * si) >> 14);
		SetupObjectPolyline(&pt[0], winColIdx[4]);
		}

	u = cos[m]; v = sin[m];
	si = 3921;
	co = 15908;
	if(dir)	{ cos[k] = (u * co + v * si) >> 14;
		sin[k] = (v * co - u * si) >> 14; }
	else	{ cos[k] = (u * co - v * si) >> 14;
		sin[k] = (v * co + u * si) >> 14; }
	}
}

/*--------------------------------------------------------------/
/	ＳＪＩＳコードからキャラジェネのアドレスを得る。	/
/--------------------------------------------------------------*/
static int GetSjisToKanjiAdr(char *ptr)
{
	int	a, b, ko;

	if(kanjiFont==NULL) return -1;
	a = *(unsigned char *)ptr++;
	if(a < 0x81) return -1;
	else if(a <= 0x84) { a -= 0x81; ko = 0; }
	else if(a < 0x88) return -1;
	else if(a <= 0x9f) { a -= 0x88 - 4; ko = 94; }
	else if(a < 0xe0) return -1;
	else if(a <= 0xea) { a -= 0xe0 - 4 - 0x18; ko = 94; }
	else return -1;
	b = *(unsigned char *)ptr;
	if(b < 0x40 || b==0x7f || b > 0xfc) return -1;
	if(b >= 0x80) b -= 0x80 - 0x3f; 
	else b -= 0x40;
	return a * 188 + b - ko;
}

/*--------------------------------------------------------------/
/	クリップされたウィンドウの中にイメージを転送する	/
/	（クリピッングは通常される）				/
/--------------------------------------------------------------*/
void DrawImageForWindow(WINDOW *win, char *img, int xo, int yo)
{
/*
	int	w, h;

	w  = *(unsigned char *)img++; w <<= 8;
	w |= *(unsigned char *)img++;
	h  = *(unsigned char *)img++; h <<= 8;
	h |= *(unsigned char *)img++;

	m = n = 0;
	if(sdx!=NULL)
	{
	i = rtc->xb - rtc->xt;
	j = (i << 16) / w;
	k = sdx->flag & 0xffff;
	sdx->flag = k;
	if(j > 65536) sdx->flag |= 0xffff0000;
	else sdx->flag |= j << 16;
	if((w - i) < 0) m = 0;
	else m = ((w - i) * k) >> 16;
	}
	if(sdy!=NULL)
	{
	i = rtc->yb - rtc->yt;
	j = (i << 16) / h;
	k = sdy->flag & 0xffff;
	sdy->flag = k;
	if(j > 65536) sdy->flag |= 0xffff0000;
	else sdy->flag |= j << 16;
	if((h - i) < 0) n = 0;
	else n = ((h - i) * k) >> 16;
	}

	top = img;
	for(i=0; i<16; ++i)
	{
	r = *(unsigned char *)top++;
	g = *(unsigned char *)top++;
	b = *(unsigned char *)top++;
	GrSetColor(i+8, r, g, b);
	}
	img += 768;
	if((rtc->xb - rtc->xt) > w) i = rtc->xt + w;
	else i = rtc->xb;
	if((rtc->yb - rtc->yt) > h) j = rtc->yt + h;
	else j = rtc->yb;

	img += m + n * w;
	for(y=rtc->yt; y<j; ++y)
	{
	top = img;
	for(x=rtc->xt; x<i; ++x)
		(*plotGo)(x, y, ((*(unsigned char *)top++)+8));
	img += w;
	}
*/
}
