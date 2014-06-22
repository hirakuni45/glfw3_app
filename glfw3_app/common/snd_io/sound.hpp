#pragma once
//=====================================================================//
/*!	@file
	@brief	効果音、BGM クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <string>
#include <pthread.h>
#include "audio_io.hpp"
#include "snd_files.hpp"
#include "tag.hpp"
#include "utils/string_utils.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	サウンド・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class sound {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ストリームの状態
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct stream_state {
			enum type {
				STALL,	///< 休止状態（再生リストが無効な状態）
				PLAY,	///< 再生中
				STOP,	///< 再生停止
				PAUSE,	///< 再生停止中
			};
		};

		struct sstream_t {
			audio_io*				audio_;
			audio_io::slot_handle	slot_;
			std::string				root_;
			std::string				file_;
			volatile bool			start_;
			volatile bool			finsh_;
			volatile bool			stop_;
			volatile bool			next_;
			volatile bool			replay_;
			volatile bool			prior_;
			volatile bool			pause_;
			volatile bool			seek_;
			volatile size_t			pos_;
			volatile size_t			len_;
			volatile time_t			time_;
			volatile time_t			etime_;
			volatile size_t			seek_pos_;
			volatile uint32_t		open_err_;
			volatile uint32_t		fph_cnt_;
			std::string				fph_;
			tag						tag_;

			sstream_t() : audio_(0), slot_(0),
				root_(), file_(),
				start_(false), finsh_(false), stop_(false),
				next_(false), replay_(false), prior_(false), pause_(false), seek_(false),
				pos_(0), len_(0), time_(0), etime_(0), seek_pos_(0),
				open_err_(0), fph_cnt_(0), fph_(), tag_()
			{ }
		};

	private:
		al::audio_io	audio_;

		al::snd_files	snd_files_;

		std::vector<al::audio_io::slot_handle>	slots_;

		struct se_t {
			al::audio_io::wave_handle	wave_;
			const i_audio*				wsrc_;
			se_t() : wave_(0), wsrc_(0) { }
			se_t(al::audio_io::wave_handle h, const i_audio* ia) : wave_(h), wsrc_(ia) { }
		};
		typedef std::vector<se_t>	se_ts;
		se_ts			se_ts_;

		int				slot_max_;

		sstream_t		sstream_t_;

		pthread_attr_t	attr_;
		pthread_t		pth_;
		volatile uint32_t	stream_fph_cnt_;
		std::string			stream_fph_;
		tag					stream_tag_;

		audio_io::slot_handle	stream_slot_;
		bool					stream_start_;

		bool request_sub_(int slot, al::audio_io::wave_handle wh, bool loop);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		sound() : slot_max_(0), stream_fph_cnt_(0),
			stream_slot_(0) {
			se_ts_.push_back(se_t());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~sound() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	slot_max	最大スロット数（同時発音数）
		 */
		//-----------------------------------------------------------------//
		void initialize(int slot_max);


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンド・ファイルか確認
			@param[in]	fn	ファイル名
			@return 「true」ならサウンドファイル
		 */
		//-----------------------------------------------------------------//
		bool probe(const std::string& fn) const { return snd_files_.probe(fn); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンドの拡張子を取得
			@return 拡張子
		 */
		//-----------------------------------------------------------------//
		const std::string& get_file_exts() const { return snd_files_.get_file_exts(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	fin	ファイル入力クラス
			@param[in]	ext	ファイルタイプ（拡張子）
			@return SE 発音ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		uint32_t load_se(utils::file_io& fin, const std::string& ext = "wav");


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	filename	ファイル名
			@return SE 発音ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		uint32_t load_se(const std::string& filename) {
			if(filename.empty()) {
				return 0;
			}

			utils::file_io fin;
			if(!fin.open(filename, "rb")) {
				return 0;
			}
			uint32_t h = load_se(fin, utils::get_file_ext(filename));
			fin.close();
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	効果音リクエスト
			@param[in]	slot	発音スロット（-1: auto）
			@param[in]	aif		波形インターフェース
			@param[in]	loop	ループなら「true」
			@return 波形ハンドルを返す。（「０」ならエラー）
		 */
		//-----------------------------------------------------------------//
		audio_io::wave_handle request_se(int slot, const i_audio* aif, bool loop);


		//-----------------------------------------------------------------//
		/*!
			@brief	効果音リクエスト
			@param[in]	slot	発音スロット（-1: auto）
			@param[in]	se_no	発音番号
			@param[in]	loop	ループ指定
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool request_se(int slot, uint32_t se_no, bool loop = false);


		//-----------------------------------------------------------------//
		/*!
			@brief	SE 一時停止
			@param[in]	slot	スロット番号
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool pause_se(int slot);


		//-----------------------------------------------------------------//
		/*!
			@brief	SE 停止
			@param[in]	slot	スロット番号
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool stop_se(int slot);


		//-----------------------------------------------------------------//
		/*!
			@brief	SE ステータス
			@param[in]	slot	スロット番号
			@return 発音中なら「true」
		 */
		//-----------------------------------------------------------------//
		bool status_se(int slot);


		//-----------------------------------------------------------------//
		/*!
			@brief	波形を廃棄する。
			@param[in]	se_no	SE ハンドル
		 */
		//-----------------------------------------------------------------//
		void destroy_se(uint32_t se_no);


		//-----------------------------------------------------------------//
		/*!
			@brief	ロードした全ての SE を廃棄する。
		 */
		//-----------------------------------------------------------------//
		void destroy_se_all();


		//-----------------------------------------------------------------//
		/*!
			@brief	ゲインを設定する。
			@param[in]	slot	発音スロット
			@param[in]	gain	ゲイン
		 */
		//-----------------------------------------------------------------//
		void set_gain(int slot, float gain);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム位置を取得
			@return ストリーム位置
		 */
		//-----------------------------------------------------------------//
		size_t get_position_stream() const { return sstream_t_.pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム長さを取得
			@return ストリーム長さ
		 */
		//-----------------------------------------------------------------//
		size_t get_length_stream() const { return sstream_t_.len_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームの現時間を取得
			@return ストリーム時間
		 */
		//-----------------------------------------------------------------//
		time_t get_time_stream() const { return sstream_t_.time_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームの終了時間を取得
			@return ストリーム終了時間
		 */
		//-----------------------------------------------------------------//
		time_t get_end_time_stream() const { return sstream_t_.etime_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームのゲインを設定
			@param[in]	gain	ゲイン（0.0 to 1.0）
		 */
		//-----------------------------------------------------------------//
		void set_gain_stream(float gain) { audio_.set_gain(stream_slot_, gain); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを再生する。
			@param[in]	root	ルートパス
			@param[in]	file	再生ファイル名（無ければディレクトリーが対象）
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool play_stream(const std::string& root, const std::string& file = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生中のファイルを取得
			@return ファイルパス
		 */
		//-----------------------------------------------------------------//
		const std::string& get_file_stream() const { return stream_fph_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生中のタグを取得
			@return タグ
		 */
		//-----------------------------------------------------------------//
		const tag& get_tag_stream() const { return stream_tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを一時停止
			@param[in]	stop	「false」でポーズ解除
		 */
		//-----------------------------------------------------------------//
		void pause_stream(bool stop = true) { sstream_t_.pause_ = stop; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームのステータス
			@return ステートを返す。
		 */
		//-----------------------------------------------------------------//
		stream_state::type get_state_stream() const {
			if(stream_start_) {
				if(sstream_t_.pause_) return stream_state::PAUSE;
				else if(sstream_t_.stop_) return stream_state::STOP;
				else return stream_state::PLAY;
			} else {
				return stream_state::STALL;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームの再生位置を変更
			@param[in]	pos	再生開始位置
			@return 成功なら「true」を返す。
		 */
		//-----------------------------------------------------------------//
		bool seek_stream(size_t pos);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生、次の曲
		 */
		//-----------------------------------------------------------------//
		void next_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再再生
		 */
		//-----------------------------------------------------------------//
		void replay_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生、前の曲
		 */
		//-----------------------------------------------------------------//
		void prior_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生を停止
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool stop_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンド関係、マイフレームサービス@n
					※毎フレームサービスする事。(1/60 間隔で呼び出される事を想定）
		 */
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy();

	};

}

