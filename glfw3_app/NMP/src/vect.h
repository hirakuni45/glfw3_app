#pragma once

#include <stdint.h>

void PickUpFirst(short *vect);
void MakeupVectors(FIELD *para, short *vect, char *);

void TestMap(int width, int height, int scale, int fff);

void DrawOutLine(short *po, int width, int height, int scale);
