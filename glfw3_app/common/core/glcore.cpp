//=====================================================================//
/*!	@file
	@brief	Windows Graphics Library (for GLUT-3.7)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "core/glcore.hpp"
#include "core/ftimg.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include <time.h>

#ifdef WIN32
#include <unistd.h>
extern "C" {
	DWORD NvOptimusEnablement = 0x00000001;
};
#endif

namespace gl {

#ifdef __APPLE__
	static const char* root_font_path_ = "/System/Library/Fonts";
	static const char* default_font_file_ = "ヒラギノ角ゴ ProN W3.otf";
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
		LARGE_INTEGER tc = { 0, 0 };
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
		LARGE_INTEGER tc = { 0, 0 };
        SetThreadAffinityMask(GetCurrentThread(), 1);
		QueryPerformanceFrequency(&tc);
		return static_cast<double>(tc.QuadPart);
	}

// windows で define されているので無効にする。
#undef DELETE
#endif

	device::bitsets core::bitsets_;

	static void key_callback_(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		bool kv;
		if(action == GLFW_PRESS) kv = true;
		else if(action == GLFW_RELEASE) kv = false;
		else {
			// とりあえず REPEAT は無視する。
			return;
		}

		int ofs = -1;
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
		case GLFW_KEY_DELETE:        ofs = device::key::DELETE;        break;
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
				ofs = key - GLFW_KEY_A + device::key::A;
			} else if(GLFW_KEY_0 <= key && key <= GLFW_KEY_9) {
				ofs = key - GLFW_KEY_0 + device::key::_0;
			} else if(GLFW_KEY_F1 <= key && key <= GLFW_KEY_F25) {
				ofs = key - GLFW_KEY_F1 + device::key::F1;
			} else if(GLFW_KEY_KP_0 <= key && key <= GLFW_KEY_KP_9) {
				ofs = key - GLFW_KEY_KP_0 + device::key::KP_0;
			}
			break;
		}
		if(ofs >= 0) {
			core::bitsets_[ofs] = kv;
		}

//		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//			glfwSetWindowShouldClose(window, GL_TRUE);
//		}

		if(mods & GLFW_MOD_SHIFT) core::bitsets_[device::key::SHIFT] = true;
		else core::bitsets_[device::key::SHIFT] = false;
		if(mods & GLFW_MOD_CONTROL) core::bitsets_[device::key::CONTROL] = true;
		else core::bitsets_[device::key::CONTROL] = false;
		if(mods & GLFW_MOD_ALT) core::bitsets_[device::key::ALT] = true;
		else core::bitsets_[device::key::ALT] = false;
		if(mods & GLFW_MOD_SUPER) core::bitsets_[device::key::SUPER] = true;
		else core::bitsets_[device::key::SUPER] = false;
	}


	static void mouse_button_callback_(GLFWwindow* window, int button, int action, int mods)
	{
		bool v;
		if(action == GLFW_PRESS) v = true;
		else if(action == GLFW_RELEASE) v = false;

		int ofs = -1;
		if(button == GLFW_MOUSE_BUTTON_LEFT) {
			ofs = device::key::MOUSE_LEFT;
		} else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
			ofs = device::key::MOUSE_MIDDLE;
		} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
			ofs = device::key::MOUSE_RIGHT;
		}
		if(ofs >= 0) {
///			std::cout << "Mouse: " << ofs << std::endl;
			core::bitsets_[ofs] = v;
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
		core::bitsets_[device::key::MOUSE_FOCUS] = v;
	}

	device::locator core::locator_;

	static void cursor_callback_(GLFWwindow* window, double x, double y)
	{
///		std::cout << x << ", " << y << std::endl;
		vtx::fpos pos(static_cast<vtx::spos::value_type>(x),
					  static_cast<vtx::spos::value_type>(y));
		const vtx::fpos& sc = core::locator_.get_scale();
		core::locator_.set_cursor(pos * sc);
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

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化プロセス
		@param[in]	exec_path	コマンドパス
		@return 正常終了したら「true」
	*/
	//-----------------------------------------------------------------//
	bool core::initialize(const std::string& exec_path)
	{
		std::string tmp;
		utils::convert_delimiter(exec_path, '\\', '/', tmp);
		std::string base;
		utils::get_file_base(tmp, base);

		char buff[2048];
		current_path_ = getcwd(buff, sizeof(buff));
		exec_path_ = current_path_ + '/' + base;

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
//				std::cout << "wglSwapIntervalEXT: OK" << std::endl;
			} else {
//				std::cout << "wglSwapIntervalEXT: NO" << std::endl;
			}
	    } else {
//			std::cout << "WGL_EXT_swap_control: NO" << std::endl;
		}
		glfwSwapInterval(1);

		{  // Lock キーの初期状態を反映する
#ifdef WIN32
			int n = GetKeyboardType(0);
			if(n == 7) keyboard_jp_ = true;
			if(GetKeyState(VK_CAPITAL)) {
				bitsets_.set(device::key::CAPS_LOCK);
			}
			if(GetKeyState(VK_SCROLL)) {
				bitsets_.set(device::key::SCROLL_LOCK);
			}
			if(GetKeyState(VK_NUMLOCK)) {
				bitsets_.set(device::key::NUM_LOCK);
			}
#endif
		}

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

		locator_.set_scale(vtx::fpos(1.0f, 1.0f));
///		    static_cast<float>(size_.x) / static_cast<float>(rect_.size.x),
///			static_cast<float>(size_.y) / static_cast<float>(rect_.size.y)));
		device_.service(bitsets_, locator_);
		locator_.reset_scroll();

        /* Poll for and process events */
        glfwPollEvents();

#ifdef WIN32
		{
			int id = glfwGetDropfilesId(window_);
			if(recv_file_id_ != id) {
				recv_file_id_ = id;
				int n;
				const char** pp = glfwGetDropfilesString(window_, &n);
				recv_file_path_.clear();
				for(int i = 0; i < n; ++i) {
					std::string src = pp[i];
					std::string file;
					utils::code_conv(src, '\\', '/', file);
					recv_file_path_.push_back(file);
				}
			}
		}
#endif
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
		frame_count_++;
#if 0
		if((frame_count_ % 60) == 0) {
			int rate = static_cast<int>((1.0 / frame_time_) + 0.5);
			std::cout << cpu_ghz_ << " GHz, Perfrmance Frame Rate: " << rate << std::endl;
		}
#endif
#endif
		glfwSwapBuffers(window_);
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
