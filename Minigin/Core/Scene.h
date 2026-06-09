#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "GameObject.h"

namespace minigin
{
	class Scene final
	{
	public:
		void Load();
		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void FixedUpdate(float fixedFrameTime);
		void Update(float deltaTime);
		void GuiRender() const;
		void Render() const;
		void Cleanup();

		size_t GetSceneId() { return m_Id; }

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene(size_t id, const std::function<void(Scene&)>& sceneLoader)
			: m_Id{ id }
			, m_Loader{ sceneLoader }
		{ }

		std::vector<std::unique_ptr<GameObject>> m_objects{};
		std::queue<std::unique_ptr<GameObject>> m_WaitingObjects{};
		size_t m_Id{ 0 };

		std::function<void(Scene&)> m_Loader{};
	};

}
