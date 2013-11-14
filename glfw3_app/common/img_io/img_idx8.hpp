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
		img_idx8() : size_(0),
			clut_max_(0), alpha_(false), color_map_(false) { }


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
		IMG::type get_type() const { return IMG::INDEXED8; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	width	横幅を指定
			@param[in]	height	高さを指定
			@param[in]	alpha	アルファ・チャネルを有効にする場合「true」
		*/
		//-----------------------------------------------------------------//
		void create(const vtx::spos& size, bool alpha = false) {
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
		*/
		//-----------------------------------------------------------------//
		void put_clut(int idx, const rgba8& c) {
			if(idx >= 0 && idx < 256) {
				if(idx >= clut_max_) clut_max_ = idx + 1;
				clut_[idx] = c;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
		*/
		//-----------------------------------------------------------------//
		void get_clut(int idx, rgba8& c) const {
			if(idx >= 0 && idx < clut_max_) {
				c = clut_[idx];
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(int x, int y, const idx8& c) {
			if(x >= 0 && x < size_.x && y >= 0 && y < size_.y) {
				img_[size_.x * y + x] = c;
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(int x, int y, idx8& c) const {
			if(x >= 0 && x < size_.x && y >= 0 && y < size_.y) {
				c  = img_[size_.x * y + x];
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(int x, int y, const gray8& c) {
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(int x, int y, gray8& c) const {
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(int x, int y, const rgba8& c) {
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(int x, int y, rgba8& c) const {
			if(x >= 0 && x < size_.x && y >= 0 && y < size_.y) {
				idx8 idx  = img_[size_.x * y + x];
				c = clut_[idx.i];
				return true;
			}
			return false;
		}


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
		int get_clut_max() const { return clut_max_; }


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
		unsigned int count_color() const {
			boost::unordered_set<idx8> n;
			BOOST_FOREACH(const idx8& c, img_) {
				n.insert(c);
			}
#if 0
			std::set<idx8> n;
			for(std::vector<idx8>::const_iterator cit = img_.begin(); cit != img_.end(); ++cit) {
				n.insert(*cit);
			}
#endif
			return static_cast<unsigned int>(n.size());
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
		void destroy() {
			size_.set(0);
			clut_max_ = 0;
			std::vector<idx8>().swap(img_);
		}

	};
}

