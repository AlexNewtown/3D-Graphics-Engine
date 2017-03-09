#ifdef _EDITOR
#include "window\LightTypeDropDownWindow.h"

BasicWindow lightTypeDropDownWindow;
HWND lightTypeDropDown, lightTypeText;

bool createLightTypeDropDownWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = lightTypeDropDownWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("lightTypeDropDown");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	lightTypeDropDownWindow.win = CreateWindow(TEXT("lightTypeDropDown"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, LIGHT_TYPE_TEXT_WIDTH + LIGHT_TYPE_DROP_DOWN_WIDTH, LIGHT_TYPE_TEXT_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(lightTypeDropDownWindow.win, SW_HIDE);
	lightTypeDropDownWindow.update = updateLightTypeDropDownWindow;
	lightTypeDropDownWindow.reset = resetLightTypeDropDownWindow;
	return true;
}

void updateLightTypeDropDownWindow(Entity* e, Scene* s)
{

}

void resetLightTypeDropDownWindow(Entity* e, Scene* s)
{

}

LRESULT CALLBACK lightTypeDropDownWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		lightTypeText = CreateWindow(TEXT("Static"), TEXT("Light Type:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, LIGHT_TYPE_TEXT_WIDTH, LIGHT_TYPE_TEXT_HEIGHT,
			hWnd, NULL, GetModuleHandle(NULL), NULL);

		lightTypeDropDown = CreateWindow(TEXT("ComboBox"), NULL,
			CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL,
			LIGHT_TYPE_TEXT_WIDTH, 0, LIGHT_TYPE_DROP_DOWN_WIDTH, LIGHT_TYPE_DROP_DOWN_HEIGHT, hWnd, NULL, GetModuleHandle(NULL), NULL);

		TCHAR A[40];
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Surface Light"));
		SendMessage(lightTypeDropDown, (UINT)CB_ADDSTRING, (WPARAM)LIGHT_TYPE_DROP_DOWN_SURFACE_LIGHT_INDEX, (LPARAM)A);
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Point Light"));
		SendMessage(lightTypeDropDown, (UINT)CB_ADDSTRING, (WPARAM)LIGHT_TYPE_DROP_DOWN_POINT_LIGHT_INDEX, (LPARAM)A);
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Directional Light"));
		SendMessage(lightTypeDropDown, (UINT)CB_ADDSTRING, (WPARAM)LIGHT_TYPE_DROP_DOWN_DIRECTIONAL_LIGHT_INDEX, (LPARAM)A);
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Frustum Light"));
		SendMessage(lightTypeDropDown, (UINT)CB_ADDSTRING, (WPARAM)LIGHT_TYPE_DROP_DOWN_FRUSTUM_LIGHT_INDEX, (LPARAM)A);

		SendMessage(lightTypeDropDown, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		break;
	}
	case WM_COMMAND:
	{
		if (lParam == (LPARAM)lightTypeDropDown)
		{
			if (currentSelectedEntity == NULL)
				break;

			int itemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);

			switch (itemIndex)
			{
				case LIGHT_TYPE_DROP_DOWN_SURFACE_LIGHT_INDEX:
				{
					((LightEntity*)currentSelectedEntity)->lightType = SURFACE_LIGHT;

					break;
				}
				case LIGHT_TYPE_DROP_DOWN_POINT_LIGHT_INDEX:
				{
					((LightEntity*)currentSelectedEntity)->lightType = POINT_LIGHT;
					//windowSettingIndex = getLightWindowsIndex(POINT_LIGHT);
					updateWindowSetting(windowSettingIndex);
					break;
				}
				case LIGHT_TYPE_DROP_DOWN_DIRECTIONAL_LIGHT_INDEX:
				{
					((LightEntity*)currentSelectedEntity)->lightType = DIRECTIONAL_LIGHT;
					//windowSettingIndex = getLightWindowsIndex(DIRECTIONAL_LIGHT);
					updateWindowSetting(windowSettingIndex);
					break;
				}
				case LIGHT_TYPE_DROP_DOWN_FRUSTUM_LIGHT_INDEX:
				{
					/* COPY RIGHT!!*/
					FrustumLight* fl = new FrustumLight(90, 16.0 / 9.0, FRUSTUM_LIGHT_NEAR_PLANE, FRUSTUM_LIGHT_FAR_PLANE);
					levelEditorScene->removeLightEntity(currentSelectedEntity);
					levelEditorScene->addLightEntity(fl);
					currentSelectedEntity = fl;
					//windowSettingIndex = getLightWindowsIndex(FRUSTUM_LIGHT);
					updateWindowSetting(windowSettingIndex);
					/* FIX ERROR */
					//delete currentSelectedEntity;
					break;
				}
			}
			
		}
	}

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

#endif