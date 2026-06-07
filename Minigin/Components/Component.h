#pragma once

namespace minigin
{
	class GameObject;
	class Component
	{
	public:
		Component(GameObject* owner);

		virtual ~Component() = default;
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		virtual void Init();
		virtual void Start();
		virtual void FixedUpdate(float fixedFrameTime);
		virtual void Update(float deltaTime);
		virtual void GuiRender();
		virtual void Render() const;

		bool Enabled() const { return m_Enabled; }
		void Enable(bool enable) { m_Enabled = enable; }

		void Destroy();

		bool MarkedForDestruction() const;
		GameObject& GetOwner() const;

	private:
		bool m_Enabled{ true };

		GameObject* m_Owner;
		bool m_Destroyed;
	};
}