//=====================================================================//
/*! @file
	@brief  main
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include "files.hpp"

typedef std::vector<uint8_t> bytes;

static bytes make_bytes_(uint32_t size)
{
	bytes bs;
	bs.resize(size);
	for(uint32_t i = 0; i < size; ++i) {
		bs[i] = rand() % 255;
	}
	return bs;
}

static std::string make_ascii_(uint32_t lines)
{
	std::string a;
	static const char tbl_[] = {
		"0123456789"
		"QWERTYUIOPASDFGHJKLZXCVBNM"
		"qwertyuiopasdfghjklzxcvbnm"
		"_   "
	};
	for(uint32_t i = 0; i < lines; ++i) {
		uint32_t l = ((rand() & 0xffff) % 40) + 10;
		for(uint32_t j = 0; j < l; ++j) {
			uint32_t idx = (rand() & 0xffff) % (sizeof(tbl_) - 1);
			a += tbl_[idx];
		}
		a += '\n';
	}
	return a;
}

// #define READ_DIR

int main(int argc, char** argv)
{
	vfs::files fs("VFS");

#ifdef READ_DIR
	fs.start(true);
	fs.list();
	return 0;
#else
	fs.start();
#endif

	fs.mkdir("qwe");
	fs.mkdir("asd");
	fs.mkdir("zxc");

	fs.cd("asd");
	{
		auto h = fs.open("test1", vfs::open_mode::write);
		auto data = make_bytes_(754);
		fs.write(h, &data[0], 754);
		fs.close(h);
	}
	
	fs.mkdir("poi");

	{
		auto h = fs.open("test2", vfs::open_mode::write);
		for(int i = 0; i < 66; ++i) {
			auto data = make_bytes_(1000);
			fs.write(h, &data[0], 1000);
		}
		fs.close(h);
	}

	fs.list();

	fs.remove("asdx");

	fs.list();

	fs.cd("/");
	fs.rmdir("asd");

	fs.list();

	{
		auto h = fs.open("readme.txt", vfs::open_mode::write);
		auto data = make_ascii_(50);
		fs.write(h, &data[0], data.size());
		fs.close(h);
	}

	// read file
	if(1) {
		auto h = fs.open("readme.txt", vfs::open_mode::read);
		while(1) {
			char data[101];
			data[0] = 0;
			int n = fs.read(h, data, 100);
//			std::cout << n << std::endl;
			if(n > 0) {
				data[n] = 0;
				std::cout << data << std::endl;
			} else {
				break;
			}
		}
		fs.close(h);
	}

	fs.list();

	// ディレクトリー情報セーブ
	fs.final();
}
