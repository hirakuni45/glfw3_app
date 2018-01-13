#pragma once
//=====================================================================//
/*!	@file
	@brief	ドライブの情報を得る Windows 依存のクラス、@n
			他のプラットホームでは、「ドライブ」無しとなる。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#ifdef WIN32
#include <windows.h>
#endif
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

		void initialize_() {
			infos_.clear();
#ifdef WIN32
			drives_ = GetLogicalDrives();
#else
			drives_ = 0;
#endif
#ifdef WIN32
			uint32_t drvbits = drives_;

			uint32_t bits = 1;
			int idx = 0;
			while(drvbits) {
				drive::type t = drive::none;
				if(drvbits & bits) {
					char drvtxt[4];
					drvtxt[0] = 'A' + idx;
					drvtxt[1] = ':';
					drvtxt[2] = '\\';
					drvtxt[3] = 0;
					switch(GetDriveType(drvtxt)) {
					case DRIVE_UNKNOWN:
						t = drive::unknown;
						break;
					case DRIVE_NO_ROOT_DIR:
						t = drive::no_root;
						break;
					case DRIVE_REMOVABLE:
						t = drive::removable;
						break;
					case DRIVE_FIXED:
						t = drive::fixed;
						break;
					case DRIVE_REMOTE:
						t = drive::remote;
						break;
					case DRIVE_CDROM:
						t = drive::cdrom;
						break;
					case DRIVE_RAMDISK:
						t = drive::ramdisk;
						break;
					default:
						t = drive::none;
						break;
					}
					if(t != drive::none) {
						info_t info;
						info.drive_ = idx;
						info.type_ = t;
						infos_.push_back(info);
					}
					drvbits &= ~bits;
				}
				idx++;
				bits <<= 1;
			}
#endif
		}

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

#if 0
		void dump() {
			BOOST_FOREACH(const info_t& inf, infos_) {
				std::cout << boost::format("%c: (%d)\n")
					% ('A' + static_cast<int>(inf.drive_))
					% static_cast<int>(inf.type_);
			}
		}
#endif
	};
}
