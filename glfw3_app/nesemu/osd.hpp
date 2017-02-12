#pragma once
//=====================================================================//
/*! @file
	@brief  NES Emulator インターフェース・クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
namespace app {

	class osd {

	public:

		char* getromdata()
		{
			return nullptr;
		}


		int init()
		{
			return 0;
		}


		void shutdown()
		{
		}


		int main(int argc, char *argv[])
		{
			return 0;
		}


	};

}
