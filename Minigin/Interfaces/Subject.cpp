#include "Subject.h"
#include "Observer.h"
#include "Events.h"
#include <algorithm>
#include <iostream>

bool minigin::Subject::Subscribe(minigin::IObserver* subscriber)
{
	if (subscriber == nullptr) return false;

	// avoid calling OnNotify of the same observer multiple times
	if (std::find(m_Observers.begin(), m_Observers.end(), subscriber) != m_Observers.end()) return false;
	
	m_Observers.push_back(subscriber);

	return true;
}

bool minigin::Subject::UnSubscribe(minigin::IObserver* subscriber)
{
	if (subscriber == nullptr) return false;
	if (m_Observers.empty()) return false;

	auto itr = std::find(m_Observers.begin(), m_Observers.end(), subscriber);

	if (itr == m_Observers.end()) return false;

	m_Observers.erase(itr);

	return true;
}

void minigin::Subject::Notify(const IEvent& event)
{
	for (auto& observer : m_Observers)
	{
		try
		{
			observer->OnNotify(event);
		}
		catch (const std::exception& e)
		{
			std::cout << "ERROR: Subject::Notify, " << e.what() << "\n";
		}
	}
}