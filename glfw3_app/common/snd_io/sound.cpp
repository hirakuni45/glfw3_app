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

	static void play_task_(sound::sstream_t* sst, snd_files& sdf,
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

		while(i < fis.size()) {
			if(sst->stop_) break;

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

			sst->fph_ = fn;
			++sst->fph_cnt_;

			utils::file_io fin;
			audio_info	ainfo;
			if(fin.open(fn, "rb")) {
				if(!sdf.open_stream(fin, stream_buff_size, ainfo)) {
					sdf.close_stream();
					fin.close();
					++sst->open_err_;
					++i;
					continue;
				}
			} else {
				fin.close();
				++sst->open_err_;
				++i;
				continue;
			}

			sst->tag_ = sdf.get_tag();

			sst->len_ = ainfo.samples;
			time_t t;
			ainfo.sample_to_time(ainfo.samples, t);
			sst->etime_ = t;
			size_t pos = 0;
			volatile bool pause = sst->pause_;
			while(pos < ainfo.samples) {
				if(sst->stop_ || sst->next_ || sst->replay_ || sst->prior_) {
					break;
				}

				if(pause != sst->pause_) {
					pause = sst->pause_;
					sst->audio_->pause_stream(sst->slot_, pause);
				}
				sst->pos_ = pos;
				time_t t;
				ainfo.sample_to_time(pos, t);
				sst->time_ = t;
				audio_io::wave_handle h = sst->audio_->status_stream(sst->slot_);
				if(h != 0) {
					uint32_t len = sdf.read_stream(fin, pos, stream_buff_size);
					if(len) {
						pos += len;
						sst->audio_->queue_stream(sst->slot_, h, sdf.get_stream());
					} else {
						pos = ainfo.samples;
					}
				}

				if(sst->seek_) {
					pos = sst->seek_pos_;
					sst->seek_ = false;
					continue;
				}

				usleep(10000);	// 10ms くらいの時間待ち

			}
			sst->audio_->purge_stream(sst->slot_);
			sst->pos_ = sst->len_;
			sdf.close_stream();
			fin.close();

			if(sst->replay_) {
				sst->replay_ = false;
			} else if(sst->prior_) {
				sst->prior_ = false;
				if(i) --i;
			} else if(sst->next_) {
				sst->next_ = false;
				++i;
			} else {
				++i;
			}
		}
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
		sound::sstream_t* sst = static_cast<sound::sstream_t*>(entry);

		snd_files sdf;
		sdf.initialize();

		sst->start_ = true;

		utils::file_infos fis;
		create_file_list(sst->root_, fis);
		play_task_(sst, sdf, sst->root_, fis, sst->file_);

		sdf.destroy();

		sst->start_ = false;
		sst->finsh_ = true;

		return NULL;
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
				if(!audio_.get_slot_status(sh)) {
					audio_.set_wave(sh, wh);
					audio_.set_loop(sh, loop);
					audio_.play(sh);
					f = true;
					break;
				}
			}
		} else if(static_cast<size_t>(slot) < slots_.size()) {
			audio_io::slot_handle sh = slots_[slot];
			audio_.stop(sh);
			audio_.set_wave(sh, wh);	// 波形を登録して再生
			audio_.set_loop(sh, loop);
			audio_.play(sh);
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
		audio_.initialize();

		snd_files_.initialize();

		slot_max_ = slot_max;
		for(int i = 0; i < slot_max; ++i) {
			slots_.push_back(audio_.create_slot(0));
		}

		stream_slot_ = audio_.create_slot(0);

		destroy_se_all();

		stream_start_ = false;
		sstream_t_.start_ = false;
		sstream_t_.finsh_ = false;
		sstream_t_.stop_ = false;
		sstream_t_.pause_ = false;
		sstream_t_.seek_ = false;

		sstream_t_.seek_pos_ = 0;
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
		const i_audio* aif = snd_files_.get_audio_if();
		if(aif) {
			al::audio_io::wave_handle wh = audio_.create_wave(aif);
			if(wh) {
				no = se_ts_.size();
				se_ts_.push_back(se_t(wh, aif));
			} else {
				delete aif;
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
	audio_io::wave_handle sound::request_se(int slot, const i_audio* aif, bool loop)
	{
		audio_io::wave_handle wh = audio_.create_wave(aif);
		if(aif != 0) {
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
		if(se_no > 0 && se_no < se_ts_.size()) {
			f = request_sub_(slot, se_ts_[se_no].wave_, loop);
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
			audio_.pause(sh);
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
			audio_.stop(sh);
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
			return audio_.get_slot_status(sh);
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
		BOOST_FOREACH(const se_t& t, se_ts_) {
			if(t.wave_) {
				audio_.destroy_wave(t.wave_);
			}
			if(t.wsrc_) {
				delete t.wsrc_;
			}
		}
		se_ts_.clear();
		se_ts_.push_back(se_t());
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
			audio_.set_gain(sh, gain);
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
		sstream_t_.audio_ = &audio_;
		sstream_t_.slot_ = stream_slot_;
		sstream_t_.root_ = root;
		sstream_t_.file_ = file;
		sstream_t_.start_ = false;
		sstream_t_.finsh_ = false;
		sstream_t_.stop_  = false;
		sstream_t_.next_  = false;
		sstream_t_.replay_ = false;
		sstream_t_.prior_ = false;
		sstream_t_.pause_ = false;
		sstream_t_.seek_ = false;
		sstream_t_.pos_ = 0;
		sstream_t_.len_ = 0;
		sstream_t_.time_  = 0;
		sstream_t_.etime_ = 0;
		sstream_t_.open_err_   = 0;

///		pthread_attr_init(&attr_);
///		pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_DETACHED);

		pthread_create(&pth_, NULL, stream_task_, &sstream_t_);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームの再生位置を変更
		@param[in]	pos	再生開始位置
		@return 成功なら「true」を返す。
	 */
	//-----------------------------------------------------------------//
	bool sound::seek_stream(size_t pos)
	{
		if(stream_start_) {
			sstream_t_.seek_pos_ = pos;
			sstream_t_.seek_ = true;
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生を停止
		@return 正常なら「true」
	 */
	//-----------------------------------------------------------------//
	bool sound::stop_stream()
	{
		if(stream_start_) {
			sstream_t_.stop_ = true;
			pthread_join(pth_ , NULL);
			stream_start_ = false;
			sstream_t_.time_ = 0;
			sstream_t_.etime_ = 0;
			stream_fph_.clear();
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生、次の曲
	 */
	//-----------------------------------------------------------------//
	void sound::next_stream()
	{
		if(stream_start_) {
			sstream_t_.next_ = true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再再生
	 */
	//-----------------------------------------------------------------//
	void sound::replay_stream()
	{
		if(stream_start_) {
			sstream_t_.replay_ = true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生、前の曲
	 */
	//-----------------------------------------------------------------//
	void sound::prior_stream()
	{
		if(stream_start_) {
			sstream_t_.prior_ = true;
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
			if(stream_fph_cnt_ != sstream_t_.fph_cnt_) {
				stream_fph_ = sstream_t_.fph_;
				stream_fph_cnt_ = sstream_t_.fph_cnt_;
			}
			if(stream_tag_.serial_ != sstream_t_.tag_.serial_) {
				stream_tag_ = sstream_t_.tag_;
			}
			if(sstream_t_.finsh_) {
				pthread_detach(pth_);
				stream_start_ = false;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void sound::destroy()
	{
		stop_stream();

		destroy_se_all();

		for(size_t i = 0; i < slots_.size(); ++i) {
			audio_.destroy_slot(slots_[i]);
		}
		slots_.clear();
	}

};
