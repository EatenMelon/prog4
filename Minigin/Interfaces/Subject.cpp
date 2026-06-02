#include "Subject.h"
#include "Subject.h"
#include "Observer.h"
#include <algorithm>

bool minigin::ISubject::Subscrube(minigin::IObserver* subscriber)
{
	if (subscriber == nullptr) return false;

	// avoid calling OnNotify of the same observer multiple times
	if (std::find(m_Observers.begin(), m_Observers.end(), subscriber) != m_Observers.end()) return false;

	m_Observers.push_back(subscriber);

	return true;
}

bool minigin::ISubject::UnSubscribe(minigin::IObserver* subscriber)
{
	if (subscriber == nullptr) return false;

	auto itr = std::find(m_Observers.begin(), m_Observers.end(), subscriber);

	if (itr == m_Observers.end()) return false;

	m_Observers.erase(itr);

	return true;
}

void minigin::ISubject::Notify(const IEvent* event)
{
	for (auto& observer : m_Observers)
	{
		observer->OnNotify(event);
	}
}