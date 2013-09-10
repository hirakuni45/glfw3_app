//=====================================================================//
/*!	@file
	@brief	SQLite3 ラッパー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "sqlite.hpp"
#include <boost/foreach.hpp>
#include <stdio.h>

namespace sys {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void sqlite::initialize()
	{
		m_db = NULL;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	データベースファイルの適正を検査
		@param[in]	dbname	データベース名
		@return 成功なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sqlite::probe(const char* dbname)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	データベースを開く
		@param[in]	dbname	データベース名
		@return 成功なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sqlite::open(const char* dbname)
	{
		sqlite3_open(dbname, &m_db);
		if(sqlite3_errcode(m_db) != SQLITE_OK) {
			return false;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	データベースを閉じる
	 */
	//-----------------------------------------------------------------//
	void sqlite::close()
	{
		if(m_db != NULL) {
			sqlite3_close(m_db);
			m_db = NULL;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	SQL コマンドを発行
		@param[in]	command コマンド
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sqlite::command(const char* command)
	{
		if(command == 0) return false;

		sqlite3_stmt* stp = NULL;
		sqlite3_prepare(m_db, command, -1, &stp, NULL);
		if(stp == NULL) {
			printf("sqlite error: '%s'\n", error_message());
			return false;
		}

		while(sqlite3_step(stp) != SQLITE_DONE) ;

		sqlite3_finalize(stp);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	SQL ステートメントをコンパイル
		@param[in]	statement	ステートメントコード
		@return ステートメントのハンドル
	 */
	//-----------------------------------------------------------------//
	int sqlite::prepare(const char* statement)
	{
		if(statement == 0) return 0;

		sqlite3_stmt* stp = NULL;
		sqlite3_prepare(m_db, statement, -1, &stp, NULL);
		if(stp) {
			m_db_stmts.push_back(stp);
		} else {
			return -1;
		}

		return static_cast<int>(m_db_stmts.size());
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	SQL ステートメントを実行
		@param[in]	handle	ステートメントのハンドル
		@return 成功なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sqlite::step(int handle)
	{
		bool stts = false;
		--handle;
		if(static_cast<size_t>(handle) < m_db_stmts.size()) {
			while(sqlite3_step(m_db_stmts[handle]) == SQLITE_BUSY) ;
			stts = true;
		}
		return stts;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	SQL ステートメントのファイナライズ
		@param[in]	handle	ステートメントのハンドル
	 */
	//-----------------------------------------------------------------//
	void sqlite::finalize(int handle)
	{
		bool stts = false;
		--handle;
		if(static_cast<size_t>(handle) < m_db_stmts.size()) {
			sqlite3_finalize(m_db_stmts[handle]);
			m_db_stmts[handle] = 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void sqlite::destroy()
	{
		if(m_db) {
			BOOST_FOREACH(sqlite3_stmt* stp, m_db_stmts) {
				if(stp) sqlite3_finalize(stp);
			}
		}
		close();
	}

}

/*-- end of file --*/
