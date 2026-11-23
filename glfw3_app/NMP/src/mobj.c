/*------------------------------------------------------/
/							/
/	SEGA SATURN Motion Object Service		/
/							/
/------------------------------------------------------*/
#include <stdio.h>
#include "saturn.h"
#include "mobj.h"
#include "top.h"
#include "task.h"

extern unsigned char *resIndex, *resImage;
extern char errMsg[];

static uint8_t vdp1_vram_top_[1024 * 512];
uint8_t *get_VDP1_VRAM_TOP() { return vdp1_vram_top_; }
static uint8_t vdp2_vram_top_[1024 * 512];
uint8_t *get_VDP2_VRAM_TOP() { return vdp2_vram_top_; }

static void PresetImage(const uint8_t *ptr);
static void PresetImageSub(int x, int y, int p);
static void ExtractForVDP1(const uint8_t *, short *);
static void ExtractForVDP2(const uint8_t *, uint8_t *);
static void BitMapForVDP2(const uint8_t *, uint8_t *);
static void PlaneForVDP2(const uint8_t *, uint8_t *);
static int ExtractFLAT(BIT_FILE *inp, uint8_t *out, int s, int p);
static int CreateRand(void);
static void ExtractOffsetForVDP2(const uint8_t *image, uint8_t *out, int ofs, int t);

int clutIdxMOBJ, clutTopMOBJ;

static VDP1_CMD *cmdPtr, cmdBuff[MOBJ_COMMAND_MAX+1];
static MOBJ *mobjPtr = NULL, mobjBuff[MOBJ_MAX];
static short *imagePtr;
static int clutIdxBG, clutTopBG;
static unsigned short randSeed, randBase;
static char installMode;
static const unsigned char bitTbl[] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static uint8_t lineBuff[1024+512+128+1024];

void ResetMotionObject(void)
{
	clutTopMOBJ = clutIdxMOBJ = 256;
	mobjPtr = &mobjBuff[0];
///	imagePtr = (short *)VDP1_VRAM_TOP + MOBJ_COMMAND_MAX * 32;
}

void ResetBackground(void)
{
	clutTopBG = clutIdxBG = 0;
}

void OpenMotionObject(int ox, int oy)
{
	cmdPtr = &cmdBuff[0];

	cmdPtr->ctrl = VDP1_CMD_SYS_CLIP;
	cmdPtr->link = 0;
	cmdPtr->cx = SPR_WIDTH - 1;
	cmdPtr->cy = SPR_HEIGHT - 1;
	cmdPtr++;

	cmdPtr->ctrl = VDP1_CMD_REL_POS;
	cmdPtr->link = 0;
	cmdPtr->ax = ox;
	cmdPtr->ay = oy;
	cmdPtr++;
}

void SetupUsrClip(CLIP *clip)
{
	cmdPtr->ctrl = VDP1_CMD_USR_CLIP;
	cmdPtr->link = 0;
	cmdPtr->ax = clip->xt;
	cmdPtr->ay = clip->yt;
	cmdPtr->cx = clip->xb - 1;
	cmdPtr->cy = clip->yb - 1;
	cmdPtr++;
}

void SetupRelativePosition(POINT *pos)
{
	cmdPtr->ctrl = VDP1_CMD_REL_POS;
	cmdPtr->link = 0;
	cmdPtr->ax = pos->x;
	cmdPtr->ay = pos->y;
	cmdPtr++;
}

void InstallCLUT(const uint8_t *ptr, int type)
{
	int	i, j, m;

	if(type==CLUT_MODE_VDP1)
	{
	if(clutIdxMOBJ >= 512) return;
	j = *ptr++;
	j++;
	clutTopMOBJ = clutIdxMOBJ;
	SetCLUTs(clutIdxMOBJ, j, ptr);
	if(j <= 16) clutIdxMOBJ += 16;
	else if(j <= 64) clutIdxMOBJ += 64;
	else clutIdxMOBJ += j;
	if(clutIdxMOBJ & 0xf) { clutIdxMOBJ |= 0xf; clutIdxMOBJ++; }
	}
	else
	{
	if(clutIdxBG >= 256) return;
	j = *ptr++;
	j++;
	clutTopBG = clutIdxBG;
	SetCLUTs(clutIdxBG, j, ptr);
	clutIdxBG += j;
	}
}

