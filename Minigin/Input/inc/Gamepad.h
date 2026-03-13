#pragma once
#include <unordered_map>
#include <memory>

#include "SDL3/SDL.h"

namespace dae
{

#ifdef WIN32

	enum class GamepadButton
	{
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,

		START = 0x0010,
		BACK = 0x0020,

		LEFT_THUMB = 0x0040,
		RIGHT_THUMB = 0x0080,
		LEFT_SHOULDER = 0x0100,
		RIGHT_SHOULDER = 0x0200,

		EAST = 0x2000,
		SOUTH = 0x1000,
		WEST = 0x4000,
		NORTH = 0x8000
	};

#else

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

#endif

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