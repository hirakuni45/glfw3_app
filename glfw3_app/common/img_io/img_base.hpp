#pragma once
//=====================================================================//
/*!	@file
	@brief	標準イメージを扱うテンプレートクラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <boost/variant.hpp>
#include "utils/vtx.hpp"
#include "img_io/img.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	任意形式の画像を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class BPX, uint32_t clutsize = 0>
	struct img_base {
		typedef BPX	value_type;
		typedef std::vector<BPX>	img_type;
		typedef std::vector<rgba8>	clut_type;

	private:
		vtx::spos	size_;
		bool		alpha_;
		img_type	img_;

		uint32_t	clut_limit_;
		clut_type	clut_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_base() : size_(0), alpha_(false), img_(),
			clut_limit_(0) {
			clut_.resize(clutsize);
std::cout << "Const: " << static_cast<int>(get_type()) << std::endl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~img_base() {
std::cout << "Destr: " << static_cast<int>(get_type()) << std::endl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのタイプを得る。
			@return	イメージタイプ
		*/
		//-----------------------------------------------------------------//
		IMG::type get_type() const { return BPX::type_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	size	サイズ
			@param[in]	alpha	アルファチャネルを有効にする場合に「true」
		*/
		//-----------------------------------------------------------------//
		void create(const vtx::spos& size, bool alpha = false) {
			img_.clear();
			size_ = size;
			img_.resize(size_.x * size_.y);
			alpha_ = alpha;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを設定
			@param[in]	idx	テーブルの位置
			@param[in]	c	設定するカラー
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_clut(uint32_t idx, const rgba8& c) {
			if(idx < clut_.size()) {
				clut_[idx] = c;
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_clut(uint32_t idx, rgba8& c) const {
			if(idx < clut_.size()) {
				c = clut_[idx];
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		template <class PIX>
		bool put_pixel(const vtx::spos& pos, const PIX& c) {
			if(BPX::type_ != PIX::type_) return false;
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				img_[size_.x * pos.y + pos.x] = c;
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	pos	描画位置
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		template <class PIX>
		bool get_pixel(const vtx::spos& pos, PIX& c) const {
			if(BPX::type_ != PIX::type_) return false;
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				c  = img_[size_.x * pos.y + pos.x];
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ合成描画
			@param[in]	pos	描画位置
			@param[in]	c	描画カラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool alpha_pixel(const vtx::spos& pos, rgba8& c) {
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				rgba8 s = img_[size_.x * pos.y + pos.x];
				unsigned short i = 256 - c.a;
				unsigned short a = c.a + 1;
				img_[size_.x * pos.y + pos.x].set(((s.r * i) + (c.r * a)) >> 8,
										   ((s.g * i) + (c.g * a)) >> 8,
										   ((s.b * i) + (c.b * a)) >> 8);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのサイズを得る
			@return	イメージのサイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの参照
			@return	イメージ
		*/
		//-----------------------------------------------------------------//
		const img_type& get_image() const { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの参照
			@return	イメージ
		*/
		//-----------------------------------------------------------------//
		img_type& at_image() { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルの最大数を返す
			@return	最大数
		*/
		//-----------------------------------------------------------------//
		int get_clut_limit() const { return clut_limit_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラーテーブルの参照
			@return	カラーテーブル
		*/
		//-----------------------------------------------------------------//
		const clut_type& get_clut() const { return clut_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラーテーブルの参照
			@return	カラーテーブル
		*/
		//-----------------------------------------------------------------//
		clut_type& at_clut() { return clut_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ・チャネルが有効か調べる
			@return	アルファ・チャネルが有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool test_alpha() const { return alpha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	画像が「空」か検査する
			@return	「空」なら「true」
		*/
		//-----------------------------------------------------------------//
		bool empty() const { return img_.empty(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	利用している色数の総数をカウントする
			@return	利用している色数
		*/
		//-----------------------------------------------------------------//
		uint32_t count_color() const {
			boost::unordered_set<BPX> n;
			BOOST_FOREACH(const BPX& c, img_) {
				n.insert(c);
			}
			return static_cast<uint32_t>(n.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの部分コピー
			@param[in]	dst		コピー先
			@param[in]	isrc	ソースイメージ
			@param[in]	rsrc	ソースの領域
			@return コピーに失敗すると「false」を返す
		*/
		//-----------------------------------------------------------------//
		template <class SPX = rgba8, uint32_t csz = 0>
		bool copy(const vtx::spos& dst, const img_base<SPX, csz>& isrc, const vtx::srect& rsrc) {
			// FULL から INDEXED
			if(SPX::type_ == IMG::FULL8 && BPX::type_ == IMG::INDEXED8) return false;
			vtx::spos p;
			for(p.y = 0; p.y < rsrc.size.y; ++p.y) {
				for(p.x = 0; p.x < rsrc.size.x; ++p.x) {
					SPX c;
					if(isrc.get_pixel(p + rsrc.org, c)) {
						put_pixel(p + dst, c);
					}
				}
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コピー
			@param[in]	src	ソースイメージ
			@return コピーに失敗すると「false」を返す
		*/
		//-----------------------------------------------------------------//
		template <class SPX = rgba8, uint32_t csz = 0>
		bool copy(const img_base<SPX, csz>& src) {
			return copy(vtx::spos(0), src, vtx::srect(vtx::spos(0), src.get_size()));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	src	ソース・コンテキスト
		*/
		//-----------------------------------------------------------------//
		void swap(img_base& src) {
//			size_.swap(src.size_);
			img_.swap(src.at_image());
//			std::swap(alpha_, src.alpha_);
//			std::swap(clut_limit_, src.clut_limit_);
			clut_.swap(src.at_clut());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	単色で塗りつぶす
			@param[in]	c	塗る色
			@param[in]	r	領域（省略すると全体）
		*/
		//-----------------------------------------------------------------//
		void fill(const BPX& c, const vtx::srect& r = vtx::srect(vtx::spos(0), get_size())) {
			vtx::spos pos = r.org;
			for(pos.y = 0; pos.y < r.end_y(); ++pos.y) {
				for(pos.x = 0; pos.x < r.end_x(); ++pos.x) {
					put_pixel(pos, c);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを廃棄する
		*/
		//-----------------------------------------------------------------//
		void destroy() {
			size_.set(0);
			alpha_ = false;
			img_type().swap(img_);
			clut_limit_ = 0;
			clut_type().swap(clut_);
		}
	};

	typedef img_base<idx8>    img_idx8;
	typedef img_base<idx16>   img_idx16;
	typedef img_base<gray8>   img_gray8;
//	typedef img_base<gray16>  img_gray16;
	typedef img_base<rgba8>   img_rgba8;
	typedef img_base<rgba16>  img_rgba16;

	typedef boost::variant<img_idx8, img_gray8, img_rgba8>  image8;
}

