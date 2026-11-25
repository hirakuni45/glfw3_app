#pragma once
//=========================================================================//
/*! @file
	@brief  タスク管理とその周辺・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

extern const uint8_t *resIndex;
extern const uint8_t *resImage;

void DoMainGameOnly(int no);
void ServiceTask(void);
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
