#pragma once
//=====================================================================//
/*!	@file
	@brief	効果音、BGM クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <string>
#include <memory>
#include <vector>
#include <pthread.h>
#include "snd_io/audio_io.hpp"
#include "snd_io/snd_files.hpp"
#include "snd_io/tag.hpp"
#include "snd_io/pcm.hpp"
#include "utils/fifo.hpp"
#include "utils/string_utils.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	サウンド・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class sound {
	public:
		typedef std::vector<int16_t>	waves16;

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


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ストリーム・スレッド・コマンド／構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct request_t {
			enum class command {
				NONE,	///< 無効
				STOP,	///< 停止
				PLAY,	///< 再生
				PAUSE,	///< 一時停止
				REPLAY,	///< 再再生
				NEXT,	///< 次
				PRIOR,	///< 前
				SEEK,	///< シーク
			};
			command		command_;
			union {
				uint32_t	seek_pos_;
				bool		pause_state_;
			};
			explicit request_t(command cmd = command::NONE) : command_(cmd) { }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ストリーム構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct sstream_t {
			audio_io*				audio_io_;
			audio_io::slot_handle	slot_;
			std::string				root_;
			std::string				file_;

			utils::fifo<request_t, 64> request_;
			stream_state::type		state_;

			volatile bool			start_;
			volatile bool			finsh_;

			volatile size_t			pos_;
			volatile size_t			len_;
			volatile time_t			time_;
			volatile time_t			etime_;
			volatile uint32_t		open_err_;

			pthread_mutex_t			sync_;
			uint32_t				fph_cnt_;
			std::string				fph_;
			tag						tag_;

			sstream_t() : audio_io_(0), slot_(0),
						  root_(), file_(), state_(stream_state::STALL),
						  start_(false), finsh_(false),
						  pos_(0), len_(0), time_(0), etime_(0),
						  open_err_(0), fph_cnt_(0), fph_(), tag_() { }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	タグ情報構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct tag_info {
			utils::fifo<std::string, 2> path_;
			volatile bool		loop_;
			pthread_mutex_t		sync_;
			tag					tag_;
			tag_info() : loop_(true) { }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	queue 構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct queue_t {
			audio_io*				audio_io_;
			audio_io::slot_handle	slot_;

			utils::fifo<int16_t, 512 * 8>	wave_;
			utils::fifo<audio, 32>			audio_;

			pthread_mutex_t			sync_;

			volatile uint32_t		frame_;
			volatile bool			start_;
			volatile bool			exit_;
			volatile bool			finsh_;

			queue_t() : audio_io_(0), slot_(0),
						frame_(0), start_(false), exit_(false), finsh_(false) { }
		};


	private:
		al::audio_io	audio_io_;

		al::snd_files	snd_files_;

		typedef std::vector<al::audio_io::slot_handle>  slots;
		slots			slots_;

		typedef std::vector<al::audio_io::wave_handle>	ses;
		ses				ses_;

		int				slot_max_;

		sstream_t		sstream_t_;

		pthread_t			pth_;
		volatile uint32_t	stream_fph_cnt_;
		std::string			stream_fph_;
		tag					stream_tag_;

		audio_io::slot_handle	stream_slot_;
		bool					stream_start_;

		pthread_t				tag_pth_;
		tag_info				tag_info_;
		volatile uint32_t		tag_serial_;
		bool					tag_thread_;
		tag						tag_;

		queue_t			queue_t_;
		bool			queue_start_;
		pthread_t		queue_pth_;

		bool request_sub_(int slot, al::audio_io::wave_handle wh, bool loop);
		static void* stream_task_(void* entry);
		static void* queue_task_(void* entry);

		void queue_setup_()
		{
			if(!queue_start_) {

				queue_start_ = true;

				queue_t_.audio_io_ = &audio_io_;
				queue_t_.slot_ = stream_slot_;
				queue_t_.exit_ = false;

				pthread_mutex_init(&queue_t_.sync_, nullptr);
				pthread_create(&queue_pth_, nullptr, queue_task_, &queue_t_);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		sound() : slot_max_(0), stream_fph_cnt_(0),
				  stream_slot_(0), stream_start_(false),
				  tag_serial_(0), tag_thread_(false),
				  queue_start_(false)
		{
			ses_.push_back(0);
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
		uint32_t load(utils::file_io& fin, const std::string& ext = "wav");


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	filename	ファイル名
			@return SE 発音ハンドルを返す
		 */
		//-----------------------------------------------------------------//
		uint32_t load(const std::string& filename) {
			if(filename.empty()) {
				return 0;
			}

			utils::file_io fin;
			if(!fin.open(filename, "rb")) {
				return 0;
			}
			uint32_t h = load(fin, utils::get_file_ext(filename));
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
		audio_io::wave_handle request(int slot, const audio aif, bool loop) {
			audio_io::wave_handle wh = audio_io_.create_wave(aif);
			if(aif) {
				request_sub_(slot, wh, loop);
			}
			return wh;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	SE リクエスト
			@param[in]	slot	発音スロット
			@param[in]	se_no	発音番号
			@param[in]	loop	ループ指定
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool request(int slot, uint32_t se_no, bool loop = false) {
			bool f = false;
			if(se_no > 0 && se_no < ses_.size()) {
				f = request_sub_(slot, ses_[se_no], loop);
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	再生
			@param[in]	slot	スロット番号
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool play(int slot) {
			if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				audio_io_.play(sh);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	一時停止
			@param[in]	slot	スロット番号
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool pause(int slot) {
			if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				audio_io_.pause(sh);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	停止
			@param[in]	slot	スロット番号
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool stop(int slot) {
			if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				audio_io_.stop(sh);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステータス
			@param[in]	slot	スロット番号
			@return 発音中なら「true」
		 */
		//-----------------------------------------------------------------//
		bool status(int slot) {
			if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				return audio_io_.get_slot_status(sh);
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ゲインを設定する。
			@param[in]	slot	発音スロット
			@param[in]	gain	ゲイン
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_gain(int slot, float gain) {
			if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				audio_io_.set_gain(sh, gain);
			} else {
				return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	SE 波形を廃棄する。
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
			@brief	オーディオのゲインを設定
			@param[in]	gain	ゲイン（0.0 to 1.0）
		 */
		//-----------------------------------------------------------------//
		void set_gain_audio(float gain) { audio_io_.set_gain(stream_slot_, gain); }


		//-----------------------------------------------------------------//
		/*!
			@brief	再生（ポーズからの再生再開）
		 */
		//-----------------------------------------------------------------//
		void play_audio() {
			audio_io_.play(stream_slot_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	一時停止
		 */
		//-----------------------------------------------------------------//
		void pause_audio() {
			audio_io_.pause(stream_slot_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	停止
		 */
		//-----------------------------------------------------------------//
		void stop_audio() {
			audio_io_.stop(stream_slot_);
			audio_io_.purge_stream(stream_slot_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステータス
			@return 発音中なら「true」
		 */
		//-----------------------------------------------------------------//
		bool status_audio() {
			return audio_io_.get_slot_status(stream_slot_);
		}


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
		void set_gain_stream(float gain) { audio_io_.set_gain(stream_slot_, gain); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを再生する。
			@param[in]	root	ルートパス
			@param[in]	file	再生ファイル名（無ければディレクトリーが対象）
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool play_stream(const std::string& root, const std::string& file = "") {
			if(file.empty()) return false;

			stop_stream();

			stream_tag_.clear();

			stream_start_ = true;
			sstream_t_.audio_io_ = &audio_io_;
			sstream_t_.slot_ = stream_slot_;
			sstream_t_.root_ = root;
			sstream_t_.file_ = file;
			sstream_t_.start_ = false;

			sstream_t_.request_.clear();

			sstream_t_.finsh_ = false;

			sstream_t_.pos_ = 0;
			sstream_t_.len_ = 0;
			sstream_t_.time_  = 0;
			sstream_t_.etime_ = 0;
			sstream_t_.open_err_   = 0;

///			pthread_attr_init(&attr_);
///			pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_DETACHED);

			pthread_mutex_init(&sstream_t_.sync_, nullptr);
			pthread_create(&pth_, nullptr, stream_task_, &sstream_t_);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオをキューイング（ストリーム）
			@param[in]	aif	オーディオ・インターフェース
			@return 「queue」出来たら「true」
		 */
		//-----------------------------------------------------------------//
		bool queue_audio(audio& aif)
		{
			if(aif == nullptr) return false;

			queue_setup_();

			bool f = false;
			if(queue_t_.audio_.length() < (queue_t_.audio_.size() - 1)) {
				pthread_mutex_lock(&queue_t_.sync_);
				queue_t_.audio_.put(aif);
				pthread_mutex_unlock(&queue_t_.sync_);
				f = true;
			}

			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオをキューイング（ストリーム）
			@param[in]	waves	wave-block
			@return 「queue」出来たら「true」
		 */
		//-----------------------------------------------------------------//
		bool queue_audio(const waves16& waves)
		{
			if(waves.empty()) return false;

			queue_setup_();

//			if(queue_t_.wave_.length() == 0) {
//				std::cout << "no waves..." << std::endl;
//			}

			if((queue_t_.wave_.size() - queue_t_.wave_.length()) > waves.size()) {
				pthread_mutex_lock(&queue_t_.sync_);
				for(auto w : waves) {
					queue_t_.wave_.put(w);
				}
				pthread_mutex_unlock(&queue_t_.sync_);
				return true;
			} else {
//				std::cout << "full..." << std::endl;
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・キューイングの波形長さを取得
			@return 波形長さ
		 */
		//-----------------------------------------------------------------//
		uint32_t get_queue_audio_length() const {
			return queue_t_.wave_.length();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生中のファイルを取得
			@return ファイルパス
		 */
		//-----------------------------------------------------------------//
		const std::string& get_file_stream() const {
			return stream_fph_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生中のタグを取得
			@return タグ
		 */
		//-----------------------------------------------------------------//
		const tag& get_tag_stream() const {
			return stream_tag_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを一時停止
			@param[in]	state	「false」でポーズ解除
		 */
		//-----------------------------------------------------------------//
		void pause_stream(bool state = true) {
			if(stream_start_) {
				request_t r(request_t::command::PAUSE);
				r.pause_state_ = state;
				sstream_t_.request_.put(r);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームのステータス
			@return ステートを返す。
		 */
		//-----------------------------------------------------------------//
		stream_state::type get_state_stream() const { return sstream_t_.state_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームの再生位置を変更
			@param[in]	pos	再生開始位置
		 */
		//-----------------------------------------------------------------//
		void seek_stream(size_t pos) {
			if(stream_start_) {
				request_t r(request_t::command::SEEK);
				r.seek_pos_ = pos;
				sstream_t_.request_.put(r);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生、次の曲
		 */
		//-----------------------------------------------------------------//
		void next_stream() {
			if(stream_start_) {
				sstream_t_.request_.put(request_t(request_t::command::NEXT));
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再再生
		 */
		//-----------------------------------------------------------------//
		void replay_stream() {
			if(stream_start_) {
				sstream_t_.request_.put(request_t(request_t::command::REPLAY));
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生、前の曲
		 */
		//-----------------------------------------------------------------//
		void prior_stream() {
			if(stream_start_) {
				sstream_t_.request_.put(request_t(request_t::command::PRIOR));
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生を停止
		 */
		//-----------------------------------------------------------------//
		void stop_stream() {
			if(stream_start_) {
				sstream_t_.request_.put(request_t(request_t::command::STOP));
				pthread_join(pth_ , nullptr);
				pthread_mutex_destroy(&sstream_t_.sync_);
				stream_start_ = false;
				sstream_t_.state_ = stream_state::STOP;
				sstream_t_.time_ = 0;
				sstream_t_.etime_ = 0;
				stream_fph_.clear();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンド関係、マイフレームサービス@n
					※毎フレームサービスする事。(1/60 間隔で呼び出される事を想定）
		 */
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	タグ情報取得の状態
			@return 取得中なら「true」、空きなら「false」
		 */
		//-----------------------------------------------------------------//
		bool state_tag_info() const { return (tag_info_.path_.length() != 0); }


		//-----------------------------------------------------------------//
		/*!
			@brief	タグ情報取得をリクエスト
			@param[in]	fpath	ファイル・パス
			@return 「false」なら失敗
		 */
		//-----------------------------------------------------------------//
		bool request_tag_info(const std::string& fpath);


		//-----------------------------------------------------------------//
		/*!
			@brief	タグ情報取得
			@return タグ情報
		 */
		//-----------------------------------------------------------------//
		const tag& get_tag_info() const { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy();

	};

}
