#ifdef _EDITOR
#include "window\MicrofacetRoughnessWindow.h"

HWND microfacetRoughnessText;
HWND microfacetRoughnessEdit;

BasicWindow microfacetRoughnessWindow;

bool createMicrofacetRoughnessWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = microfacetRoughnessWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("microfacetRoughnessWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	int w = MICROFACET_ROUGHNESS_TEXT_WIDTH + MICROFACET_ROUGHNESS_EDIT_WIDTH;
	int h = (MICROFACET_ROUGHNESS_TEXT_HEIGHT > MICROFACET_ROUGHNESS_EDIT_HEIGHT) ? MICROFACET_ROUGHNESS_TEXT_HEIGHT : MICROFACET_ROUGHNESS_EDIT_HEIGHT;
	microfacetRoughnessWindow.win = CreateWindow(TEXT("microfacetRoughnessWindow"), TEXT("liW"), WS_CHILD, 0, 0,
		w, h, parent, NULL, GetModuleHandle(NULL), NULL);
	microfacetRoughnessWindow.update = microfacetRoughnessWindowUpdate;
	microfacetRoughnessWindow.reset = microfacetRoughnessWindowReset;
	ShowWindow(microfacetRoughnessWindow.win, SW_SHOW);
}

LRESULT CALLBACK microfacetRoughnessWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		microfacetRoughnessText = CreateWindow(TEXT("Static"), TEXT("Roughness:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, MICROFACET_ROUGHNESS_TEXT_WIDTH, MICROFACET_ROUGHNESS_TEXT_HEIGHT,
			hWnd, (HMENU)0, GetModuleHandle(NULL), NULL);

		microfacetRoughnessEdit = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER,
			MICROFACET_ROUGHNESS_TEXT_WIDTH, 0, MICROFACET_ROUGHNESS_EDIT_WIDTH, MICROFACET_ROUGHNESS_EDIT_HEIGHT,
			hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void microfacetRoughnessWindowUpdate(Entity *e, Scene* s)
{
	if (e == NULL)
		return;

	char* ic = getEditField(&microfacetRoughnessEdit);
	float i = atof(ic);

	((Model_obj*)e)->microfacetM = i;
	delete ic;
}
void microfacetRoughnessWindowReset(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	SendMessage(microfacetRoughnessEdit, EM_SETSEL, 0, SendMessage(microfacetRoughnessEdit, EM_LINELENGTH, 0, 0));
	SendMessage(microfacetRoughnessEdit, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[8];
	char* fs = new char[8];
	int i = 0;

	sprintf(fs, "%0.3f", ((Model_obj*)e)->microfacetM);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(microfacetRoughnessEdit, EM_REPLACESEL, 0, (LPARAM)p);
	delete p;
	delete fs;
}

#endif