void InstallMotionObjectDouble(const char *name, ANIME *anm)
{
	installMode = 1;
	InstallMotionObject(name, anm);
	installMode = 0;
}

void InstallMotionObject(const char *name, ANIME *anm)
{
	int	i;
	const uint8_t *src;
	char res[5];
	const uint8_t *img;
	unsigned int a, c;

	anm->mobj = mobjPtr;
	anm->anime = anm->speed = 0;

	if(name==NULL) {
		anm->mobj = mobjPtr;
		clutTopMOBJ = clutIdxMOBJ;
		PresetImageSub(anm->x, anm->y, anm->count);
		anm->count = 1 << ANIME_POS_SHIFT;
		return;
	}

	src = (const uint8_t *)GetResourcePtr(name);
	i = 0;
	while((a = TakeInt(src)) < 0x20000000) {
	c = a >> 24;
	img = resImage + (a & 0xffffff);
	if(c==RESOURCE_FLAT_ID) { PresetImage(img); ++i; }
	else if(c==RESOURCE_CLUT_ID) InstallCLUT(img, CLUT_MODE_VDP1);
	src += 4;
	}

	if(i==0) {
		for(i=0; i<4; ++i) res[i] = name[i];
		res[4] = 0;
		sprintf(errMsg, "Stop.\nFalse !\nCan't install MOBJs at '%s'.\n", res);
		LaunchDebugger(errMsg);
	}
	anm->count = i << ANIME_POS_SHIFT;
}

void SetupObject(MOBJ *obj)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = obj->flip;
	cmdPtr->link = 0;
	cmdPtr->mode = obj->mode | VDP1_MODE_ECD_OFF;
	cmdPtr->color = obj->color;
	cmdPtr->chaptr = obj->chaptr;
	cmdPtr->chasiz = obj->chasiz;
	cmdPtr->ax = obj->x;
	cmdPtr->ay = obj->y;
	cmdPtr->grsptr = 0;
	cmdPtr++;
}

void SetupObjectLine(int x0, int y0, int x1, int y1, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0006;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x08c0;
	cmdPtr->color = c;
	cmdPtr->ax = x0;
	cmdPtr->ay = y0;
	cmdPtr->bx = x1;
	cmdPtr->by = y1;
	cmdPtr++;
}

void SetupObjectLineWithMesh(int x0, int y0, int x1, int y1, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0006;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x09c0;
	cmdPtr->color = c;
	cmdPtr->ax = x0;
	cmdPtr->ay = y0;
	cmdPtr->bx = x1;
	cmdPtr->by = y1;
	cmdPtr++;
}

void SetupObjectFrame(int x0, int y0, int x1, int y1, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0005;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x08c0;
	cmdPtr->color = c;
	cmdPtr->ax = x0; cmdPtr->ay = y0;
	cmdPtr->bx = x1; cmdPtr->by = y0;
	cmdPtr->cx = x1; cmdPtr->cy = y1;
	cmdPtr->dx = x0; cmdPtr->dy = y1;
	cmdPtr++;
}

void SetupObjectBox(int x0, int y0, int x1, int y1, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0004;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x08c0;
	cmdPtr->color = c;
	cmdPtr->ax = x0; cmdPtr->ay = y0;
	cmdPtr->bx = x1; cmdPtr->by = y0;
	cmdPtr->cx = x1; cmdPtr->cy = y1;
	cmdPtr->dx = x0; cmdPtr->dy = y1;
	cmdPtr++;
}

void SetupObjectPolyline(POINT *pt, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0005;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x08c0;
	cmdPtr->color = c;
	cmdPtr->ax = pt->x; cmdPtr->ay = pt->y; pt++;
	cmdPtr->bx = pt->x; cmdPtr->by = pt->y; pt++;
	cmdPtr->cx = pt->x; cmdPtr->cy = pt->y; pt++;
	cmdPtr->dx = pt->x; cmdPtr->dy = pt->y;
	cmdPtr++;
}

void SetupObjectPolygon(POINT *pt, int c)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = 0x0004;
	cmdPtr->link = 0;
	cmdPtr->mode = 0x08c0;
	cmdPtr->color = c;
	cmdPtr->ax = pt->x; cmdPtr->ay = pt->y; pt++;
	cmdPtr->bx = pt->x; cmdPtr->by = pt->y; pt++;
	cmdPtr->cx = pt->x; cmdPtr->cy = pt->y; pt++;
	cmdPtr->dx = pt->x; cmdPtr->dy = pt->y;
	cmdPtr++;
}

