#pragma once
//=========================================================================//
/*!	@file
	@brief	JPEG XL 画像を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include "i_img_io.hpp"
#include "img_rgba8.hpp"
#include "utils/file_io.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <jxl/codestream_header.h>
#include <jxl/decode.h>
#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/resizable_parallel_runner_cxx.h>
#include <jxl/types.h>

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	JPEG XL 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class jxl_io : public i_img_io {

		shared_img	img_;


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		jxl_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~jxl_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "jxl"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG XL ファイルか確認する（先頭のシグネチュアのみ）
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override
		{
			uint8_t sig[2];
			long org = fin.tell();
			size_t l = fin.read(sig, 1, 2);
			fin.seek(org, utils::file_io::SEEK::SET);
			if(l == 2) {
				if(sig[0] == 0xff && sig[1] == 0x0A) {
					return true;
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	fin	file_io クラス
			@param[in]	fo	情報を受け取る構造体
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, img::img_info& fo) override
		{
			if(!probe(fin)) {
				return false;
			}







			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG XL ファイル、ロード
			@param[in]	fin	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override
		{
			// とりあえず、ヘッダーの検査
			if(probe(fin) == false) {
				return false;
			}


			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG XL ファイル、ロード
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename, const std::string& opt = "") {
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				bool f = load(fin, opt);
				fin.close();
				return f;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG XL ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	セーブ品質の設定が可能（1～100) 省略すると「75」
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override
		{
			if(!img_) return false;
			if(img_->get_size().x == 0 || img_->get_size().y == 0) return false;





			return false;
		}


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
			@param[in]	img	イメージインターフェース
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
//			if(prgl_ref_ == 0) return 0;
//			return prgl_pos_ * scale / prgl_ref_;
			return 0;
		} 
	};
}
