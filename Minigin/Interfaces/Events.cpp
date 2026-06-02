#include "Events.h"

unsigned int minigin::PlainEvent::CreateHashSDBM(const std::string& string)
{
	auto str{ string.c_str() };

	unsigned int hash{ 0 };
	int c{};

	while ((c = *str++))
	{
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}
