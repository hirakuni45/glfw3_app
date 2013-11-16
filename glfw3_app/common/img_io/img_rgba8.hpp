#pragma once
//=====================================================================//
/*!	@file
	@brief	RGBA8 イメージを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include "i_img.hpp"
#include "img_idx8.hpp"
#include "img_gray8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	RGBA8 形式の画像を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct img_rgba8 : public i_img {

		typedef rgba8	pixel_type;

	private:
		vtx::spos	size_;

		std::vector<rgba8>	img_;

		bool	alpha_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_rgba8() : size_(0), alpha_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~img_rgba8() { }


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
			@brief	カラー・ルック・アップ・テーブルを設定
			@param[in]	idx	テーブルの位置
			@param[in]	c	設定するカラー
		*/
		//-----------------------------------------------------------------//
		void put_clut(int idx, const rgba8& c) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
		*/
		//-----------------------------------------------------------------//
		void get_clut(int idx, rgba8& c) const { }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(int x, int y, const idx8& c) { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(int x, int y, idx8& c) const { return false; }


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
		bool get_pixel(int x, int y, rgba8& c) const {
			if(x >= 0 && x < size_.x && y >= 0 && y < size_.y) {
				c  = img_[size_.x * y + x];
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ合成描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画カラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool alpha_pixel(int x, int y, rgba8& c) {
			if(x >= 0 && x < size_.x && y >= 0 && y < size_.y) {
				rgba8 s = img_[size_.x * y + x];
				unsigned short i = 256 - c.a;
				unsigned short a = c.a + 1;
				img_[size_.x * y + x].set(((s.r * i) + (c.r * a)) >> 8,
										   ((s.g * i) + (c.g * a)) >> 8,
										   ((s.b * i) + (c.b * a)) >> 8);
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのポインターを得る。
			@brief	y イメージの高さ（省略すると先頭）
			@return	イメージのポインター
		*/
		//-----------------------------------------------------------------//
		const rgba8* get_img(int y = 0) const {
			if(y >= 0 && y < size_.y) return &img_[size_.x * y]; else return 0;
		}


		const void* get_image() const { return static_cast<const void*>(&img_[0]); }


		//-----------------------------------------------------------------//
		/*!
			@brief	読み書き可能な画像のポインターを取得
			@param[in]	n	オフセット
			@return	画像ポインター
		*/
		//-----------------------------------------------------------------//
		rgba8* at_image(int n = 0) { return &img_[n]; }


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
		int get_clut_max() const { return -1; }


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
			boost::unordered_set<rgba8> n;
			BOOST_FOREACH(const rgba8& c, img_) {
				n.insert(c);
			}
			return static_cast<unsigned int>(n.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの部分コピー
			@param[in]	dstx	コピー先の X 位置
			@param[in]	dsty	コピー先の Y 位置
			@param[in]	src	ソースイメージ
			@param[in]	xx	ソースの開始 X 位置
			@param[in]	yy	ソースの開始 Y 位置
			@param[in]	xl	ソースのコピー横幅
			@param[in]	yl	ソースのコピー高さ
		*/
		//-----------------------------------------------------------------//
		void copy(int dstx, int dsty, const img_rgba8& src, int xx, int yy, int xl, int yl) {
			for(int y = 0; y < yl; ++y) {
				for(int x = 0; x < xl; ++x) {
					rgba8 c;
					src.get_pixel(x + xx, y + yy, c);
					put_pixel(x + dstx, y + dsty, c);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	rgba8 イメージからのコピー
			@param[in]	src	ソースイメージ
		*/
		//-----------------------------------------------------------------//
		void copy(const img_rgba8& src) {
			copy(0, 0, src, 0, 0, src.get_size().x, src.get_size().y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	idx8 イメージからのコピー
			@param[in]	dstx	コピー先の X 位置
			@param[in]	dsty	コピー先の Y 位置
			@param[in]	src	ソースイメージ
			@param[in]	xx	ソースの開始 X 位置
			@param[in]	yy	ソースの開始 Y 位置
			@param[in]	xl	ソースのコピー横幅
			@param[in]	yl	ソースのコピー高さ
		*/
		//-----------------------------------------------------------------//
		void copy(int dstx, int dsty, const img_idx8& src, int xx, int yy, int xl, int yl) {
			rgba8	c;
			for(int y = 0; y < yl; ++y) {
				for(int x = 0; x < xl; ++x) {
					src.get_pixel(x + xx, y + yy, c);
					put_pixel(x + dstx, y + dsty, c);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	gray8 イメージからのコピー
			@param[in]	dstx	コピー先の X 位置
			@param[in]	dsty	コピー先の Y 位置
			@param[in]	src	ソースイメージ
			@param[in]	xx	ソースの開始 X 位置
			@param[in]	yy	ソースの開始 Y 位置
			@param[in]	xl	ソースのコピー横幅
			@param[in]	yl	ソースのコピー高さ
		*/
		//-----------------------------------------------------------------//
		void copy(int dstx, int dsty, const img_gray8& src, int xx, int yy, int xl, int yl) {
			gray8	g;
			rgba8	c;
			for(int y = 0; y < yl; ++y) {
				for(int x = 0; x < xl; ++x) {
					src.get_pixel(x + xx, y + yy, g);
					c.r = c.g = c.b = g.g;
					if(g.g) c.a = 255; else c.a = 0;
					put_pixel(x + dstx, y + dsty, c);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	gray8 イメージからのコピー
			@param[in]	src	ソースイメージ
		*/
		//-----------------------------------------------------------------//
		void copy(const img_gray8& src) { copy(0, 0, src, 0, 0,
			src.get_size().x, src.get_size().y); }


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	src	ソース・コンテキスト
		*/
		//-----------------------------------------------------------------//
		void swap(img_rgba8& src) {
			img_rgba8* ptr = this;
			*this = src;
			src = *ptr;
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
			std::vector<rgba8>().swap(img_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースからのコピー
		*/
		//-----------------------------------------------------------------//
		img_rgba8& operator = (const i_img* img) {
			if(img == 0) return *this;
			create(img->get_size(), img->test_alpha());
			for(int y = 0; y < size_.y; ++y) {
				for(int x = 0; x < size_.x; ++x) {
					rgba8 c;
					img->get_pixel(x, y, c);
					put_pixel(x, y, c);
				}
			}
			return *this;
		} 
	};
}

