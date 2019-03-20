#pragma once
//=====================================================================//
/*! @file
    @brief  Ansur ツール・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "ftpsub.h"

/***** シンボル宣言 *************************************************/
#define	CONF_DBHOST		"DBHOST"
#define	CONF_DBUSER		"DBUSER"
#define	CONF_DBPASS		"DBPASS"
#define	CONF_DBNAME		"DBNAME"
#define	CONF_DBPORT		"DBPORT"
#define	CONF_FTPDIR		"FTPDIR"
#define	CONF_FTPNLISTTIMEOUT	"FTPNLISTTIMEOUT"
#define	CONF_FTPGETTIMEOUT	"FTPGETTIMEOUT" /*ver1.05*/
#define	CONF_FTPDELTIMEOUT	"FTPDELTIMEOUT" /*ver1.05*/
#define	FTPNLISTFILE	"ftpnlist.dat"
#define	FTPGETFILE		"ftpget.dat"
#define	FTPDELFILE		"ftpdel.dat"
#define	FTPNLISTRES		"ftpnlist.result"
#define	DELIMITER		"/\n"

namespace app {

	/* 昇順csvファイルデータ */
	typedef struct {
		char	csvname[128];	/* csvファイル名	*/
		int		csvdate;		/* csvファイル日時	*/
	} SORT;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ツール・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tools {

		static int ftp_listmake( struct ftpsub_buf *fbf, int sockfd, SORT *p_sort, char *localdir, char *ftpnlstres, int fd, int numfile, int maxgetcnt)
		{
#if 0
			char				csvdate[10+1];
			char				*p_dest;
			int					retval;
			int					getnum;
			int					i;

			/* ファイルリストアップ結果ファイル読み込み */
			if( file_read_p( numfile, ftpnlstres, fd, p_sort ) == ERROR ){
				fdclose( fd, __LINE__ );
				return( ERROR );
			}

			/* 対象ファイルリスト作成 */
			/* csvファイル名からファイルの日付のみを抽出 */
			for( i = 0; i < numfile; i++ ) {
				p_dest = strrchr( p_sort[i].csvname, '.' );
				sprintf( csvdate, "%.10s", p_dest - 10 );
				p_sort[i].csvdate = atoi( csvdate );
			}

			/* ファイルの日付を昇順に入れ替える */
			qsort( p_sort, numfile, sizeof(SORT), (VOID*)date_comp );

			/* 取得ファイル数指定	*/
			/* ・最大取得ファイル数以上は取得しない	*/
			/* ・更新日時が最新のファイルは取得対象外	*/
			if( numfile > maxgetcnt ) {
				getnum = maxgetcnt;
			}
			else {
				getnum = numfile - 1;
			}

			/* 取得ファイルがあるか確認 */
			if( getnum < 1 ){
				tLog_Write( "%s取得ファイルなし", pcstr );
				return( NORMAL );
			}

			/* FTP GET DELETE 呼び出し */
			retval = ftp_get_del( fbf, sockfd, p_sort, localdir, getnum );

			return( retval );
#endif
return 0;
		}



		static int ftp_nlst( struct ftpsub_buf *fbf, int sockfd, char *localdir, char *remotedir, char *id, int maxgetcnt)
		{
			char				ftpnlstres[512];
			int					retval;
			int					numfile;
			SORT				*p_sort;

			/* ローカルディレクトリ作成 */
///			if( makedir ( localdir ) == ERROR ) {
///				tLog_Write( "%sMKDIRエラー( %s )", pcstr, fbf->retmsg );
///				return( ERROR );
///			}

			/* CHDIR */
			sprintf( fbf->sndmsg, "%s %s", C_C_CWD, remotedir );
			if( CmdRUN( fbf, sockfd ) == C_RET_ERR ) {
//				tLog_Write( "%sCHDIRエラー( %s )", pcstr, fbf->retmsg );
				return( ERROR );
			}
			/* 受信メッセージチェック */
			if(OkResultCheck(fbf, 1, C_R_250) == C_RET_ERR) {
//				tLog_Write( "%sCHDIRエラー( %s )", pcstr, fbf->retmsg );
				return( ERROR );
			}

			/* NLIST出力ファイル名 */
			sprintf( ftpnlstres, "%s.%s", FTPNLISTRES, id );

			/* NLIST出力ファイル削除 */
			if( ( unlink( ftpnlstres ) ) == -1 ) {
				if( errno != ENOENT ) {								/* ファイルなし以外 */
//					tLog_Write( "%sファイル削除エラー( %d )[ %s ] err = %s( %d )"
//						, pcstr, __LINE__, id, strerror( errno ), errno );
					return( ERROR );
				}
			}

// for compile error
#if 0
			/* NLST実行 */
			ftpsub_timeout_set( fbf, g_ftpnlisttimeout );
			if( CmdLIST( fbf, sockfd, "log*.csv", ftpnlstres, C_C_NLST, 0 ) == C_RET_ERR ) {
//				tLog_Write( "%sNLSTエラー( %s )", pcstr, fbf->retmsg );
				return( ERROR );
			}
#endif

			/* 待ち時間をリセット */
			ftpsub_timeout_reset( fbf );

// for compile error
#if 0
			/* NLST数チェック */
			numfile = fbf->fline;
			if( numfile <= 0 ) {
//				tLog_Write( "%s%s NLIST ファイルなし", pcstr, id );
				return( NORMAL );
			}

			/* p_sortメモリ獲得 */
			if( ( p_sort = (SORT *)calloc( numfile, sizeof(SORT) ) ) == NULL ) {
//				tLog_Write( "%s動的メモリの割り当てエラー( %d )[ %s ] err = %s( %d )"
//					, pcstr, __LINE__, id, strerror( errno ), errno );
				return( ERROR );
			}

			retval = ftp_listmake( fbf, sockfd, p_sort, localdir, ftpnlstres, -1, numfile, maxgetcnt );
			free( p_sort );
#endif
///			return( retval );
			return 0;
		}
	};
}