void SetupResizeObject(MOBJ *obj, int xs, int ys)
{
	if(cmdPtr >= &cmdBuff[MOBJ_COMMAND_MAX]) return;
	cmdPtr->ctrl = obj->flip | 0x0001;
	cmdPtr->link = 0;
	cmdPtr->mode = obj->mode | VDP1_MODE_ECD_OFF;
	cmdPtr->color = obj->color;
	cmdPtr->chaptr = obj->chaptr;
	cmdPtr->chasiz = obj->chasiz;
	cmdPtr->ax = obj->x;
	cmdPtr->ay = obj->y;
	cmdPtr->cx = obj->x + xs;
	cmdPtr->cy = obj->y + ys;
	cmdPtr->grsptr = 0;
	cmdPtr++;
}

void CloseMotionObject(void)
{
#if 0
	int	i;
	unsigned short *dst, *src;

	cmdPtr->ctrl = VDP1_CMD_END;
	cmdPtr++;
	dst = (unsigned short *)VDP1_VRAM_TOP;
	src = (unsigned short *)&cmdBuff[0];
	i = cmdPtr - &cmdBuff[0];
	i <<= 4;
SPR_WaitDrawEnd();
	do { *dst++ = *src++; } while(i--) ;
#endif
}

void DoAnime(ANIME *anime)
{
	anime->anime += anime->speed;
	if(anime->anime >= anime->count) anime->anime -= anime->count;
	anime->mobj->x = anime->x;
	anime->mobj->y = anime->y;
}

static void PresetImage(const uint8_t *ptr)
{
	int	x, y, i;

	ExtractForVDP1(ptr, imagePtr);
	x = TakeShort(ptr);
	y = TakeShort(ptr+2);
	i = *(ptr + 4);
	if(installMode==0) PresetImageSub(x, y, i);
	else if(installMode==1) { installMode++; }
	else if(installMode==2) { PresetImageSub(x, y, i);
				installMode=1; }
}

static void PresetImageSub(int x, int y, int p)
{
	int	i;

	if(p <= 4) mobjPtr->mode = VDP1_MODE_BANK16;
	else if(p <= 6) mobjPtr->mode = VDP1_MODE_BANK64;
	else if(p <= 7) mobjPtr->mode = VDP1_MODE_BANK128;
	else mobjPtr->mode = VDP1_MODE_BANK256;

	mobjPtr->x = mobjPtr->y = mobjPtr->flip = 0;
	mobjPtr->h = x;
	mobjPtr->v = y;
///	mobjPtr->chaptr = ((imagePtr - (short *)VDP1_VRAM_TOP) >> 2) & 0xfffc;
	if(x & 7) i = 1; else i = 0;
	x >>= 3;
	x += i;
	mobjPtr->chasiz = (x << 8) + (y & 0xff);
	mobjPtr->color = clutTopMOBJ;
	mobjPtr++;
	x <<= 3;
	if(p <= 4) imagePtr += (x * y) >> 2;
	else imagePtr += (x * y) >> 1;

///	i = (char *)imagePtr - (char *)VDP1_VRAM_TOP;
///	if(i & 31) imagePtr = (short *)(VDP1_VRAM_TOP + (i & 0x7ffe0) + 32);
}

void ExtractResource(const uint8_t *image, uint8_t *out)
{
	int	h, v, p;
	BIT_FILE	bf;

	h = TakeShort(image); image += 2;
	v = TakeShort(image); image += 2;
	p  = *image++;

	bf.top = image;
	bf.bitpos = 0;
	ExtractFLAT(&bf, out, h * v, p);
}

