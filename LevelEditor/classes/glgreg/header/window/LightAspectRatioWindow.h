#pragma once
#ifdef _EDITOR

#include "BasicWindow.h"
#define LIGHT_ASPECT_RATIO_TEXT_WIDTH 100
#define LIGHT_ASPECT_RATIO_TEXT_HEIGHT 22
#define LIGHT_ASPECT_RATIO_EDIT_WIDTH 60

extern BasicWindow lightAspectRatioWindow;
extern HWND lightAspectRatioText, lightAspectRatioEdit;

bool createLightAspectRatioWindow(HWND parent);

void updateLightAspectRatioWindow(Entity* e, Scene* s);
void resetLightAspectRatioWindow(Entity* e, Scene* s);
LRESULT CALLBACK LightAspectRatioWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif