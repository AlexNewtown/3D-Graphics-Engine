#pragma once

#ifdef _EDITOR

#include <Windows.h>
#include "BasicWindow.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "EditWindow.h"

#define PARAMETER_BOX_WINDOW_WIDTH 220
#define PARAMETER_BOX_WINDOW_HEIGHT 160
#define PARAMETER_BOX_WINDOW_X 0
#define PARAMETER_BOX_WINDOW_Y 0

#define PARAMETER_BOX_LIST_WIDTH 200
#define PARAMETER_BOX_LIST_HEIGHT 120

#define PARAMETER_BOX_TEXT_WIDTH 200
#define PARAMETER_BOX_TEXT_HEIGHT 20

#define PARAMETER_LIST_BOX_INDEX 0


extern BasicWindow parameterBoxWindow;
extern HWND parameterBox;
extern HWND parameterBoxText;
extern int currentSelectedParameter;
extern std::vector<std::string> parameterStrings;

bool createParameterBoxWindow(HWND parent);
LRESULT CALLBACK createParameterBoxWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void parameterBoxWindowUpdate(Entity* e, Scene* s);
void parameterBoxWindowReset(Entity* e, Scene* s);
void populateParameterListBox();

#endif