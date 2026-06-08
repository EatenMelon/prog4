#pragma once
#include <string>

namespace minigin
{
	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual const std::string& GetEventName() const = 0;
		virtual unsigned int GetEventHash() const = 0;
	};

	class PlainEvent : public IEvent
	{
	public:
		PlainEvent(const std::string& name) 
			: m_Name{ name }, m_Hash{ CreateHashSDBM(name) } 
		{};

		const std::string& GetEventName() const override final { return m_Name; }
		unsigned int GetEventHash() const override final { return m_Hash; }

		static unsigned int CreateHashSDBM(const std::string& string);

	private:
		const std::string m_Name{};
		const unsigned int m_Hash{};
	};

	class Hitbox;
	class HitEvent final : public PlainEvent
	{
	public:
		HitEvent(Hitbox* hitWith)
			: PlainEvent("HitEvent")
			, m_Other{ hitWith }
		{}

		Hitbox* Who() const;

	private:
		Hitbox* m_Other{ nullptr };
	};
}