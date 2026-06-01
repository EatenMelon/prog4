#pragma once
#include <string>
#include <vector>
#include "Events.h"

namespace minigin
{
	class IObserver;
	class ISubject
	{
	public:
		bool Subscrube(IObserver* subscriber);
		bool UnSubscribe(IObserver* subscriber);

	protected:
		void Notify(IEvent event);

	private:
		std::vector<IObserver*> m_Observers;

	};
}
