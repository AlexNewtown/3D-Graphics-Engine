#include "window\IndexOfRefractionWindow.h"

#ifdef _EDITOR

BasicWindow indexOfRefractionWindow;
HWND indexOfRefractionText;
HWND indexOfRefractionEdit;

bool createIndexOfRefractionWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = indexOfRefractionCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("IndexOfRefraction");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	indexOfRefractionWindow.win = CreateWindow(TEXT("IndexOfRefraction"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, INDEX_OF_REFRACTION_TEXT_WIDTH, 2* INDEX_OF_REFRACTION_TEXT_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	indexOfRefractionWindow.reset = indexOfRefractionWindowReset;
	indexOfRefractionWindow.update = indexOfRefractionWindowUpdate;
	ShowWindow(indexOfRefractionWindow.win, SW_HIDE);
}
LRESULT CALLBACK indexOfRefractionCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			indexOfRefractionText = CreateWindow(TEXT("Static"), TEXT("Index of Refraction:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				0, 0, INDEX_OF_REFRACTION_TEXT_WIDTH, INDEX_OF_REFRACTION_TEXT_HEIGHT,
				hWnd, NULL, GetModuleHandle(NULL), NULL);

			indexOfRefractionEdit = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
				0, INDEX_OF_REFRACTION_TEXT_HEIGHT, INDEX_OF_REFRACTION_TEXT_WIDTH, INDEX_OF_REFRACTION_TEXT_HEIGHT,
				hWnd, NULL, GetModuleHandle(NULL), NULL);
			
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void indexOfRefractionWindowUpdate(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	char* ior_c = getEditField(&indexOfRefractionEdit);
	float ior_float = atof(ior_c);
	((Model_obj*)e)->indexOfRefraction = ior_float;
}
void indexOfRefractionWindowReset(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	SendMessage(indexOfRefractionEdit, EM_SETSEL, 0, SendMessage(indexOfRefractionEdit, EM_LINELENGTH, 0, 0));
	SendMessage(indexOfRefractionEdit, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", ((Model_obj*)e)->indexOfRefraction);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(indexOfRefractionEdit, EM_REPLACESEL, 0, (LPARAM)p);
}

#endif