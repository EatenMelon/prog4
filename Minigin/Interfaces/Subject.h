#pragma once
#include <vector>

namespace minigin
{
	class IObserver;
	class IEvent;
	class Subject
	{
	public:
		bool Subscrube(IObserver* subscriber);
		bool UnSubscribe(IObserver* subscriber);

	protected:
		void Notify(const IEvent* event);

	private:
		std::vector<IObserver*> m_Observers;

	};
}
