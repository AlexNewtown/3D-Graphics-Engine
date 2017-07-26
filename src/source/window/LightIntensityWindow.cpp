#ifdef _EDITOR

#include "window\LightIntensityWindow.h"
#include "entities\LightEntity.h"
#include "window\SpatialWindow.h"

HWND lightIntensityText;
HWND lightIntensityEdit;

BasicWindow lightIntensityWindow ;

bool createLightIntensityWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = lightIntensityWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("lightIntensityWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	int w = (LIGHT_INTENSITY_TEXT_WIDTH > LIGHT_INTENSITY_EDIT_WIDTH) ? LIGHT_INTENSITY_TEXT_WIDTH : LIGHT_INTENSITY_EDIT_WIDTH;
	int h = LIGHT_INTENSITY_TEXT_HEIGHT + LIGHT_INTENSITY_EDIT_HEIGHT;
	lightIntensityWindow.win = CreateWindow(TEXT("lightIntensityWindow"), TEXT("liW"), WS_CHILD, 0, 0,
		w, h, parent, NULL, GetModuleHandle(NULL), NULL);
	lightIntensityWindow.update = lightIntensityWinodwUpdate;
	lightIntensityWindow.reset = lightIntensityWindowReset;
	ShowWindow(lightIntensityWindow.win, SW_SHOW);
}

LRESULT CALLBACK lightIntensityWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		lightIntensityText = CreateWindow(TEXT("Static"), TEXT("Light Intensity"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, LIGHT_INTENSITY_TEXT_WIDTH, LIGHT_INTENSITY_TEXT_HEIGHT,
			hWnd, (HMENU)0, GetModuleHandle(NULL), NULL);

		lightIntensityEdit = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER,
			0, LIGHT_INTENSITY_TEXT_HEIGHT, LIGHT_INTENSITY_EDIT_WIDTH, LIGHT_INTENSITY_EDIT_HEIGHT,
			hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void lightIntensityWinodwUpdate(Entity *e, Scene* s)
{
	if (e == NULL)
		return;

	char* ic = getEditField(&lightIntensityEdit);
	float i = atof(ic);

	((LightEntity*)e)->intensity = i;
	delete ic;
}
void lightIntensityWindowReset(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	SendMessage(lightIntensityEdit, EM_SETSEL, 0, SendMessage(lightIntensityEdit, EM_LINELENGTH, 0, 0));
	SendMessage(lightIntensityEdit, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[280];
	char* fs = new char[280];
	int i = 0;

	sprintf(fs, "%0.3f", ((LightEntity*)e)->intensity);
	for (i = 0; i < 8; i++)
	{
		p[i] = fs[i];
	}
	p[i] = '\0';
	/*
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	*/
	SendMessage(lightIntensityEdit, EM_REPLACESEL, 0, (LPARAM)p);
	delete p;
	delete fs;
}

#endif