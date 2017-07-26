#pragma once

#ifdef asdjgnskdjgv

#ifdef _EDITOR

#include "BasicWindow.h"
#include "LevelEditorGlobals.h"

extern BasicWindow progressivePhotonMapWindow;
extern HWND progressivePhotonMapDropDown;

bool createProgressivePhotonMapWindow(HWND parent);

void updateProgressivePhotonMapWindow(Entity* e, Scene* s);
void resetProgressivePhotonMapWindow(Entity* e, Scene* s);
LRESULT CALLBACK progressivePhotonMapWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif

#endif