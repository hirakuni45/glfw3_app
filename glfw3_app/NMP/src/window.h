#include "win.h"

void MxInitWindow(void);
WINDOW *MxOpenWindow(PART *);
void MxCloseWindow(WINDOW *);
void ServiceWindowSystem(void);

static void EraseWindow(WINDOW *);
static void ReScanWindowTag(void);

int GetStickPosition(int);
static int GetStringCount(char *);
static void MakeupWindowArea(int, RECT *, RECT *);
static void MakeupSystemClips(RECT *);

void ServiceReactionWindow(void);

static void MakeupUnderClips(RECT *, int, int);
static int ScanWindowZone(RECT *, int, int, int);
static void MakeupPartRect(RECT *, RECT *, RECT *);
static void DrawWinParts(WINDOW *);
static void DrawWinPartsUser(WINDOW *);
static void DrawOnlyPart(PART *, RECT *);
static void DrawBaseFrame(RECT *, int);
static void DrawWinText(RECT *, char *, int, int);
static char * ScanTextSize(char *, int *, int *);
static int DrawFont(RECT *, char *, int, int);
static void DrawSliderParts(int, int, int);
/*static void SaveWireFrames(RECT *);
static void LoadWireFrames(RECT *);*/
static void DrawWireFrames(RECT *);
static void DrawPositiveFrame(RECT *);
static void DrawNegativeFrame(RECT *);
static void DrawWinFrames(RECT *);
static void DrawWinSeparateH(RECT *);
static void DrawWinSeparateV(RECT *);
void MxFilledRect(RECT *, int);
void MxRect(RECT *, int);
void MxLine(int, int, int, int, int);
void MxCircle(int, int, int, int);
void MxPlot(int, int, int);
void MxPlotClip(int, int, int);
int MxPixel(int, int);
void MxPrintf(char *, ...);

