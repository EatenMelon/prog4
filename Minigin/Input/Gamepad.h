#pragma once
#include <unordered_map>
#include <memory>

#ifdef WIN32
#include <wtypes.h>
#endif

namespace dae
{
	enum class GamepadButton
	{
		SOUTH = 0,
		EAST = 1,
		WEST = 2,
		NORTH = 3,
		BACK = 4,
		START = 6,
		LEFT_THUMB = 7,
		RIGHT_THUMB = 8,
		LEFT_SHOULDER = 9,
		RIGHT_SHOULDER = 10,
		DPAD_UP = 11,
		DPAD_DOWN = 12,
		DPAD_LEFT = 13,
		DPAD_RIGHT = 14,
		COUNT = 15
	};

	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad();
		void ProcessInput();

		bool IsDownThisFrame(GamepadButton button) const;
		bool IsUpThisFrame(GamepadButton button) const;
		bool IsPressed(GamepadButton button) const;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl{};
	};
}