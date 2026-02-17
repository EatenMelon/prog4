#pragma once
#include <memory>

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		Component(std::weak_ptr<GameObject> owner);

		virtual void FixedUpdate(float fixedFrameTime);
		virtual void Update(float deltaTime);
		virtual void Render() const;

		void Destroy();

		bool WillBeDestroyed() const;

	protected:
		GameObject& GetOwner() const;

	private:
		std::weak_ptr<GameObject> m_Owner;
		bool m_Destroyed;
	};
}