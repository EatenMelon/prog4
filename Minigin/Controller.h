#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <memory>

namespace dae
{

	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad();
		void ProcessInput();

		bool IsDownThisFrame(unsigned int button) const;
		bool IsUpThisFrame(unsigned int button) const;
		bool IsPressed(unsigned int button) const;

	private:
		class XinputImpl;
		std::unique_ptr<XinputImpl> m_pImpl;
	};
}
