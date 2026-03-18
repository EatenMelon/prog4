#pragma once
#include <string>
#include <vector>
#include "Events.h"

namespace dae
{
	class Observer;
	class Subject
	{
	public:
		bool Subscrube(Observer* subscriber);
		bool UnSubscribe(Observer* subscriber);

	protected:
		void Notify(Event event);

	private:
		std::vector<Observer*> m_Observers;

	};
}
