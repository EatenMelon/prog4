#pragma once
#include <memory>

#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>

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

		const glm::vec3& GetLocalPosition();
		void SetLocalPosition(float x, float y, float z = 0.f);
		void SetLocalPosition(const glm::vec3& position);

		const glm::vec3& GetWorldPosition();
		void SetWorldPosition(float x, float y, float z = 0.f);
		void SetWorldPosition(const glm::vec3& position);

		void SetPositionDirty();
		void UpdateWorldPosition();

		void Destroy();
		bool MarkedForDestruction() const;

		void SetParent(GameObject* parent, bool keepWorldTransform = false);

		GameObject* GetParent() const;
		size_t GetChildCount() const;
		GameObject* GetChildAt(size_t index) const;
		bool IsChild(GameObject* parent) const;

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
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);

		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_Components{};

		GameObject* m_Parent{ nullptr };
		std::vector<GameObject*> m_Children;

		Transform m_LocalTransform{ };
		Transform m_WorldTransform{ };
		bool m_PositionIsDirty{ false };

		bool m_Destroyed{ false };
	};

}
