#pragma once
#include <memory>
#include <Component.h>
#include <SpriteSheet.h>
#include <Observer.h>
#include <Subject.h>

namespace minigin
{
	class RenderComponent;
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
		bool IsInflated() const { return m_Inflation < 0; }

	private:
		minigin::Subject m_OnPopEvent{};

		minigin::RenderComponent* m_RenderComp{ nullptr };
		std::unique_ptr<minigin::SpriteSheet> m_SpriteSheet{ nullptr };

		const float m_DeflationDelay{ 1.f };
		float m_TimeUntilDeflate{ 0.f };

		int m_Inflation{ -1 };
		const int m_MaxInflation{ 4 };
	};
}