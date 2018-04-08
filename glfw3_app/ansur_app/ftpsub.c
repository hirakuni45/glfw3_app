/************************************************************************
*																		*
*	FTP共通関数															*
*																		*
*		ftpsub.c														*
*																		*
*	2017.12.05	Ver. 1.00	A.Hayashi	初版作成						*
*	2018.03.01	Ver. 1.01	A.Hayashi	SEEDA LF対応					*
*																		*
************************************************************************/

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "ftpsub.h"
#include "ftpsub2.h"

void fsync_(int fd)
{
}


/* サーバログイン */
int CmdLOGIN(struct ftpsub_buf *fbf, const char *rhost, int iport, const char *suser, const char *spasswd)
{
	int		sockfd;			/* ソケットのファイル・ディスクリプター */
	/* サーバ接続 */
	if((sockfd = CmdOPEN(fbf, rhost, iport)) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* ユーザ／パスワード送信 */
	if(SendUserPasswd(fbf, sockfd, suser, spasswd) == C_RET_ERR) {
		CmdQUIT(fbf, sockfd);								/* Ver. 1.01 */
		CmdCLOSE(fbf, &sockfd);								/* Ver. 1.01 */
		return(C_RET_ERR);
	}
	/* 転送データの形式 */
	snprintf(fbf->sndmsg, fbf->sndsiz, "%s %c", C_C_TYPE, fbf->flgType);
	if(CmdRUN(fbf, sockfd) == C_RET_ERR) {
		CmdQUIT(fbf, sockfd);								/* Ver. 1.01 */
		CmdCLOSE(fbf, &sockfd);								/* Ver. 1.01 */
		return(C_RET_ERR);
	}
        /* 受信メッセージチェック */
	if(OkResultCheck(fbf, 1, C_R_200) == C_RET_ERR) {
		CmdQUIT(fbf, sockfd);								/* Ver. 1.01 */
		CmdCLOSE(fbf, &sockfd);								/* Ver. 1.01 */
		return(C_RET_ERR);
	}
	return(sockfd);
}

/* サーバ接続 */
int CmdOPEN(struct ftpsub_buf *fbf, const char *rhost, int iport)
{
	int		sockfd;			/* ソケットのファイル・ディスクリプター */

	/* 接続 */
	if((sockfd = OpenConnect(fbf, rhost, iport)) == C_RET_ERR) {
		return(C_RET_ERR);
	};
	/* メッセージ受信 */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		CmdQUIT(fbf, sockfd);								/* Ver. 1.01 */
		CmdCLOSE(fbf, &sockfd);								/* Ver. 1.01 */
		return(C_RET_ERR);
	}
	/* メッセージチェック */
	if(OkResultCheck(fbf, 1, C_R_220) == C_RET_ERR) {
		CmdQUIT(fbf, sockfd);								/* Ver. 1.01 */
		CmdCLOSE(fbf, &sockfd);								/* Ver. 1.01 */
		return(C_RET_ERR);
	}
	return(sockfd);
}

/* QUIT */
int CmdQUIT(struct ftpsub_buf *fbf, int sockfd)
{
	/* コマンド作成 */
	strcpy(fbf->sndmsg, C_C_QUIT);

	/* コマンド実行 */
	if(CmdRUN(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	};

	/* メッセージチェック */
	if(OkResultCheck(fbf, 2, C_R_221, C_R_NUL) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	return(sockfd);
}

/* ソケットCLOSE */
int CmdCLOSE(struct ftpsub_buf *fbf, int *sockfd)
{
	if(*sockfd > 2) {
		if(close(*sockfd) == C_RET_ERR) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "close(%d)", *sockfd);
			return(C_RET_ERR);
		};
		*sockfd = 0;
	};
	return(C_RET_OK);
}

/* ファイルリストの出力 */
int CmdLIST(struct ftpsub_buf *fbf, int sockfd, const char *ffile, const char *tfile, const char *lcmd, int flgIconv)
{
	int		fd;
	int		flgfopen;
	int		retval;


	/* 受信ファイルオープン */
	flgfopen = 0;
	if(strcmp(tfile, "") == 0) {
		fd = STDOUT_FILENO;
	}
	else {
		if(OpenFd(fbf, &fd, tfile, C_OPEN_W) == C_RET_ERR) {
			return(C_RET_ERR);
		}
		flgfopen = 1;
	}

	/* コマンド作成 */
	if(strcmp(ffile, "") == 0) {
		strcpy(fbf->sndmsg, lcmd);
	}
	else {
		snprintf(fbf->sndmsg, fbf->sndsiz, "%s %s", lcmd, ffile);
	}

	/* データ転送 */
	retval = CmdRUNdatatransfer(fbf, sockfd, fd, C_TRNT_R, flgIconv);

	/* 受信ファイルクローズ */
	if(flgfopen == 1) {
		if(CloseFd(fbf, fd) == C_RET_ERR) return(C_RET_ERR);
	}

	return(retval);
}

