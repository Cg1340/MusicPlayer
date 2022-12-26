#pragma once


#include <Windows.h>


bool state[3] = { false };

bool keyDown(int key) {
	switch (key) {
	case 0x31:

		if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (GetAsyncKeyState(0x31))) {
			if (!state[0]) {
				state[0] = true;
			}
			return false;
		}
		else {
			if (state[0]) {
				state[0] = false;
				return true;
			}
			else {
				return false;
			}
		}
		break;
	case 0x32:
		if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (GetAsyncKeyState(0x32))) {
			if (!state[1]) {
				state[1] = true;
			}
			return false;
		}
		else {
			if (state[1]) {
				state[1] = false;
				return true;
			}
			else {
				return false;
			}
		}
		break;
	case 0x33:
		if ((GetAsyncKeyState(VK_LMENU) & 0x8000) && (GetAsyncKeyState(0x33))) {
			if (!state[2]) {
				state[2] = true;
			}
			return false;
		}
		else {
			if (state[2]) {
				state[2] = false;
				return true;
			}
			else {
				return false;
			}
		}
		break;
	}
}
