#include <stdio.h>
#include "mobj.h"
#include "task.h"
#include "window.h"
#include "cdfs.h"
#include "top.h"
#include "mw.h"
#include "saturn.h"

extern short monitorFlag;
extern volatile unsigned short padTriggerP[], padTriggerN[];
extern volatile short mouseX, mouseY, mouseB, mouseT;
extern char multiPlayerEnable;

static void DebugExitTask(void);

static char venusExit, debugExit;
static WINDOW *clst;

static PART arenaIcon[] = {
{	LOCATION_AUTO, LOCATION_AUTO, 8*6+16, 28,
	(void *)"EXIT",
	PART_ICON_TEXT,
	0x00000000, 0,
	NULL,
	ExitVenus,
	NULL	}
};
static PART vectorIcon[] = {
{	LOCATION_AUTO, LOCATION_AUTO, 8*9+16, 28,
	(void *)"CLUSTER",
	PART_ICON_TEXT,
	0x00000000, 0,
	NULL,
	LaunchCluster,
	NULL	}
};

/*--------------------------------------/
/	ドキュメント・画面		/
/--------------------------------------*/
static const SLIDER docSlider = {
	NULL,
	NULL,
	0, 0, 
	0, 0,
	0, 0 };
static PART windowDoc[] = {
{	LOCATION_CENTER, LOCATION_CENTER, 512+12, 256+12,	
	(void *)"\001ラスト・グラジエーターについて！\n",
	PART_HANDLE_BASE,
	BASE_WITH_RESIZE, 0,
	NULL,
	NULL,
	(void *)&windowDoc[1]	},
{	6, 6, -(16+7), -(16+7),
	NULL,
	PART_TEXT_BOX,
	0x00000000, 0,
	(void *)&docSlider,
	NULL,
	(void *)&windowDoc[2]	},
{	-16, 6, 10, -(16+7),
	(void *)&windowDoc[1],
	PART_VERTICAL_SLIDER,
	0x00000000, 0,
	(void *)&docSlider,
	NULL,
	(void *)&windowDoc[3]	},
{	6, -16, -(16+7), 10,
	(void *)&windowDoc[1],
	PART_HORIZONTAL_SLIDER,
	0x00000000, 0,
	(void *)&docSlider,
	NULL,
	NULL	}
};

/*--------------------------------------/
/	サウンド・シーケンサーの卓	/
/--------------------------------------*/
static const SLIDER soundSlider = {
	NULL,
	NULL,
	0, 0, 
	0, 0, 
	0, 0 };
static const PART soundPalet[] = {
{	100, 280, 198, 135,
	(void *)"\001シーケンサー\n",
	PART_HANDLE_BASE,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&soundPalet[1]	},
{	10, 10, 160, 96,
	NULL,
	PART_TEXT_CHOOSER,
	0x00000000, 0,
	(void *)&soundSlider,
	PlaySoundSaturn,
	(void *)&soundPalet[2]	},
{	LOCATION_CENTER, 114, 48, 16,
	(void *)" PLAY ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	PlaySoundSaturn,
	(void *)&soundPalet[3]	},
{	177, 10, 10, 96,
	(void *)&soundPalet[1],
	PART_VERTICAL_SLIDER,
	0x00000000, 0,
	(void *)&soundSlider,
	NULL,
	NULL	}
};

/*--------------------------------------/
/	シネパック・プレイヤー		/
/--------------------------------------*/
static const SLIDER cpkSlider = {
	NULL,
	NULL,
	0, 0, 
	0, 0, 
	0, 0 };
static PART cpkPalet[] = {
{	100, 280, 198, 135,
	(void *)"\001ＣＰＫ・プレイヤー\n",
	PART_HANDLE_BASE,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cpkPalet[1]	},
{	10, 10, 160, 96,
	NULL,
	PART_TEXT_CHOOSER,
	0x00000000, 0,
	(void *)&cpkSlider,
	NULL,
	(void *)&cpkPalet[2]	},
{	28, 114, 56, 16,
	(void *)"\001MOUNT",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	CpkMount,
	(void *)&cpkPalet[3]	},
{	(28+30+56), 114, 56, 16,
	(void *)"\001PLAY",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	CpkPlay,
	(void *)&cpkPalet[4]	},
{	177, 10, 10, 96,
	(void *)&cpkPalet[1],
	PART_VERTICAL_SLIDER,
	0x00000000, 0,
	(void *)&cpkSlider,
	NULL,
	NULL	}
};

