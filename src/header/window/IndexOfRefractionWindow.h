#pragma once

#ifdef _EDITOR
#include <windows.h>
#include "entities/Entity.h"
#include "Scene.h"
#include "BasicWindow.h"
#include "SpatialWindow.h"

#define INDEX_OF_REFRACTION_TEXT_WIDTH 150
#define INDEX_OF_REFRACTION_TEXT_HEIGHT 25

extern BasicWindow indexOfRefractionWindow;
extern HWND indexOfRefractionText;
extern HWND indexOfRefractionEdit;

bool createIndexOfRefractionWindow(HWND parent);
LRESULT CALLBACK indexOfRefractionCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void indexOfRefractionWindowUpdate(Entity* e, Scene* s);
void indexOfRefractionWindowReset(Entity* e, Scene* s);

#endif