/* ファイルの受信 */
int CmdGET(struct ftpsub_buf *fbf, int sockfd, char *ffile, char *tfile, int flgIconv)
{
	int		fd;
	int		retval;


	/* 受信ファイルオープン */
	if(OpenFd(fbf, &fd, tfile, C_OPEN_W) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	/* コマンド作成 */
	snprintf(fbf->sndmsg, fbf->sndsiz, "%s %s", C_C_RETR, ffile);

	/* データ転送 */
	if(fbf->flgType == C_TYPE_I) flgIconv = 0;
	retval = CmdRUNdatatransfer(fbf, sockfd, fd, C_TRNT_R, flgIconv);

	/* 受信ファイルクローズ */
	if(CloseFd(fbf, fd) == C_RET_ERR) return(C_RET_ERR);

	return(retval);
}

/* ファイルの送信 */
int CmdPUT(struct ftpsub_buf *fbf, int sockfd, char *ffile, char *tfile, int flgIconv)
{
	int		fd;
	int		retval;


	/* 送信ファイルオープン */
	if(OpenFd(fbf, &fd, ffile, C_OPEN_R) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	/* コマンド作成 */
	snprintf(fbf->sndmsg, fbf->sndsiz, "%s %s", C_C_STOR, tfile);

	/* データ転送 */
	if(fbf->flgType == C_TYPE_I) flgIconv = 0;
	retval = CmdRUNdatatransfer(fbf, sockfd, fd, C_TRNT_S, flgIconv);

	/* 送信ファイルクローズ */
	if(CloseFd(fbf, fd) == C_RET_ERR) return(C_RET_ERR);

	return(retval);
}

/* コマンド実行(汎用データコネクションなし) */
int CmdRUN(struct ftpsub_buf *fbf, int sockfd)
{
	/* コマンド送信 */
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* コマンド受信 */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	return(C_RET_OK);
}

/* データ転送 */
int CmdRUNdatatransfer(struct ftpsub_buf *fbf, int sockfd, int fd, char flgTransferType, int flgIconv)
{
	int		trans_sockfd;
	int		ffd;
	int		tfd;
	int		retval;
	int		retcd;
	iconv_t	cd;
	size_t	readsize;
	size_t	writesize;
	size_t	pnt;
	size_t	buffsize;
	size_t	retsize;
	char	readbuf[C_WK_LEN1+1];
	char	retbuf[C_WK_LEN1+1];


	retval = C_RET_OK;
	/* データコネクション確立後、コマンド実行 */
	if(fbf->flgConn == C_CONN_P) {
		/* Passive Mode */
		if((trans_sockfd = CmdPASV(fbf, sockfd)) == C_RET_ERR) {
			return(C_RET_ERR);
		}
	}
	else {
		/* Active Mode */
		if((trans_sockfd = CmdPORT(fbf, sockfd)) == C_RET_ERR) {
			return(C_RET_ERR);
		}
	}

	memset(readbuf, '\0', C_WK_LEN1+1);
	fbf->fsizer = 0;
	fbf->fsizew = 0;
	fbf->fline  = 0;

	strcpy(retbuf, "");
	buffsize = C_WK_LEN1;
	retsize = 0;
	if(flgTransferType == C_TRNT_R) {
		ffd = trans_sockfd;
		tfd = fd;
		cd  = fbf->rcvicvcd;
	}
	else {
		ffd = fd;
		tfd = trans_sockfd;
		cd  = fbf->sndicvcd;
	}
	/* データ転送 */
	while ((readsize = read(ffd, readbuf, buffsize - retsize)) > 0) {
		readbuf[readsize] = '\0';
		for(pnt=0;pnt<readsize;pnt++) {
			if(readbuf[pnt] == '\n') (fbf->fline)++;
		}
		fbf->fsizer += readsize;
		if((flgIconv == 1) && (cd != (iconv_t)(-1))) {
			if(retsize > 0) {
				strncat(retbuf, readbuf, readsize);
				retbuf[retsize + readsize] = '\0';
				strcpy(readbuf, retbuf);
			}
			while((retcd  = _iconv(fbf, cd, readbuf, retbuf, &retsize)) >= 0) {
				readsize  = strlen(fbf->icvmsg);
				writesize = write(tfd, fbf->icvmsg, readsize);
				if (writesize == -1) {
					MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "write(%d)", tfd);
					retval = C_RET_ERR;
					break;
				}
				fbf->fsizew += writesize;
				if(retcd < 2) break;
				strcpy(readbuf, retbuf);
			}
		}
		else {
			writesize = write(tfd, readbuf, readsize);
			if (writesize == -1) {
				MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "write(%d)", tfd);
				retval = C_RET_ERR;
				break;
			}
			fbf->fsizew += writesize;
		}
	}
	if (readsize == -1) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "read(%d)", ffd);
		retval = C_RET_ERR;
	}
	if(flgTransferType == C_TRNT_R) {
		fsync_(fd);
	}
	close(trans_sockfd);

	/* サーバーから、データ転送が終了した事を知らせるメッセージ */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 1, C_R_226) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	if(retval == C_RET_ERR) return(C_RET_ERR);

	return(C_RET_OK);
}

/* データコネクション確立(Passive)後、コマンド実行 */
int CmdPASV(struct ftpsub_buf *fbf, int sockfd)
{
	int		trans_sockfd;
	int		iport;
	char	sip[C_WK_NET+1];


	/* ポート番号の割り出し */
	if(SndPASV(fbf, sockfd, sip, &iport) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* コマンド送信 */
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* 接続 */
	if((trans_sockfd = OpenConnect(fbf, sip, iport)) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* コマンドへの返事 */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 2, C_R_150, C_R_125) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	return(trans_sockfd);
}

/* データコネクション確立(Active)後、コマンド実行 */
int CmdPORT(struct ftpsub_buf *fbf, int sockfd)
{
	int					data_waiting_socket;
	int					trans_sockfd;
	struct sockaddr_in	port_addr;
	socklen_t			addr_len;


	/* ポートコマンド送信 */
	if((data_waiting_socket=SndPORT(fbf, sockfd, &port_addr)) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* コマンド送信 */
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* コマンドへの返事 */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 2, C_R_150, C_R_125) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	/* 時間監視 */
	if(TimeMon(fbf, data_waiting_socket) != C_RET_OK) {
		return(C_RET_ERR);
	}
	/* データ待ち */
	addr_len = sizeof(struct  sockaddr_in);
	if((trans_sockfd = accept(data_waiting_socket, (struct sockaddr *)&port_addr, &addr_len)) == C_RET_ERR) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "accept(%d)", data_waiting_socket);
		return(C_RET_ERR);
	}
	return(trans_sockfd);
}

