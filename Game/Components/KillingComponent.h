#pragma once
#include <Component.h>
#include <vector>

namespace digdug
{
	class KillingComponent final : public minigin::Component
	{
	public:
		using minigin::Component::Component;

		enum class Target{ Player, Enemy };

		void AddTarget(Target target);
		void RemoveTarget(Target target);
		bool IsTarget(Target target) const;

	private:
		std::vector<Target> m_Targets{};

	};
}