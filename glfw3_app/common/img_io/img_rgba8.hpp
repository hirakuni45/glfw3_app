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
		IMG::type get_type() const override { return IMG::FULL8; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	size	サイズ
			@param[in]	alpha	アルファチャネルを有効にする場合に「true」
		*/
		//-----------------------------------------------------------------//
		void create(const vtx::spos& size, bool alpha = false) override {
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
		bool put_clut(int idx, const rgba8& c) override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_clut(int idx, rgba8& c) const override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(const vtx::spos& pos, const idx8& c) override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	pos	描画位置
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(const vtx::spos& pos, idx8& c) const override { return false; }


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
		bool put_pixel(const vtx::spos& pos, const rgba8& c) override {
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
		bool get_pixel(const vtx::spos& pos, rgba8& c) const override {
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
			@brief	イメージのポインターを得る。
			@brief	y イメージの高さ（省略すると先頭）
			@return	イメージのポインター
		*/
		//-----------------------------------------------------------------//
		const rgba8* get_img(int y = 0) const {
			if(y >= 0 && y < size_.y) return &img_[size_.x * y]; else return 0;
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
		const vtx::spos& get_size() const override { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルの最大数を返す
			@return	最大数
		*/
		//-----------------------------------------------------------------//
		int get_clut_max() const override { return -1; }


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
			boost::unordered_set<rgba8> n;
			BOOST_FOREACH(const rgba8& c, img_) {
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
			@brief	gray8 イメージからのコピー
			@param[in]	dst		コピー先の位置
			@param[in]	isrc	ソースイメージ
			@param[in]	rsrc  	ソース領域
		*/
		//-----------------------------------------------------------------//
		void copy(const vtx::spos& dst, const img_gray8& isrc, const vtx::srect& rsrc) {
			vtx::spos p;
			for(p.y = 0; p.y < rsrc.size.y; ++p.y) {
				for(p.x = 0; p.x < rsrc.size.x; ++p.x) {
					gray8	g;
					if(isrc.get_pixel(p + rsrc.org, g)) {
						rgba8	c;
						c.r = c.g = c.b = g.g;
						if(g.g) c.a = 255; else c.a = 0;
						put_pixel(p + dst, c);
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	gray8 イメージからのコピー
			@param[in]	src	ソースイメージ
		*/
		//-----------------------------------------------------------------//
		void copy(const img_gray8& src) { copy(vtx::spos(0), src, vtx::srect(vtx::spos(0), src.get_size())); }


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
		void destroy() override {
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
			vtx::spos p;
			for(p.y = 0; p.y < size_.y; ++p.y) {
				for(p.x = 0; p.x < size_.x; ++p.x) {
					rgba8 c;
					if(img->get_pixel(p, c)) {
						put_pixel(p, c);
					}
				}
			}
			return *this;
		} 
	};
}

