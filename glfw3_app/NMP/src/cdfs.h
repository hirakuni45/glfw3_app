#pragma once
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