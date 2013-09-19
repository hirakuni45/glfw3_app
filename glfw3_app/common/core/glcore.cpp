//=====================================================================//
/*!	@file
	@brief	Windows Graphics Library (for GLUT-3.7)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "glcore.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include <unistd.h>                
#include <time.h>

namespace gl {

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

	IGLcore* glcore::glcore_ = 0;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  コアモジュールの生成
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    void create_glcore()
    {
        glcore::glcore_ = new glcore;

        if(glcore::glcore_ == 0) {
            printf("Can't create glcore...\n");
            fflush(stdout);
            exit(0);
        }
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  コアモジュールのインターフェースクラスを得る
        @return コアモジュールのインターフェースクラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    IGLcore* get_glcore()
    {
        return dynamic_cast<IGLcore*>(glcore::glcore_);
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  コアモジュールの廃棄
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    void destroy_glcore()
    {
        delete glcore::glcore_;
        glcore::glcore_ = 0;
    }

	device::bitsets glcore::bitsets_;

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
			glcore::bitsets_[ofs] = kv;
		}

    	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        	glfwSetWindowShouldClose(window, GL_TRUE);
		}
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
			glcore::bitsets_[ofs] = v;
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
		glcore::bitsets_[device::key::MOUSE_FOCUS] = v;
	}

	device::locator glcore::locator_;
	
	static void cursor_callback_(GLFWwindow* window, double x, double y)
	{
		glcore::locator_.cursor_.x = static_cast<vtx::spos::value_type>(x);
		glcore::locator_.cursor_.y = static_cast<vtx::spos::value_type>(y);
	}

	static void scroll_callback_(GLFWwindow* window, double x, double y)
	{
		glcore::locator_.scroll_.x = static_cast<vtx::spos::value_type>(x);
		glcore::locator_.scroll_.y = static_cast<vtx::spos::value_type>(y);
	}

#if 0
	static void resize_framebuffer_(GLFWwindow* window, int w, int h)
	{
//		printf("fb: %d, %d\n", w, h);
//		fflush(stdout);
	}


	static void resize_window_(GLFWwindow* window, int w, int h)
	{
//		printf("win: %d, %d\n", w, h);
//		fflush(stdout);
	}
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化プロセス
		@param[in]	current_path	カレント・パス
		@return 正常終了したら「true」
	*/
	//-----------------------------------------------------------------//
	bool glcore::initialize(const std::string& current_path)
	{
	    if (!glfwInit()) {
			return false;
		}

		current_path_ = current_path;

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
	bool glcore::setup(const vtx::srect& rect, const std::string& title, bool fullscreen)
	{
		locate_ = rect.org;
		size_ = rect.size;
		limit_size_ = size_ / 3;

		window_ = glfwCreateWindow(size_.x, size_.y, title.c_str(), NULL, NULL);
		if(!window_) {
			return false;
		}

		// win32 handle glfwGetWin32Window
//		HWND hw = glfwGetWin32Window(window_);


		glfwSetKeyCallback(window_, key_callback_);
		glfwSetMouseButtonCallback(window_, mouse_button_callback_);
		glfwSetCursorPosCallback(window_, cursor_callback_);
		glfwSetCursorEnterCallback(window_, cursor_enter_callback_);
		glfwSetScrollCallback(window_, scroll_callback_);
		glfwSetWindowPos(window_, locate_.x, locate_.y);
//		glfwSetWindowSizeCallback(window_, resize_window_);
//		glfwSetFramebufferSizeCallback(window_, resize_framebuffer_);

		glfwMakeContextCurrent(window_);

		fonts_.initialize();
		fonts_.set_font_size(24);

        // 垂直同期を有効にする。
		if(glfwExtensionSupported("WGL_EXT_swap_control") == GL_TRUE) {
///			printf("`WGL_EXT_swap_control` OK\n");
			void (*p)() = glfwGetProcAddress("wglSwapIntervalEXT");
			if(p) {
///				printf("Swap Interval: OK\n");
			}
	    } else {
///			printf("`WGL_EXT_swap_control` NO\n");
		}
///		fflush(stdout);

		glfwSwapInterval(1);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	タイトルの登録
		@param[in]	title	タイトル文字列
	*/
	//-----------------------------------------------------------------//
	void glcore::set_title(const std::string& title)
	{
		if(!title.empty()) {
			glfwSetWindowTitle(window_, title.c_str());
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	全画面の設定
		@param[in]	flag	全画面の場合「true」
	*/
	//-----------------------------------------------------------------//
	void glcore::full_screen(bool flag)
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
	void glcore::service()
	{
		if(window_ == 0) return;

		device_.service(bitsets_, locator_);
		locator_.scroll_.set(0);

        /* Poll for and process events */
        glfwPollEvents();

		{
			int w, h;
			glfwGetWindowSize(window_, &w, &h);
			if(w < limit_size_.x) {
				w = limit_size_.x;
			}
			if(h < limit_size_.y) {
				h = limit_size_.y;
			}
			glfwSetWindowSize(window_, w, h);
		}

		{
			int x, y;
			glfwGetFramebufferSize(window_, &x, &y);
			size_.x = x;
			size_.y = y;
		}

		{
			int x, y;
			glfwGetWindowPos(window_, &x, &y);
			locate_.x = x;
			locate_.y = y;
		}


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
	void glcore::flip_frame()
	{
        glfwSwapBuffers(window_);

#ifdef WIN32
		if(cpu_ghz_ > 0.0) {
			double ref = machine_cycle_;
			machine_cycle_ = rdtsc_();
			double dt = machine_cycle_ - ref;
			double w = dt / (cpu_ghz_ * 1e6);
			double ft = 1.0 / 60.0;
			if(ft > w) {
				useconds_t usec = static_cast<useconds_t>((ft - w) * 1e6);
				usleep(usec);
			}
			{
				double ref = frame_ref_;
				frame_ref_ = machine_cycle_;
				frame_time_ = (frame_ref_ - ref) / (cpu_ghz_ * 1e6);
			}
		}

        frame_count_++;
        if(frame_count_ > 4) {
            if(!cpu_spd_enable_) {
                // ある程度「負荷」がかからないと正確な値が出ない・・
                cpu_ghz_ = get_cpu_clock_() / 1e6;
                cpu_spd_enable_ = true;
            }
        }
        // 定期的にCPU速度を評価して、boostした場合や、スローダウンに対応する。
        if((frame_count_ % 60) == 0) {
#if 0
			int rate = static_cast<int>((1.0 / frame_time_) + 0.5);
			printf("%1.3f GHz, FrameRate: %d\n", cpu_ghz_, rate);
			fflush(stdout);
#endif
            double spd = get_cpu_clock_() / 1e6;
            if(std::abs(spd - cpu_ghz_) > 0.05) {
                cpu_ghz_ = spd;
            }
        }
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄プロセス
	*/
	//-----------------------------------------------------------------//
	void glcore::destroy()
	{
		if(window_) {
			glfwDestroyWindow(window_);
			window_ = 0;

		    glfwTerminate();
		}

		fonts_.destroy();
	}
}
