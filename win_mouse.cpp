#include <iostream>
#include <lsl_cpp.h>
#include <string>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static lsl::stream_outlet *outletButtons{nullptr}, *outletPosition{nullptr};
static HHOOK mouseHook = nullptr;

LRESULT CALLBACK mouse_callback(int code, WPARAM wParam, LPARAM lParam) {
	if (code >= 0 && outletButtons && outletPosition) {
		const auto *ev = (MSLLHOOKSTRUCT *)lParam;
		if (wParam == WM_MOUSEMOVE) {
			const int pos[] = {ev->pt.x, ev->pt.y};
			outletPosition->push_sample(pos);
			return CallNextHookEx(mouseHook, code, wParam, lParam);
		}

		std::string evstr;
		const short mouseUpper = ev->mouseData >> 16;
		switch (wParam) {
		case WM_LBUTTONDOWN: evstr = "MouseButtonLeft pressed"; break;
		case WM_LBUTTONUP: evstr = "MouseButtonLeft released"; break;
		case WM_RBUTTONDOWN: evstr = "MouseButtonRight pressed"; break;
		case WM_RBUTTONUP: evstr = "MouseButtonRight released"; break;
		case WM_MBUTTONDOWN: evstr = "MouseButtonMiddle pressed"; break;
		case WM_MBUTTONUP: evstr = "MouseButtonMiddle released"; break;
		case WM_XBUTTONDOWN:
			evstr = "MouseButtonX" + std::to_string(ev->mouseData >> 16) + " pressed";
			break;
		case WM_XBUTTONUP:
			evstr = "MouseButtonX" + std::to_string(ev->mouseData >> 16) + " released";
			break;
		case WM_MOUSEWHEEL:
			if (mouseUpper > 0)
				evstr = "MouseWheelUp" + std::to_string(mouseUpper) + " pressed";
			else
				evstr = "MouseWheelDown" + std::to_string(-mouseUpper) + " pressed";
			break;
		case WM_MOUSEHWHEEL:
			if (mouseUpper < 0)
				evstr = "MouseWheelLeft" + std::to_string(-mouseUpper) + " pressed";
			else
				evstr = "MouseWheelRight" + std::to_string(mouseUpper) + " pressed";
			break;
		default: return CallNextHookEx(mouseHook, code, wParam, lParam);
		}
		outletButtons->push_sample(&evstr);
	}
	return CallNextHookEx(mouseHook, code, wParam, lParam);
}

int main(int argc, char *argv[]) {
	try {
		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouse_callback, GetModuleHandle(nullptr), 0);
		if (!mouseHook)
			throw std::runtime_error(
				"Cannot install mouse hook. Please make sure that this program has the permission "
				"to grab the Windows mouse events.");
		std::cout << "Mouse successfully hooked" << std::endl;
		// create streaminfo and outlet for buttons
		lsl::stream_info infoButtons(
			"MouseButtons", "Markers", 1, lsl::IRREGULAR_RATE, lsl::cf_string);
		outletButtons = new lsl::stream_outlet(infoButtons);
		// create streaminfo and outlet for the position
		lsl::stream_info infoPosition(
			"MousePosition", "Position", 2, lsl::IRREGULAR_RATE, lsl::cf_int32);
		lsl::xml_element setup = infoPosition.desc().append_child("setup");
		lsl::xml_element display = setup.append_child("display");
		display.append_child_value("monitors", std::to_string(GetSystemMetrics(SM_CMONITORS)));
		display.append_child("resolution_primary")
			.append_child_value("X", std::to_string(GetSystemMetrics(SM_CXSCREEN)))
			.append_child_value("Y", std::to_string(GetSystemMetrics(SM_CYSCREEN)));
		display.append_child("resolution_virtual")
			.append_child_value("X", std::to_string(GetSystemMetrics(SM_XVIRTUALSCREEN)))
			.append_child_value("Y", std::to_string(GetSystemMetrics(SM_YVIRTUALSCREEN)));
		lsl::xml_element channels = infoPosition.desc().append_child("channels");
		channels.append_child("channel")
			.append_child_value("label", "MouseX")
			.append_child_value("type", "PositionX")
			.append_child_value("unit", "pixels")
			.append_child_value("origin", "top");
		channels.append_child("channel")
			.append_child_value("label", "MouseY")
			.append_child_value("type", "PositionY")
			.append_child_value("unit", "pixels")
			.append_child_value("origin", "left");
		outletPosition = new lsl::stream_outlet(infoPosition);
		std::cout << "Started streaming data, close this window to stop." << std::endl;
	} catch (std::exception &e) { std::cerr << "Error: " << e.what() << std::endl; }
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (mouseHook > 0) UnhookWindowsHookEx(mouseHook);
}
