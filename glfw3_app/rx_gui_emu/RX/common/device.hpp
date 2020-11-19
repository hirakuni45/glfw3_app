#pragma once
//=====================================================================//
/*!	@file
	@brief	RX マイコン、デバイス固有ヘッダー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/io_utils.hpp"

#if defined(SIG_RX24T)
#include "RX24T/peripheral.hpp"
#include "RX24T/system.hpp"
#include "RX24T/power_mgr.hpp"
#include "RX24T/icu.hpp"
#include "RX24T/icu_mgr.hpp"
#include "RX24T/port_map.hpp"

#elif defined(SIG_RX62N)
#include "RX62x/system.hpp"
#include "RX62x/icu.hpp"
#include "RX62x/icu_mgr.hpp"

#elif defined(SIG_RX64M) || defined(SIG_RX71M)
#include "RX600/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX600/power_mgr.hpp"
#include "RX600/icu.hpp"
#include "RX600/icu_mgr.hpp"
#include "RX600/port_map.hpp"

#elif defined(SIG_RX72M)
#include "RX72M/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX72M/power_mgr.hpp"
#include "RX72M/icu.hpp"
#include "RX72M/icu_mgr.hpp"
#include "RX72M/port_map.hpp"

#elif defined(SIG_RX72N)
#include "RX72N/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX72N/power_mgr.hpp"
#include "RX72N/icu.hpp"
#include "RX72N/icu_mgr.hpp"
#include "RX72N/port_map.hpp"

#elif defined(SIG_RX65N)
#include "RX65x/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX65x/power_mgr.hpp"
#include "RX65x/icu.hpp"
#include "RX65x/icu_mgr.hpp"
#include "RX65x/port_map.hpp"

#elif defined(SIG_RX66T)
#include "RX66T/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX66T/power_mgr.hpp"
#include "RX66T/icu.hpp"
#include "RX66T/icu_mgr.hpp"
#include "RX66T/port_map.hpp"

#elif defined(SIG_RX72T)
#include "RX72T/peripheral.hpp"
#include "RX600/system.hpp"
#include "RX72T/power_mgr.hpp"
#include "RX72T/icu.hpp"
#include "RX72T/icu_mgr.hpp"
#include "RX72T/port_map.hpp"

#else
#  error "device.hpp: Requires SIG_XXX to be defined"
#endif
