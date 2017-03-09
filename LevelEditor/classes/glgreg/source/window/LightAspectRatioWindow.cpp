#ifdef _EDITOR

#include "window\LightAspectRatioWindow.h"
#include "window\SpatialWindow.h"

BasicWindow lightAspectRatioWindow;
HWND lightAspectRatioText, lightAspectRatioEdit;

bool createLightAspectRatioWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = LightAspectRatioWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("lightAspectRatio");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	lightAspectRatioWindow.win = CreateWindow(TEXT("lightAspectRatio"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, LIGHT_ASPECT_RATIO_TEXT_WIDTH + LIGHT_ASPECT_RATIO_EDIT_WIDTH, LIGHT_ASPECT_RATIO_TEXT_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(lightAspectRatioWindow.win, SW_HIDE);
	lightAspectRatioWindow.update = updateLightAspectRatioWindow;
	lightAspectRatioWindow.reset = resetLightAspectRatioWindow;
	return true;
}

void updateLightAspectRatioWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	char* ar_c = getEditField(&lightAspectRatioEdit);
	float ar_float = atof(ar_c);
	if (ar_float < 0.0 )
		ar_float = 1.0;

	((FrustumLight*)e)->aspectRatio = ar_float;

	resetLightAspectRatioWindow(e, s);
}
void resetLightAspectRatioWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	SendMessage(lightAspectRatioEdit, EM_SETSEL, 0, SendMessage(lightAspectRatioEdit, EM_LINELENGTH, 0, 0));
	SendMessage(lightAspectRatioEdit, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", ((FrustumLight*)e)->aspectRatio);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(lightAspectRatioEdit, EM_REPLACESEL, 0, (LPARAM)p);
}
LRESULT CALLBACK LightAspectRatioWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		lightAspectRatioText = CreateWindow(TEXT("Static"), TEXT("Aspect Ratio:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, LIGHT_ASPECT_RATIO_TEXT_WIDTH, LIGHT_ASPECT_RATIO_TEXT_HEIGHT,
			hWnd, NULL, GetModuleHandle(NULL), NULL);

		lightAspectRatioEdit = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
			LIGHT_ASPECT_RATIO_TEXT_WIDTH, 0, LIGHT_ASPECT_RATIO_EDIT_WIDTH, LIGHT_ASPECT_RATIO_TEXT_HEIGHT,
			hWnd, NULL, GetModuleHandle(NULL), NULL);

		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif