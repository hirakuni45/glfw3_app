//=====================================================================//
/*!	@file
	@brief	効果音、BGM クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "sound.hpp"
#include <ctime>
#include <unistd.h>
#include <boost/foreach.hpp>
#include "pcm.hpp"
#include "utils/file_info.hpp"

namespace al {

	using namespace al;

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
		static const int stream_buff_size = 2048;

		utils::file_infos fis;
		utils::filter_file_infos(src, sdf.get_file_exts(), fis);

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
			sst.state_ = sound::stream_state::PLAY;
			bool first_pause = true;
			while(pos < ainfo.samples) {
				if(sst.request_.length()) {
					const sound::request& r = sst.request_.get();
					if(r.command_ == sound::request::command::NEXT) {
						++i;
						cmdin = true;
						break;
					} else if(r.command_ == sound::request::command::PRIOR) {
						if(i) --i;
						cmdin = true;
						break;
					} else if(r.command_ == sound::request::command::REPLAY) {
						cmdin = true;
						break;
					} else if(r.command_ == sound::request::command::STOP) {
						sst.state_ = sound::stream_state::STOP;
						exit = true;
						cmdin = true;
						break;
					} else if(r.command_ == sound::request::command::PAUSE) {
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
					} else if(r.command_ == sound::request::command::SEEK) {
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
				usleep(8000);	// 8ms くらいの時間待ち

			}
			sst.audio_io_->purge_stream(sst.slot_);
			sst.pos_ = sst.len_;
			sdf.close_stream();
			fin.close();

			if(!cmdin) ++i;
		}
		sst.state_ = sound::stream_state::STALL;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	音楽再生を行うタスク
		@param[in]	entry	sstream_t 構造体のポインター
		@return 常に NULL を返す。
	 */
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
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


	// タグ情報収集タスク
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
					t.tag_.title_ = utils::get_file_name(path);
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


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	波形再生（sub）
		@param[in]	slot	スロット
		@param[in]	wh		波形ハンドル
		@param[in]	loop	ループ再生の場合「true」
		@return エラーが無ければ「true」
	 */
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	bool sound::request_sub_(int slot, audio_io::wave_handle wh, bool loop)
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
			audio_io_.stop(sh);
			audio_io_.set_wave(sh, wh);	// 波形を登録して再生
			audio_io_.set_loop(sh, loop);
			audio_io_.play(sh);
			f = true;
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		slot_max	最大スロット数（同時発音数）
	 */
	//-----------------------------------------------------------------//
	void sound::initialize(int slot_max)
	{
		audio_io_.initialize();

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
		@brief	ロード
		@param[in]	fin	ファイル入力クラス
		@param[in]	ext	ファイルタイプ（拡張子）
		@return SE 発音番号を返す
	 */
	//-----------------------------------------------------------------//
	uint32_t sound::load_se(utils::file_io& fin, const std::string& ext)
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
		@brief	効果音リクエスト
		@param[in]	slot	発音スロット（-1: auto）
		@param[in]	aif		波形インターフェース
		@param[in]	loop	ループ指定
		@return 波形ハンドルを返す。（「０」ならエラー）
	 */
	//-----------------------------------------------------------------//
	audio_io::wave_handle sound::request_se(int slot, const audio aif, bool loop)
	{
		audio_io::wave_handle wh = audio_io_.create_wave(aif);
		if(aif) {
			request_sub_(slot, wh, loop);
		}
		return wh;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	効果音リクエスト
		@param[in]	slot	発音スロット（-1: auto）
		@param[in]	se_no	発音番号
		@param[in]	loop	ループ指定
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::request_se(int slot, uint32_t se_no, bool loop)
	{
		bool f = false;
		if(se_no > 0 && se_no < ses_.size()) {
			f = request_sub_(slot, ses_[se_no], loop);
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	SE 一時停止
		@param[in]	slot	スロット番号
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::pause_se(int slot)
	{
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
		@brief	SE 停止
		@param[in]	slot	スロット番号
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::stop_se(int slot)
	{
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
		@brief	SE ステータス
		@param[in]	slot	スロット番号
		@return 発音中なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::status_se(int slot)
	{
		if(static_cast<size_t>(slot) < slots_.size()) {
			audio_io::slot_handle sh = slots_[slot];
			return audio_io_.get_slot_status(sh);
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロードした全ての SE を廃棄する。
	 */
	//-----------------------------------------------------------------//
	void sound::destroy_se_all()
	{
		BOOST_FOREACH(ses::value_type h, ses_) {
			if(h) {
				audio_io_.destroy_wave(h);
			}
		}
		ses_.clear();
		ses_.push_back(0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ゲインを設定する。
		@param[in]	slot	発音スロット
		@param[in]	gain	ゲイン
	 */
	//-----------------------------------------------------------------//
	void sound::set_gain(int slot, float gain)
	{
		if(slot >= 0 && slot < static_cast<int>(slots_.size())) {
			audio_io::slot_handle sh = slots_[slot];
			audio_io_.set_gain(sh, gain);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームを再生する。
		@param[in]	root	ルートパス
		@param[in]	file	再生ファイル名（無ければディレクトリーが対象）
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::play_stream(const std::string& root, const std::string& file)
	{
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

///		pthread_attr_init(&attr_);
///		pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_DETACHED);

		pthread_mutex_init(&sstream_t_.sync_, nullptr);
		pthread_create(&pth_, nullptr, stream_task_, &sstream_t_);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生を停止
	 */
	//-----------------------------------------------------------------//
	void sound::stop_stream()
	{
		if(stream_start_) {
			sstream_t_.request_.put(request(request::command::STOP));
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
				※毎フレームサービスする事。
	 */
	//-----------------------------------------------------------------//
	void sound::service()
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
		@brief	タグ情報取得をリクエスト
		@param[in]	fpath	ファイル・パス
		@return 「false」なら失敗
	*/
	//-----------------------------------------------------------------//
	bool sound::request_tag_info(const std::string& fpath)
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
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void sound::destroy()
	{
		if(tag_thread_) {
			tag_info_.loop_ = false;
			pthread_join(tag_pth_ , nullptr);
			pthread_mutex_destroy(&tag_info_.sync_);
			tag_thread_ = false;
		}

		stop_stream();

		destroy_se_all();

		for(size_t i = 0; i < slots_.size(); ++i) {
			audio_io_.destroy_slot(slots_[i]);
		}
		slots_.clear();
	}

};
