#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace Engine
{
	// Shows a UTF-8 error message in a message box.
	void showError(std::string_view message);

	// A path as UTF-8 text, for messages.
	std::string utf8(const std::filesystem::path& path);

	std::filesystem::path executableDirectory();
}
