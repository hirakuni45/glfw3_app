#include "tracks.h"

#define CDA_NO_1	2	/* ゲーム選択 */
#define CDA_NO_2	3	/* ARKHAM マルチボール・レディ */
#define CDA_NO_3	4	/* ARKHAM マルチボール・ゲーム */
#define CDA_NO_4	5	/* BLOODY マルチボール・レディ */
#define CDA_NO_5	6	/* BLOODY マルチボール・ゲーム */
#define CDA_NO_6	7	/* DREAM LANDS マルチボール・レディ */
#define CDA_NO_7	8	/* DREAM LANDS マルチボール・ゲーム */

const char convertTrackTables3[] = {
	-1, -1, CDA_NO_1,
	CDA_NO_2, CDA_NO_2, CDA_NO_2, CDA_NO_3, CDA_NO_3,
	CDA_NO_2, CDA_NO_2, CDA_NO_2, CDA_NO_2, CDA_NO_2,
	CDA_NO_2, CDA_NO_2, CDA_NO_2, CDA_NO_2, CDA_NO_2,

	CDA_NO_4, CDA_NO_4, CDA_NO_4, CDA_NO_5, CDA_NO_5,
	CDA_NO_4, CDA_NO_4, CDA_NO_4, CDA_NO_4, CDA_NO_4,
	CDA_NO_4, CDA_NO_4, CDA_NO_4, CDA_NO_4, CDA_NO_4,

	CDA_NO_2, CDA_NO_3, CDA_NO_4, CDA_NO_5, CDA_NO_2,
	CDA_NO_3, CDA_NO_4, CDA_NO_5, CDA_NO_2, CDA_NO_3,
	CDA_NO_4, CDA_NO_5, CDA_NO_2, CDA_NO_3, CDA_NO_4
};

void LaunchGameWare(void)
{
    // call_loader((char *)"GAMEWARE.PRG", (void *)0x26004000, (char *)"10" );
}
