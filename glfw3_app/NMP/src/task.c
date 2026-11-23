#include <stdio.h>
#include "saturn.h"
#include "cdfs.h"
#include "etc.h"
#include "task.h"
#include "pinball.h"
#include "top.h"

#define TASK_MAX	8

extern char errMsg[];

static void ServiceTask(void);

static void (*taskAdr[TASK_MAX])(void);
static void (*taskGo[TASK_MAX])(void);

const uint8_t *resIndex, *resImage;

/*----------------------------------------------/
/						/
/	Service Task Routine			/
/						/
/----------------------------------------------*/
void DoMainGameOnly(int no)
{
	InitNECRONOMICON();
	ReadBackupSystem();
	ResetTask();
	if(no==0) CreateTask(EntryLogo);
	else if(no==1 || no > 5) CreateTask(EntryTitle);
	else ServiceRealmsMode(no);
	while(1) ServiceTask();
}

static void ServiceTask(void)
{
	int	i;

	for(i=0; i<TASK_MAX; ++i) taskGo[i] = taskAdr[i];
	for(i=0; i<TASK_MAX; ++i) if(taskGo[i]!=NULL) (*taskGo[i])();
}

void CreateTask(void (*task)(void))
{	
	int	i;

	for(i=0; i<TASK_MAX; ++i) if(taskAdr[i]==NULL) break;
	taskAdr[i] = task;
}

void KillTask(void (*task)(void))
{
	int	i, j;

	for(i=0; i<TASK_MAX; ++i) if(taskAdr[i]==task) break;
	for(j=i; j<(TASK_MAX-1); ++j) taskAdr[j] = taskAdr[j+1];
	taskAdr[TASK_MAX-1] = NULL;
}

void KillAllTask(void)
{
	int	i;

	for(i=0; i<TASK_MAX; ++i) taskAdr[i] = NULL;
}

void ResetTask(void)
{
	KillAllTask();
	CreateTask(SyncBlanking);
}

/*----------------------------------------------/
/												/
/	Resource Functions							/
/												/
/----------------------------------------------*/
void InitResource(void)
{
	const uint8_t *p;

	resIndex = get_SATURN_DRAM_TOP();
	p = resIndex;
	while(*p != 0xff) p += 4;
	resImage = p + 4;
}

int TakeResourcePage(const char *name)
{
	int	i, j;
	const uint8_t *src;

	src = GetResourcePtr(name);
	i = 0;
	while((j = *src) <= 0x20) {
		src += 4;
		i++;
	}
	return i;
}

uint32_t TakeInt(const uint8_t *ptr)
{
	uint32_t v;

	v  = *ptr++; v <<= 8;
	v += *ptr++; v <<= 8;
	v += *ptr++; v <<= 8;
	v += *ptr;
	return v;
}

uint32_t TakeMiddle(const uint8_t *ptr)
{
	uint32_t v;

	v  = *ptr++; v <<= 8;
	v += *ptr++; v <<= 8;
	v += *ptr++;
	return v;
}

uint32_t TakeShort(const uint8_t *ptr)
{
	uint32_t v;

	v  = *ptr++; v <<= 8;
	v += *ptr;
	return v;
}

const uint8_t *GetResourcePtr(const char *name)
{
	int	c;
	const uint8_t *src;
	uint32_t a, b;

	a = TakeInt((const uint8_t *)name);
	src = resIndex;
	while((c = *(char *)src) != -1)
	{
	if(c >= 0x20)
		{
		b = TakeInt(src);
		src += 4;
		if(a==b) return src;
		}
	else src += 4;
	}
	sprintf(errMsg, "Stop.\nFalse !\nCan't find resource ID at '%s'.\n", name);
	LaunchDebugger(errMsg);
	return NULL;
}

const uint8_t *GetResource(const char *name, int no)
{
	const uint8_t *src;

	src = GetResourcePtr(name);
	return (resImage + TakeMiddle(src + 1 + (no << 2)));
}
