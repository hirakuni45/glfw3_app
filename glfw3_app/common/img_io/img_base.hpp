#pragma once
//=====================================================================//
/*!	@file
	@brief	画像テンプレート
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include "utils/vtx.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	イメージ・ベース・テンプレート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class PIX>
	struct img_base {

		typedef PIX	value_type;

	private:
		std::vector<PIX>	img_;

		vtx::spos	size_;
		bool		alpha_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_base() : img_(), size_(0), alpha_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	width	横幅を指定
			@param[in]	height	高さを指定
			@param[in]	alpha	アルファチャネルを有効にする場合に「true」
		*/
		//-----------------------------------------------------------------//
		void create(const vtx::spos& size, bool alpha) {
			img_.clear();
			size_ = size;
			img_.resize(size_.x * size_.y);
			alpha_ = alpha;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像サイズを得る
			@return	サイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ有効か
			@return	有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool alpha() const { return alpha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	「空」検査
			@return	「空」なら「true」
		*/
		//-----------------------------------------------------------------//
		bool empty() const { return img_.empty(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	使用している色数の総数
			@return	色数
		*/
		//-----------------------------------------------------------------//
		uint32_t count_color() const {
			boost::unordered_set<PIX> n;
			BOOST_FOREACH(const PIX& c, img_) {
				n.insert(c);
			}
			return static_cast<uint32_t>(n.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	描画エリアか検査
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool area(const vtx::spos& pos) const {
			if(static_cast<uint16_t>(pos.x) < static_cast<uint16_t>(size_.x) &&
			   static_cast<uint16_t>(pos.y) < static_cast<uint16_t>(size_.y)) {
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画素の読み出し参照
			@param[in]	pos	位置
			@return 画素
		*/
		//-----------------------------------------------------------------//
		const PIX& get(const vtx::spos& pos) const {
			if(area(pos)) {
				return img_[size_.x * pos.y + pos.x];
			} else {
				static PIX pix;
				return pix;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画素の参照
			@param[in]	pos	位置
			@return 画素
		*/
		//-----------------------------------------------------------------//
		PIX& at(const vtx::spos& pos) {
			if(area(pos)) {
				return img_[size_.x * pos.y + pos.x];
			} else {
				static PIX pix;
				return pix;
			}
		}
	};
}