/* Send PASV */
int SndPASV(struct ftpsub_buf *fbf, int sockfd, char *sip, int *iport)
{
	int		i;
	char	*pt;
	char	buff[C_WK_LEN1+1];
	char	svsndmsg[C_WK_LEN1+1];
	char	dm[2];
	char	*saveptr;


	/* sndmsgをバックアップする */
	/* PASVコマンド送信でバッファが変更される為、現内容をバックアップする */
	strcpy(svsndmsg, fbf->sndmsg);

	/* PASV コマンドを送る */
	strcpy(fbf->sndmsg, C_C_PASV);
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		/* sndmsgのバックアップを戻す */
		strcpy(fbf->sndmsg, svsndmsg);
		return(C_RET_ERR);
	}

	/* sndmsgのバックアップを戻す */
	strcpy(fbf->sndmsg, svsndmsg);

	/* PASVコマンドの返答を得る */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 1, C_R_227) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	strcpy(buff, &fbf->rcvmsg[3]);
	strcpy(sip, "");
	strcpy(dm, "(");
	*iport = 0;
	i=0;
	if((pt = strtok_r(buff, dm, &saveptr)) == (char *)NULL) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "strtok_r(%s,%s) 0", fbf->rcvmsg, dm);
		return(C_RET_ERR);
	}
	for(i=1; i<7; i++) {
		if(i == 1) strcpy(dm, ",");
		if(i == 6) strcpy(dm, ")");
		if((pt = strtok_r(NULL, dm, &saveptr)) == (char *)NULL) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "strtok_r(%s,%s) %d", pt, dm, i);
			return(C_RET_ERR);
		}
		if((i >= 1) && (i <=4 )) strcat(sip, pt);
		if((i >= 1) && (i <=3 )) strcat(sip, ".");
		if(i == 5)  *iport  = atoi(pt)*256;
		if(i == 6)  *iport += atoi(pt);
	}
#ifdef DEBUG2
	DbgLog("SndPASV ip:%s port:%d\n", sip, *iport);
#endif
	return(C_RET_OK);
}

/* Send PORT */
int SndPORT(struct ftpsub_buf *fbf, int sockfd, struct sockaddr_in *port_addr)
{
	char	local_ip[C_WK_NET+1];
	char	local_port[C_WK_NET+1];
	char	dmy_ip[C_WK_NET+1];
	char	dmy_port[C_WK_NET+1];
	char	svsndmsg[C_WK_LEN1+1];
	int		port_sockfd;


	/* PORTコマンド送信でバッファが変更される為、現内容をバックアップする */
	strcpy(svsndmsg, fbf->sndmsg);

        /* ソケットの作成 */
	if((port_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == C_RET_ERR) {
		/* ソケット作成失敗 */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "socket(%d, %d, %d)", AF_INET, SOCK_STREAM, 0);
		return(C_RET_ERR);
	};

        /* ソケットに名前をつける */
	memset(port_addr, '\0', sizeof(struct sockaddr_in));
	port_addr->sin_family = AF_INET;
	port_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	port_addr->sin_port = 0;
	if(bind(port_sockfd, (struct sockaddr *)port_addr, sizeof(struct sockaddr)) == C_RET_ERR) {
		/* 失敗 */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "bind(%d)", port_sockfd);
		return(C_RET_ERR);
	};

	if(listen(port_sockfd, SOMAXCONN) == C_RET_ERR) {
		/* 失敗 */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "listen(%d)", port_sockfd);
		return(C_RET_ERR);
	};

	if(GetLocalIP(fbf, sockfd, local_ip, C_WK_LEN1, dmy_port, C_WK_LEN1) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	if(GetLocalIP(fbf, port_sockfd, dmy_ip, C_WK_LEN1, local_port, C_WK_LEN1) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	memset(fbf->sndmsg, '\0', C_WK_LEN1);
	snprintf(fbf->sndmsg, fbf->sndsiz, "%s %s,%s",
				C_C_PORT,
				local_ip,
				local_port);
#ifdef DEBUG2
	DbgLog("PORT CMD :%s: length=%d\n", fbf->sndmsg, strlen(fbf->sndmsg));
#endif

	/* PORT コマンドを送る */
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		/* sndmsgのバックアップを戻す */
		strcpy(fbf->sndmsg, svsndmsg);
		return(C_RET_ERR);
	}

	/* sndmsgのバックアップを戻す */
	strcpy(fbf->sndmsg, svsndmsg);

	/* PORTコマンドの返答を得る */
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 1, C_R_200) == C_RET_ERR) {
		return(C_RET_ERR);
	}

	return(port_sockfd);
}

/* Local IP取得 */
int GetLocalIP(struct ftpsub_buf *fbf, int sockfd, char *local_ip, size_t ipsiz, char *local_port, size_t portsiz)
{
	socklen_t			addr_len;
	struct sockaddr_in	addr;


	/* PCのIP取得 */
	memset(&addr, '\0', sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr_len=sizeof(struct sockaddr_in);
	if(getsockname(sockfd, (struct sockaddr *)&addr, &addr_len) == C_RET_ERR) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "getsockname(%d)", sockfd);
		return(C_RET_ERR);
	}
	snprintf(local_ip, ipsiz, "%d,%d,%d,%d",
			(int)(ntohl(addr.sin_addr.s_addr) >> 24) & 0xff,
			(int)(ntohl(addr.sin_addr.s_addr) >> 16) & 0xff,
			(int)(ntohl(addr.sin_addr.s_addr) >>  8) & 0xff,
			(int)(ntohl(addr.sin_addr.s_addr))       & 0xff);
	snprintf(local_port, portsiz, "%d,%d",
			(int)(ntohs(addr.sin_port) >>  8) & 0xff,
			(int)(ntohs(addr.sin_port))       & 0xff);
#ifdef DEBUG2
	DbgLog("!GetLocalIP sockfd=%d CMD=%s,%s ip=%s port=%d\n",
				sockfd,
				local_ip,
				local_port,
				inet_ntoa(addr.sin_addr),
				ntohs(addr.sin_port));
#endif
	return(C_RET_OK);
}

/* ソケットの作成と接続 */
int OpenConnect(struct ftpsub_buf *fbf, const char *hostname, int iport)
{
	int					sockfd;
	struct sockaddr_in	addr;
#if 0
	struct addrinfo		hints;
	struct addrinfo		*res;
	struct servent		result_buf;
	struct servent		*result;
#endif
	char				buf[C_WK_LEN1+1];
#if 0
	uint32_t			s_addr;
	uint16_t			s_port;

	/* IP取得 */
	memset(&hints, '\0', sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "getaddrinfo(%s)", hostname);
		return(C_RET_ERR);
	}
	s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
	freeaddrinfo(res);

	/* ポート取得 */
	if(iport <= 0) {
		if(getservbyname_r("ftp", "tcp", &result_buf, buf, C_WK_LEN1, &result) != 0) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "getservbyname_r");
		}
		s_port = result->s_port;
	}
	else {
		s_port = htons(iport);
	}
#endif
	/* ソケットの作成 */
//	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == C_RET_ERR) {
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		/* ソケット作成失敗 */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "socket(%d, %d, %d)", AF_INET, SOCK_STREAM, 0);
		return(C_RET_ERR);
	};

	/* 接続 */
	memset(&addr, '\0', sizeof(struct sockaddr_in));
//	addr.sin_family = AF_INET;
	addr.sin_family = PF_INET;
//	addr.sin_addr.s_addr = s_addr;
	addr.sin_addr.s_addr = inet_addr(hostname);
//	addr.sin_port = s_port;
	addr.sin_port = htons(iport);
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == C_RET_ERR) {
		/* 接続失敗 */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno,
			"connect(%d, %x, %d)", sockfd, (struct sockaddr *)&addr, sizeof(addr));
		return(C_RET_ERR);
	}
#ifdef DEBUG2
	DbgLog("!OpenConnect sockfd=%d ip=%d.%d.%d.%d port=%d\n", sockfd,
                        (int)(ntohl(s_addr) >> 24) & 0xff,
                        (int)(ntohl(s_addr) >> 16) & 0xff,
                        (int)(ntohl(s_addr) >>  8) & 0xff,
                        (int)(ntohl(s_addr))       & 0xff,
			ntohs(s_port));
#endif

//    u_long val = 1;
//    ioctlsocket(sockfd, FIONBIO, &val);

	return(sockfd);
}

/* ユーザ／パスワード送信 */
int SendUserPasswd(struct ftpsub_buf *fbf, int sockfd, const char *suser, const char *spasswd)
{
	memset(fbf->sndmsg, '\0', fbf->sndsiz);
	snprintf(fbf->sndmsg, fbf->sndsiz, "USER %s", suser);
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR ) {
		return(C_RET_ERR);
	}
	/* 受信メッセージチェック */
	if(OkResultCheck(fbf, 2, C_R_331, C_R_230) == C_RET_ERR) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "Response Code Error(%s)", fbf->rcvmsg);
		return(C_RET_ERR);
	}

	/* 受信メッセージチェック */
	if(strncmp(fbf->rcvmsg, C_R_331, strlen(C_R_331)) == 0) {
		memset(fbf->sndmsg, '\0', fbf->sndsiz);
		snprintf(fbf->sndmsg, fbf->sndsiz, "PASS %s", spasswd);
		if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
			return(C_RET_ERR);
		}
		if(RecvMessage_(fbf, sockfd) == C_RET_ERR ) {
			return(C_RET_ERR);
		}
		/* 受信メッセージチェック */
		if(OkResultCheck(fbf, 1, C_R_230) == C_RET_ERR) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "Response Code Error(%s)", fbf->rcvmsg);
			return(C_RET_ERR);
		}
	}
	memset(fbf->sndmsg, '\0', C_WK_LEN1);
	snprintf(fbf->sndmsg, fbf->sndsiz, "SYST");
	if(SendMessage_(fbf, sockfd) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	if(RecvMessage_(fbf, sockfd) == C_RET_ERR ) {
		return(C_RET_ERR);
	}
	return(C_RET_OK);
}

/* メッセージ送信 */
int SendMessage_(struct ftpsub_buf *fbf, int sockfd)
{
	int		i;
	int		ilen;
	int		retcd;
	size_t	retflen;
	char	retf[C_WK_LEN1+1];
	char	buff[C_WK_LEN1+1];
	char	dmyb[C_WK_LEN1+1];


	strcpy(buff, fbf->sndmsg);
	for(i=strlen(buff)-1; i>=0; i--) {
		if((buff[i] != C_LF[0]) && (buff[i] != C_CR[0])) break;
		buff[i] = '\0';
	}
#ifdef DEBUG2
	DbgLog("!SEND sockfd:%d Send:%s:len=%d\n", sockfd, buff, strlen(buff));
#endif
	if(strlen(buff) <= 0) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "メッセージがありません。");
		return(C_RET_ERR);
	}
	strcat(buff, C_CRLF);			/* サーバでCRLFしか受け付けないケースがある為 */
