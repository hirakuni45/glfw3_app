//=====================================================================//
/*!	@file
	@brief	グローバル・アロケーター（ヘッダー）
			new, delete, new[], delete[] オペレーターのオーバーロード
	@author	平松邦仁 (hira@rvf-rc45.net) 
*/
//=====================================================================//
#include <iostream>
#include <boost/format.hpp>
#include <assert.h>
#include "galloc.hpp"

#ifdef OPTION_MEMORY_ALOCATE
/// ↓このサイズを変更して、グローバル記憶ブロックサイズを調整する
static const size_t global_memory_block_size = (192 * 1024 * 1024);
static BXMM* global_bxmm = 0;

#ifndef NDEBUG
galloc galloc_dummy;	///< グローバル記憶管理ダミー用
#endif

#ifndef NDEBUG
void global_check_map()
{
	int	parity;
	int	prev;
	::bxmm_check_map(global_bxmm, &parity, &prev);
	std::cout << boost::format("GLOBAL BXMM: %d, %d") % parity % prev << std::endl;
}
#endif

//-----------------------------------------------------------------//
/*!
	@brief	グローバル、記憶ブロックの作成
 */
//-----------------------------------------------------------------//
void global_memory_create()
{
	if(global_bxmm == 0) {
		global_bxmm = memory_block_create("GAPP", global_memory_block_size, 16);
	}
}


BXMM* global_memory_get() { return global_bxmm; }


//-----------------------------------------------------------------//
/*!
	@brief	グローバル、記憶ブロックの廃棄
 */
//-----------------------------------------------------------------//
void global_memory_destroy()
{
	if(global_bxmm != 0) {
		memory_block_destroy(global_bxmm);
		global_bxmm = 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@brief	記憶管理ブロックを作成する。
	@param[in]	key		記憶管理テーブルの識別キー
	@param[in]	size	記憶割り当てブロックのサイズ（バイト）
	@param[in]	align	記憶管理ブロックの最小アライメントサイズ（バイト）
	@return	記憶管理テーブル構造体のポインター
 */
//-----------------------------------------------------------------//
BXMM *memory_block_create(const char* key, size_t size, size_t align)
{
	void* mem = malloc(size);
	assert(mem != 0);
	BXMM *bx = ::bxmm_create(mem, size, align);
#ifndef NDEBUG
	::bxmm_set_system_key(bx, key);
#endif
	::bxmm_set_ptr(bx, mem);
	return bx;
}


//-----------------------------------------------------------------//
/*!
	@brief	記憶管理ブロックを廃棄する。
	@param[in]	bx		記憶管理テーブル構造体のポインター
 */
//-----------------------------------------------------------------//
void memory_block_destroy(BXMM *bx)
{
#ifndef NDEBUG
	::bxmm_list_map(bx);
#endif
	void* mem = ::bxmm_get_ptr(bx);
	::bxmm_destroy(bx);
	::free(mem);
}


//-----------------------------------------------------------------//
/*!
	@brief	グローバル new オペレーターのオーバーロード
	@param[in]	n	獲得するサイズ
	@return	獲得したメモリーのポインター
 */
//-----------------------------------------------------------------//
void* operator new(std::size_t n) throw()
{
	global_memory_create();

	if(n == 0) n = 1;
	void* p = ::bxmm_malloc(global_bxmm, n);
	return p;
}


//-----------------------------------------------------------------//
/*!
	@brief	グローバル delete オペレーターのオーバーロード
	@param[in]	p	開放するメモリーのポインター
 */
//-----------------------------------------------------------------//
void operator delete(void* p) throw()
{
	::bxmm_free(global_bxmm, p);
}


//-----------------------------------------------------------------//
/*!
	@brief	グローバル new[] オペレーターのオーバーロード
	@param[in]	n	獲得するサイズ
	@return	獲得したメモリーのポインター
 */
//-----------------------------------------------------------------//
void* operator new[](std::size_t n) throw()
{
	global_memory_create();

	if(n == 0) n = 1;
	void* p = ::bxmm_malloc(global_bxmm, n);
	return p;
}


//-----------------------------------------------------------------//
/*!
	@brief	グローバル delete[] オペレーターのオーバーロード
	@param[in]	p	開放するメモリーのポインター
 */
//-----------------------------------------------------------------//
void operator delete[](void* p) throw()
{
	::bxmm_free(global_bxmm, p);
}
#endif
