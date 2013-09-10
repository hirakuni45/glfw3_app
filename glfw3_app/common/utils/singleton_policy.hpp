#pragma once
//=====================================================================//
/*!	@file
	@brief	スコット・メイヤーズのシングルトン・デザイン・パターン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//

namespace utils {

	template <class T>
	struct singleton_policy {
		static T& get_instance() {
			static T instance_;
			return instance_;
		}
	};
}

