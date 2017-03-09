#pragma once

#ifdef _EDITOR

#include <windows.h>
#include "BasicWindow.h"
#include "LevelEditorGlobals.h"
#include "entities/Model.h"

#define ENTITY_BOX_WINDOW_WIDTH 280
#define ENTITY_BOX_WINDOW_HEIGHT 120
#define ENTITY_BOX_WINDOW_X 0
#define ENTITY_BOX_WINDOW_Y 0

#define ENTITY_BOX_LIST_WIDTH 120
#define ENTITY_BOX_LIST_HEIGHT 90

#define ENTITY_BOX_TEXT_WIDTH 120
#define ENTITY_BOX_TEXT_HEIGHT 20

#define ENTITY_LIST_BOX_INDEX 0
#define SELECTED_ENTITY_LIST_BOX_INDEX 1

extern BasicWindow entityBoxWindow;
extern HWND entityBox_entityText;
extern HWND entityBox_entityList;
extern HWND entityBox_selectedEntityText;
extern HWND entityBox_selectedEntityList;

bool createEntityBoxWindow(HWND parent);
LRESULT CALLBACK createEntityBoxWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void entityBoxWindowUpdate(Entity* e, Scene* s);
void entityBoxWindowReset(Entity* e, Scene* s);
void populateEntityListBox(Scene* s);
void populateEntryFields(Entity* e, Entity** populated);
void populateLightEntryFields(LightEntity* e, Entity** populated);
void populateBoundaryEntryFields(SphereBoundaryEntity* e, Entity** populated);

/*
void populateBoundaryEntryFields(SphereBoundaryEntity* e);
void populateLightEntryFields(LightEntity* e);
void populateEntryFields(Entity* e);
*/
#endif