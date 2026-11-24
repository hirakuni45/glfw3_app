#pragma once
#include <stdint.h>

/// #define	VDP1_VRAM_TOP		0x25C00000
/// #define	VDP2_VRAM_TOP		0x25E00000

#define CLUT_MODE_VDP1		0
#define CLUT_MODE_VDP2		1

#define SPR_WIDTH		640
#define SPR_HEIGHT		448
#define SPR_HEIGHT_PAL		480

#define VDP1_CMD_SYS_CLIP	0x0009
#define VDP1_CMD_USR_CLIP	0x0008
#define VDP1_CMD_REL_POS	0x000a
#define VDP1_CMD_MOBJ_NORM	0x0000
#define VDP1_CMD_MOBJ_FLEX	0x0001
#define VDP1_CMD_END		0x8000

#define VDP1_CMD_H_FLIP		0x0010
#define VDP1_CMD_V_FLIP		0x0020

#define VDP1_MODE_BANK16	0x0000
#define VDP1_MODE_CLUT16	0x0008
#define VDP1_MODE_BANK64	0x0010
#define VDP1_MODE_BANK128	0x0018
#define VDP1_MODE_BANK256	0x0020
#define VDP1_MODE_RGB		0x0028

#define VDP1_MODE_ECD_OFF	0x0080
#define VDP1_MODE_SP_OFF	0x0040

#define ANIME_POS_SHIFT		8
#define ANIME_MAX		64
#define MOBJ_MAX		(512+1)
#define MOBJ_COMMAND_MAX	512
#define CLUT_MAX		512

typedef struct {
	short	x, y;
} POINT;

typedef struct {
	short	xt, yt, xb, yb;
} CLIP;

typedef struct {
	short	ctrl;
	short	link;
	short	mode;
	short	color;
	short	chaptr;
	short	chasiz;
	short	ax, ay;
	short	bx, by;
	short	cx, cy;
	short	dx, dy;
	short	grsptr;
	short	dummy;
} VDP1_CMD;

typedef struct {
	short	mode;
	short	flip;
	short	x, y, h, v;
	short	color;
	short	chaptr;
	short	chasiz;
} MOBJ;

typedef struct {
	MOBJ	*mobj;
	short	speed;
	short	anime;
	short	count;
	short	x, y;
} ANIME;

typedef struct {
	const uint8_t	*top;
	uint32_t		bitpos;
	int				old;
} BIT_FILE;

uint8_t *get_VDP1_VRAM_TOP();
uint8_t *get_VDP1_VRAM_IMG();
uint8_t *get_VDP2_VRAM_TOP();

/* assembler routines */
int InputBit(BIT_FILE *bit);
int Input5Bits(BIT_FILE *bit);
int InputBits(BIT_FILE *bit, int count);

void ResetMotionObject(void);
void ResetBackground(void);
void OpenMotionObject(int ox, int oy);
void SetupUsrClip(CLIP *clip);
void SetupRelativePosition(POINT *pos);
void InstallCLUT(const uint8_t *, int);
void InstallMotionObjectDouble(const char *name, ANIME *);
void InstallMotionObject(const char *name, ANIME *);
void SetupObject(const MOBJ *mobj);
void SetupObjectLine(int, int, int, int, int);
void SetupObjectLineWithMesh(int, int, int, int, int);
void SetupObjectBox(int x0, int y0, int x1, int y1, int c);
void SetupObjectFrame(int, int, int, int, int);
void SetupObjectPolyline(POINT *, int);
void SetupObjectPolygon(POINT *, int);
void SetupResizeObject(MOBJ *obj, int xs, int ys);
void CloseMotionObject(void);
void DoAnime(ANIME *anime);

void ExtractResource(const uint8_t *src, uint8_t *out);
void ExtractBitImage(const char *src, int, int);
void CopyImage(const uint8_t *img, int x, int y, int w, int h);
void DissolveCopyImage(const uint8_t *img, int x, int y, int w, int h, int lv);
MOBJ *PresetBitMapImage(const uint8_t *, int, int, int);
void ExtractBitImageOfs(const uint8_t *src, int x, int y, int o, int t);

void ServiceSlave(void);
void ExtractBitFrame(int *in);
void Install16ColorObject(ANIME *anm);
