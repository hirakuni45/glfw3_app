#pragma once
//=====================================================================//
/*!	@file
	@brief	ディレクター・クラス（ヘッダー）@n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include "utils/i_scene.hpp"
#include <boost/foreach.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ディレクター・クラス
		@param[in]	CORE	アプリケーション・コア・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class CORE>
	struct director {

		typedef CORE	value_type;		///< コアの型

		typedef std::vector<i_scene*>   	scenes;
		typedef scenes::iterator			scenes_it;
		typedef scenes::const_iterator		scenes_cit;

	private:
		CORE		core_;

		scenes		install_scenes_;
		scenes		current_scenes_;
		scenes		erase_scenes_;

		i_scene*	current_scene_;

		uint32_t	frame_count_;

		bool find_scene_(scenes& ss, const i_scene* is) const {
			BOOST_FOREACH(i_scene* s, ss) {
				if(is == s) return true;
			}
			return false;
		}

		void destroy_() {
			BOOST_FOREACH(i_scene* is, install_scenes_) {
				delete is;
			}
			BOOST_FOREACH(i_scene* is, current_scenes_) {
				delete is;
			}
			scenes().swap(current_scenes_);
			scenes().swap(erase_scenes_);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		director() : current_scene_(0), frame_count_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~director() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	シーン追加テンプレート
		*/
		//-----------------------------------------------------------------//
		template <class T>
		T* install_scene() {
			T* is = new T(*this);
			install_scenes_.push_back(is);
			return is;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	シーンの消去
			@param[in]	is	消去対象のシーン@n
							※省略するとカレントのシーン
		*/
		//-----------------------------------------------------------------//
		void erase_scene(i_scene* is = 0) {
			if(is == 0) {
				if(current_scene_) {
					erase_scenes_.push_back(current_scene_);
				}
			} else {
				erase_scenes_.push_back(is);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フレーム数を取得
			@return フレーム数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_frame_count() const { return frame_count_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
			@return	シーンを終了する場合「true」
		*/
		//-----------------------------------------------------------------//
		bool render() {
			if(!install_scenes_.empty()) {
				BOOST_FOREACH(i_scene* is, install_scenes_) {
					is->initialize();
					current_scenes_.push_back(is);
				}
				install_scenes_.clear();
			}

			// アップデート処理
			BOOST_FOREACH(i_scene* is, current_scenes_) {
				current_scene_ = is;
				is->update();
			}

			// レンダリング処理
			BOOST_FOREACH(i_scene* is, current_scenes_) {
				current_scene_ = is;
				is->render();
			}

			if(!erase_scenes_.empty()) {
				scenes ns;
				BOOST_FOREACH(i_scene* is, erase_scenes_) {
					if(find_scene_(current_scenes_, is)) {
						is->destroy();
						delete is;
					} else {
						ns.push_back(is);
					}
				}
				current_scenes_ = ns;
				erase_scenes_.clear();
			}

			++frame_count_;

			if(current_scenes_.empty() && install_scenes_.empty()) {
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コアへの参照
			@return コア
		*/
		//-----------------------------------------------------------------//
		CORE& at() { return core_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	コアへの参照（RO）
			@return コア
		*/
		//-----------------------------------------------------------------//
		const CORE& get() const { return core_; }

	};

}

