#pragma once
#ifdef _EDITOR

#include <windows.h>
#include "entities\Entity.h"
#include "BasicWindow.h"
#include "LevelEditorGlobals.h"

#define ABSORPTION_WINDOW_X 0
#define ABSORPTION_WINDOW_Y 400
#define ABSORPTION_WINDOW_WIDTH 220
#define ABSORPTION_WINDOW_HEIGHT 100

#define ABSORPTION_HEADER_TEXT_WIDTH 60
#define ABSORPTION_HEADER_TEXT_HEIGHT 20

#define ABSORPTION_TEXT_WIDTH 80
#define ABSORPTION_TEXT_HEIGHT 20
#define ABSORPTION_ENTRY_WIDTH 60
#define ABSORPTION_ENTRY_HEIGHT 20
#define ABSORPTION_PADDING_ENTRY_Y 6

extern BasicWindow absorptionWindow;

extern HWND absorptionEditPositionHeader;
extern HWND absorptionEditRText;
extern HWND absorptionEditR;
extern HWND absorptionEditGText;
extern HWND absorptionEditG;
extern HWND absorptionEditBText;
extern HWND absorptionEditB;

bool createAbsorptionWindow(HWND parent);
LRESULT CALLBACK absorptionWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void addAbsorptionItems(HWND hWnd);
char* getAbsorptionField(HWND* win);
void updateEntityAbsorptionData(Entity* e);
void populateAbsorptionEntryFields(Entity* e);
void absorptionWindowUpdate(Entity* e, Scene* s);
void absorptionWindowReset(Entity* e, Scene* s);




#define REDUCED_SCATTERING_WINDOW_X 0
#define REDUCED_SCATTERING_WINDOW_Y 400
#define REDUCED_SCATTERING_WINDOW_WIDTH 220
#define REDUCED_SCATTERING_WINDOW_HEIGHT 100

#define REDUCED_SCATTERING_HEADER_TEXT_WIDTH 60
#define REDUCED_SCATTERING_HEADER_TEXT_HEIGHT 20

#define REDUCED_SCATTERING_TEXT_WIDTH 150
#define REDUCED_SCATTERING_TEXT_HEIGHT 20
#define REDUCED_SCATTERING_ENTRY_WIDTH 60
#define REDUCED_SCATTERING_ENTRY_HEIGHT 20
#define REDUCED_SCATTERING_PADDING_ENTRY_Y 6

extern BasicWindow reducedScatteringWindow;

extern HWND reducedScatteringEditPositionHeader;
extern HWND reducedScatteringEditRText;
extern HWND reducedScatteringEditR;
extern HWND reducedScatteringEditGText;
extern HWND reducedScatteringEditG;
extern HWND reducedScatteringEditBText;
extern HWND reducedScatteringEditB;

bool createReducedScatteringWindow(HWND parent);
LRESULT CALLBACK reducedScatteringWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void addReducedScatteringItems(HWND hWnd);
char* getReducedScatteringField(HWND* win);
void updateEntityReducedScatteringData(Entity* e);
void populateReducedScatteringEntryFields(Entity* e);
void reducedScatteringWindowUpdate(Entity* e, Scene* s);
void reducedScatteringWindowReset(Entity* e, Scene* s);

#endif