#pragma once


#include <Windows.h>


bool state[0xff] = { false };

bool keyDown(int key) {
	if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (GetAsyncKeyState(key))) {
		if (!state[key]) {
			state[key] = true;
		}
		return false;
	}
	else {
		if (state[key]) {
			state[key] = false;
			return true;
		}
		else {
			return false;
		}
	}
}
