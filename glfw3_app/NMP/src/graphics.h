#pragma once

void GrInit(char *);
void GrClear(void);
static int GrPixelFast(int x, int y);
static void GrPlotFast(int x, int y, int c);
static int GrPixelSlow(int x, int y);
static void GrPlotSlow(int x, int y, int c);
void GrLine(int x1, int y1, int x2, int y2, int c);
void GrCircle(int x1, int y1, int r, int c);
void GrBox(int x, int y, int xx, int yy, int c);
void GrFrame(int x, int y, int xx, int yy, int c);
void GrTextXY(int x, int y, char *t);
void GrPrintf(char *, ...);
void GrPutChar(int);
void SetSpeedLevel(int x, int c);
void ExtractResourceImage(char *image, char *out);
void ExtractForVDP1(char *image, short *vdp);
void ExtractForVDP2(char *image, int x, int y);
void HaltMessage(char *msg);
void MessageBox(int xt, int yt, int xb, int yb);
void CopyImage(short *, int, int, int, int);
void DissolveCopyImage(short *img, int x, int y, int w, int h, int lv);
