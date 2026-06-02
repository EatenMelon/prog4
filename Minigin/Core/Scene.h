#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"

namespace minigin
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void FixedUpdate(float fixedFrameTime);
		void Update(float deltaTime);
		void GuiRender() const;
		void Render() const;
		void Cleanup();

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector < std::unique_ptr<GameObject>> m_objects{};
	};

}