/*--------------------------------------/
/	ＣＤ・プレイヤーの卓		/
/--------------------------------------*/
static const PART cdPlayer[] = {
{	400, 80, 190, 88,
	(void *)"\001CD PLAYER\n",
	PART_HANDLE_BASE,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[1]	},
{	10+50*0+10*0, 10, 50, 16,
	(void *)"\001 STOP ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[2]	},
{	10+50*1+10*1, 10, 50, 16,
	(void *)"\001 PLAY ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[3]	},
{	10+50*2+10*2, 10, 50, 16,
	(void *)"\001 PAUSE ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[4]	},
{	10+50*0+10*0, 35, 50, 16,
	(void *)"\001 -TRACK ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[5]	},
{	10+50*1+10*1, 35, 50, 16,
	(void *)"\001 +TRACK ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[6]	},
{	10+50*0+10*0, 60, 50, 16,
	(void *)"\001 -SCAN ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&cdPlayer[7]	},
{	10+50*1+10*1, 60, 50, 16,
	(void *)"\001 +SCAN ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	NULL	}
};

/*--------------------------------------/
/	クラスター・ゲーム		/
/--------------------------------------*/
#define CLUSTER_WIDTH	20
#define CLUSTER_HEIGHT	12

static PART clusterGame[] = {
{	80, 80, 24*20+12, 24*12+12,
	(void *)"\001CLUSTER Ver 0.1\n",
	PART_HANDLE_BASE,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&clusterGame[1]	},
{	6, 4, 40, 16,
	(void *)"\001 NEW ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&clusterGame[2]	},
{	6, 6+24, -6, -6,
	NULL,
	PART_IMAGE_BOX,
	0x00000000, 0,
	NULL,
	ClusterMain,
	NULL	}
};
static short clusterMode = 0;
static short clusterPosX, clusterPosY;
static char clusterBuff[CLUSTER_WIDTH * CLUSTER_HEIGHT];
static char nameBuff[16];

/*--------------------------------------/
/	デバッグ・画面			/
/--------------------------------------*/
static PART windowDebug[] = {
{	LOCATION_CENTER, LOCATION_CENTER, 320+12, 200+12,
	(void *)"\001Venus Shell Debugger v1.1b\n",
	PART_HANDLE_BASE,
	BASE_WITH_RESIZE, 0,
	NULL,
	NULL,
	(void *)&windowDebug[1]	},
{	6, 6, -(16+7), -(16+7+26),
	NULL,
	PART_TEXT_BOX,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&windowDebug[2]	},
{	-16, 6, 10, -(16+7+26),
	NULL,
	PART_VERTICAL_SLIDER,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&windowDebug[3]	},
{	6, -(16+26), -(16+7), 10,
	NULL,
	PART_HORIZONTAL_SLIDER,
	0x00000000, 0,
	NULL,
	NULL,
	(void *)&windowDebug[4]	},
{	LOCATION_CENTER, -22, 48, 16,
	(void *)" EXIT ",
	PART_TOUCH_SWITCH,
	0x00000000, 0,
	NULL,
	DebugExitTask,
	NULL	},
};

void LaunchDebugger(const char *msg)
{
	WINDOW	*win;

	ResetMotionObject();
	MxInitWindow();

	windowDebug[0].rect.xt = LOCATION_CENTER;
	windowDebug[0].rect.yt = LOCATION_CENTER;
	windowDebug[0].rect.xb = 320+12;
	windowDebug[0].rect.yb = 200+12;
	windowDebug[1].image = (void *)msg;

	win = MxOpenWindow(&windowDebug[0]);

	debugExit = 0;
	for( ; ; )
	{
	OpenMotionObject(0, 0);
	ServiceMouse();
	ServiceReactionWindow();
	ServiceWindowSystem();
	CloseMotionObject();
	if(debugExit) break;
	}
}

static void DebugExitTask(void)
{
	debugExit = 1;
}

/*----------------------------------------------/
/						/
/	Service Task Routine			/
/						/
/----------------------------------------------*/
void ExitVenus(void)
{
	venusExit = 0;
}

/*----------------------------------------------/
/						/
/	ビジュアル・シェル			/
/						/
/----------------------------------------------*/
void VenusShell(void)
{
	int	x, y;
	WINDOW	*docw, *arena, *vect, *cine;
	char	*ptr;

	multiPlayerEnable = 0;

	LaunchResource("SYSTEM.RES");

	ResetMotionObject();
	MxInitWindow();
	MountKanjiFont();

	windowDoc[1].image = (void *)GetResource("DOCS", 0);

	docw = MxOpenWindow(&windowDoc[0]);
	arena = MxOpenWindow(&arenaIcon[0]);
	vect = MxOpenWindow(&vectorIcon[0]);
	cine = MxOpenWindow(&cpkPalet[0]);

	venusExit = 1;
	while(venusExit)
	{
	OpenMotionObject(0, 0);
	ServiceMouse();
	ServiceReactionWindow();
	ServiceWindowSystem();
	ServiceScreenSaver();
	CloseMotionObject();
	}
}

void PlaySoundSaturn(void)
{
	int	i;

	i = soundPalet[1].action;
	RingSfx(i);
}

void CpkPlay(void)
{
	int	i;
	char	*ptr, c;

	ptr = (char *)cpkPalet[1].image;
	if(ptr==NULL) return;
	i = cpkPalet[1].action;
	while(i)
		{ while((c = *ptr++)!='\n') ;
		--i; }
	i = 0;
	while((c = *ptr++)!='\n') nameBuff[i++] = c;
	nameBuff[i] = 0;
	while(mouseB!=0) SyncBlanking();
/*	ServiceCPK(&nameBuff[0]); */
	MxRefreshWindow();
}

void CpkMount(void)
{
	int	i;
	uint8_t *ptr;

	ptr = get_SATURN_DRAM_TOP();
	ptr += 0xf0000;
/*	i = MountCD(ptr); */
	cpkPalet[1].image = (void *)ptr;
	while(mouseB!=0) SyncBlanking();
	MxRefreshWindow();
}

/*----------------------------------------------/
/						/
/	クラスター・メイン・モジュール		/
/						/
/----------------------------------------------*/
void LaunchCluster(void)
{
	clst = MxOpenWindow(&clusterGame[0]);
}

void ClusterMain(void)
{
	int	i, j;

	switch(clusterMode)
	{
	case 0:
		clusterMode++;
		break;
	case 1:
		break;
	default:
		break;
	}
}
