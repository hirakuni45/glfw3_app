/**************************************************************************/
/***	FILE NAME	: CLD.c (Call_LoaDer)				***/
/***	FUNCTION 	: ロ−ダの起動を行う．				***/
/***	DATE	 	: 1996.4.9(tue)					***/
/***	AUTHOR	 	: M.Matsuyama (GE)				***/
/**************************************************************************/

/**************************************************************************/
/***				INCLUDE宣言				***/
/**************************************************************************/
#include <string.h>
#include <stdint.h>
// #include <sega_int.h>
// #include <sega_gfs.h>
// #include <sega_tim.h>

/**************************************************************************/
/***				DEFINE宣言				***/
/**************************************************************************/
#if 0
#define	LOW_ADDR		0x20200200	/* WORK RAM LOWアドレス  */
#define	HIGH_ADR		0x26004000	/* WORK RAM HIGHアドレス */
#define LOAD_ADR		0x00200200	/* LOADER先頭アドレス	 */
#define	WAIT_TIM		300		/* SMPCコマンド発行禁止期間 (300[micro sec]) */
#define MAX_DIR			256		/* ル-トディレクトリにある ファイル最大数 */
#define OPEN_MAX		2		/* 同時に開くファイルの最大数  */
#define	VB_MASK			(0x0001) 	/* ＳＣＵ割込みマスク		*/
#endif

/**************************************************************************/
/***				マクロ定義				***/
/**************************************************************************/
/// #define ei() set_imask(0x00)		/*** インタラプトを許可するマクロ ***/
/// #define di() set_imask(0x0f)		/*** インタラプトを禁止するマクロ ***/

/**************************************************************************/
/***			スレ−ブＳＨ関連デ−タ				***/
/**************************************************************************/
/// volatile uint8_t *SMPC_SF  = (uint8_t *)0x20100063;	/* SMPC Status Flag	  */
/// volatile uint8_t *SMPC_COM = (uint8_t *)0x2010001F;	/* SMPC COMmand Registor  */

/// const uint8_t SMPC_SSHON  = 0x02;			/* スレ−ブSH ON command  */
/// const uint8_t SMPC_SSHOFF = 0x03;			/* スレ−ブSH OFF command */


/**************************************************************************/
/***			ＧＦＳ関連デ−タ				***/
/**************************************************************************/
// GfsDirTbl  dir_tbl;			/* ディレクトリ情報管理領域 */
// GfsDirName dir_name_game_ware[MAX_DIR];	/* ファイル名を含んだ情報   */
// Uint8	   gfs_work[GFS_WORK_SIZE(OPEN_MAX)];


/**************************************************************************/
/***			V Blank In関連デ−タ				***/
/**************************************************************************/
static volatile uint16_t vbIcnt = 0;

static void vbIrtn(void)
{
	vbIcnt++;
}

static void syncVbI(void)
{
	uint16_t cur_cnt_value;

	cur_cnt_value = vbIcnt;
	while (cur_cnt_value == vbIcnt);
}


