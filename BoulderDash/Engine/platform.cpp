#include "platform.h"

#include <windows.h>

namespace Engine
{
	void showError(std::string_view message)
	{
		const int length = static_cast<int>(message.size());
		std::wstring text(static_cast<std::size_t>(MultiByteToWideChar(CP_UTF8, 0, message.data(), length, nullptr, 0)), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, message.data(), length, text.data(), static_cast<int>(text.size()));
		MessageBoxW(nullptr, text.c_str(), L"Error", MB_OK | MB_ICONERROR);
	}

	std::string utf8(const std::filesystem::path& path)
	{
		const std::u8string text = path.u8string();
		return { reinterpret_cast<const char*>(text.data()), text.size() };
	}

	std::filesystem::path executableDirectory()
	{
		std::wstring path(MAX_PATH, L'\0');
		for (;;)
		{
			const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
			if (length < path.size())
			{
				path.resize(length);
				return std::filesystem::path(path).parent_path();
			}
			path.resize(path.size() * 2);
		}
	}
}
