#pragma once
#include <memory>
#include <Component.h>
#include <SpriteSheet.h>
#include <Observer.h>
#include <Subject.h>

namespace minigin
{
	class RenderComponent;
	class GameObject;
}

namespace digdug
{
	// just reusing the namespace
	class InflatablePoppedEvent final : public minigin::PlainEvent
	{
	public:
		InflatablePoppedEvent()
			: minigin::PlainEvent("PoppedEvent")
		{
		}
	};

	class Inflatable final : public minigin::Component, public minigin::IObserver
	{
	public:
		using minigin::Component::Component;
		~Inflatable() override = default;

		void Start() override;
		void Update(float deltaTime) override;
		void Render() const override;

		void OnNotify(const minigin::IEvent& event) override;

		void SetSpriteSheet(const std::string& path);

		minigin::Subject& PoppedEvent() { return m_OnPopEvent; };
		bool IsInflated() const { return m_Inflation >= 0; }

		// get the gameobject that is pumping the inflatable
		minigin::GameObject* GetPumpUser() { return m_PumpUser; }

	private:
		minigin::Subject m_OnPopEvent{};
		unsigned int m_PumpEventHash{ 0 };

		minigin::RenderComponent* m_RenderComp{ nullptr };
		std::unique_ptr<minigin::SpriteSheet> m_SpriteSheet{ nullptr };

		bool m_CanDefalte{ true };
		const float m_DeflationDelay{ 1.f };
		float m_TimeUntilDeflate{ 0.f };
		unsigned int m_DetachPumpEventHash{ 0 };
		minigin::GameObject* m_PumpUser{ nullptr };

		int m_Inflation{ -1 };
		const int m_MaxInflation{ 4 };

		float m_TimeUntilDestroy{ 0.5f };
	};
}