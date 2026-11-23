#pragma once
#include <stdint.h>

void DoMainGameOnly(int no);

void CreateTask(void (*)(void));
void KillTask(void (*)(void));
void KillAllTask(void);
void ResetTask(void);

void InitResource(void);
int TakeResourcePage(const char *name);
uint32_t TakeInt(const uint8_t *ptr);
uint32_t TakeMiddle(const uint8_t *ptr);
uint32_t TakeShort(const uint8_t *ptr);
const uint8_t *GetResourcePtr(const char *name);
const uint8_t *GetResource(const char *name, int no);
