#include "window\ProgressivePhotonMapWindow.h"

#ifdef aff945830495345

BasicWindow progressivePhotonMapWindow;
HWND progressivePhotonMapDropDown;

bool createProgressivePhotonMapWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = progressivePhotonMapWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("progressivePhotonMap");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	progressivePhotonMapWindow.win = CreateWindow(TEXT("progressivePhotonMap"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, 200, 23, parent, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(progressivePhotonMapWindow.win, SW_HIDE);
	progressivePhotonMapWindow.update = updateProgressivePhotonMapWindow;
	progressivePhotonMapWindow.reset = resetProgressivePhotonMapWindow;
	return true;
}

LRESULT CALLBACK progressivePhotonMapWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	progressivePhotonMapDropDown = CreateWindow(TEXT("ComboBox"), NULL,
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL,
		0, 0, 200, 80, hWnd, NULL, GetModuleHandle(NULL), NULL);

	TCHAR A[40];
	wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Alluminum"));
	SendMessage(progressivePhotonMapDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_TORRANCE_SPARROW, (LPARAM)A);
}

void updateProgressivePhotonMapWindow(Entity* e, Scene* s)
{


}

#endif