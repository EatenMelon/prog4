#pragma once
#include <string>
#include <memory>

#include <iostream>
#include <unordered_map>
#include <typeindex>

#include <stdexcept>

#include "Component.h"
#include "Transform.h"

namespace dae
{
	class Texture2D;
	class GameObject : public std::enable_shared_from_this<GameObject> // has to be enabled to use shared_from_this()
	{
	public:
		virtual void FixedUpdate([[maybe_unused]] float fixedFrameTime);
		virtual void Update([[maybe_unused]] float deltaTime);
		virtual void Render() const;
		void Cleanup();

		void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);

		template<typename T>
		bool AddComponent()
		{
			// works for now, make this cleaner later
			if (!std::is_base_of_v<Component, T>)
			{
				throw std::runtime_error("Cannot add non-component classes as a component!");
			}

			if (typeid(T) == typeid(Component))
			{
				throw std::runtime_error("Cannot add the base component class as a component!");
			}

			if (m_Components.find(typeid(T)) != m_Components.end())
			{
				std::cerr << "Error: GameObject already ahs this component!\n";
				return false;
			}

			try
			{
				std::shared_ptr<T> newComponent{};
				newComponent = std::make_shared<T>(weak_from_this());

				m_Components.emplace(typeid(T), newComponent);

				return true;
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error: " << e.what() << "\n";
			}

			return false;
		}

		template<typename T>
		std::shared_ptr<T> GetComponent()
		{
			if (m_Components.find(typeid(T)) == m_Components.end())
			{
				return m_Components[typeid(T)];
			}

			return std::shared_ptr<T>{};
		}

		GameObject() = default;
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		Transform m_transform{};
		std::shared_ptr<Texture2D> m_texture{};

		std::unordered_map<std::type_index, std::shared_ptr<Component>> m_Components{};
	};

}
