#pragma once
#include <Component.h>
#include <Observer.h>

#include <functional>

namespace digdug
{
	class Button final : public minigin::Component, public minigin::IObserver
	{
	public:
		using minigin::Component::Component;

		void Start();
		void OnNotify(const minigin::IEvent& event) override;
		void SetSubmitFunction(const std::function<void()>& f);

	private:
		std::function<void()> m_Function{};
		unsigned int m_SubmitEventHash{};
	};
}