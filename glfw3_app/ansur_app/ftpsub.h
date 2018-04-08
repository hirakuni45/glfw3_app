/************************************************************************
*																		*
*	FTP共通関数															*
*																		*
*		ftpsub.h														*
*																		*
*	2017.12.05	Ver. 1.00	A.Hayashi	初版作成						*
*																		*
************************************************************************/

#ifndef _FTPSUB_HL
#define _FTPSUB_HL

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdarg.h>
#include <iconv.h>
#include <stdint.h>


typedef int32_t socklen_t;


#define C_RET_ERR		-1
#define C_RET_OK		0
#define C_RET_TOV		1
#define C_END_ERR		1
#define C_END_OK		0

#define C_WK_NET		128
#define C_WK_NAME		256
#define C_WK_LEN1		1024

#define C_SELECT_SEC	30			/* select wait sec */
#define C_SELECT_USEC	0			/* select wait μsec */

#define C_CRLF			"\r\n"		/* CRLF */
#define C_CR			"\r"		/* CR */
#define C_LF			"\n"		/* LF */

#define C_CONN_A		'A'			/* Active Mode */
#define C_CONN_P		'P'			/* Passive Mode */

#define C_TYPE_A		'A'			/* ASCII */
#define C_TYPE_I		'I'			/* IMAGE(BINARY) */

#define C_TRNT_S		'S'			/* 送信 */
#define C_TRNT_R		'R'			/* 受信 */

#define C_DISP_Y		'Y'			/* 受信メッセージを表示する */
#define C_DISP_N		'N'			/* 受信メッセージを表示しない */

#define C_OPEN_R		'R'			/* READ */
#define C_OPEN_W		'W'			/* WRITE */


#define C_C_USER		"USER"		/* 【USER ユーザー名】　指定したユーザー名でログインする */
#define C_C_PASS		"PASS"		/* 【PASS パスワード】　ユーザーのパスワードを指定する */
#define C_C_ACCT		"ACCT"		/* 【ACCT 課金情報】　課金情報を指定する。課金情報は特定のシステムで */
									/*  ユーザーの利用状況に合わせた課金のための情報収集を行うことを想定している。 */
									/*  実装していない場合も多い */
#define C_C_CWD			"CWD"		/* 【CWD ディレクトリ名】　指定したディレクトリをカレントディレクトリとする。 */
									/*  絶対パスか相対パスが使用できる */
#define C_C_CDUP		"CDUP"		/* 【CDUP】　1つ上位のディレクトリ(親ディレクトリ)をカレントディレクトリとする */
#define C_C_QUIT		"QUIT"		/* 【QUIT】　ログアウトする */
#define C_C_PORT		"PORT"		/* 【PORT IPアドレスとポート番号】　データコネクションで使用するIPアドレス */
									/*  （通常はクライアント）とポート番号を指示する */
#define C_C_PASV		"PASV"		/* 【PASV】　パッシブモードへの移行を指示する。サーバはデータコネクションで */
									/*  使用するIPアドレスとポート番号を表示する */
#define C_C_TYPE		"TYPE"		/* 【TYPE 形式オプション】　転送データの形式を指定する。 */
									/*  (A：ASCII,I：Image（バイナリ）,E：EBCDIC　など) */
#define C_C_STRU		"STRU"		/* 【STRU ファイル構造オプション】　転送するファイルのファイル構造をオプションで示す。 */
									/*  これはOSによりファイルシステム上のファイル構造には差異があるためで、 */
									/*  この差異を吸収する役目を持っている。(F：ファイル構造（バイト志向の通常のバイナリ）, */
									/*  R：レコード構造（シーケンシャルなどのレコードイメージを持ったデータ）, */
									/*  P：ページ構造（インデックス化されたページ志向のファイル）)。 */
									/*  デフォルトはファイルである。ただし、実質上は汎用機などのためのオプションで、 */
									/*  現在ではほとんど使用されることはない。 */
#define C_C_MODE		"MODE"		/* 【MODE 転送モードオプション】　転送モードを指定する。 */
									/*  (S：ストリーム（バイトストリームによる転送）,B：ブロック（転送をブロックごとに行う）, */
									/*  C：圧縮（Run-Length手法による圧縮）)。デフォルトはストリーム。 */
									/*  ストリーム以外はほぼ使用されていない。 */
#define C_C_RETR		"RETR"		/* 【RETR ファイル名《データコネクション有》】　指定したファイルの内容をサーバから */
									/*  取得する */
#define C_C_STOR		"STOR"		/* 【STOR ファイル名《データコネクション有》】　指定したファイル名で、サーバへ送信する */
									/*  データでファイルを作成する。同一名のファイルがすでにある場合には、上書きする */
#define C_C_APPE		"APPE"		/* 【APPE ファイル名《データコネクション有》】　サーバへ送信するデータを指定した */
									/*  ファイルへ追加する。ファイルが存在していない場合は、新規作成する */
#define C_C_RNFR		"RNFR"		/* 【RNFR ファイル名】　指定したファイル名を変更する。変更元ファイル名の指定である。 */
									/*  RNTOを続けて実行しなくてはならない */
#define C_C_RNTO		"RNTO"		/* 【RNTO ファイル名】　RNFRの後に実行される。RNFRコマンドで指定したファイルを、 */
									/*  指定したファイル名に変更する。変更先ファイル名の指定である */
#define C_C_DELE		"DELE"		/* 【DELE ファイル名】　指定したファイルを削除する */
#define C_C_RMD			"RMD"		/* 【RMD ディレクトリ名】　指定したディレクトリを削除する。ディレクトリ名には、 */
									/*  絶対パスか相対パスが使用できる */
#define C_C_MKD			"MKD"		/* 【MKD ディレクトリ名】　指定したディレクトリを作成する。ディレクトリ名には、 */
									/*  絶対パスか相対パスが使用できる */
#define C_C_PWD			"PWD"		/* 【PWD】　現在のワーキングディレクトリを表示する */
#define C_C_NLST		"NLST"		/* 【NLST [ファイル／ディレクトリ名] [オプション]《データコネクション有》】　 */
									/*  現在のワーキングディレクトリ内のファイル一覧を表示する。ファイル名や */
									/*  ディレクトリ名が指定された場合は、そのファイルやディレクトリの内容を示す。 */
									/*  オプションにより、詳細表示が行える */
#define C_C_LIST		"LIST"		/* 【LIST [ファイル／ディレクトリ名]《データコネクション有》】　現在のワーキング */
									/*  ディレクトリ内のファイル一覧を表示する。ファイル名やディレクトリ名が指定された */
									/*  場合はそのファイルやディレクトリの内容を示す。NLSTとの違いは、LISTでは属性を */
									/*  含めた詳細情報を表示する点である。ただし、OSに依存した結果かも */
									/*  知れないことに注意が必要 */
#define C_C_SYST		"SYST"		/* 【SYST】　システム名を表示する */
#define C_C_STAT		"STAT"		/* 【STAT [ファイル／ディレクトリ名]】　現在のシステムや転送状態の情報を表示する。 */
									/*  ファイル／ディレクトリ名が与えられた場合は、その情報を表示する（NLSTなどとほぼ等価） */
#define C_C_HELP		"HELP"		/* 【HELP】　コマンド一覧を表示する */
#define C_C_NOOP		"NOOP"		/* 【NOOP】　何もしない。サーバの稼動を確認するために実行される。常に正常終了する */


#define C_R_NUL			""			/*  SEEDA対応(2017/12/22) */
#define C_R_110			"110"		/* 【Restart marker reply.】　RESTコマンドのためのマーカー返答である */
#define C_R_120			"120"		/* 【Service ready in nnn minutes.】　サービスは停止しているが、nnn分後に準備できる */
#define C_R_125			"125"		/* 【Data connection already open; transfer starting.】　データコネクションはすでに */
									/*  確立されている。このコネクションで転送を開始する */
#define C_R_150			"150"		/* 【File status okay; about to open data connection.】　ファイルステータスは */
									/*  正常である。データコネクションを確立する */
#define C_R_200			"200"		/* 【Command okay.】　コマンドは正常に受け入れられた */
#define C_R_202			"202"		/* 【Command not implemented, superfluous at this site.】　コマンドは実装されていない。 */
									/*  SITEコマンドでOSコマンドが適切でない場合など */
#define C_R_211			"211"		/* 【System status, or system help reply.】　STATコマンドに対するレスポンス */
#define C_R_212			"212"		/* 【Directory status.】　STATコマンドによるディレクトリ情報を示す */
#define C_R_213			"213"		/* 【File status.】　STATコマンドによるファイル情報を示す */
#define C_R_214			"214"		/* 【Help message.】　HELPコマンドに対するレスポンス */
#define C_R_215			"215"		/* 【NAME system type.】　SYSTコマンドに対するレスポンス */
#define C_R_220			"220"		/* 【Service ready for new user.】　新規ユーザー向けに準備が整った。ログイン時に */
									/*  表示される場合を想定している */
