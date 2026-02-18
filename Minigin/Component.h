#pragma once
#include <memory>

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		Component(GameObject* const owner);

		virtual void FixedUpdate(float fixedFrameTime);
		virtual void Update(float deltaTime);
		virtual void Render() const;

		void Destroy();

		bool WillBeDestroyed() const;

	protected:
		GameObject& GetOwner() const;

	private:
		GameObject *const m_Owner;
		bool m_Destroyed;
	};
}