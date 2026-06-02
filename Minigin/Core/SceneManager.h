#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include "Singleton.h"
#include <climits>

namespace minigin
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void FixedUpdate(float fixedFrameTime);
		void Update(float deltaTime);
		void GuiRender();
		void Render() const;
		void Cleanup();
		bool SetActiveScene(size_t index);
		size_t ActiveSceneId() const;

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::unique_ptr<Scene>> m_scenes{};

		size_t m_ActiveSceneIdx{ SIZE_MAX };
	};
}
