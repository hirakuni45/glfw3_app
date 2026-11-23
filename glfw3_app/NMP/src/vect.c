#include "ballsim.h"
#include "fnc.h"
#include "vect.h"
#include "saturn.h"

#define HIT_MAX		800
#define TAG_MAX		2200

typedef struct {
	int	xt, yt, zt;
	int	xb, yb, zb;
} LINE;

extern int screenWidth, screenHeight;

static int ValuePrintf(char *t, int v);
static void TestMapSub(HIT *hit, int w, int h, int s);
static void ScanArea(void);
static int CheckArea(HIT *ptr, int xs, int ys, int xe, int ye);
static int GetPositions(short *vect, short *out);
static int SetBox(int a, short *vect, HIT *hit, int r);
static int SetLine(int a, short *vect, HIT *hit, int r);

static void SetLineSub(LINE *, int r, HIT *hit);
static void SetCircleSub(LINE *line, int r, HIT *hit);
static void SortFirstBig(int *val, short *idx, int count);
static int HitCheckLine(int *, HIT *);
static int LineToSinCos_(LINE *lin, SINCOS *sico);
static int PresetPolyline(int a, short *vect, HIT *hit, int r);
static int PresetCircle(int a, short *vect, HIT *hit, int r);
static int PresetFlipper(int a, short *vect, HIT *hit, int r);
static int PresetSwitch(int a, short *vect, HIT *hit, int r);
static int PresetOpenPolyline(int a, short *vect, HIT *hit, int r);
static int PresetDropTarget(int a, short *vect, HIT *hit, int r);
static int PresetStandUp(int a, short *vect, HIT *hit, int r);
static int PresetHole(int a, short *vect, HIT *hit, int r);
static int PresetSaucer(int a, short *vect, HIT *hit, int r);

static int linCount, cirCount, fieldHeightMax;

HIT hitList[HIT_MAX];
short hitMax, tagMax;
short listTag[TAG_MAX];
short zoneMap[ZONE_DIVIDE][ZONE_DIVIDE];
BALL ball[MULTI_BALL_MAX];
SWITCH swPara[32];
FLIPPER flipper[4];
short fieldWidthMax;
short startBallX, startBallY, startBallZ;
short psychoSaveX, psychoSaveY, psychoSaveZ;
short vukPosX[VUK_MAX], vukPosY[VUK_MAX], vukPosZ[VUK_MAX];
short magnaPosX, magnaPosY, magnaRadius;
int magnaRadius2;
short ballRadius;
short flipperMax, ballMax, psychoSaveMax,  messengerBallMax;
short jetBumperMax, dropTargetMax, standUpMax, holeMax;
short vukMax, rampMax, flipperMax;
short slingShotMax;
short movegateMax, magnaLockMax;

/*------------------------------------------------------/
/							/
/	Create HIT-MAP data.				/
/							/
/------------------------------------------------------*/
void PickUpFirst(short *vect)
{
	int	a, bc;
	short	pos[32];
	BALL	*blp;
	HIT	*hit;

	ballMax = 0;
	blp = &ball[0];
	while((a=*vect++)!=-1)
	{
	switch(a & ATTR_MASK)
		{
	case BALLS:
		vect += GetPositions(vect, &pos[0]);
		if((a & 3)==2) {
		psychoSaveX = pos[0];
		psychoSaveY = pos[1];
		psychoSaveZ = pos[2];
		}
		else if((a & 3)==1) {
		startBallX = pos[0];
		startBallY = pos[1];
		startBallZ = pos[2];
		}
		else if((a & 3)==0) {
		if(ballMax >= MULTI_BALL_MAX) LaunchDebugger("Too Match Balls !!!");
		if(ballMax < MULTI_BALL_MAX)
			{
			blp->xp = pos[0];
			blp->xp <<= POSITION_SHIFT;
			blp->yp = pos[1];
			blp->yp <<= POSITION_SHIFT;
			blp->zp = 0;
			ballRadius = (pos[3] + pos[4]) / 4;
			blp++;
			ballMax++;
			}
		}
		break;
	default:
		vect += GetPositions(vect, &pos[0]);
		break;
		}
	}
}

