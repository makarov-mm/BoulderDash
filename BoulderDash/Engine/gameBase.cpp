#include "gameBase.h"

#include <windows.h>
#include <objbase.h>
#include <GL/gl.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <utility>

namespace Engine
{
	namespace
	{
		constexpr wchar_t className[] = L"Engine.GameBase";

		// Longer frames (the window is dragged or minimized) count as this long, so that the game skips the pause.
		constexpr int maxFrameTime = 100; // ms
	}

	struct GameBase::Window
	{
		GameBase& game;
		HWND hwnd = nullptr;
		HDC dc = nullptr;
		HGLRC context = nullptr;
		bool comInitialized = false;
		bool vsync = false;
		bool sizing = false; // in the modal loop of moving or resizing the window
		Input input;
		std::exception_ptr error; // thrown while handling a message

		explicit Window(GameBase& owner) : game(owner) { }
		~Window();

		void open(const wchar_t* title, int width, int height);
		void render();
		LRESULT onMessage(UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	};

	void GameBase::Window::open(const wchar_t* title, int width, int height)
	{
		comInitialized = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)); // textures are decoded with WIC

		const HINSTANCE instance = GetModuleHandleW(nullptr);
		const WNDCLASSEXW windowClass{
			.cbSize = sizeof(WNDCLASSEXW),
			.style = CS_OWNDC,
			.lpfnWndProc = procedure,
			.hInstance = instance,
			.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1)), // the application icon, if it has one
			.hCursor = LoadCursorW(nullptr, IDC_ARROW),
			.lpszClassName = className,
		};
		RegisterClassExW(&windowClass);

		// In the middle of the screen, and not bigger than it
		const UINT dpi = GetDpiForSystem();
		RECT rect{ 0, 0, MulDiv(width, static_cast<int>(dpi), 96), MulDiv(height, static_cast<int>(dpi), 96) };
		AdjustWindowRectExForDpi(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0, dpi);
		RECT screen{ 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
		SystemParametersInfoW(SPI_GETWORKAREA, 0, &screen, 0);
		const int windowWidth = std::min(rect.right - rect.left, screen.right - screen.left);
		const int windowHeight = std::min(rect.bottom - rect.top, screen.bottom - screen.top);
		CreateWindowExW(0, className, title, WS_OVERLAPPEDWINDOW,
			screen.left + (screen.right - screen.left - windowWidth) / 2, screen.top + (screen.bottom - screen.top - windowHeight) / 2,
			windowWidth, windowHeight, nullptr, nullptr, instance, this);
		if (!hwnd)
			throw std::runtime_error("Cannot create a window");

		RECT client{};
		GetClientRect(hwnd, &client);
		input.width = client.right;
		input.height = client.bottom;

		dc = GetDC(hwnd);
		const PIXELFORMATDESCRIPTOR format{
			.nSize = sizeof(PIXELFORMATDESCRIPTOR),
			.nVersion = 1,
			.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			.iPixelType = PFD_TYPE_RGBA,
			.cColorBits = 32,
			.iLayerType = PFD_MAIN_PLANE,
		};
		if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &format), &format) || !(context = wglCreateContext(dc)) || !wglMakeCurrent(dc, context))
			throw std::runtime_error("Cannot initialize OpenGL");

		using SwapInterval = BOOL(WINAPI*)(int);
		if (const auto swapInterval = reinterpret_cast<SwapInterval>(reinterpret_cast<void (*)()>(wglGetProcAddress("wglSwapIntervalEXT"))))
			vsync = swapInterval(1) != FALSE;

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // textures have premultiplied alpha
		glColor4f(1, 1, 1, 1);
		glClearColor(0.2f, 0.2f, 0.2f, 1);
	}

	GameBase::Window::~Window()
	{
		if (context)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(context);
		}
		if (hwnd)
		{
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
			if (dc)
				ReleaseDC(hwnd, dc);
			DestroyWindow(hwnd);
		}
		if (comInitialized)
			CoUninitialize();
	}

	void GameBase::Window::render()
	{
		if (input.width <= 0 || input.height <= 0)
			return;

		glViewport(0, 0, input.width, input.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, input.width, 0, input.height, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT);
		game.draw(input.width, input.height);
		SwapBuffers(dc);
	}

	LRESULT GameBase::Window::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		const auto mouse = [&] { return Point<float>{ static_cast<float>(static_cast<short>(LOWORD(lParam))), static_cast<float>(input.height - static_cast<short>(HIWORD(lParam))) }; };

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (!(lParam & 0x40000000)) // not an auto-repeat
			{
				const int key = static_cast<int>(wParam);
				input.pressed.push_back(key);
				if (!std::ranges::contains(input.held, key))
					input.held.push_back(key);
			}
			break; // Alt+F4 and the like still work

		case WM_KEYUP:
		case WM_SYSKEYUP:
			std::erase(input.held, static_cast<int>(wParam));
			break;

		case WM_KILLFOCUS:
			input.held.clear(); // the key-up messages go to another window
			break;

		case WM_MOUSEMOVE:
			input.mouse = mouse();
			return 0;

		case WM_LBUTTONDOWN:
			input.mouse = mouse();
			input.clicked = true;
			return 0;

		case WM_SIZE:
			input.width = LOWORD(lParam);
			input.height = HIWORD(lParam);
			if (sizing) // the game loop waits for the resizing to end: keep the picture up to date
			{
				try
				{
					render();
				}
				catch (...)
				{
					error = std::current_exception();
				}
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			sizing = true;
			return 0;

		case WM_EXITSIZEMOVE:
			sizing = false;
			return 0;

		case WM_GETMINMAXINFO:
			reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize = { 320, 240 };
			return 0;

		case WM_DPICHANGED:
		{
			const RECT& rect = *reinterpret_cast<const RECT*>(lParam);
			SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
			return 0;
		}

		case WM_PAINT:
			ValidateRect(hwnd, nullptr); // the game loop redraws the window anyway
			return 0;

		case WM_ERASEBKGND:
			return 1;

		case WM_CLOSE:
			PostQuitMessage(0); // the window is destroyed with the game, after its textures
			return 0;
		}

		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	LRESULT CALLBACK GameBase::Window::procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const auto window = static_cast<Window*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
			window->hwnd = handle;
			SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}

		if (const auto window = reinterpret_cast<Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA)))
			return window->onMessage(message, wParam, lParam);
		return DefWindowProcW(handle, message, wParam, lParam);
	}

	GameBase::GameBase(const wchar_t* title, int width, int height)
		: m_window(std::make_unique<Window>(*this))
	{
		m_window->open(title, width, height);
	}

	GameBase::~GameBase() = default;

	void GameBase::run()
	{
		Window& window = *m_window;
		ShowWindow(window.hwnd, SW_SHOWNORMAL);

		using Clock = std::chrono::steady_clock;
		const Clock::time_point start = Clock::now();
		long long elapsed = 0; // ms

		for (;;)
		{
			MSG message;
			while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
			{
				if (message.message == WM_QUIT)
					return;
				TranslateMessage(&message);
				DispatchMessageW(&message);
			}
			if (window.error)
				std::rethrow_exception(std::exchange(window.error, nullptr));

			if (IsIconic(window.hwnd))
			{
				WaitMessage();
				continue;
			}

			const long long now = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
			const int time = static_cast<int>(std::min<long long>(now - elapsed, maxFrameTime));
			elapsed = now;

			update(time, window.input);
			window.input.pressed.clear();
			window.input.clicked = false;
			window.render();
			if (!window.vsync)
				Sleep(1);
		}
	}
}
