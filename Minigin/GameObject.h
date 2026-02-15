#pragma once
#include <string>
#include <memory>
#include <vector>
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

		void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);

		template<typename T>
		void AddComponent()
		{
			if (!std::is_base_of_v<Component, T>)
			{
				throw std::runtime_error("Cannot add non-component classes as a component!");
			}

			if (typeid(T) == typeid(Component))
			{
				throw std::runtime_error("Cannot add the base component class as a component!");
			}

			std::shared_ptr<T> newComponent{};
			newComponent = std::make_shared<T>(weak_from_this());

			m_Components.push_back(newComponent);
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

		std::vector<std::shared_ptr<Component>> m_Components{};
	};

}
