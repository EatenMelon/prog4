#pragma once
#include <memory>

#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

#include "Component.h"

namespace dae
{
	class Texture2D;
	class GameObject final : public std::enable_shared_from_this<GameObject> // has to be enabled to use shared_from_this()
	{
	public:
		virtual void FixedUpdate([[maybe_unused]] float fixedFrameTime);
		virtual void Update([[maybe_unused]] float deltaTime);
		virtual void Render() const;
		void Cleanup();

		template<typename T>
		T* AddComponent()
		{
			if (!std::is_base_of_v<Component, T>) return nullptr;
			if (typeid(T) == typeid(Component)) return nullptr;
			if (m_Components.find(typeid(T)) != m_Components.end()) return nullptr;

			try
			{
				// add new component
				std::shared_ptr<T> newComponent{};
				newComponent = std::make_shared<T>(weak_from_this());

				m_Components.emplace(typeid(T), newComponent);

				// return component
				auto comp = m_Components.find(typeid(T));
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

		std::unordered_map<std::type_index, std::shared_ptr<Component>> m_Components{};
	};

}