#define C_R_221			"221"		/* 【Service closing control connection.】　コントロールコネクションを切断する。 */
									/*  QUITコマンド時のレスポンス */
#define C_R_225			"225"		/* 【Data connection open; no transfer in progress.】　データコネクションを確立した。 */
									/*  データの転送は行われていない */
#define C_R_226			"226"		/* 【Closing data connection.】　要求されたリクエストは成功した。データコネクションを */
									/*  クローズする */
#define C_R_227			"227"		/* 【Entering Passive Mode (h1,h2,h3,h4,p1,p2).】　PASVコマンドへのレスポンス。 */
									/*  h1～h4はIPアドレス、p1～p2はポート番号を示す */
#define C_R_230			"230"		/* 【User logged in, proceed.】　ユーザーログインの成功 */
#define C_R_250			"250"		/* 【Requested file action okay, completed.】　要求されたコマンドによる操作は正常終了した */
#define C_R_257			"257"		/* 【"PATHNAME" created.】　ファイルやディレクトリを作成したというのがRFCでの意味だが、 */
									/*  MKDコマンドの結果以外にも、実際にはPWDコマンドの結果にも用いられる */
#define C_R_331			"331"		/* 【User name okay, need password.】　パスワードの入力を求める */
#define C_R_332			"332"		/* 【Need account for login.】　ACCTコマンドで課金情報を指定する必要がある */
#define C_R_350			"350"		/* 【Requested file action pending further information】　他の何らかの情報を求めている */
#define C_R_421			"421"		/* 【Service not available, closing control connection. / Timeout.等】 */
									/*  サービスを提供できない。コントロールコネクションを終了する。 */
									/*  サーバのシャットダウン時など / タイムアウト等 */
#define C_R_425			"425"		/* 【Can't open data connection.】　データコネクションをオープンできない */
#define C_R_426			"426"		/* 【Connection closed; transfer aborted.】　何らかの原因により、コネクションを */
									/*  クローズし、データ転送も中止した */
#define C_R_450			"450"		/* 【Requested file action not taken.】　要求されたリクエストはアクセス権限や */
									/*  ファイルシステムの理由で実行できない */
#define C_R_451			"451"		/* 【Requested action aborted. Local error in processing.】　ローカルエラーのため */
									/*  処理を中止した */
#define C_R_452			"452"		/* 【Requested action not taken.】　ディスク容量の問題で実行できない */
#define C_R_500			"500"		/* 【Syntax error, command unrecognized.】　コマンドの文法エラー */
#define C_R_501			"501"		/* 【Syntax error in parameters or arguments.】　引数やパラメータの文法エラー */
#define C_R_502			"502"		/* 【Command not implemented.】　コマンドは未実装である */
#define C_R_503			"503"		/* 【Bad sequence of commands.】　コマンドを用いる順番が間違っている */
#define C_R_504			"504"		/* 【Command not implemented for that parameter.】　引数やパラメータが未実装 */
#define C_R_530			"530"		/* 【Not logged in.】　ユーザーはログインできなかった */
#define C_R_532			"532"		/* 【Need account for storing files.】　ファイル送信には、ACCTコマンドで課金情報を */
									/*  確認しなくてはならない */
#define C_R_550			"550"		/* 【Requested action not taken.】　要求されたリクエストはアクセス権限やファイル */
									/*  システムの理由で実行できない */
#define C_R_551			"551"		/* 【Requested action aborted. Page type unknown.】　ページ構造のタイプの問題で */
									/*  実行できない */
#define C_R_552			"552"		/* 【Requested file action aborted.】　ディスク容量の問題で実行できない */
#define C_R_553			"553"		/* 【Requested action not taken.】　ファイル名が間違っているため実行できない */

