#pragma once
#ifdef _EDITOR

#include "BasicWindow.h"
#define LIGHT_FOV_TEXT_WIDTH 50
#define LIGHT_FOV_TEXT_HEIGHT 22
#define LIGHT_FOV_EDIT_WIDTH 60

extern BasicWindow lightFovWindow;
extern HWND lightFovText, lightFovEdit;

bool createLightFovWindow(HWND parent);

void updateLightFovWindow(Entity* e, Scene* s);
void resetLightFovWindow(Entity* e, Scene* s);
LRESULT CALLBACK LightFovWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif