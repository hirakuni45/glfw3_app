#pragma once
//=========================================================================//
/*!	@file
	@brief	効果音、BGM クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <vector>
#include <string>
#include <memory>
#include <vector>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include "snd_io/audio_io.hpp"
#include "snd_io/snd_files.hpp"
#include "snd_io/tag.hpp"
#include "snd_io/pcm.hpp"
#include "utils/fifo.hpp"
#include "utils/string_utils.hpp"
#include "utils/file_info.hpp"

#include "utils/format.hpp"

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
			::sound::tag_t			tag_;

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
			::sound::tag_t		tag_;
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
		::sound::tag_t		stream_tag_;

		audio_io::slot_handle	stream_slot_;
		bool					stream_start_;

		pthread_t				tag_pth_;
		tag_info				tag_info_;
		volatile uint32_t		tag_serial_;
		bool					tag_thread_;
		::sound::tag_t			tag_;

		queue_t			queue_t_;
		bool			queue_start_;
		pthread_t		queue_pth_;

		static void silent_(sound::sstream_t& sst, const audio_info& ainfo, uint32_t len)
		{
			audio_io::wave_handle h = sst.audio_io_->status_stream(sst.slot_);
			if(h) {
				audio aif;
				if(ainfo.chanels == 1) {
					aif = audio(new audio_mno16);
				} else if(ainfo.chanels == 2) {
					aif = audio(new audio_sto16);
				}
				aif->create(ainfo.frequency, len);
				aif->zero();
				sst.audio_io_->queue_stream(sst.slot_, h, aif);
			}
		}


		static void play_task_(sound::sstream_t& sst, snd_files& sdf,
			std::string& root, utils::file_infos& src, const std::string& file)
		{
			static constexpr int stream_buff_size = 2048;

			auto fis = utils::filter_file_infos(src, sdf.get_file_exts());

			uint32_t i = 0;
			if(!file.empty()) {
				for(uint32_t n = 0; n < fis.size(); ++n) {			
					if(file == fis[n].get_name()) {
						i = n;
						break;
					}
				}
			}

			bool exit = false;
			bool pause = false;
			while(!exit && i < fis.size()) {
				const utils::file_info& fi = fis[i];

				if(fi.get_name() == "." || fi.get_name() == "..") {
					++i;
					continue;
				}

				std::string fn = root;
				fn += '/';
				fn += fi.get_name();

				if(fi.is_directory()) {
					utils::file_infos tmp;				
					create_file_list(fn, tmp);
					play_task_(sst, sdf, fn, tmp, "");
					++i;
					continue;
				}

				pthread_mutex_lock(&sst.sync_);
				sst.fph_ = fn;
				++sst.fph_cnt_;
				pthread_mutex_unlock(&sst.sync_);

				utils::file_io fin;
				audio_info ainfo;
				if(fin.open(fn, "rb")) {
					if(!sdf.open_stream(fin, stream_buff_size, ainfo)) {
						sdf.close_stream();
						fin.close();
						++sst.open_err_;
						++i;
						continue;
					}
				} else {
					fin.close();
					++sst.open_err_;
					++i;
					continue;
				}

				pthread_mutex_lock(&sst.sync_);
				sst.tag_ = sdf.get_tag();
				pthread_mutex_unlock(&sst.sync_);

				sst.len_ = ainfo.samples;
				time_t t;
				ainfo.sample_to_time(ainfo.samples, t);
				sst.etime_ = t;
				size_t pos = 0;
				bool cmdin = false;
				bool purge = false;
				sst.state_ = sound::stream_state::PLAY;
				bool first_pause = true;
				while(pos < ainfo.samples) {
					if(sst.request_.length()) {
						const sound::request_t& r = sst.request_.get();
						if(r.command_ == sound::request_t::command::NEXT) {
							++i;
							cmdin = true;
							purge = true;
							break;
						} else if(r.command_ == sound::request_t::command::PRIOR) {
							if(i) --i;
							cmdin = true;
							purge = true;
							break;
						} else if(r.command_ == sound::request_t::command::REPLAY) {
							cmdin = true;
							purge = true;
							break;
						} else if(r.command_ == sound::request_t::command::STOP) {
							sst.state_ = sound::stream_state::STOP;
							exit = true;
							cmdin = true;
							purge = true;
							break;
						} else if(r.command_ == sound::request_t::command::PAUSE) {
							if(pause != r.pause_state_) {
								pause = r.pause_state_;
								if(pause) {
									sst.state_ = sound::stream_state::PAUSE;
									if(first_pause) {
										first_pause = false;
										silent_(sst, ainfo, stream_buff_size);
									}
								} else {
									sst.state_ = sound::stream_state::PLAY;
								}
								sst.audio_io_->pause_stream(sst.slot_, pause);
							}
						} else if(r.command_ == sound::request_t::command::SEEK) {
							pos = r.seek_pos_;
						}
					}

					sst.pos_ = pos;
					time_t t;
					ainfo.sample_to_time(pos, t);
					sst.time_ = t;

					if(!pause) {

						audio_io::wave_handle h = sst.audio_io_->status_stream(sst.slot_);
						if(h) {
							uint32_t len = sdf.read_stream(fin, pos, stream_buff_size);
							if(len) {
								pos += len;
								sst.audio_io_->queue_stream(sst.slot_, h, sdf.get_stream());
							} else {
								pos = ainfo.samples;
							}
						}
					}
					// デコードのパフォーマンス的には１６ミリ秒程度のインターバルで
					// 間に合うよう調整されているので、余裕を考えて、その２倍を設定する。
#ifdef __APPLE__
					usleep(5000);	// 5ms くらいの時間待ち
#else
					usleep(8000);	// 8ms くらいの時間待ち
#endif
				}
				if(purge) sst.audio_io_->purge_stream(sst.slot_);
				else {
					sst.audio_io_->sync_stream(sst.slot_);
					sst.audio_io_->purge_stream(sst.slot_);
				}
				sst.pos_ = sst.len_;
				sdf.close_stream();
				fin.close();

				if(!cmdin) ++i;
			}
			sst.state_ = sound::stream_state::STALL;
		}


	static void* stream_task_(void* entry)
		{
			sound::sstream_t& sst = *(static_cast<sound::sstream_t*>(entry));

			snd_files sdf;

			sst.start_ = true;

			utils::file_infos fis;
			create_file_list(sst.root_, fis);
			play_task_(sst, sdf, sst.root_, fis, sst.file_);

			sst.start_ = false;
			sst.finsh_ = true;

			return nullptr;
		}

		static void* queue_task_(void* entry)
		{
			sound::queue_t& qt = *(static_cast<sound::queue_t*>(entry));

			qt.start_ = true;

			static const uint32_t pcm_size = 2048;
			audio aif;
			aif = al::create_audio(al::audio_format::PCM16_STEREO);
			aif->create(44100, pcm_size);

			while(!qt.exit_) {
				if(qt.wave_.length() >= pcm_size) {

					audio_io::wave_handle h = qt.audio_io_->status_stream(qt.slot_);
					if(h) {
						pthread_mutex_lock(&qt.sync_);
						for(uint32_t i = 0; i < pcm_size; ++i) {
							auto a = qt.wave_.get();
							al::pcm16_s w(a, a);
							aif->put(i, w);
						}
						pthread_mutex_unlock(&qt.sync_);

						qt.audio_io_->set_loop(h, false);
						qt.audio_io_->queue_stream(qt.slot_, h, aif);
					}
					++qt.frame_;
				}
				if(qt.audio_.length() > 0) {
					audio_io::wave_handle h = qt.audio_io_->status_stream(qt.slot_);
					if(h) {
						audio aif;
						pthread_mutex_lock(&qt.sync_);
						aif = qt.audio_.get();
						pthread_mutex_unlock(&qt.sync_);

						qt.audio_io_->set_loop(h, false);
						qt.audio_io_->queue_stream(qt.slot_, h, aif);
					}
					++qt.frame_;
				}
#ifdef __APPLE__
				usleep(8000);	// 8ms くらいの時間待ち
#else
				usleep(10000);	// 10ms くらいの時間待ち
#endif
			}

			qt.start_ = false;
			qt.finsh_ = true;

			return nullptr;
		}

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

		static void* tag_info_task_(void* entry)
		{
			sound::tag_info& t = *(static_cast<sound::tag_info*>(entry));
			snd_files sdf;

			while(t.loop_) {
				if(t.path_.length()) {
					pthread_mutex_lock(&t.sync_);
					std::string path = t.path_.top();
					pthread_mutex_unlock(&t.sync_);
					al::audio_info ai;
					bool f = sdf.info(path, ai, i_snd_io::info_state::none);
					pthread_mutex_lock(&t.sync_);
					t.tag_ = sdf.get_tag();
					if(!f) {
						uint32_t n = t.tag_.serial_;
						t.tag_.clear();
						t.tag_.at_title() = utils::get_file_name(path);
						t.tag_.serial_ = n + 1;
					}
					if(t.path_.length()) {
						t.path_.get();
					}
					// if(!f) std::cout << "Error: '" << path << "'" << std::endl;
					pthread_mutex_unlock(&t.sync_);
				} else {
					usleep(20000);	// 20ms
				}
			}
			// 抜ける前に少し待つ〜
			usleep(1000);	// 1ms

			return nullptr;
		}

		bool request_sub_(int slot, audio_io::wave_handle wh, bool loop)
		{
			bool f = false;
			if(slot < 0) {
				for(size_t i = 0; i < slots_.size(); ++i) {
					audio_io::slot_handle sh = slots_[i];
					if(!audio_io_.get_slot_status(sh)) {
						audio_io_.set_wave(sh, wh);
						audio_io_.set_loop(sh, loop);
						audio_io_.play(sh);
						f = true;
						break;
					}
				}
			} else if(static_cast<size_t>(slot) < slots_.size()) {
				audio_io::slot_handle sh = slots_[slot];
				if(audio_io_.get_slot_init(sh)) {  // 波形を登録して再生
					audio_io_.set_wave(sh, wh);
				} else {
					audio_io_.stop(sh);
					audio_io_.rewind(sh);
				}
				audio_io_.set_loop(sh, loop);
				audio_io_.play(sh);
				f = true;
			}
			return f;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		sound() noexcept : slot_max_(0), stream_fph_cnt_(0),
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
			@brief	audio_io の参照
		*/
		//-----------------------------------------------------------------//
		al::audio_io& at_audio_io() noexcept { return audio_io_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	slot_max	最大スロット数（同時発音数）
			@param[in]	inp_rate	オーディオ入力サンプリングレート
			@param[in]	inp_buff	オーディオ入力バッファ数
		 */
		//-----------------------------------------------------------------//
		void initialize(int slot_max, uint32_t inp_rate = 0, uint32_t inp_buff = 0) noexcept
		{
			audio_io_.initialize(inp_rate, inp_buff);

			slot_max_ = slot_max;
			for(int i = 0; i < slot_max; ++i) {
				slots_.push_back(audio_io_.create_slot(0));
			}

			stream_slot_ = audio_io_.create_slot(0);

			destroy_se_all();

			stream_start_ = false;
			sstream_t_.start_ = false;
			sstream_t_.finsh_ = false;

			sstream_t_.pos_ = 0;
			sstream_t_.len_ = 0;
			sstream_t_.time_ = 0;
			sstream_t_.etime_ = 0;
			sstream_t_.open_err_ = 0;
		}


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
		uint32_t load(utils::file_io& fin, const std::string& ext = "wav")
		{
			if(!snd_files_.load(fin, ext)) {
				fin.close();
				return 0;
			}

			uint32_t no = 0;
			const audio aif = snd_files_.get_audio();
			if(aif) {
				al::audio_io::wave_handle wh = audio_io_.create_wave(aif);
				if(wh) {
					no = ses_.size();
					ses_.push_back(wh);
				}
			}
			return no;
		}


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
		void destroy_se_all()
		{
			for(ses::value_type h : ses_) {
				if(h) {
					audio_io_.destroy_wave(h);
				}
			}
			ses_.clear();
			ses_.push_back(0);
		}


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
			@param[in]	waves	wave-block（モノラル）
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
		const auto& get_tag_stream() const {
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
			@brief	サウンド関係、マイフレームサービス @n
					※毎フレームサービスする事。(1/60 間隔で呼び出される事を想定）
		 */
		//-----------------------------------------------------------------//
		void service()
		{
			// 曲の終了を感知して、フラグを下げる～
			if(stream_start_) {
				pthread_mutex_lock(&sstream_t_.sync_);
				if(stream_fph_cnt_ != sstream_t_.fph_cnt_) {
					stream_fph_ = sstream_t_.fph_;
					stream_fph_cnt_ = sstream_t_.fph_cnt_;
				}
				if(stream_tag_.serial_ != sstream_t_.tag_.serial_) {
					stream_tag_ = sstream_t_.tag_;
				}
				pthread_mutex_unlock(&sstream_t_.sync_);
				if(sstream_t_.finsh_) {
					pthread_detach(pth_);
					pthread_mutex_destroy(&sstream_t_.sync_);
					stream_start_ = false;
				}
			}

			volatile uint32_t n = tag_info_.tag_.serial_;
			if(n != tag_serial_) {
				tag_serial_ = n;
				pthread_mutex_lock(&tag_info_.sync_);
				tag_ = tag_info_.tag_;
				pthread_mutex_unlock(&tag_info_.sync_);
			}
		}


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
		bool request_tag_info(const std::string& fpath)
		{
			if(tag_info_.path_.length()) return false;

			// スレッドを生成
			if(!tag_thread_) {
				pthread_mutex_init(&tag_info_.sync_, nullptr);
				pthread_create(&tag_pth_, nullptr, tag_info_task_, &tag_info_);
				tag_thread_ = true;
			}

			pthread_mutex_lock(&tag_info_.sync_);
			tag_info_.path_.put(fpath);
			pthread_mutex_unlock(&tag_info_.sync_);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	タグ情報取得
			@return タグ情報
		 */
		//-----------------------------------------------------------------//
		const auto& get_tag_info() const { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy()
		{
			if(queue_start_) {
				queue_t_.exit_ = true;
				pthread_join(queue_pth_ , nullptr);
				pthread_mutex_destroy(&queue_t_.sync_);
			}

			if(tag_thread_) {
				tag_info_.loop_ = false;
				pthread_join(tag_pth_ , nullptr);
				pthread_mutex_destroy(&tag_info_.sync_);
				tag_thread_ = false;
			}

			stop_stream();
			audio_io_.destroy_slot(stream_slot_);

			destroy_se_all();

			for(size_t i = 0; i < slots_.size(); ++i) {
				audio_io_.destroy_slot(slots_[i]);
			}
			slots_.clear();

			audio_io_.destroy();
		}
	};
}