/**************************************************************************/
/***	NAME	 : call_loader (Call_LoaDer)				***/
/***	FUNCTION : スレ−ブＳＨ停止,割り込みクリア後,ロ−ダをＬメモリ	***/
/***		   (0x06004000)に読み込み，ロ−ダの起動を行う．		***/
/***	INPUT	 : *arg_file_name	: ファイル名称アドレス		***/
/***		   *load_addr		: ファイル読み込みアドレス	***/
/***		   *path_name		: 				***/
/***	OUTPUT	 : なし							***/
/***	DATE	 : 1996.4.10(wed)					***/
/***	AUTHOR	 : M.Matsuyama (GE)					***/
/**************************************************************************/
void call_loader(const char *arg_file_name, void *load_addr, const char *path_name)
{
#if 0
	int32_t	   fid, sct_size, sct_no, sct_last_size, f_size;
//	GfsHn	   gfs;
	uint16_t   count;
	float	   micro_cnt;
	volatile   Uint32   *LowHeadAd = (Uint32 *)0x20200000;

	/* WORK RAM-L先頭(0x20200000)からH'200バイトを0クリア */
	memset((void *)LowHeadAd, (int)0, (size_t)512);

	/* パス名をWORK RAM-L先頭にコピ−する */
	if(path_name != NULL){
		strcpy((char *)LowHeadAd,(char *)path_name);
	}

	/**************************/
	/***  スレ−ブＳＨ停止	***/
	/**************************/
	/* V-Blank In 待ち合わせ */
	INT_SetScuFunc(INT_SCU_VBLK_IN, vbIrtn); /* V Blank In  (0x40)	  */
	INT_ChgMsk(VB_MASK, INT_MSK_NULL);	 /* V Blank In割込み許可  */

	syncVbI();			/* スレ−ブＳＨ停止のため同期	  */

	/* コマンド発行禁止期間待ち (300[micro sec]) */
	TIM_FRT_INIT(8);
	TIM_FRT_SET_16(0);
	do{
		count	  = TIM_FRT_GET_16();
		micro_cnt = TIM_FRT_CNT_TO_MCR(count);
	}while(micro_cnt < WAIT_TIM);

	/* スレ−ブＳＨ停止処理 */
	while((*SMPC_SF & 0x01) == 0x01); /* SMPCステ−タスチェック	*/

	*SMPC_SF  = 1;			  /* ステ−タスビジ−		*/
	*SMPC_COM = SMPC_SSHOFF;	  /* スレ−ブＳＨ　ＯＦＦ	*/

	while((*SMPC_SF & 0x01) == 0x01); /* 処理完了待ち		*/

	/************************/
	/***  割り込みクリア  ***/
	/************************/
	di();					/* CPU  インタラプト禁止 */
	INT_SetMsk (-1);			/* 全割り込みマスク */
	
	/* SCU割り込みダミ−化 */
	INT_SetScuFunc(INT_SCU_SPR, NULL);	/* SPR (0x4d)	*/
	INT_SetScuFunc(INT_SCU_DMAI, NULL);	/* DMAI(0x4c)	*/
	INT_SetScuFunc(INT_SCU_DMA0, NULL);	/* DMA0(0x4b)	*/
	INT_SetScuFunc(INT_SCU_DMA1, NULL);	/* DMA1(0x4a)	*/
	INT_SetScuFunc(INT_SCU_DMA2, NULL);	/* DMA2(0x49)	*/
	INT_SetScuFunc(INT_SCU_PAD, NULL);	/* PAD (0x48)	*/
	INT_SetScuFunc(INT_SCU_SYS, NULL);	/* システムマネージャ (0x47) */
	INT_SetScuFunc(INT_SCU_SND, NULL);	/* サウンドリクエスト (0x46) */
	INT_SetScuFunc(INT_SCU_DSP, NULL);	/* DSP終了 (0x45)	*/
	INT_SetScuFunc(INT_SCU_TIM1, NULL);	/* Timer1  (0x44)	*/
	INT_SetScuFunc(INT_SCU_TIM0, NULL);	/* Timer0  (0x43)	*/
	INT_SetScuFunc(INT_SCU_HBLK_IN, NULL);	/* H Blank In  (0x42)	*/
	INT_SetScuFunc(INT_SCU_VBLK_OUT, NULL);	/* V Blank Out (0x41)	*/
	INT_SetScuFunc(INT_SCU_VBLK_IN, NULL);	/* V Blank In  (0x40)	*/

	/* CPU割り込みダミ−化 */
/*	INT_SetScuFunc(INT_CPU_DIVU, NULL);*/	/* 除算器 	    (0x6e) */
	INT_SetScuFunc(INT_CPU_DMAC0, NULL);	/* DMACチャネル0    (0x6d) */
	INT_SetScuFunc(INT_CPU_DMAC1, NULL);	/* DMACチャネル1    (0x6c) */
	INT_SetScuFunc(INT_CPU_WDT, NULL);	/* WDTインタ−バル  (0x68) */
	INT_SetScuFunc(INT_CPU_BSC, NULL);	/* BSCコンペアマッチ(0x69) */
	INT_SetScuFunc(INT_CPU_SCI_ERI, NULL);	/* SCI受信エラ−    (0x60) */
	INT_SetScuFunc(INT_CPU_SCI_RXI, NULL);	/* SCI受信デ−タフル(0x61) */
	INT_SetScuFunc(INT_CPU_SCI_TXI, NULL);	/* SCI受信デ−タエンプティ (0x62) */
	INT_SetScuFunc(INT_CPU_SCI_TEI, NULL);	/* SCI送信終了      (0x63) */
	INT_SetScuFunc(INT_CPU_FRT_ICI, NULL);	/* FRTインプットキャプチャ (0x64) */
	INT_SetScuFunc(INT_CPU_FRT_OCI, NULL);	/* FRTアウトプットコンペア (0x65) */
	INT_SetScuFunc(INT_CPU_FRT_OVI, NULL);	/* FRTオ−バ−フロ−(0x66) */

#if 0
	INT_SetFunc (0x94, NULL);		 /* SlaveSH リセット解除後の飛び先 */
	INT_SetFunc (0x141, NULL);		 /* SlaveSH V-Blank IN */
#endif

	/* SCUインタラプト部分許可	*/
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_ALL);
	ei();					/* CPU インタラプト許可*/

	/*********************************/
	/* Ｌメモリへロ−ダを読み込む	 */
	/*********************************/
	/***  ファイル初期化  ***/
	GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME; /* GFSの初期化 */
	GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;
	GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name_game_ware;
	GFS_Init(OPEN_MAX, gfs_work, &dir_tbl);
	GFS_LoadDir(1, &dir_tbl);
	GFS_SetDir(&dir_tbl);

	/* ファイル名からファイル識別子を求める */
	fid = GFS_NameToId( "NLD.PRG" );

	/******************************/
	/***   LOADER ロ-ディング   ***/
	/******************************/
	gfs = GFS_Open( fid );
	GFS_GetFileSize( gfs, &sct_size, &sct_no, &sct_last_size);
	f_size = sct_size * (sct_no -1) + sct_last_size;
	GFS_NwFread( gfs, sct_no, (void *)LOW_ADDR, f_size);
	/* 一応、ロ-ディング中に(簡単な)何らかの処理を入れられる事にしておく */
	while( (GFS_NwExecOne( gfs )) != GFS_SVR_COMPLETED ){
		;
	}
	GFS_Close( gfs );

	/**************************/
	/***  LOADER起動	***/
	/**************************/
	((void(*)())LOAD_ADR)( (char *)arg_file_name, (void *)load_addr );
#endif
}
