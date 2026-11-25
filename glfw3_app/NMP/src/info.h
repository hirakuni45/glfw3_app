#pragma once
//=========================================================================//
/*! @file
	@brief  Necronomicon インフォメーション関係・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>
#include "mobj.h"

#define INFO_MULTI_MAX		4

#define INFO_MAIN_POS_X		320
#define INFO_MAIN_POS_Y		150
#define INFO_DIAG_POS_X		100
#define INFO_DIAG_POS_Y		200

#define INFO_ANIME_POWER_SELECT	0
#define INFO_ANIME_ROUND	1
#define INFO_ANIME_DOUBLE_ROUND	2
#define INFO_ANIME_FREEZE	3
#define INFO_ANIME_GAMEOVER	4
#define INFO_ANIME_POWER_TITLE	5
#define INFO_ANIME_BONUS_END	6
#define INFO_ANIME_MYSTERY	7
#define INFO_ANIME_5M		8
#define INFO_ANIME_10M		9
#define INFO_ANIME_15M		10
#define INFO_ANIME_20M		11
#define INFO_ANIME_30M		12
#define INFO_ANIME_40M		13
#define INFO_ANIME_50M		14
#define INFO_ANIME_60M		15
#define INFO_ANIME_70M		16
#define INFO_ANIME_80M		17
#define INFO_ANIME_90M		18
#define INFO_ANIME_100M		19
#define INFO_ANIME_JACKPOT	20
#define INFO_ANIME_SP_JACKPOT	21
#define INFO_ANIME_SPR_JACKPOT	22
#define INFO_ANIME_DARK_JACKPOT	23
#define INFO_ANIME_EXTRA_BALL	24
#define INFO_DIAG_DOUBLE_ARK	25
#define INFO_DIAG_ASYLUM_READY	26
#define INFO_DIAG_ASYLUM_DIRECT	27
#define INFO_DIAG_ROUND_OVER	28
#define INFO_DIAG_DEATH_READY	29
#define INFO_DIAG_DEATH_DIRECT	30
#define INFO_DIAG_CEME_READY	31
#define INFO_DIAG_CEME_DIRECT	32
#define INFO_DIAG_WITCH_READY	33
#define INFO_DIAG_SOUL_SCORE	34
#define INFO_DIAG_SOUL_READY	35
#define INFO_DIAG_SOUL_XBALL	36
#define INFO_DIAG_CREA_READY	37
#define INFO_DIAG_CREA_RESTART	38
#define INFO_DIAG_MULTI_DIRECT	39
#define INFO_DIAG_MULTI2_DIRECT	40
#define INFO_DIAG_MB_READY	41
#define INFO_DIAG_MB_RESTART	42
#define INFO_DIAG_MB_DIRECT	43
#define INFO_DIAG_MB_HOLE	44
#define INFO_DIAG_NRAMP_SCORE	45
#define INFO_DIAG_NRAMP_MYSTERY	46
#define INFO_DIAG_NRAMP_XBALL	47
#define INFO_DIAG_MAZE_COMP	48
#define INFO_DIAG_DOUBLE_CLT	49
#define INFO_DIAG_EXTRA_READY	50
#define INFO_DIAG_SPELL_0	51
#define INFO_DIAG_SPELL_1	52
#define INFO_DIAG_SPELL_2	53
#define INFO_DIAG_SPELL_3	54
#define INFO_DIAG_SPELL_4	55
#define INFO_DIAG_SPELL_5	56
#define INFO_DIAG_SPELL_6	57
#define INFO_DIAG_SPELL_7	58
#define INFO_DIAG_SPELL_8	59
#define INFO_DIAG_SPELL_9	60
#define INFO_DIAG_SPELL_10	61
#define INFO_DIAG_SPELL_11	62
#define INFO_DIAG_SPELL_12	63
#define INFO_DIAG_SPELL_13	64
#define INFO_ANIME_POWER_SEL2	65
#define INFO_ANIME_AGAIN	66
#define INFO_DIAG_DOUBLE_DRM	67
#define INFO_DIAG_WOOD_READY	68
#define INFO_DIAG_WOOD_DIRECT	69
#define INFO_DIAG_MOON_READY	70
#define INFO_DIAG_MOON_DIRECT	71
#define INFO_DIAG_SILVER_READY	72
#define INFO_DIAG_SILVER_DIRECT	73
#define INFO_DIAG_DREAMER_READY	74
#define INFO_DIAG_NECRO_READY	75
#define INFO_DIAG_MYSTERY_READY	76
#define INFO_DIAG_ANCIENT_READY	77
#define INFO_DIAG_LOCK_READY	78
#define INFO_DIAG_LOCK_COUNT	79
#define INFO_DIAG_LOCK_COUNTS	80
#define INFO_DIAG_MYSTERY_AWARD	81
#define INFO_DIAG_NIGHTMARE	82
#define INFO_DIAG_NIGHTMARE_N	83
#define INFO_DIAG_NIGHTMARE_I	84
#define INFO_DIAG_NIGHTMARE_G	85
#define INFO_DIAG_NIGHTMARE_H	86
#define INFO_DIAG_NIGHTMARE_T	87
#define INFO_DIAG_NIGHTMARE_M	88
#define INFO_DIAG_NIGHTMARE_A	89
#define INFO_DIAG_NIGHTMARE_R	90
#define INFO_DIAG_NIGHTMARE_E	91
#define INFO_DIAG_MOON_MULTI	92
#define INFO_DIAG_RAMP_AWARD	93
#define INFO_DIAG_MBT_HOLE	94
#define INFO_ANIME_CLEAR	95
#define INFO_DIAG_TIME_OUT	96
#define INFO_DIAG_RISE_JACKPOT	97
#define INFO_DIAG_RAMP_XBALL	98
#define INFO_ANIME_WISDOM_W	99
#define INFO_ANIME_WISDOM_I	100
#define INFO_ANIME_WISDOM_S	101
#define INFO_ANIME_WISDOM_D	102
#define INFO_ANIME_WISDOM_O	103
#define INFO_ANIME_WISDOM_M	104
#define INFO_DIAG_SIL_T_DIRECT	105
#define INFO_DIAG_LOOP_BONUS	106
#define INFO_DIAG_ECLIPS_READY	107
#define INFO_DIAG_ECLIPS_DIRECT	108
#define INFO_DIAG_STONE_DIRECT	109
#define INFO_DIAG_ANGELS_READY	110
#define INFO_DIAG_MACNE_READY	111
#define INFO_DIAG_GOD_READY	112
#define INFO_DIAG_ANGELS_DIRECT	113
#define INFO_DIAG_ANGELS_AWARD	114
#define INFO_DIAG_SOLAR_AWARD	115
#define INFO_DIAG_LUNAR_AWARD	116
#define INFO_ANIME_RO2		117
#define INFO_ANIME_DOUBLE_RO2	118
#define INFO_DIAG_MACHINE_JAC	119
#define INFO_DIAG_MACHINE	120
#define INFO_DIAG_MACHINE1	121
#define INFO_DIAG_MACHINE2	122
#define INFO_DIAG_MACHINE3	123
#define INFO_DIAG_MACHINE4	124
#define INFO_DIAG_MACHINE5	125
#define INFO_DIAG_MACHINE6	126
#define INFO_DIAG_MACHINE7	127
#define INFO_DIAG_MACHINE8	128
#define INFO_DIAG_MACHINE9	129
#define INFO_DIAG_MACHINE10	130
#define INFO_DIAG_MACHINE11	131
#define INFO_DIAG_MACHINE12	132
#define INFO_DIAG_MACHINE13	133
#define INFO_DIAG_MACHINE14	134
#define INFO_DIAG_MACHINE15	135
#define INFO_DIAG_MULTI_LOOP	136
#define INFO_DIAG_MULTI_LOOPS	137
#define INFO_DIAG_ENERGY_DIRECT	138
#define INFO_DIAG_ENERGY_FULL	139
#define INFO_DIAG_DIST_DIRECT	140
#define INFO_DIAG_DISTS_DIRECT	141
#define INFO_DIAG_MOON_RESTART	142
#define INFO_ANIME_COMPLETED	143
#define INFO_DIAG_NEXT_STAGE	144
#define INFO_ANIME_COMPLETED2	145
#define INFO_ANIME_SOON		146
#define INFO_ANIME_PLAYER	147
#define	INFO_ANIME_NAME_ENTRY	148
#define INFO_ANIME_NAME_ENTRY_F	149
#define INFO_ANIME_REPLAY	150
#define INFO_DIAG_DANGER	151
#define INFO_ANIME_TILT		152

#define INFO_NO_POWER_BASE	0
#define INFO_NO_POWER_PARTS0	1
#define INFO_NO_POWER_PARTS1	2
#define INFO_NO_POWER_PARTS2	3
#define INFO_NO_POWER_PARTS3	4
#define INFO_NO_POWER_PARTS4	5
#define INFO_NO_POWER_PARTS5	6
#define INFO_NO_POWER_PARTS6	7
#define INFO_NO_DOUBLE		8
#define INFO_NO_ROUND_START	9
#define INFO_NO_ROUND_PARTS0	10
#define INFO_NO_ROUND_PARTS1	11
#define INFO_NO_ROUND_PARTS2	12
#define INFO_NO_ROUND_PARTS3	13
#define INFO_NO_ROUND_PARTS4	14
#define INFO_NO_ROUND_PARTS5	15
#define INFO_NO_ROUND_PARTS6	16

#define INFO_NO_ROUND_1		18
#define INFO_NO_ROUND_2		19
#define INFO_NO_ROUND_3		20
#define INFO_NO_ROUND_4		21
#define INFO_NO_ROUND_5		22
#define INFO_NO_ROUND_6		23
#define INFO_NO_POWER_TITLE	24
#define INFO_NO_BONUS		25
#define INFO_NO_MYSTERY		26
#define INFO_NO_SHOOT_AGAIN	27
#define INFO_NO_FREEZE		28
#define INFO_NO_GAMEOVER	29
#define INFO_NO_PLAYER		30
#define INFO_NO_ENTRY		31
#define INFO_NO_SIDE_BASE	32
#define INFO_NO_SIDE_SPELL	33
#define INFO_NO_SIDE_PARTS	34
#define INFO_NO_ARROW		35
#define INFO_NO_SPELL0		36
#define INFO_NO_SPELL1		37
#define INFO_NO_SPELL2		38
#define INFO_NO_SPELL3		39
#define INFO_NO_SPELL4		40
#define INFO_NO_SPELL5		41
#define INFO_NO_SPELL6		42
#define INFO_NO_SPELL7		43
#define INFO_NO_SPELL8		44
#define INFO_NO_SPELL9		45
#define INFO_NO_SPELL10		46
#define INFO_NO_SPELL11		47
#define INFO_NO_SPELL12		48
#define INFO_NO_SPELL13		49
#define INFO_NO_SPELL14		50

#define INFO_FRAME_END		-1
#define INFO_ANIME_END		-2
#define INFO_TEXT_POWER1	-3
#define INFO_TEXT_POWER2	-4
#define INFO_SET_ADDRESS	-5
#define INFO_JUMP_ADDRESS	-6
#define INFO_SET_STYLE		-7
#define INFO_SET_FRAME_RATE	-8
#define INFO_SET_VALUE		-9
#define INFO_SET_SPEED		-10
#define INFO_SET_ANIME		-11
#define INFO_PLAY_ANIME		-12
#define INFO_TEXT_POWER		-13
#define INFO_STRINGS		-14
#define INFO_4COLORS		-15
#define INFO_DIAG_TEXT		-16
#define INFO_MAIN_TEXT		-17
#define INFO_OFFSET		-18
#define INFO_DIAG_MODE		-19
#define INFO_DIAG_NUMBER	-20
#define INFO_CHANGE_TEXTS	-21
#define INFO_4COLORS2		-22
#define INFO_SET_ARROW		-23
#define INFO_PLAYER_NUMBER	-24
#define INFO_NAME_ENTRY		-25
#define INFO_TILT_MESSAGE	-26

#define INFO_STYLE_NORMAL	0
#define INFO_STYLE_SLASH_IN	1
#define INFO_STYLE_SLASH_OUT	3
#define INFO_STYLE_ZOOM_WIDTH	5
#define INFO_STYLE_ZOOM_HEIGHT	7
#define INFO_STYLE_ZOOM		9

void MobjDecimal(int x, int y, int n);
void MobjInfo(int x, int y, int no, int sx, int sy);
void MobjArrow(int);
void MobjInfoSlash(int x, int y, int no);
int MobjInfoText(int x, int y, int s, int c);
void MobjInfoTexts(int x, int y, int s, const char *);
int InfoTextCode(int c);
void MobjInfoScore(int x, int y, int s, const char *t);
void MobjInfo2(int x, int y, int sx, int sy);
void CreateInfoScore(const char *ptr);
char *CreateInfoScores(char *scr, const char *ptr);
void InitInfomations(int);
void ServiceInfomations(void);
void EffectInfomations(int no);
void SetInfomations(int no, int pri);
void CreateSlashFlag(int);
void InitDiffAnime(int x, int y, const uint8_t *ptr);

void SetMultiInfo(int, int);
void SetDiffAnime(int no);

extern ANIME fontAnime, infoAnime, ifonAnime, info2Anime;
extern char infoPause;
extern char *infoText[8], infoNumberTab[4], nameTextPos, nameText[4];
extern volatile short slaveTaskNo;
extern short infoNo, infoValue, infoPriority, infoAnimeNo[8];
extern short infoNumber, infoAnimeMax, infoTextNo, infoTimer;
