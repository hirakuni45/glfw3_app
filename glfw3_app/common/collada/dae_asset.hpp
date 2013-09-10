#pragma once
//=====================================================================//
/*!	@file
	@brief	collada asset のパーサー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "utils/string_utils.hpp"

#include "utils/verbose.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada asset のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_asset {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	up_axis タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct up_axis {
			enum type {
				none,	///< 未定
				X_UP,	///< X 軸上
				Y_UP,	///< Y 軸上
				Z_UP,	///< Z 軸上
				X_DOWN,	///< X 軸下
				Y_DOWN,	///< Y 軸下
				Z_DOWN,	///< Z 軸下
			};
		};

	private:
		std::string		created_;
		std::string		modified_;
		float			unit_meter_;
		up_axis::type	up_axis_;

		int		error_;

	public:

		dae_asset() : unit_meter_(0.0f), up_axis_(up_axis::none), error_(0) { }

		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);

		//-----------------------------------------------------------------//
		/*!
			@brief	'modified' 時間文字列を取得
			@return 時間文字列
		*/
		//-----------------------------------------------------------------//
		const std::string& get_modified() const { return modified_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	'created' 時間文字列を取得
			@return 時間文字列
		*/
		//-----------------------------------------------------------------//
		const std::string& get_created() const { return created_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	単位メートルの取得
			@return 単位メートル
		*/
		//-----------------------------------------------------------------//
		float get_unit_meter() const { return unit_meter_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	上向き方向を取得
			@return 上向き方向タイプ
		*/
		//-----------------------------------------------------------------//
		up_axis::type get_up_axis() const { return up_axis_; }

	};

}
