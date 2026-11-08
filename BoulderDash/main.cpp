#include "Engine/platform.h"
#include "Game/game.h"

#include <exception>
#include <filesystem>

namespace
{
	bool hasAssets(const std::filesystem::path& directory)
	{
		return std::filesystem::is_directory(directory / "Gfx") && std::filesystem::is_directory(directory / "Levels");
	}

	// Makes the folder with Gfx and Levels the current one: Visual Studio starts the game there; otherwise it is
	// the folder of the executable or one above it (bin\x64\Release\BoulderDash.exe).
	void findAssets()
	{
		if (hasAssets(std::filesystem::current_path()))
			return;

		for (std::filesystem::path directory = Engine::executableDirectory(); !directory.empty(); directory = directory.parent_path())
		{
			if (hasAssets(directory))
			{
				std::filesystem::current_path(directory);
				return;
			}
			if (directory == directory.parent_path())
				return;
		}
	}
}

int main()
{
	try
	{
		findAssets();
		BoulderDash::Game game;
		game.run();
		return 0;
	}
	catch (const std::exception& error)
	{
		Engine::showError(error.what());
		return 1;
	}
}
