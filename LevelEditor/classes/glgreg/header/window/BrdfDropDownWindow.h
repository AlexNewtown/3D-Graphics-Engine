#pragma once

#ifdef _EDITOR

#include "BasicWindow.h"
#include "LevelEditorGlobals.h"

extern BasicWindow brdfDropDownWindow;
extern HWND brdfDropDown;

bool createBrdfDropDownWindow(HWND parent);

void updateBrdfDropDownWindow(Entity* e, Scene* s);
void resetBrdfDropDownWindow(Entity* e, Scene* s);
LRESULT CALLBACK brdfDropDownWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif