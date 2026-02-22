#pragma once
#include <memory>

#include <iostream>
#include <unordered_map>
#include <typeindex>

#include "Component.h"
#include "Transform.h"

namespace dae
{
	class Texture2D;
	class GameObject final // has to be enabled to use shared_from_this()
	{
	public:
		void FixedUpdate(float fixedFrameTime);
		void Update(float deltaTime);
		void Render() const;
		void Cleanup();

		const glm::vec3& GetPosition();
		void SetPosition(float x, float y, float z = 0.f);

		void Destroy();
		bool MarkedForDestruction() const;

		template<typename T>
		T* AddComponent()
		{
			if (!std::is_base_of_v<Component, T>) return nullptr;
			if (typeid(T) == typeid(Component)) return nullptr;
			if (m_Components.find(typeid(T)) != m_Components.end()) return nullptr;

			try
			{
				// add new component
				std::unique_ptr<T> newComponent{};
				newComponent = std::make_unique<T>(this);

				m_Components.emplace(typeid(T), std::move(newComponent));

				// return component
				auto comp = m_Components.find(typeid(T));

				comp->second.get()->Init();

				return static_cast<T*>(comp->second.get());
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error: " << e.what() << "\n";
			}

			return nullptr;
		}

		template<typename T>
		T* GetComponent()
		{
			auto comp = m_Components.find(typeid(T));

			if (comp != m_Components.end())
			{
				return static_cast<T*>(comp->second.get());
			}

			return nullptr;
		}

		GameObject() = default;
		~GameObject() = default;
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:

		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_Components{};

		Transform m_Transform{ };
		bool m_Destroyed{  };
	};

}
