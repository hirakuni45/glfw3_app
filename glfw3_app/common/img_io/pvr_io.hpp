#pragma once
//=====================================================================//
/*!	@file
	@brief	PVR 画像を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "i_img_io.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PVR 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pvr_io : public i_img_io {

		shared_img	img_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pvr_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pvr_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "pvr"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	PVR ファイルか確認する
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
			@brief	PVR ファイル、ロード
			@param[in]	fin		file_io クラス
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	PVR ファイル、ロード
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
			@brief	ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをセーブする
			@param[in]	fn	ファイル名
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& fn, const std::string& opt = "") {
			utils::file_io out;
			if(out.open(fn, "wb")) {
				bool f = save(out, opt);
				out.close();
				return f;
			} else {
				return false;
			}
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
			@brief	イメージの登録
			@param[in]	img	イメージ
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
			return 0;
		}
	};

}
