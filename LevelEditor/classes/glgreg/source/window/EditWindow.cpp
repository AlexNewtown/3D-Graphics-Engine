
//#include "window\EditWindow.h"
#include "window\EditWindow.h"
#include <Windows.h>

#ifdef _EDITOR

BasicWindow editWindow;
HWND updateSceneButton = NULL;
std::vector<BasicWindow*> staticEditWindows;
std::vector<BasicWindow*> nonstaticEditWindows;

bool createEditWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = createEditWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("EditWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	editWindow.win = CreateWindow(TEXT("EditWindow"), TEXT("Edit"), WS_VISIBLE, SCREEN_WIDTH + 20, 0,
		EDIT_POPOUT_WIDTH, EDIT_POPOUT_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);

	editWindow.update = updateEditWindow;
	editWindow.reset = resetEditWindow;
	ShowWindow(editWindow.win, SW_SHOW);
}

void buttonHit()
{
	updateEditWindow(currentSelectedEntity, levelEditorScene);
}

LRESULT CALLBACK createEditWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

		case WM_CREATE:
		{
#ifdef _EDITOR

			createEntityBoxWindow(hWnd);
			createSpatialWindow(hWnd);

			createLightIntensityWindow(hWnd);
			createBrdfDropDownWindow(hWnd);
			createIndexOfRefractionWindow(hWnd);
			createMicrofacetRoughnessWindow(hWnd);
			createLightFovWindow(hWnd);
			createLightTypeDropDownWindow(hWnd);
			createLightAspectRatioWindow(hWnd);
			createParameterBoxWindow(hWnd);
			createAbsorptionWindow(hWnd);
			createReducedScatteringWindow(hWnd);

			staticEditWindows.push_back(&entityBoxWindow);
			staticEditWindows.push_back(&spatialWindow);
			staticEditWindows.push_back(&parameterBoxWindow);



			nonstaticEditWindows.push_back(&lightIntensityWindow);
			nonstaticEditWindows.push_back(&brdfDropDownWindow);
			nonstaticEditWindows.push_back(&indexOfRefractionWindow);
			nonstaticEditWindows.push_back(&microfacetRoughnessWindow);
			nonstaticEditWindows.push_back(&lightAspectRatioWindow);
			nonstaticEditWindows.push_back(&lightTypeDropDownWindow);
			nonstaticEditWindows.push_back(&absorptionWindow);
			nonstaticEditWindows.push_back(&reducedScatteringWindow);

			parameterStrings.push_back(std::string("Light Intensity"));
			parameterStrings.push_back(std::string("BRDF type"));
			parameterStrings.push_back(std::string("Index of Refraction"));
			parameterStrings.push_back(std::string("Microfacet Roughness"));
			parameterStrings.push_back(std::string("Light Aspect Ratio"));
			parameterStrings.push_back(std::string("Light Type"));
			parameterStrings.push_back(std::string("Absorption Coefficient"));
			parameterStrings.push_back(std::string("Reduced Scattering Coefficient"));


			updateSceneButton = CreateWindow(TEXT("Button"), TEXT("update"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, EDIT_POPOUT_HEIGHT - 100 - UPDATE_BUTTON_HEIGHT, UPDATE_BUTTON_WIDTH, UPDATE_BUTTON_HEIGHT,
				hWnd, (HMENU)UPDATE_BUTTON_INDEX, GetModuleHandle(NULL), NULL);
	#endif
			break;
			
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == UPDATE_BUTTON_INDEX)
			{
				buttonHit();
			}
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void updateEditWindow(Entity* e, Scene* s)
{
	for (int i = 0; i < staticEditWindows.size(); i++)
	{
		staticEditWindows[i]->update(e,s);
	}
	for (int i = 0; i < nonstaticEditWindows.size(); i++)
	{
		nonstaticEditWindows[i]->update(e, s);
	}
}
void resetEditWindow(Entity* e, Scene* s)
{
	for (int i = 0; i < staticEditWindows.size(); i++)
	{
		staticEditWindows[i]->reset(e, s);
	}
	for (int i = 0; i < nonstaticEditWindows.size(); i++)
	{
		nonstaticEditWindows[i]->reset(e,s);
	}
}

#endif