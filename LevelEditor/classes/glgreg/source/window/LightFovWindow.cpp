#ifdef _EDITOR
#include "window\LightFovWindow.h"
#include "entities\LightEntity.h"
#include "window\SpatialWindow.h"

BasicWindow lightFovWindow;
HWND lightFovText, lightFovEdit;

bool createLightFovWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = LightFovWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("lightFovWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	lightFovWindow.win = CreateWindow(TEXT("lightFovWindow"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, LIGHT_FOV_TEXT_WIDTH + LIGHT_FOV_EDIT_WIDTH, LIGHT_FOV_TEXT_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(lightFovWindow.win, SW_HIDE);
	lightFovWindow.update = updateLightFovWindow;
	lightFovWindow.reset = resetLightFovWindow;
	return true;
}

void updateLightFovWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	char* fov_c = getEditField(&lightFovEdit);
	float fov_float = atof(fov_c);
	if (fov_float < 0.0 || fov_float > 180)
		fov_float = 90.0;

	((FrustumLight*)e)->fov = fov_float;

	resetLightFovWindow(e, s);
}

void resetLightFovWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	SendMessage(lightFovEdit, EM_SETSEL, 0, SendMessage(lightFovEdit, EM_LINELENGTH, 0, 0));
	SendMessage(lightFovEdit, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", ((FrustumLight*)e)->fov);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(lightFovEdit, EM_REPLACESEL, 0, (LPARAM)p);
}

LRESULT CALLBACK LightFovWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			lightFovText = CreateWindow(TEXT("Static"), TEXT("FOV:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				0, 0, LIGHT_FOV_TEXT_WIDTH, LIGHT_FOV_TEXT_HEIGHT,
				hWnd, NULL, GetModuleHandle(NULL), NULL);

			lightFovEdit = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
				LIGHT_FOV_TEXT_WIDTH, 0, LIGHT_FOV_EDIT_WIDTH, LIGHT_FOV_TEXT_HEIGHT,
				hWnd, NULL, GetModuleHandle(NULL), NULL);

			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#endif