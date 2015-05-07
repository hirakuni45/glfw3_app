//=====================================================================//
/*!	@file
	@brief	RS232C 操作クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "rs232c.hpp"
#include <limits.h>
#include <sys/ioctl.h>

namespace utils {

	void rs232c::close_()
	{
		tcsetattr(fd_, TCSANOW, &attr_back_);
		::close(fd_);
		fd_ = -1;
	}


	bool rs232c::open(std::string& path, speed_t brate, char_len clen, parity par)
	{
		fd_ = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(fd_ < 0) {
			return false;
		}

		if(tcgetattr(fd_, &attr_back_) == -1) {
			::close(fd_);
			fd_ = -1;
		}

		termios    attr;
		memset(&attr, 0, sizeof(attr));

		int cpar = 0;
		int ipar = IGNPAR;
		switch(par) {
		default:
		case parity::none:
			cpar = 0;
			ipar = IGNPAR;
			break;
		case parity::even:
			cpar = PARENB;
			ipar = INPCK;
			break;
		case parity::odd:
			cpar = (PARENB | PARODD);
			ipar = INPCK;
			break;
		}

		int bstop = 0;
#if 0
  switch(mode[2])
  {
    case '1': bstop = 0;
              break;
    case '2': bstop = CSTOPB;
              break;
#endif

		int cbits;
		switch(clen) {
		default:
		case char_len::bits8:
			cbits = CS8;
			break;
		case char_len::bits7:
			cbits = CS7;
			break;
		}

		attr.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
		attr.c_iflag = ipar;
		attr.c_oflag = 0;
		attr.c_lflag = 0;
		attr.c_cc[VMIN]  = 0;     // block untill n bytes are received
		attr.c_cc[VTIME] = 0;     // block untill a timer expires (n * 100 mSec.)

		if(cfsetispeed(&attr, brate) == -1) {
			close_();
			return false;
		}
		if(cfsetospeed(&attr, brate) == -1) {
			close_();
			return false;
		}

		if(tcsetattr(fd_, TCSANOW, &attr) == -1) {
			close_();
			return false;
		}

		int status;
		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			close_();
			return false;
		}

		return true;
	}



	bool rs232c::close()
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			close_();
			return false;
		}

		status &= ~TIOCM_DTR;    /* turn off DTR */
		status &= ~TIOCM_RTS;    /* turn off RTS */

		if(ioctl(fd_, TIOCMSET, &status) == -1) {
			close_();
			return false;
		}

		close_();

		return true;
	}


	bool rs232c::get_DCD() const
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			return false;
		}

		return status & TIOCM_CAR;
	}


	bool rs232c::get_CTS() const
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			return false;
		}

		return status & TIOCM_CTS;
	}


	bool rs232c::get_DSR() const
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			return false;
		}

		return status & TIOCM_DSR;
	}

	bool rs232c::enable_DTR(bool ena)
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			return false;
		}

		if(ena) status |= TIOCM_DTR;
		else status &= ~TIOCM_DTR;

		if(ioctl(fd_, TIOCMSET, &status) == -1) {
			return false;
		}

		return true;
	}


	bool rs232c::enable_RTS(bool ena)
	{
		if(fd_ < 0) return false;

		int status;

		if(ioctl(fd_, TIOCMGET, &status) == -1) {
			return false;
		}

		if(ena) status |= TIOCM_RTS;
		else status &= ~TIOCM_RTS;

		if(ioctl(fd_, TIOCMSET, &status) == -1) {
			return false;
		}

		return true;
	}

}
