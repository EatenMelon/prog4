#include "ObjectSelector.h"
#include <Component.h>
#include <algorithm>
#include <GameObject.h>
#include <Button.h>

void digdug::ObjectSelector::Start()
{
	UpdatePointer();
}

void digdug::ObjectSelector::SelectForwards()
{
	++m_CurrentlySelected;
	UpdatePointer();
}

void digdug::ObjectSelector::SelectBackwards()
{
	--m_CurrentlySelected;
	UpdatePointer();
}

void digdug::ObjectSelector::Submit()
{
	if (m_Selectables.empty()) return;

	SubmitEvent submit{};
	m_Selectables[m_CurrentlySelected].onSubmit.Notify(submit);
}

minigin::GameObject* digdug::ObjectSelector::GetSelectedObject() const
{
	if (m_Selectables.empty())
	{
		return nullptr;
	}

	return m_Selectables[m_CurrentlySelected].object;
}

bool digdug::ObjectSelector::AddSelectableObject(minigin::GameObject* object)
{
	auto itr = std::find_if
	(
		m_Selectables.begin(),
		m_Selectables.end(),
		[object](auto s)
		{
			return s.object == object;
		}
	);

	if (itr != m_Selectables.end()) return false;

	Selectable newItem{};
	newItem.object = object;

	auto button = object->GetComponent<digdug::Button>();
	if (button != nullptr)
	{
		newItem.onSubmit.Subscribe(button);
	}

	m_Selectables.push_back(newItem);

	return true;
}

bool digdug::ObjectSelector::RemoveSelectableObject(minigin::GameObject* object)
{
	auto itr = std::find_if
	(
		m_Selectables.begin(),
		m_Selectables.end(),
		[object](auto s)
		{
			return s.object == object;
		}
	);

	if (itr == m_Selectables.end()) return false;

	auto button = itr->object->GetComponent<digdug::Button>();
	if (button != nullptr)
	{
		itr->onSubmit.UnSubscribe(button);
	}

	m_Selectables.erase(itr);
	ClampSelection();

	return false;
}

minigin::Subject* digdug::ObjectSelector::GetSubmitEvent(minigin::GameObject* object)
{
	auto itr = std::find_if
	(
		m_Selectables.begin(),
		m_Selectables.end(),
		[object](auto s)
		{
			return s.object == object;
		}
	);

	if (itr == m_Selectables.end()) return nullptr;

	return &itr->onSubmit;
}

void digdug::ObjectSelector::ClampSelection()
{
	m_CurrentlySelected = std::clamp(m_CurrentlySelected, size_t{ 0 }, m_Selectables.size() - 1);
}

void digdug::ObjectSelector::UpdatePointer()
{
	if (m_Selectables.empty()) return;

	ClampSelection();

	GetOwner().SetParent(m_Selectables[m_CurrentlySelected].object);
}