static void ExtractForVDP1(const uint8_t *image, short *vdp)
{
	int	i, j, k, s, o, type, h, v, p, w, cm;
	const uint8_t *src;
	uint8_t *dst;
	const uint16_t *in;
	BIT_FILE bf;

	h = TakeShort(image); image += 2;
	v = TakeShort(image); image += 2;
	p  = *image++;

	if(p <= 4) cm = 1; else cm = 0;
	bf.top = image;
	bf.bitpos = 0;
	s = h * v;
	w = 1024;
	o = 0;
	while(s > 0)
	{
	j = ExtractFLAT(&bf, &lineBuff[o], w-o, p);
	in = (const uint16_t *)&lineBuff[0];
	if(cm)	{ src = dst = &lineBuff[0];
		for(i=0; i<512; ++i)
			{ o = *src++;
			o <<= 4;
			o |= *src++;
			*dst++ = o; }
		k = 256; }
	else k = 512;

	if(installMode==1) for(i=0; i<k; ++i) *vdp++ = ( *in++ ) << 2;
	else if(installMode==2) for(i=0; i<k; ++i) { *vdp |= *in; vdp++; in++; }
	else for(i=0; i<k; ++i) *vdp++ = *in++;

	s -= w;
	if(j < 0)
		{ j = -j;
		for(i=0; i<j; ++i) lineBuff[i] = lineBuff[w+i];
		o = j; }
	else o = 0;
	}
}

void ExtractBitImage(const char *name, int x, int y)
{
	int	i, k, l;
	const uint8_t	*ptr;
	const uint8_t	*cmd;
	uint8_t	*out;

	ptr = GetResourcePtr(name);
	for(i=0; i<2; ++i)
	{
	k = *ptr++;
	cmd = resImage + TakeMiddle(ptr); ptr += 3;
	if(k==0 || k==2 || k==3)
		{ 
		l = TakeShort(cmd);
		if(x < 0) x = (640 - l) >> 1;
		l = TakeShort(cmd+2);
		if(y < 0) y = (448 - l) >> 1; 
		out = get_VDP2_VRAM_TOP() + x + (y << 10);
		if(k==0) ExtractForVDP2(cmd, out);
		else if(k==2) BitMapForVDP2(cmd, out);
		else PlaneForVDP2(cmd, out);
		}
	else if(k==1) InstallCLUT(cmd, CLUT_MODE_VDP2);
	}
}

static void ExtractForVDP2(const uint8_t *image, uint8_t *out)
{
	int	w, h, i, j, s, p;
	const uint8_t	*in;
	BIT_FILE bf;

	w = TakeShort(image); image += 2;
	h = TakeShort(image); image += 2;
	p = *image++;

	bf.top = image;
	bf.bitpos = 0;

	s = w * h;
	j = 0;
	while(s > 0)
	{ 
	j = ExtractFLAT(&bf, &lineBuff[j], (w - j), p);
	in = lineBuff;
	i = w;
	while(i) { *out++ = *in++; i--; }
	out += 1024 - w;
	s -= w;
	if(j < 0)
		{ j = -j;
		in = &lineBuff[w];
		for(i=0; i<j; ++i) lineBuff[i] = *in++;
		}
	}
}

static void BitMapForVDP2(const uint8_t *image, uint8_t *out)
{
	int	w, h, x, y, p;
	BIT_FILE bf;

	w = TakeShort(image); image += 2;
	h = TakeShort(image); image += 2;
	p = *image++;

	bf.top = image;
	bf.bitpos = 0;

	for(y=0; y<h; ++y)
	{
	for(x=0; x<w; ++x) *out++ = InputBits(&bf, p);
	out += 1024 - w;
	}
}

static void PlaneForVDP2(const uint8_t *image, uint8_t *out)
{
	int	w, h, x, y, p;

	w = TakeShort(image); image += 2;
	h = TakeShort(image); image += 2;
	p = *image++;

	for(y=0; y<h; ++y)
	{
	for(x=0; x<w; ++x) *out++ = *image++;
	out += 1024 - w;
	}
}

static int ExtractFLAT(BIT_FILE *inp, uint8_t *out, int s, int p)
{
	int	i, j;

	if(inp->bitpos==0) { *out++ = i = InputBits(inp, p); s--; }
	else i = inp->old;
	while(s > 0)
	{
	if(InputBit(inp)) { *out++ = i = InputBits(inp, p); s--; }
	else	{
		if(InputBit(inp))
			{ j = Input5Bits(inp) + 3;
			s -= j;
			while(j) { *out++ = i; j--; }
			}
		else { *out++ = i; s--; }
		}
	}
	inp->old = i;
	return s;
}

void CopyImage(const uint8_t *img, int x, int y, int w, int h)
{
	int	i, j, k;
	uint8_t *out, *dst;

	out = get_VDP2_VRAM_TOP() + x + y * 1024;
	for(i=0; i<h; ++i)
	{
	dst = out;
	for(j=0; j<w; ++j) *dst++ = *img++;
	out += 1024;
	}
}

