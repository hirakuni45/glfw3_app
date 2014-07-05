#pragma once
//=====================================================================//
/*!	@file
	@brief	グレー・スケール・イメージを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include "img_io/i_img.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット・グレースケールの画像を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct img_gray8 : public i_img {

		typedef gray8 value_type;

	private:
		vtx::spos	size_;

		std::vector<value_type>	img_;

		bool	alpha_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_gray8() : size_(0), alpha_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~img_gray8() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのタイプを得る。
			@return	イメージタイプ
		*/
		//-----------------------------------------------------------------//
		IMG::type get_type() const override { return IMG::GRAY8; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	size	サイズ
			@param[in]	alpha	アルファチャネルを有効にする場合「true」
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
		bool put_pixel(const vtx::spos& pos, const gray8& c) override {
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
			@brief	イメージに点を描画
			@param[in]	pos	描画位置
			@param[in]	c	描画するカラー
		*/
		//-----------------------------------------------------------------//
		bool put_pixel(const vtx::spos& pos, const rgba8& c) override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	pos	取得位置
			@param[out]	g	グレースケールを受け取るリファレンス
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(const vtx::spos& pos, gray8& g) const override {
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				g = img_[size_.x * pos.y + pos.x];
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
		*/
		//-----------------------------------------------------------------//
		bool get_pixel(const vtx::spos& pos, rgba8& c) const override {
			if(img_.empty()) return false;
			if(pos.x >= 0 && pos.x < size_.x && pos.y >= 0 && pos.y < size_.y) {
				const gray8& g = img_[size_.x * pos.y + pos.x];
				c.r = c.g = c.b = g.g;
				c.a = 255;
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
		const gray8* get_img(int y = 0) const {
			if(y >= 0 && y < size_.y) return &img_[size_.x * y]; else return 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのアドレスを得る。
			@return	イメージのポインター
		*/
		//-----------------------------------------------------------------//
		const void* operator() () const override { return static_cast<const void*>(&img_[0]); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズを得る
			@return	サイズ
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
			boost::unordered_set<gray8> n;
			BOOST_FOREACH(const gray8& c, img_) {
				n.insert(c);
			}
			return static_cast<uint32_t>(n.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	描画色で埋める
			@param[in]	c		描画色
			@param[in]	rect	領域
		*/
		//-----------------------------------------------------------------//
		void fill(const gray8& c, const vtx::srect& rect) {
			vtx::spos p;
			for(p.y = rect.org.y; p.y < rect.end_y(); ++p.y) {
				for(p.x = rect.org.x; p.x < rect.end_x(); ++p.x) {
					put_pixel(p, c);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	描画色で埋める
			@param[in]	c	描画色
		*/
		//-----------------------------------------------------------------//
		void fill(const gray8& c) {
			fill(c, vtx::srect(vtx::spos(0), size_));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	src	ソース・コンテキスト
		*/
		//-----------------------------------------------------------------//
		void swap(img_gray8& src) { *this = src; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを廃棄する
		*/
		//-----------------------------------------------------------------//
		void destroy() override {
			size_.set(0);
			std::vector<gray8>().swap(img_);
		}
	};
}

