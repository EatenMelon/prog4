#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include <Minigin.h>
#include <SceneManager.h>
#include <InputManager.h>

#include <filesystem>
#include <Renderer.h>
#include <LevelLoader.h>
#include <GameManager.h>

namespace fs = std::filesystem;

static void LoadAllScenes()
{
	digdug::GameManager::GetInstance().Init();
	minigin::Renderer::GetInstance().SetBackgroundColor(SDL_Color{ 0, 0, 50, 255 });
}

int main(int, char*[])
{
#if defined(WIN32) && defined(_DEBUG)
	// open console window in debug mode on windows
	AllocConsole();

	// assinging the correct steams to the console
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);
#endif

#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
	{
		data_location = "../Data/";
	}
#endif

	digdug::LevelLoader::GetInstance().Init(data_location/"Levels");
	minigin::Minigin engine(data_location, 1080, 820);
	engine.Run(LoadAllScenes);

	digdug::GameManager::GetInstance().Quit();

    return 0;
}