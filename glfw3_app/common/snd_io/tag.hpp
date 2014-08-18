#pragma once
//=====================================================================//
/*!	@file
	@brief	音楽タグ
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include "utils/array.hpp"
#include "img_io/img_files.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	タグ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tag {
		uint32_t		serial_;		///< タグのＩＤ

		std::string		title_;			///< タイトル
		std::string		artist_;		///< アーティスト
		std::string		writer_;		///< ライター
		std::string		album_;			///< アルバム
		std::string		track_;			///< トラック
		std::string		total_tracks_;	///< トータル・トラック
		std::string		disc_;			///< ディスク
		std::string		total_discs_;	///< トータル・ディスク
		std::string		date_;			///< 日付

		std::string		image_mime_;	///< 画像形式
		char			image_cover_;	///< 画像カバー
		std::string		image_dscrp_;	///< 画像ディスクリプター

		utils::shared_array_u8	image_;

		tag() : serial_(0), image_cover_(0) { }

		void clear() {
			title_.clear();
			artist_.clear();
			writer_.clear();
			album_.clear();
			track_.clear();
			total_tracks_.clear();
			disc_.clear();
			total_discs_.clear();
			date_.clear();
			image_ = nullptr;
		}


		void reset() {
			serial_ = 0;
			clear();
		}


		void update() { ++serial_; }


		img::shared_img decode_image() const {
			if(image_) {
				utils::file_io fmem;
				fmem.open(image_->get(), image_->size());
				img::img_files imgs;
				if(imgs.load(fmem)) {
					return imgs.get_image();
				}
			}
			return img::shared_img();
		}

	};
}
