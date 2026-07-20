#pragma once

namespace GameEngine {
	inline const int DEFAULT_WIDTH = 1280;
	inline const int DEFAULT_HEIGHT = 720;
	inline const float BACKGROUND_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	HWND GetWindowHandle();
	float GetDeltaTime();
}