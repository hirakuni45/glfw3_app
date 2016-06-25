//=====================================================================//
/*!	@file
	@brief	Windows Graphics Library (for GLUT-3.7)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#ifdef WIN32
#include <windows.h>
// windows で define されているので無効にする。
// #undef DELETE
#endif
#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include "core/glcore.hpp"
#include "core/ftimg.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include "utils/sjis_utf16.hpp"

#ifdef WIN32
#include <unistd.h>
extern "C" {
	DWORD NvOptimusEnablement = 0x00000001;
};
#endif

namespace gl {

#ifdef __APPLE__
	static const char* root_font_path_ = "/System/Library/Fonts";
	static const char* default_font_file_ = "ヒラギノ角ゴシック W3.ttc";
#endif

#ifdef WIN32
	static const char* root_font_path_ = "c:/WINDOWS/Fonts";
	static const char* default_font_file_ = "MSGOTHIC.TTC";
#endif
	static const char* default_font_face_ = "gothic";

#ifdef WIN32
	//=================================================================//
	/*!
		@brief	CPU のマシンサイクルをカウントする（WIN32-API）
		@return マシンサイクル数
	*/
	//=================================================================//
	static inline double rdtsc_() {
		LARGE_INTEGER tc;
		tc.QuadPart = 0;
        // 使用するCPUコアを限定する
        SetThreadAffinityMask(GetCurrentThread(), 1);
		QueryPerformanceCounter(&tc);
		return static_cast<double>(tc.QuadPart);
	}


	//=================================================================//
	/*!
		@brief	１秒辺りのマシンサイクル数を得る。
		@return マシンサイクル数
	*/
	//=================================================================//
	static inline double get_cpu_clock_() {
		LARGE_INTEGER tc;
		tc.QuadPart = 0;
        SetThreadAffinityMask(GetCurrentThread(), 1);
		QueryPerformanceFrequency(&tc);
		return static_cast<double>(tc.QuadPart);
	}
