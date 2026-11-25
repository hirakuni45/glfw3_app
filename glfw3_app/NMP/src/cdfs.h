#pragma once
//=========================================================================//
/*! @file
	@brief  SEGA SATURN CD file-system・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

void InitFileSystem(void);
void ReadFile(const char *name, void *dst);
void LaunchResource(const char *name);
void ServiceAudioCD(void);
void PlayCD(int, int);
int ProbeBuckupMemory(void);
void ReadBackupSystem(void);
void WriteBackupSystem(void);

uint8_t *get_SATURN_DRAM_TOP();

extern int cdStatus;
extern short cdFadeLevel;