struct ftpsub_buf {
	char		sndmsg[C_WK_LEN1+1];	/* 送信メッセージ用バッファ */
	size_t		sndsiz;					/* 送信メッセージ用バッファサイズ */
	char		rcvmsg[C_WK_LEN1+1];	/* 受信メッセージ用バッファ */
	size_t		rcvsiz;					/* 受信メッセージ用バッファサイズ */
	char		retmsg[C_WK_LEN1+1];	/* エラーメッセージ用バッファ */
	size_t		retsiz;					/* エラーメッセージ用バッファサイズ */
	long		isec_it;				/* 時間監視（秒）         デフォルト */
	long		usec_it;				/* 時間監視（マイクロ秒） デフォルト  */
	long		isec;					/* 時間監視（秒） */
	long		usec;					/* 時間監視（マイクロ秒） */
	char		flgType;				/* TYPE 形式 A：ASCII,I：Image（バイナリ） */
	char		flgConn;				/* データコネクション A:Active,P:Passive */
	char		flgDisp;				/* 受信メッセージ表示 Y:表示する,N:表示しない */
	size_t		fsizer;					/* 読取りファイルサイズ CmdRUNdatatransfer使用時にセット */
	size_t		fsizew;					/* 書込みファイルサイズ CmdRUNdatatransfer使用時にセット */
	size_t		fline;					/* 行数 CmdRUNdatatransfer使用時にセット */
	iconv_t		sndicvcd;				/* 変換ディスクリプター */
	iconv_t		rcvicvcd;				/* 変換ディスクリプター */
	char		clntcode[C_WK_NAME+1];	/* クライアント側コード */
	char		servcode[C_WK_NAME+1];	/* サーバ側コード */
	char		icvmsg[C_WK_LEN1*2+1];	/* 変換用バッファ */
	size_t		icvsiz;					/* 変換用バッファサイズ */
};
/* ???sizはftpsub_buf_init()関数にて、C_WK_LEN1をセットする */

#ifdef __cplusplus
extern "C" {
#endif
/* プロトタイプ(ftpsub.c) */
int CmdLOGIN(struct ftpsub_buf *fbf, const char *rhost, int iport, const char *suser, const char *spasswd);
int CmdOPEN(struct ftpsub_buf *fbf, const char *rhost, int iport);
int CmdQUIT(struct ftpsub_buf *fbf, int sockfd);
int CmdCLOSE(struct ftpsub_buf *fbf, int *sockfd);
int CmdLIST(struct ftpsub_buf *fbf, int sockfd, const char *ffile, const char *tfile, const char *lcmd, int flgIconv);
int CmdGET(struct ftpsub_buf *fbf, int sockfd, char *ffile, char *tfile, int flgIconv);
int CmdPUT(struct ftpsub_buf *fbf, int sockfd, char *ffile, char *tfile, int flgIconv);
int CmdRUN(struct ftpsub_buf *fbf, int sockfd);
int CmdRUNdatatransfer(struct ftpsub_buf *fbf, int sockfd, int fd, char flgTransferType, int flgIconv);
int CmdPASV(struct ftpsub_buf *fbf, int sockfd);
int CmdPORT(struct ftpsub_buf *fbf, int sockfd);
int SndPASV(struct ftpsub_buf *fbf, int sockfd, char *sip, int *iport);
int SndPORT(struct ftpsub_buf *fbf, int sockfd, struct sockaddr_in *port_addr);
int GetLocalIP(struct ftpsub_buf *fbf, int sockfd, char *local_ip, size_t ipsiz, char *local_port, size_t portsiz);
int OpenConnect(struct ftpsub_buf *fbf, const char *hostname, int iport);
int SendUserPasswd(struct ftpsub_buf *fbf, int sockfd, const char *suser, const char *spasswd);
int SendMessage_(struct ftpsub_buf *fbf, int sockfd);
int RecvMessage_(struct ftpsub_buf *fbf, int sockfd);
int OpenFd(struct ftpsub_buf *fbf, int *fd, const char *fnm, const char rw);
int CloseFd(struct ftpsub_buf *fbf, int fd);
char *MkRetMsg(struct ftpsub_buf *fbf, char mtype, char *mfile, int mline, int merrno, const char *mfmt, ...);
int OkResultCheck(struct ftpsub_buf *fbf, int chkcnt, ...);
int TimeMon(struct ftpsub_buf *fbf, int sockfd);
void SecDiv(double tsec, long *t_sec, long *t_sec2, long rate);
void ftpsub_buf_init(struct ftpsub_buf *fbf);
void ftpsub_timeout_set(struct ftpsub_buf *fbf, double tsec);
void ftpsub_timeout_init_set(struct ftpsub_buf *fbf, double tsec);
void ftpsub_timeout_reset(struct ftpsub_buf *fbf);
int ftpiconvopen(struct ftpsub_buf *fbf);
int ftpiconvclose(struct ftpsub_buf *fbf);
int _iconvopen(struct ftpsub_buf *fbf, iconv_t *icvcd, char *fcode, char *tcode);
int _iconv(struct ftpsub_buf *fbf, iconv_t icvcd, char *fstr, char *rstr, size_t *rstrlen);
int _iconvclose(struct ftpsub_buf *fbf, iconv_t *icvcd);
int sleepnano(struct ftpsub_buf *fbf, double t_sec);

#ifdef __cplusplus
};
#endif

#endif /* _FTPSUB_HL */
