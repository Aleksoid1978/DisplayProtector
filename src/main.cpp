#ifdef _WIN32
#include <windows.h>
#endif

#include <list>
#include <string>
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

constexpr auto szUsage = "Параметры командной строки:\n"
						 "  --monitor=N\tНомер монитора(экрана), начиная с 1\n"
						 "  --percent=N\tШирина, в % от ширины экрана(1..99)\n"
						 "  --show=N\tВремя отображения, в секундах\n"
						 "  --delay=N\tИнтервал между запусками, в минутах\n"
						 "  --start\tУказывает что сразу запускать(опционально)";

constexpr Sint32 timerCodeStartMovement = 1;
constexpr Sint32 timerCodeMovement = 2;

static void HorizontalGradient(SDL_Surface* surf)
{
	static const SDL_Color c1 = { 0x00, 0x00, 0x00 };
	static const SDL_Color c2 = { 0xFF, 0xFF, 0xFF };

	Uint8 r, g, b;
	SDL_Rect dest;

	auto width = surf->w;
	auto height = surf->h;

	for (int x = 0; x < width; x++) {
		r = (c2.r * x / width) + (c1.r * (width - x) / width);
		g = (c2.g * x / width) + (c1.g * (width - x) / width);
		b = (c2.b * x / width) + (c1.b * (width - x) / width);

		dest.x = x;
		dest.y = 0;
		dest.w = 1;
		dest.h = height;

		auto color = SDL_MapRGB(surf->format, r, g, b);

		SDL_FillRect(surf, &dest, color);
	}
}

static bool StrToInt32(const char* str, int32_t& value)
{
	char* end;
	int32_t v = strtol(str, &end, 10);
	if (end > str) {
		value = v;
		return true;
	}
	return false;
}

static Uint32 timerCallback(Uint32 interval, void* param)
{
	SDL_UserEvent userevent = {};
	userevent.type = SDL_USEREVENT;
	userevent.code = *(reinterpret_cast<Sint32*>(param));

	SDL_Event event = {};
	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return interval;
}

int main (int argc, char** args)
{
	int monitor = 0;
	int percent = 0;
	int display_time = 0;

	int timer_start_movement_period = 0;
	bool bStart = false;

	std::list<std::string> cmdlines;
	for (int i = 1; i < argc; i++) {
		cmdlines.emplace_back(args[i]);
	}

	for (const auto& param : cmdlines) {
		if (param.empty()) {
			continue;
		}

		if (param.size() > 2 && param[0] == '-' && param[1] == '-') {
			const auto key = param.substr(2);
			if (key == "start") {
				bStart = true;
			} else {
				const auto pos = key.find('=');
				if (pos != std::string::npos) {
					const auto _key = key.substr(0, pos);
					const auto _value = key.substr(pos + 1);

					if (_key == "monitor") {
						StrToInt32(_value.c_str(), monitor);
					} else if (_key == "percent") {
						StrToInt32(_value.c_str(), percent);
						if (percent < 1 || percent > 99) {
							percent = 0;
							break;
						}
					} else if (_key == "show") {
						if (StrToInt32(_value.c_str(), display_time)) {
							display_time *= 1000;
						}
					} else if (_key == "delay") {
						if (StrToInt32(_value.c_str(), timer_start_movement_period)) {
							timer_start_movement_period *= 60 * 1000;
						}
					}
				}
			}
		}
	}

	if (!monitor || !percent || !display_time || !timer_start_movement_period) {
#ifdef _WIN32
		SetConsoleOutputCP(65001);
#endif
		std::cerr << szUsage << std::endl;
		return -1;
	}

	monitor--;

	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		return -1;
	}

	if (monitor >= SDL_GetNumVideoDisplays()) {
		monitor = 0;
	}

	SDL_Rect displayBound = {};
	SDL_GetDisplayBounds(monitor, &displayBound);
	auto displayRight = displayBound.x + displayBound.w;

	auto width = displayBound.w * percent / 100;
	auto all_width = width + displayBound.w;
	auto timer_movement_period = display_time / all_width;

	auto window = SDL_CreateWindow("Display Protector",
								   displayBound.x, displayBound.y, 1, displayBound.h,
								   (bStart ? SDL_WINDOW_SHOWN : SDL_WINDOW_HIDDEN) | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
	if (!window) {
		return -1;
	}

	if (auto icon = IMG_Load("DisplayProtector.png")) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	}

	HorizontalGradient(SDL_GetWindowSurface(window));
	SDL_UpdateWindowSurface(window);

	bool bGetStartPos = false;
	SDL_Rect startPos = {};

	SDL_TimerID timerStartMovementID = -1;
	SDL_TimerID timerMovementID = -1;

	if (bStart) {
		timerMovementID = SDL_AddTimer(timer_movement_period,
									   timerCallback,
									   reinterpret_cast<void*>(const_cast<Sint32*>(&timerCodeMovement)));
	} else {
		timerStartMovementID = SDL_AddTimer(timer_start_movement_period,
											timerCallback,
											reinterpret_cast<void*>(const_cast<Sint32*>(&timerCodeStartMovement)));
	}

	for (;;) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT ||
					(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x && (event.key.keysym.mod & KMOD_CTRL))) {
				break;
			} else if (event.type == SDL_USEREVENT) {
				switch (event.user.code) {
					case timerCodeStartMovement:
						SDL_RemoveTimer(timerStartMovementID);
						SDL_SetWindowPosition(window, 0, 0);
						//SDL_ShowWindow(window);
						//SDL_RaiseWindow(window);
						timerMovementID = SDL_AddTimer(timer_movement_period,
													   timerCallback,
													   reinterpret_cast<void*>(const_cast<Sint32*>(&timerCodeMovement)));
						break;
					case timerCodeMovement:
						SDL_Rect pos = {};
						SDL_GetWindowPosition(window, &pos.x, &pos.y);

						if (!bGetStartPos) {
							startPos = pos;
							bGetStartPos = true;
						}

						SDL_Rect size = {};
						SDL_GetWindowSize(window, &size.x, &size.y);

						if (size.x < width && pos.x == startPos.x) {
							size.x += 1;
							SDL_SetWindowSize(window, size.x, size.y);

							HorizontalGradient(SDL_GetWindowSurface(window));
							SDL_UpdateWindowSurface(window);
						} else {
							if (pos.x + size.x == displayRight) {
								size.x -= 1;
								if (size.x == 0) {
									SDL_RemoveTimer(timerMovementID);
									//SDL_HideWindow(window);
									timerStartMovementID = SDL_AddTimer(timer_start_movement_period,
																		timerCallback,
																		reinterpret_cast<void*>(const_cast<Sint32*>(&timerCodeStartMovement)));
									break;
								}
								SDL_SetWindowSize(window, size.x, size.y);

								HorizontalGradient(SDL_GetWindowSurface(window));
								SDL_UpdateWindowSurface(window);
							}

							pos.x += 1;
							SDL_SetWindowPosition(window, pos.x, pos.y);
						}
						break;
				}

				SDL_FlushEvent(SDL_USEREVENT);
			}
		}

		SDL_Delay(5);
	}

	SDL_RemoveTimer(timerStartMovementID);
	SDL_RemoveTimer(timerMovementID);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
};