#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget 共通パーツ（ヘッダー）@n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/glmobj.hpp"
#include "widgets/widget.hpp"
#include <boost/unordered_map.hpp>

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	widget 共通パーツ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct common_parts {

	private:
		gl::mobj&		mobj_;

		typedef gl::mobj::handle handle;
		typedef boost::unordered_map<share_t, handle>	share_map;
		typedef share_map::iterator			share_map_it;
		typedef share_map::const_iterator	share_map_cit;
		share_map	share_map_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		common_parts(gl::mobj& mo) : mobj_(mo) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	共有ハンドルを追加
			@param[in]	k	キー
			@return	描画ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle add(const share_t& k);


		//-----------------------------------------------------------------//
		/*!
			@brief	共有ハンドルを取得
			@param[in]	k	キー
			@return	共有ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle get(const share_t& k) const {
			gl::mobj::handle h = 0;
			share_map_cit cit = share_map_.find(k);
			if(cit != share_map_.end()) {
				h = cit->second;
			}
			return h;
		}

	};


}