void DissolveCopyImage(const uint8_t *img, int x, int y, int w, int h, int lv)
{
	int	i, j, k, b, l, m;
	uint8_t *out, *dst;

	l = 0;
	m = (w * h) / 951;
	out = get_VDP2_VRAM_TOP() + x + y * 1024;
	for(i=0; i<h; ++i)
	{
	dst = out;
	for(j=0; j<w; ++j)
		{
		if(l) l--;
		else { b = CreateRand();
			l = CreateRand(); l %= m; l += m >> 1; }
		if(b < lv) *dst++ = *img++;
		else { dst++; img++; }
		}
	out += 1024;
	}
}

static int CreateRand(void)
{
	if((randBase & 0x4008)==0) randBase += randBase + 1;
	else if((randBase & 0x4008)==0x4008) randBase += randBase + 1;
	else randBase += randBase;
	randSeed += (randSeed << 2) + 1;
	randSeed ^= randBase;
	return randSeed;
}

MOBJ *PresetBitMapImage(const uint8_t *top, int w, int h, int ic)
{
	int	x, y, c, i;
	const uint8_t	*img;
	short	*vdp;
	MOBJ	*mobj;

	mobj = mobjPtr;
	vdp = imagePtr;
	for(y=0; y<h; ++y)
	{
	img = top;
	i = 0;
	for(x=0; x<w; x++)
		{
		if((x & 7)==0) c = *img++;
		i <<= 4; if(c & 0x80) i |= ic & 15;
		c += c;
		if((x & 3)==3) { *vdp++ = i; i=0; }
		}
	top += w / 8;
	}
	PresetImageSub(w, h, 4);
	return mobj;
}


int Input5Bits(BIT_FILE *bit)
{
	int	val;
	const uint8_t *top;

	top = bit->top + (bit->bitpos >> 3);
	val = *(unsigned char *)top++;
	switch(bit->bitpos & 7)
	{
	case 0:	val >>= 3; break;
	case 1: val >>= 2; val &= 0x1f; break;
	case 2: val >>= 1; val &= 0x1f; break;
	case 3: val &= 0x1f; break;
	case 4: val <<= 1; val &= 0x1e; val |= (*top >> 7) & 0x01; break;
	case 5: val <<= 2; val &= 0x1c; val |= (*top >> 6) & 0x03; break;
	case 6: val <<= 3; val &= 0x18; val |= (*top >> 5) & 0x07; break;
	case 7: val <<= 4; val &= 0x10; val |= (*top >> 4) & 0x0f; break;
	}
	bit->bitpos += 5;
	return val;
}

int InputBits(BIT_FILE *bit, int count)
{
	int	i, val;

	val = 0;
	for(i=0; i<count; ++i)
	{
	val += val;
	if(*(bit->top + (bit->bitpos >> 3)) & bitTbl[bit->bitpos & 7]) val++;
	bit->bitpos++;
	}
	return val;
}

int InputBit(BIT_FILE *bit)
{
	return InputBits(bit, 1);
}

static void ExtractOffsetForVDP2(const uint8_t *image, uint8_t *out, int ofs, int t)
{
	int	w, h, i, j, s, c;
	const uint8_t *in;
	BIT_FILE bf;
	uint32_t p;

	w = TakeShort(image); image += 2;
	h = TakeShort(image); image += 2;
	p = *image++;

	bf.top = image;
	bf.bitpos = 0;

	s = w * h;
	j = 0;
	while(s > 0)
	{ 
	j = ExtractFLAT(&bf, &lineBuff[j], (w - j), p);
	in = &lineBuff[0];
	i = w;
	while(i) { c = *(unsigned char *)in++;
		if(c!=t) *out = c + ofs;
		out++;
		i--; }
	out += 1024 - w;
	s -= w;
	if(j < 0)
		{ j = -j;
		in = &lineBuff[w];
		for(i=0; i<j; ++i) lineBuff[i] = *in++;
		}
	}
}

void ExtractBitImageOfs(const uint8_t *ptr, int x, int y, int o, int t)
{
	int	l;
	uint8_t *out;

	l = TakeShort(ptr);
	if(x < 0) x = (640 - l) >> 1;
	l = TakeShort(ptr + 2);
	if(y < 0) y = (448 - l) >> 1; 
	out = get_VDP2_VRAM_TOP() + x + (y << 10); 
	ExtractOffsetForVDP2(ptr, out, o, t);
}
