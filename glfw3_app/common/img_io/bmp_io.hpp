#pragma once
//=====================================================================//
/*!	@file
	@brief	BMP 画像を扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "i_img_io.hpp"
#include "img_idx8.hpp"
#include "img_rgba8.hpp"

namespace img {

	struct bmp_info;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	BMP 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bmp_io : public i_img_io {

		shared_img	img_;

		uint32_t	prgl_ref_;
		uint32_t	prgl_pos_;

		bool read_idx_(utils::file_io& fin, shared_img img, const bmp_info& bmp);
		bool read_rgb_(utils::file_io& fin, shared_img img, const bmp_info& bmp);
		bool read_bitfield_(utils::file_io& fin, shared_img img, const bmp_info& bmp);
		bool decompress_rle_(utils::file_io& fin, shared_img img, const bmp_info& bmp);
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		bmp_io() : prgl_ref_(0), prgl_pos_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~bmp_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "bmp"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	BMP ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	fin	file_io クラス
			@param[in]	fo	情報を受け取る構造体
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, img::img_info& fo) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	BMP ファイル、ロード(utils::file_io)
			@param[in]	fin	ファイル I/O クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	BMP ファイルをセーブする
			@param[in]	fout	ファイル I/O クラス
			@param[in]	opt		フォーマット固有の設定文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		const shared_img get_image() const override { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースの登録
			@param[in]	imf	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(shared_img img) override { img_ = img; }


		//-----------------------------------------------------------------//
		/*!
			@brief	decode/encode の進行状態を取得する
			@return 完了の場合 scale を返す
		*/
		//-----------------------------------------------------------------//
		uint32_t get_progless(uint32_t scale) const override {
			if(prgl_ref_ == 0) return 0;
			return prgl_pos_ * scale / prgl_ref_;
		} 
	};
}
