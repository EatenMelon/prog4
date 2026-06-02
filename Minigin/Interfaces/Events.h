#pragma once
#include <string>

namespace minigin
{
	// add custom events here
	//enum class IEvent
	//{
	//	ReceivedDamage,
	//	ReceivedPoints
	//};

	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual const std::string& GetEventName() = 0;
		virtual unsigned int GetEventHash() const = 0;
	};

	class PlainEvent : public IEvent
	{
	public:
		PlainEvent(const std::string& name) 
			: m_Name{ name }, m_Hash{ CreateHashSDBM(name) } 
		{};

		const std::string& GetEventName() override final { return m_Name; }
		unsigned int GetEventHash() const override final { return m_Hash; }

		static unsigned int CreateHashSDBM(const std::string& string);

	private:
		const std::string m_Name{};
		const unsigned int m_Hash{};
	};
}