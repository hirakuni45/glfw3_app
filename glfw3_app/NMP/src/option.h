#pragma once

#define OPTION_PAD_MAX		8

#define OPTION_NO_PLAYER	0
#define OPTION_NO_CONTROLLER	1
#define OPTION_NO_ARROW		2
#define OPTION_NO_BGM		3
#define OPTION_NO_REALMS_COMP	4

typedef struct {
	char	topScore[12 * 8];
	char	topName[3 * 8];
} NECRONOMICON;
