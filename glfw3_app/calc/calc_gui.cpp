//=====================================================================//
/*! @file
    @brief  RX65N/RX72N 計算機サンプル(EMU インスタンス)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020, 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#define EMU
#include "calc_gui.hpp"
#include "calc_cmd.hpp"

typedef app::calc_cmd CMD;
typedef app::calc_gui<CMD> GUI;

static CMD	cmd_;
static GUI	gui_(cmd_);

namespace gui_sim {

	app::calc_cmd& at_calc_cmd() { return cmd_; }

	const void* get_fbp()
	{
		return gui_.at_glcdc().get_fbp();
	}

    void set_pos(const vtx::spos& pos, bool touch)
	{
		gui_.at_touch().set_pos(pos);		
	}

	void start_gui()
	{
		gui_.start();
	}

	void setup_gui()
	{
		gui_.setup();
	}

	void update_gui()
	{
		gui_.update();
		gui_.at_touch().reset();
	}
}

/// widget の登録・グローバル関数
bool insert_widget(gui::widget* w)
{
    return gui_.insert_widget(w);
}

/// widget の解除・グローバル関数
void remove_widget(gui::widget* w)
{
    gui_.remove_widget(w);
}
