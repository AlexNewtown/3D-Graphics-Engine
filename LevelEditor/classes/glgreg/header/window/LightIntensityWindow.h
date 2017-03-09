#pragma once

#include <windows.h>
#include "BasicWindow.h"

#ifdef _EDITOR

#define LIGHT_INTENSITY_TEXT_WIDTH 120
#define LIGHT_INTENSITY_EDIT_WIDTH 120
#define LIGHT_INTENSITY_TEXT_HEIGHT 20
#define LIGHT_INTENSITY_EDIT_HEIGHT 20


extern HWND lightIntensityText;
extern HWND lightIntensityEdit;

extern BasicWindow lightIntensityWindow;

bool createLightIntensityWindow(HWND parent);
LRESULT CALLBACK lightIntensityWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void lightIntensityWinodwUpdate(Entity *e, Scene* s);
void lightIntensityWindowReset(Entity* e, Scene* s);

#endif