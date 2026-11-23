#pragma once
#include <stdint.h>

#if 0
#define SATURN_SCU_SMPC_VEC	0x48

#define SATURN_SH2_BARA		0xFFFFFF40
#define SATURN_SH2_BAMRA	0xFFFFFF44
#define SATURN_SH2_BBRA		0xFFFFFF48
#define SATURN_SH2_BARB		0xFFFFFF60
#define SATURN_SH2_BAMRB	0xFFFFFF64
#define SATURN_SH2_BDRB		0xFFFFFF70
#define SATURN_SH2_BDMRB	0xFFFFFF74
#define SATURN_SH2_BBRB		0xFFFFFF68
#define SATURN_SH2_BRCR		0xFFFFFF78

#define SATURN_SMPC_CMD		0x2010001F
#define SATURN_SMPC_SR		0x20100061
#define SATURN_SMPC_SF		0x20100063 
#define SATURN_SMPC_IREG	0x20100001
#define SATURN_SMPC_OREG	0x20100021

#define SATURN_PORT_PDR1	0x20100075
#define SATURN_PORT_DDR1	0x20100079
#define SATURN_PORT_PDR2	0x20100077
#define SATURN_PORT_DDR2	0x2010007B
#define SATURN_PORT_PSEL	0x2010007D
#define SATURN_PORT_EXLE	0x2010007F

#define SATURN_DRAM_TOP		0x00200000
#define SATURN_CRAM_TOP		0x06000000

#define PLASMA_DATA_TOP		(SATURN_CRAM_TOP+0x78000)

#define	VDP1_VRAM_TOP		0x25C00000
#define	VDP2_VRAM_TOP		0x25E00000
#endif

/// #define SS_PEID_6B		0x02
/// #define SS_PEID_MULCON		0x16
/// #define SS_PEID_MOUSE		0xE3
/*	#define SS_PEID_PTR		0x20
	#define SS_PEID_KBD		0x30
*/
#define SS_PEIDPL_1		0x0100
#define SS_PEIDPL_2		0x0200
#define SS_PEIDPL_AP		0x0300
#define SS_PEIDPL_T1		0x0400
#define SS_PEIDPL_T2		0x0800
#define SS_PEIDPL_T3		0x1000
#define SS_PEIDPL_T4		0x2000
#define SS_PEIDPL_T5		0x4000
#define SS_PEIDPL_T6		0x8000
#define SS_PEIDPL_AT		0xFC00

#define SATURN_PAD_LEFT		0x0008
#define SATURN_PAD_Z		0x0010
#define SATURN_PAD_Y		0x0020
#define SATURN_PAD_X		0x0040
#define SATURN_PAD_RIGHT	0x0080
#define SATURN_PAD_B		0x0100
#define SATURN_PAD_C		0x0200
#define SATURN_PAD_A		0x0400
#define SATURN_PAD_START	0x0800
#define SATURN_DIR_UP		0x1000
#define SATURN_DIR_DOWN		0x2000
#define SATURN_DIR_LEFT		0x4000
#define SATURN_DIR_RIGHT	0x8000
#define SATURN_DIR_ALL		0xf000
#define SATURN_PAD_ABC		0x0700
#define SATURN_PAD_XYZ		0x0070
#define SATURN_PAD_ABCXYZ	0x0770

#define SATURN_MS_A		0x01
#define SATURN_MS_B		0x04
#define SATURN_MS_C		0x02
#define SATURN_MS_START 	0x08

#define MON_GAGE		0x0001

#define RESOURCE_FLAT_ID	0	/* Image of FLAT style. */
#define RESOURCE_CLUT_ID	1	/* Color Luck Up Tables */
#define RESOURCE_SOUND_ID	2	/* Sound data. */

#define SMPC_SLAVE_ON		0x02
#define SMPC_SLAVE_OFF		0x03
#define SMPC_RESET_ENABLE	0x19
#define SMPC_RESET_DISABLE	0x1a

#define SCREEN_NTSC_FRAME	60
#define SCREEN_PAL_FRAME	50

typedef struct {
	unsigned short year;
	unsigned char month;
	unsigned char week;
	unsigned char day;
	unsigned char time;
	unsigned char min;
	unsigned char sec;
} DateTime;

void LaunchDebugger(const char *msg);

void ExitVenus(void);

void VenusShell(void);

void PlaySound(void);
void CpkPlay(void);
void CpkMount(void);

void LaunchCluster(void);
void ClusterMain(void);
