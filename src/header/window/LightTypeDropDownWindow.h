#pragma once

#ifdef _EDITOR

#include "BasicWindow.h"
#include "LevelEditorGlobals.h"
#include "LevelEditorWindow.h"

#define LIGHT_TYPE_TEXT_WIDTH 90
#define LIGHT_TYPE_DROP_DOWN_WIDTH 120
#define LIGHT_TYPE_TEXT_HEIGHT 22
#define LIGHT_TYPE_DROP_DOWN_HEIGHT 80

#define LIGHT_TYPE_DROP_DOWN_SURFACE_LIGHT_INDEX 0
#define LIGHT_TYPE_DROP_DOWN_POINT_LIGHT_INDEX 1
#define LIGHT_TYPE_DROP_DOWN_DIRECTIONAL_LIGHT_INDEX 2
#define LIGHT_TYPE_DROP_DOWN_FRUSTUM_LIGHT_INDEX 3

extern BasicWindow lightTypeDropDownWindow;
extern HWND lightTypeDropDown,lightTypeText;

bool createLightTypeDropDownWindow(HWND parent);

void updateLightTypeDropDownWindow(Entity* e, Scene* s);
void resetLightTypeDropDownWindow(Entity* e, Scene* s);
LRESULT CALLBACK lightTypeDropDownWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif