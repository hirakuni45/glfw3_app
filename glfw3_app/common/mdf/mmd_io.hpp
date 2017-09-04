#pragma once
//=====================================================================//
/*!	@file
	@brief	MMD ファイルを扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "mdf/pmd_io.hpp"
#include "mdf/pmx_io.hpp"

namespace mdf {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	mmd_io クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class mmd_io {
	public:
		enum class type {
			none,
			pmd,
			pmx,
		};

	private:
		type		type_;

		pmd_io		pmd_;
		pmx_io		pmx_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		mmd_io() : type_(type::none) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~mmd_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	タイプを取得
			@return タイプ
		*/
		//-----------------------------------------------------------------//
		type get_type() const { return type_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	バージョンを取得
			@return バージョン
		*/
		//-----------------------------------------------------------------//
		float get_version() const {
			if(type_ == type::pmd) return pmd_.get_version();
			else if(type_ == type::pmx) return pmx_.get_version();
			return 0.0f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	モデル名を取得
			@param[in]	en	英語表記の場合「true」
			@return モデル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_model_name(bool en = false) const {
			if(type_ == type::pmd) return pmd_.get_model_name();
			else if(type_ == type::pmx) {
				if(en) return pmx_.get_model_info().name_en;
				else return pmx_.get_model_info().name;
			}
			static std::string dummy;
			return dummy;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コメントを取得
			@param[in]	en	英語表記の場合「true」
			@return コメント
		*/
		//-----------------------------------------------------------------//
		const std::string& get_comment(bool en = false) const {
			if(type_ == type::pmd) return pmd_.get_comment();
			else if(type_ == type::pmx) {
				if(en) return pmx_.get_model_info().comment_en;
				else return pmx_.get_model_info().comment;
			}
			static std::string dummy;
			return dummy;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PMD ファイル情報の取得
			@param[in]	info	PMD ファイル情報
		*/
		//-----------------------------------------------------------------//
		void get_info(std::string& info) {
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルが有効か検査
			@return 有効なら「true」
		*/
		//-----------------------------------------------------------------//
		type probe(utils::file_io& fio) {
			if(pmd_.probe(fio)) return type::pmd;
			else if(pmx_.probe(fio)) return type::pmx;
			return type::none;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	fio	ファイル入出力クラス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fio);


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& fn) {
			utils::file_io fio;
			if(!fio.open(fn, "rb")) {
				return false;
			}
			bool f = load(fio);
			fio.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブ
			@param[in]	fio	ファイル入出力クラス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fio);


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブ
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& fn) {
			utils::file_io fio;
			if(!fio.open(fn, "wb")) {
				return false;
			}
			bool f = save(fio);
			fio.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング・セットアップ
		*/
		//-----------------------------------------------------------------//
		void render_setup();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render_surface();


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンのフルパスを生成
			@param[in]	index	ボーンのインデックス
			@param[out]	path	フル・パスを受け取る参照	
		*/
		//-----------------------------------------------------------------//
		bool create_bone_path(uint32_t index, std::string& path);


		//-----------------------------------------------------------------//
		/*!
			@brief	PMD の参照
			@return pmd_io
		*/
		//-----------------------------------------------------------------//
		pmd_io& at_pmd() { return pmd_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	PMX の参照
			@return pmx_io
		*/
		//-----------------------------------------------------------------//
		pmx_io& at_pmx() { return pmx_; }
	};
}
