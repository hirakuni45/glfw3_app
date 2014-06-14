//=====================================================================//
/*! @file
	@brief  Space Invader クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "spinv.hpp"
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "utils/unzip.hpp"
#include "widgets/widget_dialog.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void spinv::initialize()
	{
		texfb_.initialize(InvadersMachine::ScreenWidth, InvadersMachine::ScreenHeight, 32);

        using namespace gui;
        widget_director& wd = director_.at().widget_director_;

		std::string errstr;
		// sounds.zip 展開
		{
			al::sound& sound = director_.at().sound_;

			static const char* sounds[] = {
    			"Ufo", "Walk1", "Walk2", "Walk3",
				"Walk4", "Shot", "UfoHit", "BaseHit", "InvHit"
			};
			utils::unzip zip;
			std::string sda = "sounds.zip";
			if(zip.open(sda)) {
				if(zip.file_count() == 9) {
					for(int i = 0; i < 9; ++i) {
						std::string fn = sounds[i];
						fn += ".wav";
						int h = zip.find(fn);
						if(h < 0) {
							fn = sounds[i];
							fn += ".Wav";
							h = zip.find(fn);
						}
						if(h >= 0) {
							std::vector<char> wav;
							size_t wn = zip.get_filesize(h);
							wav.reserve(wn);
							zip.get_file(h, &wav[0]);
							utils::file_io fin;
							if(fin.open(&wav[0], wn)) {
								se_id_[i] = sound.load_se(fin, "wav");
								fin.close();
							}
						} else {
							errstr += "load error: '" + fn + "'";
						}
					}
				} else {
					errstr = "Open sound archive files: '" + sda + "'";
				}
			} else {
				errstr = "Can't open sound archive: '" + sda + "'";
			}
		}

	    // Setup scanlines color
		for(int i = 0; i < InvadersMachine::ScreenHeight; i++) {
			uint32_t c = 0xFFFFFFFF;
        	if( i >=  32 && i <  64 ) c = 0xFFFF0000; // Red
        	if( i >= 184 && i < 240 ) c = 0xFF00FF00; // Green
        	scan_line_color_.push_back(c);
    	}

		fb_.reserve(InvadersMachine::ScreenWidth * InvadersMachine::ScreenHeight);

		// invaders.zip 展開（ROM イメージ）
		std::string romerr;
		{
			utils::unzip zip;
			std::string romzip = "invaders.zip";
			if(zip.open(romzip)) {
				rom_.reserve(0x2000);
				static const char* rom_files[] = {
					"invaders.h", "invaders.g", "invaders.f", "invaders.e"
				};
				for(int i = 0; i < 4; ++i) {
					int h = zip.find(rom_files[i]);
					if(h < 0) {
						if(!romerr.empty()) romerr += ", ";
						romerr += rom_files[i];
					}
					zip.get_file(h, &rom_[i * 0x800]);
				}
				spinv_.setROM(&rom_[0]);
			} else {
				errstr = "Can't open ROM archive: '" + romzip + "'";
			}
		}
		if(!romerr.empty()) {
			errstr = "Can't open ROM file: '" + romerr + "'";
		}

		if(!errstr.empty()) {
			gl::core& core = gl::core::get_instance();
			vtx::spos size(500, 100);
			vtx::spos pos = (core.get_rect().size - size) / 2;
            widget::param wp(vtx::srect(pos, size));
            widget_dialog::param wp_;
            wp_.style_ = widget_dialog::param::style::OK;
            widget_dialog* d = wd.add_widget<widget_dialog>(wp, wp_);
            d->set_text(errstr);
		} else {
			spinv_.reset(ships_, n_easy_);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void spinv::update()
	{
   		gl::core& core = gl::core::get_instance();

		const gl::device& dev = core.get_device();

        gui::widget_director& wd = director_.at().widget_director_;

		// 1P
		if(dev.get_positive(gl::device::key::_1)) {
			spinv_.fireEvent(InvadersMachine::KeyOnePlayerDown);
		}
		if(dev.get_negative(gl::device::key::_1)) {
			spinv_.fireEvent(InvadersMachine::KeyOnePlayerUp);
		}
		// 2P
		if(dev.get_positive(gl::device::key::_2)) {
			spinv_.fireEvent(InvadersMachine::KeyTwoPlayersDown);
		}
		if(dev.get_negative(gl::device::key::_2)) {
			spinv_.fireEvent(InvadersMachine::KeyTwoPlayersUp);
		}
		// coin
		if(dev.get_positive(gl::device::key::_3)) {
			spinv_.fireEvent(InvadersMachine::CoinInserted);
		}

		// Left
		if(dev.get_positive(gl::device::key::LEFT)) {
   		 	spinv_.fireEvent( InvadersMachine::KeyLeftDown);
		}
		if(dev.get_positive(gl::device::key::GAME_LEFT)) {
			spinv_.fireEvent( InvadersMachine::KeyLeftDown);
		}
		if(dev.get_negative(gl::device::key::LEFT)) {
			spinv_.fireEvent( InvadersMachine::KeyLeftUp);
		}
		if(dev.get_negative(gl::device::key::GAME_LEFT)) {
			spinv_.fireEvent( InvadersMachine::KeyLeftUp);
		}

		// Right
		if(dev.get_positive(gl::device::key::RIGHT)) {
			spinv_.fireEvent( InvadersMachine::KeyRightDown);
		}
		if(dev.get_positive(gl::device::key::GAME_RIGHT)) {
    		spinv_.fireEvent( InvadersMachine::KeyRightDown);
		}
		if(dev.get_negative(gl::device::key::RIGHT)) {
			spinv_.fireEvent( InvadersMachine::KeyRightUp);
		}
		if(dev.get_negative(gl::device::key::GAME_RIGHT)) {
			spinv_.fireEvent( InvadersMachine::KeyRightUp);
		}

		// Fire
		if(dev.get_positive(gl::device::key::SPACE)) {
			spinv_.fireEvent( InvadersMachine::KeyFireDown);
		}
		if(dev.get_positive(gl::device::key::GAME_0)) {
			spinv_.fireEvent( InvadersMachine::KeyFireDown);
		}
		if(dev.get_negative(gl::device::key::SPACE)) {
			spinv_.fireEvent( InvadersMachine::KeyFireUp);
		}
		if(dev.get_negative(gl::device::key::GAME_0)) {
			spinv_.fireEvent( InvadersMachine::KeyFireUp);
		}

		al::sound& sound = director_.at().sound_;
		if(dev.get_positive(gl::device::key::A)) {
			sound.request_se(0, se_id_[8]);
		}

		// sound request
		{
			static const unsigned mask[9] = {
				InvadersMachine::SoundUfo, 
				InvadersMachine::SoundWalk1,
				InvadersMachine::SoundWalk2,
				InvadersMachine::SoundWalk3,
				InvadersMachine::SoundWalk4,
				InvadersMachine::SoundShot,
				InvadersMachine::SoundUfoHit,
				InvadersMachine::SoundBaseHit, 
				InvadersMachine::SoundInvaderHit
			};

			unsigned bits = spinv_.getSounds();

			for(int i = 1; i < 9; ++i) {
				if(se_id_[i] && (bits & mask[i])) {
					sound.request_se(i, se_id_[i]);
				}
			}
			if(bits & InvadersMachine::SoundUfo) {
				if(!sound.status_se(0)) {
					sound.request_se(0, se_id_[0], true);
				}
			} else {
				sound.stop_se(0);
			}
		}

		spinv_.step();

    	const unsigned char* video = spinv_.getVideo();
		if(video) {
			for(int y = 0; y < InvadersMachine::ScreenHeight; ++y) {
				uint32_t c = scan_line_color_[y];
				for(int x = 0; x < InvadersMachine::ScreenWidth; ++x) {
					if( *video ) {
						fb_[y * InvadersMachine::ScreenWidth + x] = c;
					} else {
						fb_[y * InvadersMachine::ScreenWidth + x] = 0;
					}
					++video;
    	    	}
    		}
		}

		texfb_.rendering(gl::texfb::image::RGBA, (const char*)&fb_[0]);
		texfb_.flip();

//		spinv_.getFrameRate();
		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void spinv::render()
	{
   		gl::core& core = gl::core::get_instance();

		const vtx::spos& siz = core.get_rect().size;

		texfb_.setup_matrix(0, 0, siz.x, siz.y);

		float scale = 1.0f;
		float ofsx = 0.0f;
		float ofsy = 0.0f;
		if(siz.x < siz.y) {
			scale = static_cast<float>(siz.x) / static_cast<float>(InvadersMachine::ScreenWidth);
			float h = static_cast<float>(InvadersMachine::ScreenHeight);
			ofsy = (static_cast<float>(siz.y) - h * scale) * 0.5f;
		} else {
			scale = static_cast<float>(siz.y) / static_cast<float>(InvadersMachine::ScreenHeight);
			float w = static_cast<float>(InvadersMachine::ScreenWidth);
			ofsx = (static_cast<float>(siz.x) - w * scale) * 0.5f;
		}
		gl::glTranslate(ofsx, ofsy);
		gl::glScale(scale);
		texfb_.draw();

        director_.at().widget_director_.service();
        director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void spinv::destroy()
	{
	}
}
