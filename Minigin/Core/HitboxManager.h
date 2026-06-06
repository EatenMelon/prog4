#pragma once
#include <Singleton.h>
#include <Observer.h>
#include <vector>

namespace minigin
{
	class Hitbox;
	class HitboxManager final : public Singleton<HitboxManager>, public IObserver
	{
	public:
		void Init();
		void ChechCollisions();
		void OnNotify(const IEvent& event) override;

	private:
		friend class Hitbox;
		bool Join(Hitbox& box);
		void Leave(const Hitbox& box);

		std::vector<Hitbox*> m_Hitboxes{};
		unsigned int m_ResetEventHash{};
	};
}