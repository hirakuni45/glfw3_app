#pragma once
//=====================================================================//
/*!	@file
	@brief	ドライブの情報を得る Windows 依存のクラス、@n
			他のプラットホームでは、「ドライブ」無しとなる。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ドライブ情報 クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct drive_info {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
		  @brief	ドライブの種類
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct drive {
			enum type {
				none,		///< 利用不可
				unknown,	///< 不明なタイプ
				no_root,	///< 「ルート」を持たない
				removable,	///< リムーバブル・ドライブ
				fixed,		///< 通常のドライブ
				remote,		///< リモートドライブ
				cdrom,		///< CD/DVD/BR ROM ドライブ
				ramdisk		///< RAM disk ドライブ
			};
		};

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
		  @brief	ドライブ情報構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct info_t {
			uint32_t	drive_;		///< ドライブ番号（A:0 to Z:25)
			drive::type	type_;		///< ドライブの種類
			info_t() : drive_(26), type_(drive::none) { }
		};

	private:
		uint32_t		drives_;
		std::vector<info_t>	infos_;

		void initialize_();

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
		  @brief	コンストラクター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		drive_info() : drives_(0) { initialize_(); }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
		  @brief	ドライブ数を得る
		  @return ドライブ数
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		uint32_t get_num() const { return infos_.size(); }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
		  @brief	ドライブ・タイプを取得する
		  @param[in]	drive	ドライブレターインデックス番号
		  @return ドライブのタイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		const info_t& get_info(uint32_t drive) const {
			if(drive < infos_.size()) {
				return infos_[drive];
			} else {
				static info_t info;
				return info;
			}
		}
	};
}
