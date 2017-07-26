#pragma once

#ifdef _EDITOR

#include "BasicWindow.h"
#include <Windows.h>
#include "window\EntityBoxWindow.h"
#include "window\SpatialWindow.h"
#include "window\LightIntensityWindow.h"
#include "window\BrdfDropDownWindow.h"
#include "window\LightFovWindow.h"
#include "window\LightTypeDropDownWindow.h"
#include "window\LightAspectRatioWindow.h"
#include "..\LevelEditorGlobals.h"
#include "window\ParameterBoxWindow.h"
#include "window\ScatteringCoefficientWindow.h"

#define EDIT_POPOUT_WIDTH 300
#define EDIT_POPOUT_HEIGHT 800

#define UPDATE_BUTTON_HEIGHT 30 
#define UPDATE_BUTTON_WIDTH 80

#define UPDATE_BUTTON_INDEX 3

extern BasicWindow editWindow;
extern HWND updateSceneButton;

extern std::vector<BasicWindow*> staticEditWindows;
extern std::vector<BasicWindow*> nonstaticEditWindows;

bool createEditWindow(HWND parent);
void updateEditWindow(Entity* e, Scene* s);
void resetEditWindow(Entity* e, Scene* s);
LRESULT CALLBACK createEditWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void buttonHit();

#endif