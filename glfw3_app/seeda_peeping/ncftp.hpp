#pragma once
//=====================================================================//
/*! @file
    @brief  libncftp サポート・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <ncftp.h>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ncftp サポート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ncftp {

		FtwInfo		info_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ncftp() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
			@param[in]	target	接続先
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(const std::string& target)
		{
			std::memset(&info_, sizeof(FtwInfo), 0);

			info_.dirSeparator = '/';
			info_.rootDir[0] = '/';
			info_.init = kFtwMagic;


			return true;
		}


#if 0

int
Ftw(const FtwInfoPtr ftwip, const char *const path, FtwProc proc)
{
	size_t len, alen;
	int rc;
	char *cp, *endp;
	size_t debufsize = 256;
#ifdef HAVE_PATHCONF
	long nmx;
#endif

	if ((ftwip->init != kFtwMagic) || (path == NULL) || (path[0] == '\0') || (proc == (FtwProc) 0)) {
		errno = EINVAL;
		return (-1);
	}

	ftwip->rlinkto = NULL;
	ftwip->startPathLen = 0;
	len = strlen(path);
	if (ftwip->curPath == NULL) {
		/* Call FtwSetBuf before calling Ftw for
		 * the first time, otherwise you get the
		 * default behavior.
		 */
		ftwip->autoGrow = kFtwAutoGrow;
		alen = len + 30 /* room to append filenames */ + 2 /* room for / and \0 */;
		if (alen < 256)
			alen = 256;
		ftwip->curPath = (char *) malloc(alen);
		if (ftwip->curPath == NULL)
			return (-1);
		ftwip->curPathAllocSize = alen - 2;
	}

	ftwip->curType = 'd';
	memset(ftwip->curPath, 0, ftwip->curPathAllocSize);
	memcpy(ftwip->curPath, path, len + 1);
	cp = ftwip->curPath + strlen(ftwip->curPath);
	--cp;
	while ((cp > ftwip->curPath) && IsLocalPathDelim(*cp))
		*cp-- = '\0';
	endp = cp + 1;
	ftwip->curPathLen = ftwip->startPathLen = len = (size_t) (endp - ftwip->curPath);
	while (cp >= ftwip->curPath) {
		if (IsLocalPathDelim(*cp))
			break;
		--cp;
	}
	ftwip->curFile = ++cp;
	ftwip->curFileLen = (size_t) (endp - cp);

	/* Note: we use Stat instead of Lstat here because we allow the
	 * top level node (as specified by path) to be a symlink
	 * to a directory.
	 */
	memset(&ftwip->curStat, 0, sizeof(ftwip->curStat));
	if (Stat(ftwip->curPath, &ftwip->curStat) < 0) {
		return (-1);
	} else if (! S_ISDIR(ftwip->curStat.st_mode)) {
		errno = ENOTDIR;
		return (-1);
	}

#ifdef HAVE_PATHCONF
	nmx = pathconf(ftwip->curPath, _PC_NAME_MAX);
	if (nmx >= 256)
		debufsize = (size_t) nmx;
#endif
	debufsize += sizeof(struct dirent) + 8;
	ftwip->direntbuf = calloc(debufsize, (size_t) 1);
	if (ftwip->direntbuf == NULL) {
		return (-1);
	}
	ftwip->direntbufSize = debufsize;

	ftwip->isBaseDir = 1;
	ftwip->proc = proc;
	if ((*proc)(ftwip) < 0) {
		free(ftwip->direntbuf);
		ftwip->direntbuf = NULL;
		return (-1);
	}
	ftwip->isBaseDir = 0;

	ftwip->depth = ftwip->maxDepth = ftwip->numDirs = ftwip->numFiles = ftwip->numLinks = 0;
	rc = FtwTraverse(ftwip, len, 1);

	/* Restore the start path when finished. */
	memset(ftwip->curPath + ftwip->startPathLen, 0, ftwip->curPathAllocSize - ftwip->startPathLen);
	ftwip->curPathLen = ftwip->startPathLen;

	/* Clear these out since you shouldn't be using them
	 * after Ftw returns.
	 */
	memset(&ftwip->curStat, 0, sizeof(ftwip->curStat));
	ftwip->proc = (FtwProc) 0;
	ftwip->curFile = ftwip->curPath;
	ftwip->curFileLen = 0;
	ftwip->cip = 0;
	ftwip->rlinkto = NULL;
	free(ftwip->direntbuf);
	ftwip->direntbuf = NULL;

	return (rc);
}	/* Ftw */

#endif


	};
}

#if 0
static int
FtwTraverse(const FtwInfoPtr ftwip, size_t dirPathLen, size_t depth)
{
	DIR *DIRp;
	char *cp;
	size_t fnLen;
	struct dirent *dentp;
	mode_t m;
	char *filename;
	char *newBuf;
	char *path = ftwip->curPath;
	int nSubdirs;
	FtwSubDirListPtr head = NULL, tail = NULL, sdp, nextsdp;
	int rc = (-1);
	int isRootDir;

	isRootDir = ((dirPathLen == 1) && (IsLocalPathDelim(path[0]))) ? 1 : 0;
	if ((DIRp = opendir(dirPathLen ? path : ".")) == NULL) {
		/* Not an error unless the first directory could not be opened. */
		return (0);
	}

	nSubdirs = 0;
	++ftwip->numDirs;
	ftwip->depth = depth;
	if (ftwip->maxDepth < ftwip->depth) {
		ftwip->maxDepth = ftwip->depth;
	}
	filename = path + dirPathLen;
	if (isRootDir == 0) {	/* Root directory is a separator. */
		*filename++ = (char) ftwip->dirSeparator;
		dirPathLen++;
	}
	*filename = '\0';
	/* Path now contains dir/  */

	dentp = (struct dirent *) ftwip->direntbuf;
	for (;;) {
		if (Readdir(DIRp, dentp, ftwip->direntbufSize) == NULL)
			break;
		cp = dentp->d_name;
		if ((cp[0] == '.') && ((cp[1] == '\0') || ((cp[1] == '.') && (cp[2] == '\0'))))
			continue;	/* Skip . and .. */

		*filename = '\0';
		fnLen = strlen(cp) + 1	/* include \0 */;
		if ((fnLen + dirPathLen) > ftwip->curPathAllocSize) {
			if (ftwip->autoGrow == kFtwNoAutoGrowAndFail) {
				goto panic;
			} else if (ftwip->autoGrow == kFtwNoAutoGrowButContinue) {
				continue;
			}
			newBuf = (char *) realloc(ftwip->curPath, fnLen + dirPathLen + 30 + 2 /* room for / and \0 */);
			if (newBuf == NULL)
				goto panic;
			ftwip->curPath = newBuf;
			ftwip->curPathAllocSize = fnLen + dirPathLen + 30;
			path = ftwip->curPath;
			filename = path + dirPathLen;
			if (isRootDir == 0)	/* Root directory is a separator. */
				*filename++ = (char) ftwip->dirSeparator;
			*filename = '\0';
		}
		memcpy(filename, cp, fnLen);
		ftwip->curPathLen = dirPathLen + fnLen - 1;
		ftwip->curFile = filename;
		ftwip->curFileLen = fnLen - 1;
		if (Lstat(path, &ftwip->curStat) == 0) {
			m = ftwip->curStat.st_mode;
			if (S_ISREG(m)) {
				++ftwip->numFiles;
				ftwip->curType = '-';
				if ((*ftwip->proc)(ftwip) < 0) {
					goto panic;
				}
#ifdef S_ISLNK
			} else if (S_ISLNK(m)) {
				ftwip->curType = 'l';
				++ftwip->numLinks;
				if ((*ftwip->proc)(ftwip) < 0) {
					goto panic;
				}
#endif	/* S_ISLNK */
			} else if (S_ISDIR(m)) {
				/* We delay entering the subdirectories
				 * until we have closed this directory.
				 * This will conserve file descriptors
				 * and also have the effect of having
				 * the files processed first.
				 */
				sdp = (FtwSubDirListPtr) malloc(sizeof(FtwSubDirList) + fnLen);
				if (sdp == NULL)
					goto panic;
				memcpy(&sdp->st, &ftwip->curStat, sizeof(sdp->st));
				memcpy(sdp->name, cp, fnLen);
				sdp->fnLen = fnLen;
				sdp->next = NULL;
				if (head == NULL) {
					head = tail = sdp;
				} else {
					tail->next = sdp;
					tail = sdp;
				}
				nSubdirs++;
			}
		}
	}
	(void) closedir(DIRp);
	DIRp = NULL;

	/* Now enter each subdirectory. */
	for (sdp = head; sdp != NULL; sdp = nextsdp) {
		nextsdp = sdp->next;
		memcpy(&ftwip->curStat, &sdp->st, sizeof(ftwip->curStat));
		fnLen = sdp->fnLen;
		memcpy(filename, sdp->name, fnLen);
		ftwip->curPathLen = dirPathLen + fnLen - 1;
		ftwip->curFile = filename;
		ftwip->curFileLen = fnLen - 1;
		head = nextsdp;
		free(sdp);

		ftwip->curType = 'd';
		if ((*ftwip->proc)(ftwip) < 0) {
			goto panic;
		}
		if (((ftwip->maxDepthAllowed == 0) || (depth < ftwip->maxDepthAllowed)) && (FtwTraverse(ftwip, dirPathLen + fnLen - 1, depth + 1) < 0))
			goto panic;

		/* Reset these, since buffer could have
		 * been reallocated.
		 */
		path = ftwip->curPath;
		filename = path + dirPathLen;
		*filename = '\0';
	}
	head = NULL;
	rc = 0;

panic:
	if (DIRp != NULL)
		(void) closedir(DIRp);

	for (sdp = head; sdp != NULL; sdp = nextsdp) {
		nextsdp = sdp->next;
		free(sdp);
	}

	return (rc);
}	/* FtwTraverse */
#endif
