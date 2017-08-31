//=====================================================================//
/*!	@file
	@brief	ユーザー・インターフェース・ポリシー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/ui_policy.hpp"

namespace gui {

#if 0
		{  // ダイアログがある場合の優先順位とストール処理
			reset_mark();
			widgets ws;
			BOOST_FOREACH(widget* w, widgets_) {
				if(!w->get_state(widget::state::ENABLE)) continue;
				if(w->type() == get_type_id<widget_dialog>()) {
					parents_widget_mark_(w);
					ws.push_back(w);
				}
			}
			if(ws.empty()) {
				BOOST_FOREACH(widget* w, widgets_) {
					w->set_state(widget::state::SYSTEM_STALL, false);
				}
			} else {
				BOOST_FOREACH(widget* w, widgets_) {
					if(!w->get_mark()) {
						w->set_state(widget::state::SYSTEM_STALL); 
					}
				}
			}
			BOOST_FOREACH(widget* w, ws) {
				top_widget(w);
			}
		}
#endif

}
