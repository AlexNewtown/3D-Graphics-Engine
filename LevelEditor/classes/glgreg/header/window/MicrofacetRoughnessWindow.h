#pragma once
#include "BasicWindow.h"
#include <windows.h>
#include "SpatialWindow.h"

#ifdef _EDITOR

#define MICROFACET_ROUGHNESS_TEXT_WIDTH 90
#define MICROFACET_ROUGHNESS_EDIT_WIDTH 60
#define MICROFACET_ROUGHNESS_TEXT_HEIGHT 20
#define MICROFACET_ROUGHNESS_EDIT_HEIGHT 20


extern HWND microfacetRoughnessText;
extern HWND microfacetRoughnessEdit;

extern BasicWindow microfacetRoughnessWindow;

bool createMicrofacetRoughnessWindow(HWND parent);
LRESULT CALLBACK microfacetRoughnessWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void microfacetRoughnessWindowUpdate(Entity *e, Scene* s);
void microfacetRoughnessWindowReset(Entity* e, Scene* s);

#endif