#ifdef DEBUG
	DbgLog("---> %s", buff);
#endif
	if(fbf->sndicvcd != (iconv_t)(-1)) {
		retcd = _iconv(fbf, fbf->sndicvcd, buff, retf, &retflen);
		if(fbf->icvsiz > C_WK_LEN1) {
			strncpy(buff, fbf->icvmsg, C_WK_LEN1);
			buff[C_WK_LEN1] = '\0';
		}
		else {
			strcpy(buff, fbf->icvmsg);
		}
	}

#if 0
	/* 受信キューにゴミがあれば取出して空にする */
///	while((ilen = recv(sockfd, dmyb, C_WK_LEN1, MSG_DONTWAIT)) > 0) {
	while((ilen = recv(sockfd, dmyb, C_WK_LEN1, 0)) > 0) {
#ifdef DEBUG
		DbgLog("dmy recv (len = %d) msg=%s\n", ilen, dmyb);
#endif
		if(dmyb[ilen - 1] == *C_LF) break;
		sleepnano(fbf, 0.001);
	}
#endif

	if(send(sockfd, buff, strlen(buff), 0 ) == C_RET_ERR) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "send(%s)", fbf->sndmsg);
		return(C_RET_ERR);
	}

	return(C_RET_OK);
}

/* メッセージ受信 */
int RecvMessage_(struct ftpsub_buf *fbf, int sockfd)
{
	int		retcd;					/* 関数戻り値用ワーク */
	int     lpcnt;					/* 受信回数 */
	long	isec;					/* 保存用 */
	long	usec;					/* 保存用 */
	size_t	ilp;					/* ループ変数 */								/* Ver. 1.01 */
	size_t	lfcnt;					/* LF回数 */									/* Ver. 1.01 */
	size_t	msglen;					/* トータル受信サイズ */
	size_t	bufsiz;					/* *p使用可能サイズ */
	size_t	retflen;				/* iconv未変換データサイズ */
	ssize_t	recvlen; 				/* 受信長 */
	char	wrkmsg[C_WK_LEN1+1];	/* 受信バッファ */
	char	retf[C_WK_LEN1+1];		/* iconv未変換データバッファー */
	char	*p;						/* rcvmsg　pointer */


	memset(fbf->rcvmsg, '\0', sizeof(fbf->rcvmsg));
	p      = fbf->rcvmsg;
	bufsiz = fbf->rcvsiz;
	isec   = fbf->isec;
	usec   = fbf->usec;
	msglen = 0;
	lpcnt  = 0;
	do {
		/* 時間監視 */
		if((retcd = TimeMon(fbf, sockfd)) != C_RET_OK) {
			if((retcd == C_RET_ERR) || (lpcnt == 0)) {
				/* エラー、又は初回のタイムオーバー */
				return(C_RET_ERR);
			}
			else {
				/* タイムオーバーだけど、一回以上メッセージを受けている為、エラーとせずにループを抜ける */
				break;
			}
		}
		memset(wrkmsg, '\0', C_WK_LEN1);
		if((recvlen = recv(sockfd, wrkmsg, C_WK_LEN1, 0)) == C_RET_ERR) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "recv(%s)", wrkmsg);
			return(C_RET_ERR);
		}

		/* Ver 1.01 START */
		if(lpcnt == 0 && recvlen > 0) {
			/* 受信回数(lpcnt)が0の時、メッセージが全てLFかどうかをチェック */
			lfcnt = 0;
			for(ilp = 0;ilp < recvlen;ilp++) {						/* 1文字ずつチェック */
				if(wrkmsg[ilp] != *C_LF) {							/* LF以外の文字でループを抜ける */
					break;
				}
				lfcnt++;											/* LFの数をインクリメント */
			}
			if(lfcnt == recvlen) continue;							/* 全てLFの場合、while(受信回数(lpcnt)も0のままとする) */
		}
		/* Ver 1.01 START */
		lpcnt++;				/* 受信回数カウントアップ */
		msglen += recvlen;
		if(recvlen < bufsiz) {
			strcpy(p, wrkmsg);
			p      += recvlen;
			bufsiz -= recvlen;
		}
		else if(bufsiz > 0) {
			strncpy(p, wrkmsg, bufsiz);
			p      += bufsiz;
			bufsiz  = 0;
			*p      = '\0';
		}
#ifdef DEBUG
		if((wrkmsg[recvlen - 1] == *C_LF) || (recvlen == 0)) {
			DbgLog("!RECV sockfd:%d (len = %d) rcvmsg=%s\n", sockfd, recvlen, wrkmsg);
		}
		else {
			DbgLog("!RECV sockfd:%d (len = %d) LF NASHI rcvmsg=%s\n", sockfd, recvlen, wrkmsg);
		}
#endif
		ftpsub_timeout_set(fbf, 0.5);		/* ２回目以降の時間監視は０．５秒に変更 */
	}
//	while(wrkmsg[recvlen - 1] != *C_LF && msglen > 0);								/* Ver 1.01 */
	while((wrkmsg[recvlen - 1] != *C_LF && msglen > 0) || (lpcnt == 0));			/* Ver 1.01 */
	fbf->isec = isec;
	fbf->usec = usec;

	/* Ver 1.01 START */
	/* メッセージの先頭がLFとならないようにLFを除外 */
	lfcnt = 0;
	for(ilp = 0;ilp < msglen;ilp++) {						/* 1文字ずつチェック */
		if(fbf->rcvmsg[ilp] != *C_LF) {						/* LF以外の文字でループを抜ける */
			break;
		}
		lfcnt++;											/* LFの数をインクリメント */
	}
	if((lfcnt > 0) && (msglen > lfcnt)) {					/* 先頭がLFで文字列にLF以外の文字が存在する場合 */
#ifdef DEBUG
		DbgLog("!RECV           (len = %d) rcvmsg=%s\n", msglen, fbf->rcvmsg);
#endif
		msglen -= lfcnt;									/* 受信サイズ変更 */
		for(ilp = 0;ilp < msglen;ilp++) {					/* １文字ずつコピー(重複エリアのstrcpyは危険な為) */
			fbf->rcvmsg[ilp] = fbf->rcvmsg[ilp+lfcnt];
		}
		fbf->rcvmsg[msglen] = '\0';
#ifdef DEBUG
		DbgLog("!RECV LF DELETE (len = %d) rcvmsg=%s\n", msglen, fbf->rcvmsg);
#endif
	}
	/* Ver 1.01 END */

	if(fbf->rcvicvcd != (iconv_t)(-1)) {
		retcd = _iconv(fbf, fbf->rcvicvcd, fbf->rcvmsg, retf, &retflen);
		if(fbf->icvsiz > fbf->rcvsiz) {
			strncpy(fbf->rcvmsg, fbf->icvmsg, fbf->rcvsiz);
			fbf->rcvmsg[fbf->rcvsiz] = '\0';
		}
		else {
			strcpy(fbf->rcvmsg, fbf->icvmsg);
		}
	}
#ifdef DEBUG
	if(fbf->flgDisp == C_DISP_Y) {
		DbgLog("%s", fbf->rcvmsg);
	}
#endif
	return(recvlen);
}

/* ファイルオープン（ファイル名） */
int OpenFd(struct ftpsub_buf *fbf, int *fd, const char *fnm, const char rw)
{
	int		amode;
	int		mode;


	if(rw == C_OPEN_W) {
		amode = O_CREAT|O_WRONLY|O_TRUNC;
		mode  = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
		*fd = open(fnm, amode, mode);
	}
	else {
		amode = O_RDONLY;
		mode  = 0;
		*fd = open(fnm, amode);
	}
	if(*fd == -1) {
		if(rw == C_OPEN_W) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "open(%s ,%d, %d) [%c]", fnm, amode, mode, rw);
		}
		else {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "open(%s ,%d) [%c]", fnm, amode, rw);
		}
#ifdef DEBUG
		DbgLog("open(%s, %d, %d) NG\n", fnm, amode, mode);
#endif
		return(C_RET_ERR);
	}
#ifdef DEBUG
	DbgLog("open(%s, %d, %d) OK fd=%d\n", fnm, amode, mode, *fd);
#endif
	return(C_RET_OK);
}

/* ファイルクローズ */
int CloseFd(struct ftpsub_buf *fbf, int fd)
{
	char	buff[C_WK_LEN1];


	sprintf(buff, "%d", fd);
	if(close(fd) == -1) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "close(%s)", buff);
#ifdef DEBUG
		DbgLog("close(%s) NG\n", buff);
#endif
		return(C_RET_ERR);
	}
#ifdef DEBUG
	DbgLog("close(%s) OK\n", buff);
#endif
	return(C_RET_OK);
}

/* リターンメッセージ作成 */
char *MkRetMsg(struct ftpsub_buf *fbf, char mtype, char *mfile, int mline, int merrno, const char *mfmt, ...)
{
	va_list	margs;
	char 	spribuff[C_WK_LEN1+1];
	char 	sterbuff[C_WK_LEN1+1];


	memset(fbf->retmsg,  '\0', fbf->retsiz);
	memset(spribuff, '\0', C_WK_LEN1+1);
	memset(sterbuff, '\0', C_WK_LEN1+1);

	va_start(margs, mfmt);
	vsnprintf(spribuff, C_WK_LEN1, mfmt, margs);
	va_end(margs);

	if(merrno > 0) {
#if 0
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
		if(strerror_r(merrno, sterbuff, C_WK_LEN1) == 0) {
			snprintf(fbf->retmsg, fbf->retsiz, "%c %s[%d] %s %s(%d)\n",
				 	mtype, mfile, mline, spribuff, sterbuff, merrno);
		}
		else {
			snprintf(fbf->retmsg, fbf->retsiz, "%c %s[%d] %s (%d)[strerror_r error(%d)]\n",
				 	mtype, mfile, mline, spribuff, merrno, errno);
		}
#else
		snprintf(fbf->retmsg, fbf->retsiz, "%c %s[%d] %s %s(%d)\n", mtype, mfile, mline,
			 	spribuff, strerror_r(merrno, sterbuff, C_WK_LEN1), merrno);
#endif
#endif
	}
	else {
		snprintf(fbf->retmsg, fbf->retsiz, "%c %s[%d] %s\n", mtype, mfile, mline, spribuff);
	}
#ifdef DEBUG
	DbgLog("!MkRetMsg %s\n", fbf->retmsg);
#endif
	return(fbf->retmsg);
}

/* OK Result Check */
int OkResultCheck(struct ftpsub_buf *fbf, int chkcnt, ...)
{
	int		i;
	char	*cpt;
	va_list	margs;


#ifdef DEBUG2
	DbgLog("rcvmsg %s chkcnt %d\n", fbf->rcvmsg, chkcnt);
#endif
	/* 可変個引数初期化 */
	va_start(margs, chkcnt);
	for(i=0; i<chkcnt; i++) {
		cpt = va_arg(margs, char*);
#ifdef DEBUG2
	DbgLog("chkmsg %s\n", cpt);
#endif
		if(strncmp(fbf->rcvmsg, cpt, 3) == 0) {
			return(C_RET_OK);
		}
	}
	/* 可変個引数終了 */
	va_end(margs);

	MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "Response Code Error %s", fbf->rcvmsg);
	return(C_RET_ERR);
}

/* Time Monitoring */
int TimeMon(struct ftpsub_buf *fbf, int sockfd)
{
	fd_set			readfds;
	int				ret;
	struct timeval	tv;

	/* fd_setの初期化 */
	FD_ZERO(&readfds);

	/* selectで待つ読み込みソケットとしてsockfdを登録 */
	FD_SET(sockfd, &readfds);

	/* タイムアウトをセット */
	tv.tv_sec = fbf->isec;			/* 秒 */
	tv.tv_usec = fbf->usec;			/* マイクロ秒 */

	/* fdsに設定されたソケットが読み込み可能になるまで待ちます */
//	if((ret = select(sockfd+1, &readfds, NULL, NULL, &tv)) == C_RET_ERR) {
	if((ret = select(0, &readfds, NULL, NULL, &tv)) == C_RET_ERR) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "select(%d)", sockfd);
		return(C_RET_ERR);
	}
	if (ret == 0) {
		/* タイムオーバー */
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, 0, "Time Out(%d.%06d)", fbf->isec, fbf->usec);
		return(C_RET_TOV);
	}
	return(C_RET_OK);
}

/* 秒分割 */
void SecDiv(double tsec, long *t_sec, long *t_sec2, long rate)
{
	if( tsec < (double)0 ) {
		*t_sec  = 0;
		*t_sec2 = 0;
	}
	else {
		*t_sec  = (long)tsec;
		*t_sec2 = (tsec - (double)(*t_sec)) * rate;
		if(*t_sec2 > (rate - 1))
			*t_sec2 = (rate - 1);
		else if(*t_sec2 < 0)
			*t_sec2 = 0;
	}
	return;
}

/* ftpsub_buf 初期化 */
void ftpsub_buf_init(struct ftpsub_buf *fbf)
{
	memset(fbf, '\0', sizeof(struct ftpsub_buf));
	/* ???msgはC_WK_LEN1+1バイトで指定 */
	fbf->sndsiz  = sizeof(fbf->sndmsg) - 1;
	fbf->rcvsiz  = sizeof(fbf->rcvmsg) - 1;
	fbf->retsiz  = sizeof(fbf->retmsg) - 1;
	/* タイムアウトの初期値 */
	fbf->isec_it = C_SELECT_SEC;
	fbf->usec_it = C_SELECT_USEC;
	fbf->isec    = fbf->isec_it;
	fbf->usec    = fbf->usec_it;
	fbf->flgType = C_TYPE_I;	/* バイナリーモード */
	fbf->flgConn = C_CONN_P;	/* パッシブモード */
	fbf->flgDisp = C_DISP_N;	/* 受信メッセージを表示しない */
	fbf->fsizer  = 0;
	fbf->fsizew  = 0;
	fbf->fline   = 0;
	fbf->sndicvcd  = (iconv_t)(-1);
	fbf->rcvicvcd  = (iconv_t)(-1);
	fbf->icvsiz    = sizeof(fbf->icvmsg) - 1;
#ifdef DEBUG2
	DbgLog("ftpsub_buf sndsiz=%d,rcvsiz=%d,retsiz=%d,icvsiz=%d,isec=%d,usec=%d,flgType=%c,flgConn=%c,fsizer=%d,fsizew=%d,fline=%d,sizeof(struct ftpsub_buf)=%d\n",
			fbf->sndsiz,
			fbf->rcvsiz,
			fbf->retsiz,
			fbf->icvsiz,
			fbf->isec,
			fbf->usec,
			fbf->flgType,
			fbf->flgConn,
			fbf->fsizer,
			fbf->fsizew,
			fbf->fline,
			sizeof(struct ftpsub_buf));
#endif
	return;
}

/* タイムアウト時間セット */
void ftpsub_timeout_set(struct ftpsub_buf *fbf, double tsec)
{
	SecDiv(tsec, &(fbf->isec), &(fbf->usec), 1000000);
#ifdef DEBUG2
	DbgLog("ftpsub_timeout_set %d sec %06d μsec\n", fbf->isec, fbf->usec);
#endif
	return;
}

/* デフォルトタイムアウト時間セット */
void ftpsub_timeout_init_set(struct ftpsub_buf *fbf, double tsec)
{
	SecDiv(tsec, &(fbf->isec_it), &(fbf->usec_it), 1000000);
#ifdef DEBUG2
	DbgLog("ftpsub_timeout_init_set %d sec %06d μsec\n", fbf->isec_it, fbf->usec_it);
#endif
	return;
}

/* タイムアウト時間リセット */
void ftpsub_timeout_reset(struct ftpsub_buf *fbf)
{
	fbf->isec = fbf->isec_it;
	fbf->usec = fbf->usec_it;
#ifdef DEBUG2
	DbgLog("ftpsub_timeout_reset %d sec %06d μsec\n", fbf->isec, fbf->usec);
#endif
	return;
}

/* ftpiconv ディスクリプタ割当 */
int ftpiconvopen(struct ftpsub_buf *fbf)
{
	if(strcmp(fbf->clntcode, "") == 0) {
		MkRetMsg(fbf, 'I', __FILE__, __LINE__, 0, "ftpiconvopen(クライアント側コード指定なし");
		return(C_RET_OK);
	}
	if(strcmp(fbf->servcode, "") == 0) {
		MkRetMsg(fbf, 'I', __FILE__, __LINE__, 0, "ftpiconvopen(サーバ側コード指定なし");
		return(C_RET_OK);
	}
	if(strcasecmp(fbf->clntcode, fbf->servcode) == 0) {
		MkRetMsg(fbf, 'I', __FILE__, __LINE__, 0, "ftpiconvopen(クライアント側コードとサーバ側コードが同じ");
		return(C_RET_OK);
	}

	if(_iconvopen(fbf, &(fbf->rcvicvcd), fbf->servcode, fbf->clntcode) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	if(_iconvopen(fbf, &(fbf->sndicvcd), fbf->clntcode, fbf->servcode) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	return(C_RET_OK);
}

/* ftpiconv ディスクリプタ解除 */
int ftpiconvclose(struct ftpsub_buf *fbf)
{
	strcpy(fbf->clntcode, "");
	strcpy(fbf->servcode, "");
	if(_iconvclose(fbf, &(fbf->sndicvcd)) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	if(_iconvclose(fbf, &(fbf->rcvicvcd)) == C_RET_ERR) {
		return(C_RET_ERR);
	}
	return(C_RET_OK);
}

/* iconv ディスクリプタ割当 */
int _iconvopen(struct ftpsub_buf *fbf, iconv_t *icvcd, char *fcode, char *tcode)
{
	if(*icvcd != (iconv_t)(-1)) {
		_iconvclose(fbf, icvcd);
	}
	if(strcasecmp(fcode, tcode) == 0) {
		return(C_RET_OK);
	}
	if((*icvcd = iconv_open(tcode, fcode)) == (iconv_t)(-1)) {
		MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "iconv_open(to:%s, from:%s)", tcode, fcode);
		return(C_RET_ERR);
	}
	return(C_RET_OK);
}

/* iconv 変換メイン */
/* rstrは未変換の文字列を返す為、rstrのサイズはfstrのサイズ以上であること */
int _iconv(struct ftpsub_buf *fbf, iconv_t icvcd, char *fstr, char *rstr, size_t *rstrlen)
{
	char	*inbuf;
	char	*outbuf;
	size_t	retval;
	size_t	inbyt;
	size_t	outbyt;
	int		s_errno;


	strcpy(rstr, "");
	*rstrlen = 0;
	inbyt    = strlen(fstr);
	inbuf    = fstr;
	outbyt   = fbf->icvsiz;
	outbuf   = fbf->icvmsg;

	if(icvcd == (iconv_t)(-1)) {
		/* 変換なし */
		if(inbyt > fbf->icvsiz) {
			strncpy(fbf->icvmsg, fstr, fbf->icvsiz);
			fbf->icvmsg[fbf->icvsiz] = '\0';
			strcpy(rstr, &fstr[fbf->icvsiz]);
			*rstrlen = inbyt - fbf->icvsiz;
		}
		else {
			strcpy(fbf->icvmsg, fstr);
		}
		return(2);
	}

#ifdef DEBUG2
	DbgLog("iconv %5d sta %s\n", inbyt, fstr);
#endif

	if(inbyt == 0) {
		fbf->icvmsg[0] = (char)0;
	}
	else {
		while(inbyt > 0) {
			if((retval = iconv(icvcd, &inbuf, &inbyt, &outbuf, &outbyt)) == (size_t)(-1)) {
				s_errno = errno;
				*outbuf='\0';					/* 変換文字列END */
				*rstrlen = inbyt;				/* 未変換文字数 */
				MkRetMsg(fbf, 'I', __FILE__, __LINE__, errno, "iconv(%d, in %d, out %d)", icvcd, inbyt, outbyt);
#ifdef DEBUG2
				DbgLog("iconv %5d err %2d %s\n", inbyt, errno, fstr);
				DbgLog("iconv %5d err %2d %s\n", fbf->icvsiz-outbyt, errno, fbf->icvmsg);
#endif
				switch(s_errno) {
					case EILSEQ:				/* 入力に無効なマルチバイト文字列 */
						inbuf++;			/* 入力文字列のポインターを進める */
						inbyt--;			/* 入力文字列数を減らす */
						break;
					case EINVAL:				/* 入力に不完全なマルチバイト文字列 */
						strcpy(rstr, inbuf);		/* 未変換文字列セット */
						return(1);
					case E2BIG:				/* 出力バッファー空きなし */
						strcpy(rstr, inbuf);		/* 未変換文字列セット */
						return(2);
				}
			}
		}
		*outbuf = '\0';
#ifdef DEBUG2
		DbgLog("iconv %5d end %s\n", fbf->icvsiz-outbyt, fbf->icvmsg);
#endif
	}
	return(0);
}

int _iconvclose(struct ftpsub_buf *fbf, iconv_t *icvcd)
{
	if(*icvcd != (iconv_t)(-1) ) {
		if(iconv_close(*icvcd) == -1) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "iconv_close(%d)", *icvcd);
			return(C_RET_ERR);
		};
		*icvcd = (iconv_t)(-1);
	}
	return(C_RET_OK);
}

int sleepnano(struct ftpsub_buf *fbf, double t_sec)
{
	struct timespec req;
	struct timespec rem;


	if(t_sec < (double)0) return(C_RET_OK);

	long sec = req.tv_sec;
	long nsec = req.tv_nsec;
	SecDiv( t_sec, &sec, &nsec, 1000000000 );

	if ( ( req.tv_sec > 0 ) || ( req.tv_nsec > 0 ) ) {
		if(nanosleep( &req, &rem ) == -1 ) {
			MkRetMsg(fbf, 'E', __FILE__, __LINE__, errno, "nanosleep(%d.%09d)", req.tv_sec, req.tv_nsec);
			return(C_RET_OK);
		}
#ifdef DEBUG
		DbgLog("nanosleep(%d.%09d)\n", req.tv_sec, req.tv_nsec);
#endif
	}
	return(C_RET_OK);
}
