#pragma once
//=====================================================================//
/*!	@file
	@brief	デバイスレコーダー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <bitset>
#include "vtx.hpp"
#include "utils/file_io.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	デバイスレコーダークラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class devrec {

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	記録パッド、構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct rec_pad {
			vtx::spos		m_mouse_pos;	///< マウスの位置
			unsigned char	m_mouse_btn;	///< マウスボタンの状態

			std::bitset<DEV_KEY_MAX>	m_key_level;

			inline bool operator == (const rec_pad& pad) const {
				if(m_mouse_pos == pad.m_mouse_pos && m_mouse_btn == pad.m_mouse_btn && m_key_level == pad.m_key_level) return true;
				else return false;
			}
			inline bool operator != (const rec_pad& pad) const {
				if(m_mouse_pos == pad.m_mouse_pos && m_mouse_btn == pad.m_mouse_btn && m_key_level == pad.m_key_level) return false;
				else return true;
			}

			bool write(utils::file_io& out) {
				out.write(&m_mouse_pos, sizeof(vtx::spos), 1);
				out.write(&m_mouse_btn, 1, 1);
				unsigned char tmp[DEV_KEY_MAX / 8];
				for(int i = 0; i < DEV_KEY_MAX / 8; ++i) tmp[i] = 0;
				for(int i = 0; i < DEV_KEY_MAX; ++i) {
					if(m_key_level.test(i)) tmp[i >> 3] |= 1 << (i & 7);
				}
				return out.write(tmp, 1, DEV_KEY_MAX / 8);
			}

			bool read(utils::file_io& inp) {
				inp.read(&m_mouse_pos, sizeof(vtx::spos), 1);
				inp.read(&m_mouse_btn, 1, 1);
				unsigned char tmp[DEV_KEY_MAX / 8];
				bool f = inp.read(tmp, 1, DEV_KEY_MAX / 8);
				for(int i = 0; i < DEV_KEY_MAX; ++i) {
					if(tmp[i >> 3] & (1 << (i & 7))) m_key_level.set(i);
					else m_key_level.reset(i);
				}
				return f;
			}
		};
		static const int rec_pad_size = sizeof(vtx::spos) + 1 + (DEV_KEY_MAX / 8);

	private:
		std::vector<rec_pad>	m_record;

		unsigned int			m_get_pos;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		devrec() : m_get_pos(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~devrec() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() { m_record.clear(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	記録
			@param[in]	pad	レコード・パッド
		*/
		//-----------------------------------------------------------------//
		void push_back(const rec_pad& pad) {
			m_record.push_back(pad);
//			if(m_record.empty()) {
//				m_record.push_back(pad);
//			} else {
//				if(m_record[m_record.size() - 1] != pad) {
//					m_record.push_back(pad);
//				}
//			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	取得
			@param[out]	pad	レコード・パッド
			@return レコードが有効な場合「true」
		*/
		//-----------------------------------------------------------------//
		bool get(rec_pad& pad) {
			if(m_get_pos < m_record.size()) {
				pad = m_record[m_get_pos];
				m_get_pos++;
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズを得る
			@return 配列のサイズ
		*/
		//-----------------------------------------------------------------//
		unsigned int size() const { return m_record.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	レコードのロード
			@param[in]	fin コンテキスト
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin);


		//-----------------------------------------------------------------//
		/*!
			@brief	レコードのセーブ
			@param[in]	fout コンテキスト
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout);


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() { m_record.clear(); }

	};

}

