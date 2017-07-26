#pragma once
#ifdef _EDITOR

#include <windows.h>
#include "entities\Entity.h"
#include "BasicWindow.h"
#include "LevelEditorGlobals.h"

#define SPATIAL_WINDOW_X 0
#define SPATIAL_WINDOW_Y 400
#define SPATIAL_WINDOW_WIDTH 220
#define SPATIAL_WINDOW_HEIGHT 200

#define SPATIAL_HEADER_TEXT_WIDTH 60
#define SPATIAL_HEADER_TEXT_HEIGHT 20

#define SPATIAL_TEXT_WIDTH 60
#define SPATIAL_TEXT_HEIGHT 20
#define SPATIAL_ENTRY_WIDTH 60
#define SPATIAL_ENTRY_HEIGHT 20
#define SPATIAL_PADDING_ENTRY_Y 6

extern BasicWindow spatialWindow;

extern HWND spatialEditPositionHeader;
extern HWND spatialEditXText;
extern HWND spatialEditX;
extern HWND spatialEditYText;
extern HWND spatialEditY;
extern HWND spatialEditZText;
extern HWND spatialEditZ;


extern HWND spatialRotationHeader;
extern HWND spatialRotXText;
extern HWND spatialRotX;
extern HWND spatialRotYText;
extern HWND spatialRotY;
extern HWND spatialRotZText;
extern HWND spatialRotZ;


extern HWND spatialScaleHeader;
extern HWND spatialScaleXText;
extern HWND spatialScaleX;
extern HWND spatialScaleYText;
extern HWND spatialScaleY;
extern HWND spatialScaleZText;
extern HWND spatialScaleZ;

bool createSpatialWindow(HWND parent);
LRESULT CALLBACK spatialWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void addSpatialItems(HWND hWnd);
char* getEditField(HWND* win);
void updateEntitySpatialData(Entity* e);
void populateSpatialEntryFields(Entity* e);
void spatialWindowUpdate(Entity* e, Scene* s);
void spatialWindowReset(Entity* e, Scene* s);

#endif