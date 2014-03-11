#pragma once
//=====================================================================//
/*!	@file
	@brief	IDX8 イメージを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <set>
#include "img_io/i_img.hpp"
#include "img_io/img_clut.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Indexed8 形式の画像を扱うクラス@n
		※RGBA8 形式のカラー・ルック・アップ・テーブル付き
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct img_idx8 : public i_img {

		typedef idx8	pixel_type;

	private:
		vtx::spos	size_;

		int		clut_max_;
		rgba8	clut_[256];

		std::vector<idx8>	img_;

		bool	alpha_;

		bool	color_map_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_idx8() : size_(0), clut_max_(0), alpha_(false), color_map_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~img_idx8() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのタイプを得る。
			@return	イメージタイプ
		*/
		//-----------------------------------------------------------------//
		IMG::type get_type() const override { return IMG::INDEXED8; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	size	サイズ
			@param[in]	alpha	アルファ・チャネルを有効にする場合「true」
		*/
		//-----------------------------------------------------------------//
		void create(const vtx::spos& size, bool alpha = false) override {
			destroy();
			size_ = size;
			clut_max_ = 0;
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
		bool put_clut(int idx, const rgba8& c) {
			if(idx >= 0 && idx < 256) {
				if(idx >= clut_max_) clut_max_ = idx + 1;
				clut_[idx] = c;
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_clut(int idx, rgba8& c) const override {
			if(idx >= 0 && idx < clut_max_) {
				c = clut_[idx];
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(const vtx::spos& pos, const idx8& c) override {
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
		bool get_pixel(const vtx::spos& pos, idx8& c) const override {
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
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(const vtx::spos& pos, const gray8& c) override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	pos	描画位置
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(const vtx::spos& pos, gray8& c) const override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(const vtx::spos& pos, const rgba8& c) override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	pos	描画位置
			@param[in]	c	描画されたカラーを受け取るリファレンス
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(const vtx::spos& pos, rgba8& c) const override {
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				idx8 idx  = img_[size_.x * pos.y + pos.x];
				c = clut_[idx.i];
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
		const void* get_image() const override { return static_cast<const void*>(&img_[0]); }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのサイズを得る
			@return	イメージのサイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_size() const override { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルの最大数を返す
			@return	最大数
		*/
		//-----------------------------------------------------------------//
		int get_clut_max() const override { return clut_max_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ・チャネルが有効か調べる
			@return	アルファ・チャネルが有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool test_alpha() const override { return alpha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	画像が「空」か検査する
			@return	「空」なら「true」
		*/
		//-----------------------------------------------------------------//
		bool empty() const override { return img_.empty(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	利用している色数の総数をカウントする
			@return	利用している色数
		*/
		//-----------------------------------------------------------------//
		uint32_t count_color() const override {
			boost::unordered_set<idx8> n;
			BOOST_FOREACH(const idx8& c, img_) {
				n.insert(c);
			}
			return static_cast<uint32_t>(n.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オプティマイズ（使っているカラー番号を順番に並べなおす）
		*/
		//-----------------------------------------------------------------//
		void index_optimize() {
			std::set<idx8> n;
			for(std::vector<idx8>::const_iterator cit = img_.begin(); cit != img_.end(); ++cit) {
				n.insert(*cit);
			}
			if(static_cast<int>(n.size()) >= clut_max_) {
				// 並べなおす必要無し！
				return;
			}
			unsigned char cnv[256];
			rgba8 clut[256];
			int i = 0;
			for(std::set<idx8>::const_iterator cit = n.begin(); cit != n.end(); ++cit) {
				int idx = (*cit).i;
				cnv[idx] = i;
				clut[i] = clut_[idx];
				++i;
			}
			clut_max_ = i;
			for(int i = 0; i < clut_max_; ++i) clut_[i] = clut[i];
			for(std::vector<idx8>::iterator it = img_.begin(); it != img_.end(); ++it) {
				idx8& i = *it;
				i.i = cnv[i.i];
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	src	ソース・コンテキスト
		*/
		//-----------------------------------------------------------------//
		void swap(img_idx8& src) { *this = src; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを塗りつぶす
			@param[in]	c	塗りつぶすカラー
		*/
		//-----------------------------------------------------------------//
		void fill(const idx8& c) {
			if(size_.x <= 0 || size_.y <= 0) return;

			idx8* dst = &img_[0];
			memset(dst, c.i, size_.x * size_.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを廃棄する
		*/
		//-----------------------------------------------------------------//
		void destroy() override {
			size_.set(0);
			clut_max_ = 0;
			std::vector<idx8>().swap(img_);
		}

	};
}

