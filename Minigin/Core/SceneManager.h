#pragma once
#include <vector>
#include <memory>
#include <limits>

#include "Scene.h"
#include "Singleton.h"
#include <Subject.h>
#include <Events.h>

namespace minigin
{
	class SelectSceneEvent final : public PlainEvent
	{
	public:
		SelectSceneEvent() : PlainEvent("OnSceneSelect") {}
	};

	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		void CreateScene(const std::function<void(Scene&)>& loader);

		void StartNextFrame();
		void FixedUpdate(float fixedFrameTime);
		void Update(float deltaTime);
		void GuiRender();
		void Render() const;
		void Cleanup();
		bool SetActiveScene(size_t index);
		Scene& GetActiveScene();
		size_t ActiveSceneId() const;
		size_t GetSceneCount() const;

		Subject& SceneSelectEvent() { return m_OnSelectScene; }

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::unique_ptr<Scene>> m_scenes{};

		size_t m_ActiveSceneIdx{ std::numeric_limits<size_t>::max() };
		Subject m_OnSelectScene{};
		bool m_ChangedScene{ false };
	};
}
