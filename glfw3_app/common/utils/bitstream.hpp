#pragma once
//=====================================================================//
/*!	@file
	@breif	�r�b�g�P�ʂœ��o�́i�w�b�_�[�j
	@author	�����M�m (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/foreach.hpp>
#include "utils/file_io.hpp"

namespace utils {

	class bitstream {

		uint32_t	bit_pos_;		///> �J�����g�̃r�b�g�ʒu
		uint32_t	bit_limit_;		///> �r�b�g�̍ő吔
		uint8_t		bit_val_;		///> �J�����g�̃r�b�g���i�o�C�g�P�ʁj
		std::vector<uint8_t>	bit_list_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	bitio �N���X�E�R���X�g���N�^�[
		*/
		//-----------------------------------------------------------------//
		bitstream() : m_bit_pos(0), m_bit_limit(0), m_bit_val(0) { };


		//-----------------------------------------------------------------//
		/*!
			@breif	bitio �N���X�E�f�X�g���N�^�[
		*/
		//-----------------------------------------------------------------//
		~bitstream() { };


		//-----------------------------------------------------------------//
		/*!
			@breif	�r�b�g����ݒ肵�āA�J�����g�̃r�b�g�ʒu��i�߂�B
			@param[in]	val �r�b�g���
						true �̏ꍇ�́u�P�v�Afalse �̏ꍇ�́u�O�v���ݒ�
		*/
		//-----------------------------------------------------------------//
		void put_bit(bool val) {
			if(val == true) {
				bit_val_ |= 1 << (bit_limit_ % 8);
			}
			++bit_limit_;
			if((bit_limit_ % 8) == 0) {
				bit_list_.push_back(bit_val_);
				bit_val_ = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	�C�Ӄr�b�g��ݒ肵�āA�J�����g�̃r�b�g�ʒu��i�߂�B
					���ő�R�Q�r�b�g
			@param[in]	val �ݒ肷��l
			@param[in]	num �ݒ肷��r�b�g��
		*/
		//-----------------------------------------------------------------//
		void put_bits(uint32_t val, int num) {
			for(int i = 0; i < num; ++i) {
				bool bit;
				if(val & (1 << i)) bit = true; else bit = false;
				put_bit(bit);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	�J�����g�̃r�b�g���𓾂�A�J�����g�̃r�b�g�ʒu��i�߂�B
			@return	true �̏ꍇ�́u�P�v�Afalse �̏ꍇ�́u�O�v
		*/
		//-----------------------------------------------------------------//
		bool get_bit() {
			if(bit_pos_ >= bit_limit_) {
				return false;
			}
			uint32_t bpos = bit_pos_ % 8;
			uint8_t	c = bit_list_[bit_pos_ >> 3];
			++bit_pos_;
			if(c & (1 << bpos)) return true; else return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	�J�����g�̃r�b�g���𓾂�A�J�����g�̃r�b�g�ʒu��i�߂�B
			@param[in]	num	�ǂݍ��ރr�b�g��
							���ő�R�Q
			@return	�ǂݍ��񂾒l
		*/
		//-----------------------------------------------------------------//
		uint32_t get_bits(int num) {
			uint32_t val = 0;

			for(int i = 0; i < num; ++i) {
				if(get_bit() == true) val |= 1 << i;
			}
			return val;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	�J�����g�̃r�b�g�ʒu��ύX����B
			@param[in]	pos	�r�b�g�ʒu
		*/
		//-----------------------------------------------------------------//
		void set_pos(unsigned int pos) {
			if(bit_limit_ < pos) pos = bit_limit_ - 1;
			bit_pos_ = pos;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	�J�����g�̃r�b�g�ʒu�𓾂�B
			@return	�J�����g�̃r�b�g�ʒu
		*/
		//-----------------------------------------------------------------//
		uint32_t get_pos() const { return bit_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@breif	�ő�r�b�g�ʒu�𓾂�B
			@return	�ő�r�b�g�ʒu
		*/
		//-----------------------------------------------------------------//
		uint32_t get_limit() const { return bit_limit_; };


		void clear() { bit_limit_ = bit_pos_ = 0; bit_list_.clear(); }


		size_t size() const { return bit_list_.size(); }


		const uint8_t* get_array() const { return &bit_list_[0]; }


		//-----------------------------------------------------------------//
		/*!
			@breif		�S�Ẵr�b�g�����t�@�C���ɃZ�[�u����B
			@param[in]	fname �t�@�C����
			@return		�Z�[�u�����o�C�g��
		*/
		//-----------------------------------------------------------------//
		uint32_t save_file(const std::string& fname) {
			if(fname.empty() || bit_list_.size() == 0) {
				return 0;
			}

			file_io::fout;
			if(!fout.open(fname, "wb")) {
				return 0;
			}

			// �e���|�����[�̗]�蕪�������i�u�O�v�Ŗ��߂�j
			while((bit_limit_ % 8) != 0) {
				put_bit(false);
			}
			BOOST_FOREACH(uint8_t ch, bit_list_) {
				fout.putc(ch);
			}

			fout.close();

			return bit_list_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif		�S�Ẵr�b�g�����t�@�C���ɃZ�[�u����B
			@param[in]	fname �t�@�C����
			@return		�Z�[�u�����o�C�g��
		*/
		//-----------------------------------------------------------------//
		uint32_t save_append_file(const std::string& fname) {
			if(fname.empty() || bit_list_.size() == 0) {
				return 0;
			}

			// �e���|�����[�̗]�蕪�������i�u�O�v�Ŗ��߂�j
			while((bit_limit_ % 8) != 0) {
				put_bit(false);
			}

			file_io::fout;
			fout.open(fname, "ab");

			for(std::vector<uint8_t>::iterator i = bit_list_.begin(); i != bit_list_.end(); ++i) {
				uint8_t c = *i;
				fputc(c, fp);
			}
			fclose(fp);
			return bit_list_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif		�t�@�C�����r�b�g���Ƃ��ă��[�h
			@param[in]	fname �t�@�C����
			@return		���[�h�����o�C�g��
		*/
		//-----------------------------------------------------------------//
		uint32_t load_file(const std::string& fname) {
			if(fname.empty()) return 0;

			bit_list_.clear();

			file_io::fin;
			if(!fin.open(fname, "rb")) {
				return 0;
			}
			int ch;
			while(fin.get_char(ch)) {
				bit_list_.push_back(ch);
			}
			fin.close();
			bit_limit_ = num * 8;
			return num;
		}
	};
}
