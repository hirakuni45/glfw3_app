#pragma once
//=====================================================================//
/*! @file
	@brief  Logic クラス @n
			※最大３２チャネルのロジックレベル操作クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <random>
#include <boost/format.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/optional.hpp>
#include "utils/file_io.hpp"

namespace tools {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ロジック・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class logic {

		std::vector<uint32_t>	level_;

		std::mt19937	noise_;

		// 10進変換
		typedef boost::optional<int32_t> decimal;
		decimal get_decimal_(const std::string& s)
		{
			try {
				return decimal(std::stoi(s));
			} catch(const std::invalid_argument& er) {
				return decimal();
			}
		}

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		logic() { }


		//-------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-------------------------------------------------------------//
		~logic() { }


		//-------------------------------------------------------------//
		/*!
			@brief  波形ストレージを生成
			@param[in]	length	長さ
		*/
		//-------------------------------------------------------------//
		void create(uint32_t length)
		{
			level_.resize(length);
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ストレージをクリア
		*/
		//-------------------------------------------------------------//
		void clear() { level_.clear(); }


		//-------------------------------------------------------------//
		/*!
			@brief  サイズの取得
			@return サイズ
		*/
		//-------------------------------------------------------------//
		uint32_t size() const { return level_.size(); }


		//-------------------------------------------------------------//
		/*!
			@brief  ロジック・レベルの取得
			@param[in]	ch		チャネル（０～３１）
			@param[in]	wpos	波形位置
			@return レベル
		*/
		//-------------------------------------------------------------//
		bool get_logic(uint32_t ch, uint32_t wpos) const
		{
			if(wpos >= level_.size()) return 0;  // 範囲外は「０」

			return level_[wpos] & (1 << ch);
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ロジック・レベル設定
			@param[in]	ch	チャネル（０～３１）
			@param[in]	wpos	波形位置
			@param[in]	value	値
		*/
		//-------------------------------------------------------------//
		void set_logic(uint32_t ch, uint32_t wpos, bool value = true)
		{
			if(wpos >= level_.size()) return;

			if(value) level_[wpos] |= (1 << ch);
			else level_[wpos] &= ~(1 << ch);
		}


		//-------------------------------------------------------------//
		/*!
			@brief  １のビットを数える
			@param[in]	ch		チャネル（０～３１）
			@return 数
		*/
		//-------------------------------------------------------------//
		uint32_t count1(uint32_t ch) const {
			uint32_t n = 0;
			for(uint32_t i = 0; i < size(); ++i) {
				if(get_logic(ch, i)) ++n;
			}
			return n;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ロジック・レベルの反転
			@param[in]	ch		チャネル（０～３１）
			@param[in]	wpos	波形位置
			@return 反転後のレベル
		*/
		//-------------------------------------------------------------//
		bool flip_logic(uint32_t ch, uint32_t wpos) 
		{
			if(wpos >= level_.size()) return 0;  // 範囲外は「０」

			level_[wpos] ^= (1 << ch);
			return level_[wpos] & (1 << ch);
		}


		//-------------------------------------------------------------//
		/*!
			@brief  クロック信号の生成
			@param[in]	ch	チャネル（０～３１）
			@param[in]	org	開始位置
			@param[in]	len	長さ（０の場合、最大サイズ）
			@param[in]	lc	"0" カウント
			@param[in]	hc	"1" カウント
			@param[in]	inv	反転の場合「true」
		*/
		//-------------------------------------------------------------//
		void build_clock(uint32_t ch, uint32_t org = 0, uint32_t len = 0, uint32_t lc = 1, uint32_t hc = 1, bool inv = false)
		{
			if(len == 0) len = size() - org;

			uint32_t lim = lc + hc;
			uint32_t cnt = 0;
			for(uint32_t i = org; i < (org + len); ++i) {
				bool lvl = (cnt % lim) < lc ? 0 : 1;
				if(inv) lvl = !lvl;
				set_logic(ch, i, lvl);
				++cnt;
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ノイズ生成シード設定
			@param[in]	seed	シード
		*/
		//-------------------------------------------------------------//
		void set_noise_seed(uint32_t seed)
		{
			noise_.seed(seed);
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ノイズの生成
			@param[in]	ch	チャネル（０～３１）
			@param[in]	org	開始位置
			@param[in]	len	長さ（０の場合、最大サイズ）
		*/
		//-------------------------------------------------------------//
		void build_noise(uint32_t ch, uint32_t org = 0, uint32_t len = 0)
		{
			if(len == 0) len = size() - org;
			for(uint32_t i = org; i < (org + len); ++i) {
				set_logic(ch, i, noise_() & 1);
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  埋める
			@param[in]	ch	チャネル（０～３１）
			@param[in]	lvl	値
			@param[in]	org	開始位置
			@param[in]	len	長さ（０の場合、最大サイズ）
		*/
		//-------------------------------------------------------------//
		void fill(uint32_t ch, bool lvl, uint32_t org, uint32_t len = 0)
		{
			if(len == 0) len = size() - org;
			for(uint32_t i = org; i < (org + len); ++i) {
				set_logic(ch, i, lvl);
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  反転
			@param[in]	ch	チャネル（０～３１）
			@param[in]	org	開始位置
			@param[in]	len	長さ（０の場合、最大サイズ）
		*/
		//-------------------------------------------------------------//
		void flip(uint32_t ch, uint32_t org, uint32_t len = 1)
		{
			if(len == 0) len = size() - org;

			for(uint32_t i = org; i < (org + len); ++i) {
				flip_logic(i, ch);
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  コピー
			@param[in]	ch	チャネル（０～３１）
			@param[in]	src	コピー元
			@param[in]	dst	コピー先
			@param[in]	len	長さ
		*/
		//-------------------------------------------------------------//
		void copy(uint32_t ch, uint32_t org, uint32_t len)
		{
			if(len == 0 || size() <= org) return;

			if(size() < (org + len)) { len = size() - org; }

			boost::dynamic_bitset<> bs(len);
			uint32_t idx = 0;
			for(uint32_t i = org; i < (org + len); ++i) {
				bs[idx] = get_logic(ch, i);
				++idx;
			}
			idx = 0;
			for(uint32_t i = org; i < (org + len); ++i) {
				set_logic(ch, i, bs[idx]);
				++idx;
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  チャネル間コピー
			@param[in]	src	ソース・チャネル（０～３１）
			@param[in]	dst	コピー先チャネル（０～３１）
			@param[in]	org	開始位置
			@param[in]	len	長さ（０の場合、最大サイズ）
		*/
		//-------------------------------------------------------------//
		void copy_chanel(uint32_t src, uint32_t dst, uint32_t org, uint32_t len = 0)
		{
			if(len == 0) len = size() - org;

			for(uint32_t i = org; i < (org + len); ++i) {
				auto lvl = get_logic(src, i);
				set_logic(dst, i, lvl);
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  チャネルのセーブ
			@param[in]	name	ファイル名
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool save(utils::file_io& fio, uint32_t ch)
		{
			fio.put((boost::format("# CH %d\n") % ch).str());
			fio.put("CH:" + std::to_string(ch) + "\n");
			uint32_t cn = 0;
			for(uint32_t i = 0; i < size(); ++i) {
				auto l = get_logic(ch, i);
				if(l) fio.put_char('1'); else fio.put_char('0');
				++cn;
				if(cn >= 64) {
					cn = 0;
					fio.put_char('\n');
				}
			}
			fio.put(";\n");
			return !fio.error();
		}


		//-------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	name	ファイル名
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool save(const std::string& name)
		{
//			std::cout << name << std::endl;

			if(level_.empty()) return false;

			utils::file_io fio;
			if(!fio.open(name, "wb")) {
				return false;
			}

			fio.put((boost::format("# Logic stream %d\n") % size()).str());
			fio.put("NUM:" + std::to_string(size()) + "\n\n");			

			for(uint32_t ch = 0; ch < 32; ++ch) {
				if(count1(ch) == 0) continue;
				if(!save(fio, ch)) {
					fio.close();
					return false;
				}
			}

			fio.close();

			return true;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	name	ファイル名
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool load(const std::string& name)
		{
			utils::file_io fio;
			if(!fio.open(name, "rb")) {
				return false;
			}

			clear();

			uint32_t lno = 1;
			enum class decode_func {
				none,
				num,
				ch,
				stream
			};
			decode_func func = decode_func::num;
			uint32_t cch = 0;
			uint32_t pos = 0;
			while(!fio.eof()) {
				auto s = fio.get_line();
				if(s.empty()) continue;
				if(s[0] == '#') continue;
				switch(func) {
				case decode_func::num:
					if(s.find("NUM:") != std::string::npos) {
						auto num = get_decimal_(&s[4]);
						if(num) {
							create(*num);
							func = decode_func::ch;
						} else {
							return false;
						}				
					} else {
						return false;
					}
					break;
				case decode_func::ch:
					if(s.find("CH:") != std::string::npos) {
						auto ch = get_decimal_(&s[3]);
						if(ch) {
							cch = *ch;
							pos = 0;
							func = decode_func::stream;
						} else {
							return false;
						}
					} else {
						return false;
					}
					break;
				case decode_func::stream:
					for(auto c : s) {
						if(c == '0') set_logic(cch, pos, false);
						else if(c == '1') set_logic(cch, pos, true);
						else if(c ==';') {
							func = decode_func::ch;
						} else {
							return false;
						}
						++pos;
					}
					break;
				default:
					break;
				}

				++lno;
			}

			fio.close();

			return true;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  波形の取得
			@param[in]	wpos	波形位置
			@return 波形
		*/
		//-------------------------------------------------------------//
		uint32_t get(uint32_t wpos) const {
			if(wpos >= level_.size()) return 0;
			return level_[wpos];
		}


		//-------------------------------------------------------------//
		/*!
			@brief  参照
			@param[in]	pos	参照位置
			@return 現物
		*/
		//-------------------------------------------------------------//
		uint32_t& at(uint32_t pos) { return level_[pos]; }
	};
}
