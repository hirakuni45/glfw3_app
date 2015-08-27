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
	bs.clear();
	for(uint32_t i = 0; i < size; ++i) {
		bs[i] = rand() % 255;
	}
	return bs;
}


int main(int argc, char** argv)
{
	vfs::files fs("VFS");

	fs.start();

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
		fs.close(h);
	}

	fs.list();

	// ディレクトリー情報セーブ
	fs.final();
}
