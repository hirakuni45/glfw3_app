#pragma once
//=====================================================================//
/*!	@file
	@brief	標準イメージを扱うテンプレートクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include "i_img.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	任意形式の画像を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T, uint32_t clutsize = 0>
	struct img_basic {
		typedef T	value_type;

	private:
		vtx::spos	size_;
		bool		alpha_;
		std::vector<T>	img_;

		uint32_t			clut_limit_;
		std::vector<rgba8>	clut_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_basic() : size_(0), alpha_(false), img_(),
			clut_limit_(0) {
			clut_.resize(clutsize);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~img_basic() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのタイプを得る。
			@return	イメージタイプ
		*/
		//-----------------------------------------------------------------//
		IMG::type get_type() const { return IMG::FULL8; }


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
			if(T::type_ != PIX::type_) return false;
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
			if(T::type_ != PIX::type_) return false;
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
			if(T::type_ != PIX::type_) return false;
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
			@brief	イメージのアドレスを得る。
			@return	イメージのポインター
		*/
		//-----------------------------------------------------------------//
		const void* get_image() const { return static_cast<const void*>(&img_[0]); }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのサイズを得る
			@return	イメージのサイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルの最大数を返す
			@return	最大数
		*/
		//-----------------------------------------------------------------//
		int get_clut_limit() const { return clut_limit_; }


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
			boost::unordered_set<T> n;
			BOOST_FOREACH(const T& c, img_) {
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
		*/
		//-----------------------------------------------------------------//
		void copy(const vtx::spos& dst, const img_rgba8& isrc, const vtx::srect& rsrc) {
			vtx::spos p;
			for(p.y = 0; p.y < rsrc.size.y; ++p.y) {
				for(p.x = 0; p.x < rsrc.size.x; ++p.x) {
					rgba8 c;
					if(isrc.get_pixel(p + rsrc.org, c)) {
						put_pixel(p + dst, c);
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	rgba8 イメージからのコピー
			@param[in]	src	ソースイメージ
		*/
		//-----------------------------------------------------------------//
		void copy(const img_rgba8& src) { copy(vtx::spos(0), src, vtx::srect(vtx::spos(0), src.get_size())); }


		//-----------------------------------------------------------------//
		/*!
			@brief	idx8 イメージからのコピー
			@param[in]	dst		コピー先の位置
			@param[in]	isrc	ソースイメージ
			@param[in]	rsrc	ソースの領域
		*/
		//-----------------------------------------------------------------//
		void copy(const vtx::spos& dst, const img_idx8& isrc, const vtx::srect& rsrc) {
			vtx::spos p;
			for(p.y = 0; p.y < rsrc.size.y; ++p.y) {
				for(p.x = 0; p.x < rsrc.size.x; ++p.x) {
					rgba8 c;
					if(isrc.get_pixel(p + rsrc.org, c)) {
						put_pixel(p + dst, c);
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	src	ソース・コンテキスト
		*/
		//-----------------------------------------------------------------//
		void swap(img_basic& src) {
			size_.swap(src.size_);
			img_.swap(src.img_);
			std::swap(clut_limit_, src.clut_limit_);
			clut_.swap(src.clut_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	単色で塗りつぶす
			@param[in]	c	塗る色
		*/
		//-----------------------------------------------------------------//
		inline void fill(const rgba8& c) {
			for(size_t i = 0; i < img_.size(); ++i) {
				img_[i] = c;
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
			std::vector<T>().swap(img_);
			clut_limit_ = 0;
			std::vector<rgba8>.swap(clut_);
		}
	};
}

