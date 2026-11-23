#pragma once

#include <stdint.h>

void InitBloody(PLAYER *ply);
void InitBloodyPlay(PLAYER *ply);
void ServiceBloody(PLAYER *ply, char *sct);

void DoBloodyMultiBallReady(PLAYER *ply);

void StartRoundBloody(PLAYER *ply);