#endif

	device::bits_t core::bits_;

	static void key_callback_(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		bool kv;
		if(action == GLFW_PRESS) kv = true;
		else if(action == GLFW_RELEASE) kv = false;
		else {
			// とりあえず REPEAT は無視する。
			return;
		}

		auto ofs = device::key::count_;
		switch(key) {
		case GLFW_KEY_SPACE:         ofs = device::key::SPACE;         break;
		case GLFW_KEY_APOSTROPHE:    ofs = device::key::APOSTROPHE;    break;
		case GLFW_KEY_COMMA:         ofs = device::key::COMMA;         break;
		case GLFW_KEY_MINUS:         ofs = device::key::MINUS;         break;
		case GLFW_KEY_PERIOD:        ofs = device::key::PERIOD;        break;
		case GLFW_KEY_SLASH:         ofs = device::key::SLASH;         break;
		case GLFW_KEY_SEMICOLON:     ofs = device::key::SEMICOLON;     break;
		case GLFW_KEY_EQUAL:         ofs = device::key::EQUAL;         break;
		case GLFW_KEY_LEFT_BRACKET:  ofs = device::key::LEFT_BRACKET;  break;
		case GLFW_KEY_BACKSLASH:     ofs = device::key::BACKSLASH;     break;
		case GLFW_KEY_RIGHT_BRACKET: ofs = device::key::RIGHT_BRACKET; break;
		case GLFW_KEY_GRAVE_ACCENT:  ofs = device::key::GRAVE_ACCENT;  break;
		case GLFW_KEY_WORLD_1:       ofs = device::key::WORLD_1;       break;
		case GLFW_KEY_WORLD_2:       ofs = device::key::WORLD_2;       break;
		case GLFW_KEY_ESCAPE:        ofs = device::key::ESCAPE;        break;
		case GLFW_KEY_ENTER:         ofs = device::key::ENTER;         break;
		case GLFW_KEY_TAB:           ofs = device::key::TAB;           break;
		case GLFW_KEY_BACKSPACE:     ofs = device::key::BACKSPACE;     break;
		case GLFW_KEY_INSERT:        ofs = device::key::INSERT;        break;
		case GLFW_KEY_DELETE:        ofs = device::key::DEL;           break;
		case GLFW_KEY_RIGHT:         ofs = device::key::RIGHT;         break;
		case GLFW_KEY_LEFT:          ofs = device::key::LEFT;          break;
		case GLFW_KEY_DOWN:          ofs = device::key::DOWN;          break;
		case GLFW_KEY_UP:            ofs = device::key::UP;            break;
		case GLFW_KEY_PAGE_UP:       ofs = device::key::PAGE_UP;       break;
		case GLFW_KEY_PAGE_DOWN:     ofs = device::key::PAGE_DOWN;     break;
		case GLFW_KEY_HOME:          ofs = device::key::HOME;          break;
		case GLFW_KEY_END:           ofs = device::key::END;           break;
		case GLFW_KEY_CAPS_LOCK:     ofs = device::key::CAPS_LOCK;     break;
		case GLFW_KEY_SCROLL_LOCK:   ofs = device::key::SCROLL_LOCK;   break;
		case GLFW_KEY_NUM_LOCK:      ofs = device::key::NUM_LOCK;      break;
		case GLFW_KEY_PRINT_SCREEN:  ofs = device::key::PRINT_SCREEN;  break;
		case GLFW_KEY_PAUSE:         ofs = device::key::PAUSE;         break;
		case GLFW_KEY_KP_DECIMAL:    ofs = device::key::KP_DECIMAL;    break;
		case GLFW_KEY_KP_DIVIDE:     ofs = device::key::KP_DIVIDE;     break;
		case GLFW_KEY_KP_MULTIPLY:   ofs = device::key::KP_MULTIPLY;   break;
		case GLFW_KEY_KP_SUBTRACT:   ofs = device::key::KP_SUBTRACT;   break;
		case GLFW_KEY_KP_ADD:        ofs = device::key::KP_ADD;        break;
		case GLFW_KEY_KP_ENTER:      ofs = device::key::KP_ENTER;      break;
		case GLFW_KEY_KP_EQUAL:      ofs = device::key::KP_EQUAL;      break;
		case GLFW_KEY_LEFT_SHIFT:    ofs = device::key::LEFT_SHIFT;    break;
		case GLFW_KEY_LEFT_CONTROL:  ofs = device::key::LEFT_CONTROL;  break;
		case GLFW_KEY_LEFT_ALT:      ofs = device::key::LEFT_ALT;      break;
		case GLFW_KEY_LEFT_SUPER:    ofs = device::key::LEFT_SUPER;    break;
		case GLFW_KEY_RIGHT_SHIFT:   ofs = device::key::RIGHT_SHIFT;   break;
		case GLFW_KEY_RIGHT_CONTROL: ofs = device::key::RIGHT_CONTROL; break;
		case GLFW_KEY_RIGHT_ALT:     ofs = device::key::RIGHT_ALT;     break;
		case GLFW_KEY_RIGHT_SUPER:   ofs = device::key::RIGHT_SUPER;   break;
		case GLFW_KEY_MENU:          ofs = device::key::MENU;          break;
		default:
			if(GLFW_KEY_A <= key && key <= GLFW_KEY_Z) {
				auto d = key - GLFW_KEY_A;
				ofs = static_cast<device::key>(d + static_cast<int>(device::key::A));
			} else if(GLFW_KEY_0 <= key && key <= GLFW_KEY_9) {
				auto d = key - GLFW_KEY_0;
				ofs = static_cast<device::key>(d + static_cast<int>(device::key::_0));
			} else if(GLFW_KEY_F1 <= key && key <= GLFW_KEY_F25) {
				auto d = key - GLFW_KEY_F1;
				ofs = static_cast<device::key>(d + static_cast<int>(device::key::F1));
			} else if(GLFW_KEY_KP_0 <= key && key <= GLFW_KEY_KP_9) {
				auto d = key - GLFW_KEY_KP_0;
				ofs = static_cast<device::key>(d + static_cast<int>(device::key::KP_0));
			}
			break;
		}
		if(ofs != device::key::count_) {
			core::bits_.set(ofs, kv);
		}

//		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//			glfwSetWindowShouldClose(window, GL_TRUE);
//		}

		if(mods & GLFW_MOD_SHIFT) core::bits_.set(device::key::SHIFT);
		else core::bits_.reset(device::key::SHIFT);
		if(mods & GLFW_MOD_CONTROL) core::bits_.set(device::key::CONTROL);
		else core::bits_.reset(device::key::CONTROL);
		if(mods & GLFW_MOD_ALT) core::bits_.set(device::key::ALT);
		else core::bits_.reset(device::key::ALT);
		if(mods & GLFW_MOD_SUPER) core::bits_.set(device::key::SUPER);
		else core::bits_.reset(device::key::SUPER);
	}


	static void mouse_button_callback_(GLFWwindow* window, int button, int action, int mods)
	{
		bool v;
		if(action == GLFW_PRESS) v = true;
		else if(action == GLFW_RELEASE) v = false;

		device::key ofs = device::key::count_;
		if(button == GLFW_MOUSE_BUTTON_LEFT) {
			ofs = device::key::MOUSE_LEFT;
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			ofs = device::key::MOUSE_MIDDLE;
		} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
			ofs = device::key::MOUSE_RIGHT;
		}
		if(ofs != device::key::count_) {
///			std::cout << "Mouse: " << ofs << std::endl;
			core::bits_.set(ofs, v);
		}
	}

	static void cursor_enter_callback_(GLFWwindow* window, int entered)
	{
		bool v;
		if(entered == GL_TRUE) v = true;
		else if(entered == GL_FALSE) v = false;
		else {
			return;
		}
		core::bits_.set(device::key::MOUSE_FOCUS, v);
	}

	device::locator core::locator_;

	static void cursor_callback_(GLFWwindow* window, double x, double y)
	{
///		std::cout << x << ", " << y << std::endl;
		vtx::spos pos(static_cast<vtx::spos::value_type>(x),
					  static_cast<vtx::spos::value_type>(y));
		core::locator_.set_cursor(pos);
	}

	static void scroll_callback_(GLFWwindow* window, double x, double y)
	{
		vtx::fpos pos(static_cast<vtx::spos::value_type>(x),
					  static_cast<vtx::spos::value_type>(y));
		core::locator_.set_scroll(pos);
	}

#if 0
	static void resize_framebuffer_(GLFWwindow* window, int w, int h)
	{
	}


	static void resize_window_(GLFWwindow* window, int w, int h)
	{
	}
#endif


	static void dropfile_callback_(GLFWwindow* window, int num, const char** path)
	{
		core::get_instance().set_recv_files(num, path);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化プロセス
		@param[in]	argc	起動パラメーター数
		@param[in]	argv	起動パラメーター
		@return 正常終了したら「true」
	*/
	//-----------------------------------------------------------------//
	bool core::initialize(int argc, char** argv)
	{
		utils::init_utf16_to_sjis();

		std::string exec_path;
		if(argc > 0) exec_path = argv[0];
///		std::cout << "Exec path: '" << exec_path << std::endl;

		std::string tmp = utils::convert_delimiter(exec_path, '\\', '/');
		std::string base = utils::get_file_base(tmp);

		if(!tmp.empty() && tmp[0] == '/') {
			current_path_ = utils::get_file_path(tmp);
		} else {
			char buff[2048];
#ifdef WIN32
			std::string tmp = utils::sjis_to_utf8(getcwd(buff, sizeof(buff)));
#else
			std::string tmp = getcwd(buff, sizeof(buff));
#endif
			current_path_ = utils::convert_delimiter(tmp, '\\', '/');
///			std::cout << "Current(getcwd): '" << current_path_ << std::endl;
		}
		exec_path_ = current_path_ + '/' + base;

///		std::cout << "Current: '" << current_path_ << std::endl;
///		std::cout << "Exec:    '" << exec_path_ << std::endl;

		for(int i = 1; i < argc; ++i) {
			std::string tmp;
#ifdef WIN32
			std::string s = utils::sjis_to_utf8(argv[i]);
			tmp = utils::convert_delimiter(s, '\\', '/');
#else
			tmp = argv[i];
#endif
			command_path_.push_back(tmp);
		}

	    if (!glfwInit()) {
			return false;
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vm = glfwGetVideoMode(monitor);
		best_size_.x  = vm->width;
		best_size_.y  = vm->height;
		limit_size_ = best_size_  / 2;	///< 最小のサイズはベストの半分とする
///		setlocale(LC_CTYPE, "jpn");
		setlocale(LC_CTYPE, "");

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セットアップ・プロセス
		@param[in]	rect	描画領域と位置
		@param[in]	title	タイトルの設定
		@param[in]	fullscreen	全画面の場合は「ture」
		@return 正常終了したら「true」
	*/
	//-----------------------------------------------------------------//
	bool core::setup(const vtx::srect& rect, const std::string& title, bool fullscreen)
	{
		rect_ = rect;
		limit_size_ = rect_.size / 3;

		window_ = glfwCreateWindow(rect_.size.x, rect_.size.y, title.c_str(), NULL, NULL);
		if(!window_) {
			std::cerr << "glcore setup false: 'glfwCreateWindow'" << std::endl;
			return false;
		}

		glfwSetKeyCallback(window_, key_callback_);
		glfwSetMouseButtonCallback(window_, mouse_button_callback_);
		glfwSetCursorPosCallback(window_, cursor_callback_);
		glfwSetCursorEnterCallback(window_, cursor_enter_callback_);
		glfwSetScrollCallback(window_, scroll_callback_);
		glfwSetWindowPos(window_, rect_.org.x, rect_.org.y);
//		glfwSetWindowSizeCallback(window_, resize_window_);
//		glfwSetFramebufferSizeCallback(window_, resize_framebuffer_);
		glfwSetDropCallback(window_, dropfile_callback_);

		glfwMakeContextCurrent(window_);
		{
			int x, y;
			glfwGetFramebufferSize(window_, &x, &y);
			size_.x = x;
			size_.y = y;
		}

		// GLEWの初期化
		int err = glewInit();
		if(err != GLEW_OK) {
			std::cout << "GLEW initalization error: " << err << std::endl;
			return -1;
		}

		bool f = img::ftimg::get_instance().initialize(root_font_path_);
		if(f) {
			fonts_.initialize(default_font_file_, default_font_face_);
			fonts_.set_font_size(24);
		} else {
			std::cerr << "FreeType initialize error..." << std::endl;
		}

        // 垂直同期を有効にする。
		if(glfwExtensionSupported("WGL_EXT_swap_control") == GL_TRUE) {
//			std::cout << "WGL_EXT_swap_control: OK" << std::endl;
			bool (*p)(int) = (bool (*)(int)) glfwGetProcAddress("wglSwapIntervalEXT");
			if(p) {
				(*p)(1);
				soft_sync_ = false;
//				std::cout << "wglSwapIntervalEXT: OK" << std::endl;
			} else {
				soft_sync_ = true;
//				std::cout << "wglSwapIntervalEXT: NO" << std::endl;
			}
	    } else {
			soft_sync_ = true;
//			std::cout << "WGL_EXT_swap_control: NO" << std::endl;
		}
		glfwSwapInterval(1);

		{  // Lock キーの初期状態を反映する
#ifdef WIN32
			int n = GetKeyboardType(0);
			if(n == 7) keyboard_jp_ = true;
			if(GetKeyState(VK_CAPITAL)) {
				bits_.set(device::key::CAPS_LOCK);
			}
			if(GetKeyState(VK_SCROLL)) {
				bits_.set(device::key::SCROLL_LOCK);
			}
			if(GetKeyState(VK_NUMLOCK)) {
				bits_.set(device::key::NUM_LOCK);
			}
#endif
		}

#ifdef __APPLE__
		start_sync_();
#endif

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	タイトルの登録
		@param[in]	title	タイトル文字列
	*/
	//-----------------------------------------------------------------//
	void core::set_title(const std::string& title)
	{
		if(!title.empty()) {
			if(title_ != title) {
				glfwSetWindowTitle(window_, title.c_str());
				title_ = title;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	全画面の設定
		@param[in]	flag	全画面の場合「true」
	*/
	//-----------------------------------------------------------------//
	void core::full_screen(bool flag)
	{
		if(flag) {
			full_screen_ = true;
		} else {
			full_screen_ = false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	毎フレーム・サービス
	*/
	//-----------------------------------------------------------------//
	void core::service()
	{
		if(window_ == 0) return;

		{
			int x, y;
			glfwGetWindowSize(window_, &x, &y);
			bool toset = false;
			if(x < limit_size_.x) {
				x = limit_size_.x;
				toset = true;
			}
			if(y < limit_size_.y) {
				y = limit_size_.y;
				toset = true;
			}
			if(toset) glfwSetWindowSize(window_, x, y);
		}

		{
			int x, y;

			glfwGetWindowPos(window_, &x, &y);
			rect_.org.x = x;
			rect_.org.y = y;

			glfwGetWindowSize(window_, &x, &y);
			rect_.size.x = x;
			rect_.size.y = y;

			// フレームバッファサイズとwindowのサイズは異なる
			glfwGetFramebufferSize(window_, &x, &y);
			size_.x = x;
			size_.y = y;
		}

		device_.service(bits_, locator_);
		locator_.reset_scroll();

        /* Poll for and process events */
        glfwPollEvents();

   		glViewport(0, 0, size_.x, size_.y);

	 	if(glfwWindowShouldClose(window_)) {
			exit_signal_ = true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・フリップ
	*/
	//-----------------------------------------------------------------//
	void core::flip_frame()
	{
#ifdef WIN32
		// ソフト同期
		if(soft_sync_) {
			double cpuz = get_cpu_clock_() / 1e6;
			if(cpu_ghz_ < cpuz) cpu_ghz_ = cpuz;
			if(cpu_ghz_ > 0.0) {
				double ref = machine_cycle_;
				machine_cycle_ = rdtsc_();
				double wa = (machine_cycle_ - ref) / (cpu_ghz_ * 1e6);
				double ft = 1.0 / 60.0;
				if(ft > wa) {
					useconds_t usec = static_cast<useconds_t>((ft - wa) * 1e6);
					usleep(usec);
				}
				frame_time_ = wa;
			}
		}
		frame_count_++;
#if 0
		if((frame_count_ % 60) == 0) {
			int rate = static_cast<int>((1.0 / frame_time_) + 0.5);
			std::cout << cpu_ghz_ << " GHz, Perfrmance Frame Rate: " << rate << std::endl;
		}
#endif
#endif

///		std::this_thread::sleep_for(std::chrono::milliseconds(10));
#ifdef __APPLE__
		wait_sync_();
#endif
		glfwSwapBuffers(window_);
#ifdef __APPLE__
		start_sync_();
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄プロセス
	*/
	//-----------------------------------------------------------------//
	void core::destroy()
	{
		if(window_) {
			glfwDestroyWindow(window_);
			window_ = 0;

		    glfwTerminate();
		}

		fonts_.destroy();
	}
}