void MakeupVectors(FIELD *para, short *vect, char *text)
{
	int	i, j, k, a;
	int	vc, fp, sw;
	int	xyz[3], xys[3];
	HIT	*hit;
	short	pos[32];

	for(i=0; i<FLIPPER_MAX; ++i) flipper[i].hl = -1;

	hit = &hitList[0];
	ballMax = 0;
	messengerBallMax = 0;
	psychoSaveMax = 0;
	fieldWidthMax = 0;
	fieldHeightMax = para->fieldHeight;
	dropTargetMax = standUpMax = flipperMax = 0;
	jetBumperMax = holeMax = rampMax = 0;
	slingShotMax = vukMax = 0;
	movegateMax = 0;
	magnaLockMax = 0;

	vc = fp = sw = 0;
	while((a=*vect++)!=-1)
	{
	if(hit >= &hitList[HIT_MAX]) LaunchDebugger("Over Flow HITs !!!");
	switch(a & ATTR_MASK)
		{
	case BALLS:
		vect += GetPositions(vect, &pos[0]);
		if((a & 3)==0) ballMax++;
		else if((a & 3)==2) psychoSaveMax++;
		else if((a & 3)==3) messengerBallMax++;
		break;
	case POLYLINE:
		k = a & 15;
		if(k==0) j = _METAL;
		else if(k==1) j = _METAL;
		else if(k==2) j = _PLASTIC;
		else if(k==3) j = _RUBBER_POST;
		else if(k==4) j = _RUBBER_BAND;
		else j = _METAL;

		if(a & 0x8000)
		{
		vect += PresetCircle(POLYLINE+0x8000+j, vect, hit, ballRadius);
		hit++;
		}
		else
		{
		vect += PresetPolyline(POLYLINE+j, vect, hit, ballRadius);
		hit += linCount + cirCount;
		}
		break;
	case JET_BUMPER:
		vect += PresetCircle(a+0x8000, vect, hit, -(ballRadius / 2));
		hit++;
		jetBumperMax++;
		break;
	case SLING_SHOT:
		vect += PresetPolyline(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		slingShotMax++;
		break;
	case DROP_TARGET:
		vect += PresetDropTarget(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		dropTargetMax++;
		break;
	case STRUCT_RAMP:
		j = _RAMP + _LAYER_1;
		vect += PresetOpenPolyline(POLYLINE + j, vect, hit, ballRadius);
		hit += linCount + cirCount;
		rampMax++;
		break;
	case STAND_UP:
		vect += PresetStandUp(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		standUpMax++;
		break;
	case LR_FLIPPER:
		vect += PresetFlipper(a | _FLIPPER, vect, hit, ballRadius);
		hit += linCount + cirCount;
		fp++;
		break;
	case SWITCH_GATE:
		vect += PresetSwitch(a, vect, hit, 0);
		hit += linCount + cirCount;
		k = a & 31;
		sw++;
		break;
	case HOPPER:
		vect += GetPositions(vect, &pos[0]);
		k = a & 3;
		vukPosX[k] = pos[0];
		vukPosY[k] = pos[1];
		vukPosZ[k] = pos[2];
		vukMax++;
		break;
	case HOLE_ZONE:
		vect += PresetHole(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		holeMax++;
		break;
	case MOVE_GATE:
		vect += PresetPolyline(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		movegateMax++;
		break;
	case MAGNA_LOCK:
		vect += GetPositions(vect, &pos[0]);
		magnaPosX = pos[0];
		magnaPosY = pos[1];
		i = (pos[3] + pos[4]) / 4;
		magnaRadius = i;
		magnaRadius2 = i * i;
		magnaLockMax++;
		break;
	case SAUCER_ZONE:
		vect += PresetSaucer(a, vect, hit, ballRadius);
		hit += linCount + cirCount;
		holeMax++;
		break;
	default:
		vect += PresetPolyline(a, vect, hit, ballRadius);
		break;
		}
	vc++;
	}
	hitMax = hit - &hitList[0];

	ScanArea();

/*	Report Attribute, ETC,...	*/

/*	if(text!=NULL)
	{
	text += sprintf(text, "------------ Vector transfome complete. ------------\n");
	text += sprintf(text, "Total Blocks = %d\n", vc);
	text += sprintf(text, "Total Hits (Max:%d) = %d\n", HIT_MAX, hitMax);
	text += sprintf(text, "Total Tags (Max:%d) = %d\n", TAG_MAX, tagMax);
	text += sprintf(text, "---\n");
	text += sprintf(text, "Total Balls (Max:%d) = %d\n", MULTI_BALL_MAX, ballMax);
	text += sprintf(text, "Total Messenger Balls = %d\n", messengerBallMax);
	text += sprintf(text, "Total Flipper: %d\n", fp / FLIPPER_STEP);
	text += sprintf(text, "Total Jet-Bumper: %d\n", jetBumperMax); 
	text += sprintf(text, "Total Sling-Shot: %d\n", slingShotMax); 
	text += sprintf(text, "Total Drop-Target: %d\n", dropTargetMax);
	text += sprintf(text, "Total Stand-Up: %d\n", standUpMax);
	text += sprintf(text, "Total Ramp: %d\n", rampMax);
	text += sprintf(text, "Total VUK: %d\n", vukMax);
	text += sprintf(text, "Total Hole: %d\n", holeMax);
	text += sprintf(text, "Total Switch: %d\n", sw);

	text += sprintf(text, "\n");
	for(i=0; i<4; ++i)
	{
	if(flipper[i].hl > 0)
		{
		xyz[0] = flipper[i].cx;
		xyz[1] = flipper[i].cy;
		xyz[2] = flipper[i].cz;
		Perspective(&xyz[0], &xys[0]);
		flipper[i].prx = screenWidth / 2 + xys[0];
		flipper[i].pry = screenHeight / 2 - xys[1];
	text += sprintf(text, "Flipper #%d location: X=%d, Y=%d [pixcel]\n",
		i+1, flipper[i].prx, flipper[i].pry);
		}
	}
	text += sprintf(text, "\n");
	text += sprintf(text, "Field Height = %d [mm]\n", para->fieldHeight);
	text += sprintf(text, "Field Width  = %d [mm]\n", para->fieldWidth);
	text += sprintf(text, "DXF Field Width = ");
	text += ValuePrintf(text, fieldWidthMax * 2); text += sprintf(text, " [mm]\n");
	text += sprintf(text, "Ball Radius = ");
	text += ValuePrintf(text, ballRadius); text += sprintf(text, " [mm]\n");

	text += sprintf(text, "\n");
	text += sprintf(text, "Shooter speed = %d [mm/s]\n", para->shooterSpd);
	text += sprintf(text, "Kickback speed = %d [mm/s]\n", para->kickbackSpd);
	text += sprintf(text, "VUK Launch speed = %d [mm/s]\n", para->vukSpd);
	text += sprintf(text, "Sling-Shot speed = %d [mm/s]\n", para->slingshotSpd);
	text += sprintf(text, "Jet-Bumper speed = %d [mm/s]\n", para->jetbumperSpd);

	text += sprintf(text, "\n");
	text += sprintf(text, "Sling-Shot trigger speed = %d [mm/s]\n", para->slingshotTrgSpd);
	text += sprintf(text, "Jet-Bumper trigger speed = %d [mm/s]\n", para->jetbumperTrgSpd);
	text += sprintf(text, "Drop-Target trigger speed = %d [mm/s]\n", para->droptargetTrgSpd);
	text += sprintf(text, "Stand-Up trigger speed = %d [mm/s]\n", para->standupTrgSpd);

	text += sprintf(text, "\n");
	i = METAL_HIT_FACTOR * 100 / 1024;
	j = METAL_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Metal Hit factor = %d.%0d [%%]\n", i, j);
	i = PLASTIC_HIT_FACTOR * 100 / 1024;
	j = PLASTIC_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Plastic Hit factor = %d.%0d [%%]\n", i, j);
	i = RUBBER_BAND_HIT_FACTOR * 100 / 1024;
	j = RUBBER_BAND_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Rubber-Band Hit factor = %d.%0d [%%]\n", i, j);
	i = RUBBER_POST_HIT_FACTOR * 100 / 1024;
	j = RUBBER_POST_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Rubber-Post Hit factor = %d.%0d [%%]\n", i, j);
	i = RAMP_HIT_FACTOR * 100 / 1024;
	j = RAMP_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Ramp-Lane Hit factor = %d.%0d [%%]\n", i, j);
	i = HOLE_GUIDE_HIT_FACTOR * 100 / 1024;
	j = HOLE_GUIDE_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Hole-Guide Hit factor = %d.%0d [%%]\n", i, j);
	i = FLIPPER_HIT_FACTOR * 100 / 1024;
	j = FLIPPER_HIT_FACTOR * 100 % 1024;
	text += sprintf(text, "Flipper Hit factor = %d.%0d [%%]\n", i, j);

	text += sprintf(text, "\n");
	text += sprintf(text, "[EOF]\n");
	*text = 0;
	}
*/
}

static int ValuePrintf(char *t, int v)
{
	int	a, b;
/*
	a = (v * fieldHeightMax) / 63956;
	v -= (a * 63956) / fieldHeightMax;
	b = (v * fieldHeightMax * 10) / 63956;
	return sprintf(t, "%d.%0d", a, b);
*/
return 0;
}

/*------------------------------------------------------/
/							/
/	Display HIT-MAP Vector.				/
/							/
/------------------------------------------------------*/
/*
void TestMap(int width, int height, int scale, int fff)
{
	int	i, j, k, w, h, vs, sin, cos;
	int	x, y, xx, yy;
	HIT	*hit;
	char	*cp, bf[80];
	short	*top;

	w = width / 2;
	h = height / 2;

if(fff & SIM_OPTION_VECT)
    {
	hit = &hitList[0];
	i=0;
	for(j=0; j<hitMax; ++j)
	{
	TestMapSub(hit, w, h, scale);
	hit++;
	++i;
	}
    }

if(fff & SIM_OPTION_ZONE)
    {
	for(y=0; y<ZONE_DIVIDE; ++y)
	for(x=0; x<ZONE_DIVIDE; ++x)
	{
	i = (x - ZONE_DIVIDE / 2) * ZONE_SIZE;
	j = (y - ZONE_DIVIDE / 2) * ZONE_SIZE;
	xx = (i + ZONE_SIZE) / scale;
	yy = (j + ZONE_SIZE) / scale;
*/
/*	GrFrame(w+(i/scale), h-yy, w+xx, h-(j/scale), 4);*/
/*
	cp = &bf[0];
	k = zoneMap[x][y];
	if(k >= 0)
		{
		top = &listTag[k];
		k = 0;
		while( *top++ >= 0 ) k++;
		}
	else k = 0;
	sprintf(cp, "%d", k);
*/
/*	GrTextXY(w+(i/scale)+2, h-yy+1, cp, 4, 0);*/
/*
	}
    }
}
*/
/*
static void TestMapSub(HIT *hit, int w, int h, int s)
{
	int	i, j, xx, yy, x, y, vs, m;
	int	sin, cos;

	vs = hit->vs;
	if((hit->atr & ATTR_MASK)==LR_FLIPPER)
		{
		if((hit->atr & 0x0f0f)==LR_FLIPPER) m = 1;
		else m = 0;
		}
	else m = 1;
	m = 1;
	if(m)
	{
	if(hit->atr & 0x8000) MxCircle(w+hit->xt/s, h-hit->yt/s, vs/s, 5);
	else	{
		vs /= s;
		x = hit->xt;
		y = hit->yt;
		sin = hit->sin;
		cos = hit->cos;
		xx = (vs * cos) >> SINCOS_SHIFT;
		yy = (vs * sin) >> SINCOS_SHIFT;
		x /= s;
		y /= s;
		MxLine(w+x, h-y, w+(x+xx), h-(y+yy), 6);
		x += xx / 2;
		y += yy / 2;
		MxLine(w+x, h-y, w+x-(hit->sin>>11), h-(y+(hit->cos>>11)), 4);
		}	
	}

	for(i=0; i<flipperMax; ++i)
	{
	for(j=0; j<FLIPPER_STEP; ++j)
		{
		x = flipper[i].cos1[j] >> 9;
		y = flipper[i].sin1[j] >> 9;
		xx = flipper[i].cx / s;
		yy = flipper[i].cy / s;
		MxLine(w+xx, h-yy, w+(xx+x), h-(yy+y), 1);
		}
	}
}
*/
static void ScanArea(void)
{
	int	i, j, k, l, m, n, f, x, y;
	HIT	*hit;
	short	*list;

	for(y=0; y<ZONE_DIVIDE; y++)
	for(x=0; x<ZONE_DIVIDE; x++) zoneMap[x][y] = -1;

	l = 0;
	list = &listTag[0];
	for(y=0; y<ZONE_DIVIDE; y++)
	for(x=0; x<ZONE_DIVIDE; x++)
	{
	i = (x - ZONE_DIVIDE / 2) * ZONE_SIZE;
	j = (y - ZONE_DIVIDE / 2) * ZONE_SIZE;
	m = k = 0;
	hit = &hitList[0];
	for(n=0; n<hitMax; ++n)
	    {
	f=CheckArea(hit, i+ZONE_SIZE+AREA_GAP, j+ZONE_SIZE+AREA_GAP, i-AREA_GAP, j-AREA_GAP);
	if(f)	{
		if(list >= &listTag[TAG_MAX]) LaunchDebugger("Over Flow TAG-LIST");
		*list++ = k;
		zoneMap[x][y] = l;
		m++;
		}
	    hit++;
	    k++;
	    }
	*list++ = -1;
	l += m + 1;
	}
	tagMax = list - &listTag[0];
}

static int CheckArea(HIT *hit, int xs, int ys, int xw, int yw)
{
	int	i, j, k, x, y, dx, dy, xb, yb, sin, cos;
	int	pos[4];

	if(hit->atr & 0x8000)
	{
	x = hit->xt;		/* circle center position by X */
	y = hit->yt;		/* circle center position by Y */
	x -= (xs + xw) / 2;	/* Area center X */
	y -= (ys + yw) / 2;	/* Area center Y */
	i = (xs - xw) / 2;
	if(i < 0) i = -i;
	j = (ys - yw) / 2;
	if(j < 0) j = -j;
	k = hit->vs;
	dx = k + i + AREA_GAP;
	dy = k + j + AREA_GAP;
	if(x >= -dx && x <= dx && y >= -dy && y <= dy) return 0x0001;
	}
	else
	{
	xb = hit->xt; cos = hit->cos;
	yb = hit->yt; sin = hit->sin;
	i = hit->vs;
	xb += (i * cos) >> SINCOS_SHIFT;
	yb += (i * sin) >> SINCOS_SHIFT;
	x = (hit->xt + xb) / 2;
	y = (hit->yt + yb) / 2;
	x -= (xs + xw) / 2;
	y -= (ys + yw) / 2;
	i = (xs - xw) / 2;
	if(i < 0) i = -i;
	j = (ys - yw) / 2;
	if(j < 0) j = -j;
	k = (hit->xt - xb) / 2;
	if(k < 0) k = -k;
	dx = k + i + AREA_GAP;
	k = (hit->yt - yb) / 2;
	if(k < 0) k = -k;
	dy = k + j + AREA_GAP;
	i = 0;
	if(x >= -dx && x <= dx && y >= -dy && y <= dy) i += 0x0100;
	pos[0] = xs;
	pos[1] = ys;
	pos[2] = xw;
	pos[3] = ys;
	if(HitCheckLine(&pos[0], hit)) i += 0x0100;
	pos[0] = xw;
	pos[1] = ys;
	pos[2] = xw;
	pos[3] = yw;
	if(HitCheckLine(&pos[0], hit)) i += 0x0100;
	pos[0] = xw;
	pos[1] = yw;
	pos[2] = xs;
	pos[3] = yw;
	if(HitCheckLine(&pos[0], hit)) i += 0x0100;
	pos[0] = xs;
	pos[1] = yw;
	pos[2] = xs;
	pos[3] = ys;
	if(HitCheckLine(&pos[0], hit)) i += 0x0100;
	return i;
	}
	return 0x0000;
}

static int GetPositions(short *vect, short *out)
{
	int	i, j, x, y, z;
	int	minx, miny, minz, maxx, maxy, maxz;

	i = *vect++;
/*	i--; */
	minx = miny = minz = 32767;
	maxx = maxy = maxz = -32768;
	for(j=0; j<i; ++j)
	{
	x = *vect++;
	y = *vect++;
	z = *vect++;
	if(minx > x) minx=x;
	if(miny > y) miny=y;
	if(minz > z) minz=z;
	if(maxx < x) maxx=x;
	if(maxy < y) maxy=y;
	if(maxz < z) maxz=z;
	}
	*out++ = (minx + maxx) / 2;
	*out++ = (miny + maxy) / 2;
	*out++ = (minz + maxz) / 2;
	*out++ = maxx - minx;
	*out++ = maxy - miny;
	*out++ = maxz - minz;
/*	i++; */
	return i*3+1;
}

static int SetBox(int a, short *vect, HIT *hit, int r)
{
	int	i, j, x, y, vss;
	LINE	line;
	SINCOS	sico;

	linCount = 0;
	i = *vect++;
	if(i<=2) return i*3+1;
r = 0;
	line.xt = *(vect+i*3-3);	/* X final position */
	line.yt = *(vect+i*3-2);	/* Y final position */
	line.zb = *(vect+i*3-1);	/* Z final position */
	for(j=0; j<i; ++j)
	{
	line.xb = *vect++;
	line.yb = *vect++;
	line.zb = *vect++;
	hit->vs = LineToSinCos_(&line, &sico);
	hit->vss = hit->vs * hit->vs;
	hit->sin = sico.sin;
	hit->cos = sico.cos;
	x = -(sico.sin * r) >> SINCOS_SHIFT;
	y =  (sico.cos * r) >> SINCOS_SHIFT;
	hit->xt = line.xt + x;
	hit->yt = line.yt + y;
	hit->atr = a;
	hit++;
	linCount++;
	line.xt = line.xb;
	line.yt = line.yb;
	line.zt = line.zb;
	}
	return i*3+1;
}

static int SetLine(int a, short *vect, HIT *hit, int r)
{
	int	i, j;
	LINE	line;

	linCount = 0;
	i = *vect++;
	if(i<=2) return i*3+1;

	line.xt = *(vect+i*3-3);	/* X final position */
	line.yt = *(vect+i*3-2);	/* Y final position */
	line.zt = *(vect+i*3-1);	/* Z final position */
	for(j=0; j<i; ++j)
	{
	line.xb = *vect++;
	line.yb = *vect++;
	line.zb = *vect++;

	SetLineSub(&line, r, hit);
	hit->atr = a;			/* Attribute */
	hit++;
	linCount++;
	line.xt = line.xb;
	line.yt = line.yb;
	line.zt = line.zb;
	}
	return i*3+1;
}

static void SetLineSub(LINE *line, int r, HIT *hit)
{
	int	x, y;
	SINCOS	sico;

	hit->vs = LineToSinCos_(line, &sico);	/* Sin, Cos */
	hit->vss = hit->vs * hit->vs;
	hit->sin = sico.sin;
	hit->cos = sico.cos;
	x = (-sico.sin * r) >> SINCOS_SHIFT;
	y = ( sico.cos * r) >> SINCOS_SHIFT;
	hit->xt = line->xt + x;
	hit->yt = line->yt + y;
}

static void SetCircleSub(LINE *line, int r, HIT *hit)
{
	int	x, y, a;

	x = line->xt - line->xb;
	y = line->yt - line->yb;
	a = Isqrt(x * x + y * y);
	hit->vs = (a / 2) + r;
	hit->vss = hit->vs * hit->vs;
	hit->xt = (line->xt + line->xb) / 2;
	hit->yt = (line->yt + line->yb) / 2;
	hit->sin = hit->cos = 0;
}
/*
void DrawOutLine(short *po, int width, int height, int scale)
{
	int	i, j, a;
	int	x0, y0, z0, x1, y1, z1;

	while((a=*po++)!=-1)
	{
	switch(a & ATTR_MASK)
		{
	case BALLS:
	case LR_FLIPPER:
		j = *po++;
		po += j * 3;
		break;
	default:
		j = *po++;
		x0 = *(po+(j*3)-3);
		y0 = *(po+(j*3)-2);
		z0 = *(po+(j*3)-1);
		x0 /= scale;
		y0 /= scale;
		for(i=0; i<j; ++i)
			{
			x1 = *po++;
			y1 = *po++;
			z1 = *po++;
			x1 /= scale;
			y1 /= scale;
			MxLine(width/2+x0, height/2-y0,
					width/2+x1, height/2-y1, 7);
			x0 = x1;
			y0 = y1;
			z0 = z1;
			}
		break;
		}
	}
}
*/
static void SortFirstBig(int *val, short *idx, int count)
{
	int	i, j, u, w;

	for(i=0; i<count; ++i) idx[i]=i;

	for(i=0; i<count; ++i)
	{
	u = val[i];
	for(j=(i+1); j<count; ++j)
		{
		if(u < val[j])
			{ w=val[i]; val[i]=val[j]; val[j]=w;
			  w=idx[i]; idx[i]=idx[j]; idx[j]=w;
			  u = val[i]; }
		}
	}
}

static int HitCheckLine(int *a, HIT *b)
{
	int	xt, xb, yt, yb, dx, tt;
	int	x0, x1, y0, y1, vs;

	x0 = *a++;
	y0 = *a++;
	x1 = *a++;
	y1 = *a;
	x0 -= b->xt;
	y0 -= b->yt;
	x1 -= b->xt;
	y1 -= b->yt;
	yt = y0 * b->cos - x0 * b->sin;
	yb = y1 * b->cos - x1 * b->sin;

/* Hit Check Phase I */
	if((yt ^ yb)>= 0) return 0;

	xt = x0 * b->cos + y0 * b->sin;
	xb = x1 * b->cos + y1 * b->sin;
	if(xt < 0 && xb < 0) return 0;
	vs = b->vs;
	vs <<= SINCOS_SHIFT;
	if(xt > vs && xb > vs) return 0;

	if(yt>=0 && yb<0)
	{
	if(xt<xb) { dx = ((xb-xt) * yt) / (yt-yb); tt = dx + xt; }
	else { dx = ((xt-xb) * -yb) / (yt-yb); tt = dx + xb; }
	}
	else
	{
	if(xt<xb) { dx = ((xb-xt) * -yt) / (yb-yt); tt = dx + xt; }
	else { dx = ((xt-xb) * yb) / (yb-yt); tt = dx + xb; }
	}

/* Hit Check Phase III */
	if(tt > vs || tt < 0) return 0;
	return 1;
}

static int LineToSinCos_(LINE *line, SINCOS *sico)
{
	int	a, dx, dy;
	unsigned int b, c;

	dx = line->xb - line->xt;
	dy = line->yb - line->yt;
	if(dx < 0) b = -dx; else b = dx;
	if(dy < 0) c = -dy; else c = dy;
	a = Isqrt(b * b + c * c);
	if(a)
	{
	sico->sin = (dy << SINCOS_SHIFT) / a;
	sico->cos = (dx << SINCOS_SHIFT) / a;
	}
	else { sico->sin = dy; sico->cos = dx; }
	return a;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PresetPolyline(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, l, x, y, vs;
	LINE	line;
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;

	line.xt = *vect++;
	line.yt = *vect++;
	line.zt = *vect++;
	if(line.xt > fieldWidthMax) fieldWidthMax = line.xt;
	vect -= 3;
	if(i > 2) l = i; else l = 1;
	for(j=0; j<l; ++j)
	{
	k = ((j + 1) % i) * 3;
	line.xb = *(vect + k    );
	line.yb = *(vect + k + 1);
	line.zb = *(vect + k + 2);
	if(line.xb > fieldWidthMax) fieldWidthMax = line.xb;
	vs = LineToSinCos_(&line, &sico);
	x  = (-sico.sin * r) >> SINCOS_SHIFT;
	y  = ( sico.cos * r) >> SINCOS_SHIFT;
	hit->atr = a;
	hit->xt = line.xt + x;
	hit->yt = line.yt + y;
	hit->sin = sico.sin;
	hit->cos = sico.cos;
	hit->vs  = vs;
	hit->vss = hit->vs * hit->vs;
	hit++;
	linCount++;

	k = ((j + 2) % i) * 3;
	x = *(vect + k    ) - line.xt;
	y = *(vect + k + 1) - line.yt;
	if((y * sico.cos - x * sico.sin) < 0 && i > 2)
		{
		hit->atr = a | 0x8000;
		hit->xt = line.xb;
		hit->yt = line.yb;
		hit->vs = r;
		hit->vss = hit->vs * hit->vs;
		hit->sin = hit->cos = 0;
		hit++;
		cirCount++;
		}
	line.xt = line.xb;
	line.yt = line.yb;
	line.zt = line.zb;
	}
	return (i * 3 + 1);
}

static int PresetCircle(int a, short *vect, HIT *hit, int r)
{
	int	i, j, v, zs, sin, cos;
	int	x, y, z, minx, miny, maxx, maxy;
	LINE	line;

	i = *vect++;
	minx = miny =  65536;
	maxx = maxy = -65536;
	zs = 0;
	for(j=0; j<i; ++j)
	{
	x = *vect++;
	y = *vect++;
	z = *vect++;
	zs += z;
	if(minx > x) minx = x;
	if(miny > y) miny = y;
	if(maxx < x) maxx = x;
	if(maxy < y) maxy = y;
	}
	hit->atr = a;
	hit->xt = (maxx + minx) / 2;
	hit->yt = (maxy + miny) / 2;
	hit->vs = (((maxx - minx) + (maxy - miny)) / 4) + r;
	hit->vss = hit->vs * hit->vs;
	hit->sin = hit->cos = 0;
	return (i * 3 + 1);
}

static int PresetFlipper(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, l, m, u, v, dx, dy;
	int	val[128];
	short	idx[128], *ptr;
	LINE	line, cp[2];
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;
	ptr = vect;
	if(i > 128) LaunchDebugger("Over Flow SORT buffer !!!");

	line.xt = *(ptr + i * 3 - 3);
	line.yt = *(ptr + i * 3 - 2);
	line.zt = *(ptr + i * 3 - 1);
	for(j=0; j<i; ++j)
	{
	line.xb = *ptr++;
	line.yb = *ptr++;
	line.zb = *ptr++;
	dx = line.xb - line.xt;
	dy = line.yb - line.yt;
	idx[j] = j;
	val[j] = dx * dx + dy * dy;
	line.xt = line.xb;
	line.yt = line.yb;
	line.zt = line.zb;
	}
	SortFirstBig(&val[0], &idx[0], i);

	for(j=0; j<2; ++j)
	{
	k = ((idx[j]+i-1) % i) * 3;
	line.xt = *(vect + k    );
	line.yt = *(vect + k + 1);
	line.zt = *(vect + k + 2);
	k = idx[j] * 3;
	line.xb = *(vect + k    );
	line.yb = *(vect + k + 1);
	line.zb = *(vect + k + 2);

	hit->atr = a;
	SetLineSub(&line, r, hit);
	cp[j].xt = line.xt;
	cp[j].yt = line.yt;
	cp[j].zt = line.zt;
	cp[j].xb = line.xb;
	cp[j].yb = line.yb;
	cp[j].zb = line.zb;
	hit++;
	linCount++;
	}

	u = v = k = 0;
	for(j=0; j<2; ++j)
	{
	line.xt = cp[  k  ].xt;
	line.yt = cp[  k  ].yt;
	line.zt = cp[  k  ].zt;
	line.xb = cp[k ^ 1].xb;
	line.yb = cp[k ^ 1].yb;
	line.zb = cp[k ^ 1].zb;
	hit->atr = a | 0x8000;
	SetCircleSub(&line, r, hit);
	if(v < hit->vs) { u = j; v = hit->vs; }
	hit++;
	cirCount++;
	k++;
	}

	j = (a >> 4) & 0x3;
	if((j+1) > flipperMax) flipperMax = j + 1;
	k = a & 15;
	line.xt = (hit-2+u)->xt;
	line.yt = (hit-2+u)->yt;
	flipper[j].cx = line.xt;
	flipper[j].cy = line.yt;
flipper[j].cz = 0;
	u ^= 1;
	line.xb = (hit-2+u)->xt;
	line.yb = (hit-2+u)->yt;
	l = line.xb - line.xt;	
	m = line.yb - line.yt;	
	flipper[j].hl = Isqrt(l * l + m * m);
	LineToSinCos_(&line, &sico);
	if(a & 0x10) { sico.cos = -sico.cos; sico.sin = -sico.sin; }
	flipper[j].sin1[k] =  sico.cos;
	flipper[j].cos1[k] = -sico.sin;

	return (i * 3 + 1);
}

static int PresetSwitch(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, l, x, y, vs;
	int	val[128];
	short	idx[128], *ptr;
	LINE	line;
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;

	if(i <= 2)
	{
	line.xt = *vect++;
	line.yt = *vect++;
	line.zt = *vect++;
	line.xb = *vect++;
	line.yb = *vect++;
	line.zb = *vect++;
	SetLineSub(&line, 0, hit);
	hit->atr = a;
	hit++;
	linCount++;	
	return (i * 3 + 1);
	}

/*----------------------------------------------/
/	Ramp Enter 1 to 4			/
/----------------------------------------------*/
	for(j=0; j<i; ++j)
	{
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	line.zt = *(vect + j * 3 + 2);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	line.zb = *(vect + k * 3 + 2);
	idx[j] = j;
	val[j] = (line.yt + line.yb) / 2;
	}
	SortFirstBig(&val[0], &idx[0], i);

	k = (idx[i-1] + 1) % i;
	j = (idx[i-1] - 1) % i;
	if( *(vect + k * 3 + 1) > *(vect + j * 3 + 1) ) {
		x = -1; y = 0; }
	else {	x = 0; y  = 1; }

	k = (idx[i-1] + x) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	line.zb = *(vect + k * 3 + 2);
	k = (idx[i-1] + y) % i;
	line.xt = *(vect + k * 3);
	line.yt = *(vect + k * 3 + 1);
	line.zt = *(vect + k * 3 + 2);
	SetLineSub(&line, 0, hit);
	hit->atr = a;
	hit++;
	linCount++;	

	k = a & 31;
	j = idx[0];
	vs = *(vect + j * 3 + 1);
	j++; j %= i;
	vs += *(vect + j * 3 + 1);
	swPara[k].yt = vs >> 1;
	vs = *(vect + j * 3 + 2);
	swPara[k].zw = vs;
	vs = (line.yt + line.yb) / 2;
	swPara[k].yb = vs;

	return (i * 3 + 1);
}

static int PresetOpenPolyline(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, l, x, y, vs;
	LINE	line;
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;

	line.xt = *vect++;
	line.yt = *vect++;
	line.zt = *vect++;
	if(line.xt > fieldWidthMax) fieldWidthMax = line.xt;
	vect -= 3;
	for(j=0; j<(i-1); ++j)
	{
	k = ((j + 1) % i) * 3;
	line.xb = *(vect + k    );
	line.yb = *(vect + k + 1);
	line.zb = *(vect + k + 1);
	if(line.xb > fieldWidthMax) fieldWidthMax = line.xb;
	vs = LineToSinCos_(&line, &sico);
	x  = (-sico.sin * r) >> SINCOS_SHIFT;
	y  = ( sico.cos * r) >> SINCOS_SHIFT;
	hit->atr = a;
	hit->xt = line.xt + x;
	hit->yt = line.yt + y;
	hit->sin = sico.sin;
	hit->cos = sico.cos;
	hit->vs  = vs;
	hit->vss = hit->vs * hit->vs;
	hit++;
	linCount++;

	if((j + 2) >= i) ;
	else {
	k = ((j + 2) % i) * 3;
	x = *(vect + k    ) - line.xt;
	y = *(vect + k + 1) - line.yt;
	if((y * sico.cos - x * sico.sin) < 0 && i > 2)
		{
		hit->atr = a | 0x8000;
		hit->xt = line.xb;
		hit->yt = line.yb;
		hit->vs = r;
		hit->vss = hit->vs * hit->vs;
		hit->sin = hit->cos = 0;
		hit++;
		cirCount++;
		}
	     }
	line.xt = line.xb;
	line.yt = line.yb;
	line.zt = line.zb;
	}
	return (i * 3 + 1);
}

static int PresetDropTarget(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, u, v, dx, dy;
	int	val[128];
	short	idx[128];
	LINE	line;

	cirCount = linCount = 0;
	i = *vect++;
	if(i!=4) LaunchDebugger("FALSE 'DropTarget' !!!");

	for(j=0; j<i; ++j)
	{
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	line.zt = *(vect + j * 3 + 2);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	line.zb = *(vect + k * 3 + 2);
	dx = line.xb - line.xt;
	dy = line.yb - line.yt;
	idx[j] = j;
	val[j] = dx * dx + dy * dy;
	}
	SortFirstBig(&val[0], &idx[0], i);

	j = idx[0];
	k = (j + 1) % i;
	u  = *(vect + j * 3 + 1);
	u += *(vect + k * 3 + 1);
	u >>= 1;
	j = idx[1];
	k = (j + 1) % i;
	v  = *(vect + j * 3 + 1);
	v += *(vect + k * 3 + 1);
	v >>= 1;

	if(u < v) j = 0; else j = 1;
	j = idx[j];
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	SetLineSub(&line, r, hit);
	hit->atr = a;
	hit++;
	linCount++;

	for(j=0; j<2; ++j)
	{
	hit->atr = a | 0x8000;
	hit->xt = line.xt;
	hit->yt = line.yt;
	hit->vs = r;
	hit->vss = hit->vs * hit->vs;
	hit->sin = hit->cos = 0;
	hit++;
	cirCount++;
	line.xt = line.xb;
	line.yt = line.yb;
	}

	for(j=0; j<2; ++j)
	{
	k = idx[2+j];
	line.xt = *(vect + k * 3);
	line.yt = *(vect + k * 3 + 1);
	SetLineSub(&line, r, hit);
	hit->atr = a;
	hit++;
	linCount++;
	}

	return (i * 3 + 1);
}

static int PresetStandUp(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, u, v, dx, dy;
	int	val[128];
	short	idx[128];
	LINE	line;

	cirCount = linCount = 0;
	i = *vect++;
	if(i!=4) LaunchDebugger("FALSE 'Stand Up' !!!");

	for(j=0; j<i; ++j)
	{
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	line.zt = *(vect + j * 3 + 2);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	line.zb = *(vect + k * 3 + 2);
	dx = line.xb - line.xt;
	dy = line.yb - line.yt;
	idx[j] = j;
	val[j] = dx * dx + dy * dy;
	}
	SortFirstBig(&val[0], &idx[0], i);

	j = idx[0];
	k = (j + 1) % i;
	u  = *(vect + j * 3 + 1);
	u += *(vect + k * 3 + 1);
	u >>= 1;
	j = idx[1];
	k = (j + 1) % i;
	v  = *(vect + j * 3 + 1);
	v += *(vect + k * 3 + 1);
	v >>= 1;

	if(u < v) j = 0; else j = 1;
	j = idx[j];
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	SetLineSub(&line, r, hit);
	hit->atr = a;
	hit++;
	linCount++;

	return (i * 3 + 1);
}

static int PresetHole(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, u, v;
	int	val[64];
	short	idx[64];
	LINE	line;
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;
	if(i!=4) LaunchDebugger("FALSE 'Hole' !!!");

	j = 1;
	line.xb = *(vect + j * 3);
	line.yb = *(vect + j * 3 + 1);
	line.zb = *(vect + j * 3 + 2);
	k = (j + 1) % i;
	line.xt = *(vect + k * 3);
	line.yt = *(vect + k * 3 + 1);
	line.zt = *(vect + k * 3 + 2);
	LineToSinCos_(&line, &sico);
	u = (sico.cos * r) >> SINCOS_SHIFT;
	v = (sico.sin * r) >> SINCOS_SHIFT;
/*	u *= 3;
	v *= 3;
	u >>= 2;
	v >>= 2; */
	u >>= 1;
	v >>= 1;
	line.xt += u;
	line.yt += v;
	line.xb -= u;
	line.yb -= v;
	SetLineSub(&line, r, hit);
	hit->atr = a | _HOLE_GUIDE;
	hit++;
	linCount++;

	return (i * 3 + 1);
}

static int PresetSaucer(int a, short *vect, HIT *hit, int r)
{
	int	i, j, k, u, v;
	LINE	line;
	SINCOS	sico;

	cirCount = linCount = 0;
	i = *vect++;
	if(i!=4) LaunchDebugger("FALSE 'Hole' !!!");

	j = 1;
	line.xt = *(vect + j * 3);
	line.yt = *(vect + j * 3 + 1);
	line.zt = *(vect + j * 3 + 2);
	k = (j + 1) % i;
	line.xb = *(vect + k * 3);
	line.yb = *(vect + k * 3 + 1);
	line.zb = *(vect + k * 3 + 2);
	LineToSinCos_(&line, &sico);
	u = (sico.cos * r) >> SINCOS_SHIFT;
	v = (sico.sin * r) >> SINCOS_SHIFT;
	u >>= 1;
	v >>= 1;
	line.xt += u;
	line.yt += v;
	line.xb -= u;
	line.yb -= v;
	SetLineSub(&line, r, hit);
	hit->atr = a | _HOLE_GUIDE;
	hit++;
	linCount++;

	return (i * 3 + 1);
}
