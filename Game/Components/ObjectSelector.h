#pragma once
#include <Component.h>
#include <vector>
#include <Subject.h>
#include <Events.h>

namespace digdug
{
	class SubmitEvent final : public minigin::PlainEvent
	{
	public:
		SubmitEvent() : minigin::PlainEvent("SubmitEvent") {}
	};

	class ObjectSelector final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		void Start();
		void SelectForwards();
		void SelectBackwards();
		void Submit();

		minigin::GameObject* GetSelectedObject() const;

		bool AddSelectableObject(minigin::GameObject* object);
		bool RemoveSelectableObject(minigin::GameObject* object);

		minigin::Subject* GetSubmitEvent(minigin::GameObject* object);

	private:
		void ClampSelection();
		void UpdatePointer();

		struct Selectable
		{
			minigin::GameObject* object{ nullptr };
			minigin::Subject onSubmit{};
		};

		std::vector<Selectable> m_Selectables{};
		size_t m_CurrentlySelected{};
	};
}