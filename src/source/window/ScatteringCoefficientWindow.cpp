#ifdef _EDITOR
#include "../../header/window/ScatteringCoefficientWindow.h"

BasicWindow absorptionWindow;

HWND absorptionEditPositionHeader = NULL;
HWND absorptionEditRText = NULL;
HWND absorptionEditR = NULL;
HWND absorptionEditGText = NULL;
HWND absorptionEditG = NULL;
HWND absorptionEditBText = NULL;
HWND absorptionEditB = NULL;

bool createAbsorptionWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = absorptionWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("AbsorptionWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	absorptionWindow.win = CreateWindow(TEXT("AbsorptionWindow"), TEXT("AW"), WS_CHILD, ABSORPTION_WINDOW_X, ABSORPTION_WINDOW_Y,
		ABSORPTION_WINDOW_WIDTH, ABSORPTION_WINDOW_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	absorptionWindow.update = absorptionWindowUpdate;
	absorptionWindow.reset = absorptionWindowReset;
	ShowWindow(absorptionWindow.win, SW_SHOW);
}

LRESULT CALLBACK absorptionWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		addAbsorptionItems(hWnd);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void addAbsorptionItems(HWND hWnd)
{
	int paddingX = (ABSORPTION_WINDOW_WIDTH - 3 * ABSORPTION_ENTRY_WIDTH) / 2;
	int paddingY = (ABSORPTION_WINDOW_HEIGHT - ABSORPTION_TEXT_HEIGHT - ABSORPTION_ENTRY_HEIGHT - ABSORPTION_HEADER_TEXT_HEIGHT) / 2;

	int posX = 0;
	int posY = 0;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	absorptionEditPositionHeader = CreateWindow(TEXT("Static"), TEXT("Absorption:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, ABSORPTION_TEXT_WIDTH, ABSORPTION_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += ABSORPTION_HEADER_TEXT_HEIGHT;
	absorptionEditRText = CreateWindow(TEXT("Static"), TEXT("R:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, ABSORPTION_TEXT_WIDTH, ABSORPTION_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	absorptionEditGText = CreateWindow(TEXT("Static"), TEXT("G:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + ABSORPTION_ENTRY_WIDTH + paddingX, posY, ABSORPTION_TEXT_WIDTH, ABSORPTION_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	absorptionEditBText = CreateWindow(TEXT("Static"), TEXT("B:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + (ABSORPTION_ENTRY_WIDTH + paddingX) * 2, posY, ABSORPTION_TEXT_WIDTH, ABSORPTION_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += ABSORPTION_TEXT_HEIGHT;
	absorptionEditR = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX, posY, ABSORPTION_ENTRY_WIDTH, ABSORPTION_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	absorptionEditG = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + ABSORPTION_ENTRY_WIDTH + paddingX, posY, ABSORPTION_ENTRY_WIDTH, ABSORPTION_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	absorptionEditB = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + (ABSORPTION_ENTRY_WIDTH + paddingX) * 2, posY, ABSORPTION_ENTRY_WIDTH, ABSORPTION_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);

}

char* getAbsorptionField(HWND* win)
{
	int iLine = SendMessage(*win, EM_LINELENGTH, 0, 0) + 1;
	TCHAR* p = new TCHAR[iLine];
	int iLength = SendMessage(*win, WM_GETTEXT, iLine, (LPARAM)p);
	char* pConvert = new char[iLine];
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, iLine, pConvert, sizeof(char)*(iLine), NULL, NULL);
	pConvert[iLine - 1] = '\0';

	delete p;
	return pConvert;
}

void updateEntityAbsorptionData(Entity* e)
{
	if (e == NULL)
		return;

	char* pX = getAbsorptionField(&absorptionEditR);
	char* pY = getAbsorptionField(&absorptionEditG);
	char* pZ = getAbsorptionField(&absorptionEditB);

	float x = atof(pX);
	float y = atof(pY);
	float z = atof(pZ);

	((Model_obj*)e)->absorption[0] = x;
	((Model_obj*)e)->absorption[1] = y;
	((Model_obj*)e)->absorption[2] = z;

	delete pX;
	delete pY;
	delete pZ;
	return;
}

void populateAbsorptionEntryFields(Entity* e)
{
	SendMessage(absorptionEditR, EM_SETSEL, 0, SendMessage(absorptionEditR, EM_LINELENGTH, 0, 0));
	SendMessage(absorptionEditR, WM_CLEAR, 0, 0);


	SendMessage(absorptionEditG, EM_SETSEL, 0, SendMessage(absorptionEditG, EM_LINELENGTH, 0, 0));
	SendMessage(absorptionEditG, WM_CLEAR, 0, 0);


	SendMessage(absorptionEditB, EM_SETSEL, 0, SendMessage(absorptionEditB, EM_LINELENGTH, 0, 0));
	SendMessage(absorptionEditB, WM_CLEAR, 0, 0);

	TCHAR p[64];
	char fs[64];
	int i = 0;

	sprintf(fs, "%0.3f", ((Model_obj*)e)->absorption[0]);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(absorptionEditR, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", ((Model_obj*)e)->absorption[1]);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(absorptionEditG, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", ((Model_obj*)e)->absorption[2]);
	i = 0;
	while (fs[i] != '\0' && i < 63)
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(absorptionEditB, EM_REPLACESEL, 0, (LPARAM)p);

}

void absorptionWindowUpdate(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	updateEntityAbsorptionData(e);
}

void absorptionWindowReset(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	populateAbsorptionEntryFields(e);
}







BasicWindow reducedScatteringWindow;

HWND reducedScatteringEditPositionHeader = NULL;
HWND reducedScatteringEditRText = NULL;
HWND reducedScatteringEditR = NULL;
HWND reducedScatteringEditGText = NULL;
HWND reducedScatteringEditG = NULL;
HWND reducedScatteringEditBText = NULL;
HWND reducedScatteringEditB = NULL;

bool createReducedScatteringWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = reducedScatteringWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("ReducedScatteringnWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	reducedScatteringWindow.win = CreateWindow(TEXT("ReducedScatteringnWindow"), TEXT("RSW"), WS_CHILD, REDUCED_SCATTERING_WINDOW_X, REDUCED_SCATTERING_WINDOW_Y,
		REDUCED_SCATTERING_WINDOW_WIDTH, REDUCED_SCATTERING_WINDOW_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	reducedScatteringWindow.update = reducedScatteringWindowUpdate;
	reducedScatteringWindow.reset = reducedScatteringWindowReset;
	ShowWindow(reducedScatteringWindow.win, SW_SHOW);
}

LRESULT CALLBACK reducedScatteringWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		addReducedScatteringItems(hWnd);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void addReducedScatteringItems(HWND hWnd)
{
	int paddingX = (REDUCED_SCATTERING_WINDOW_WIDTH - 3 * REDUCED_SCATTERING_ENTRY_WIDTH) / 2;
	int paddingY = (REDUCED_SCATTERING_WINDOW_HEIGHT - 3 * REDUCED_SCATTERING_TEXT_HEIGHT - 3 * REDUCED_SCATTERING_ENTRY_HEIGHT - 3 * REDUCED_SCATTERING_HEADER_TEXT_HEIGHT) / 2;

	int posX = 0;
	int posY = 0;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	reducedScatteringEditPositionHeader = CreateWindow(TEXT("Static"), TEXT("Reduced Scattering:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, REDUCED_SCATTERING_TEXT_WIDTH, REDUCED_SCATTERING_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += REDUCED_SCATTERING_HEADER_TEXT_HEIGHT;
	reducedScatteringEditRText = CreateWindow(TEXT("Static"), TEXT("R:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, REDUCED_SCATTERING_TEXT_WIDTH, REDUCED_SCATTERING_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	reducedScatteringEditGText = CreateWindow(TEXT("Static"), TEXT("G:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + REDUCED_SCATTERING_ENTRY_WIDTH + paddingX, posY, REDUCED_SCATTERING_TEXT_WIDTH, REDUCED_SCATTERING_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	reducedScatteringEditBText = CreateWindow(TEXT("Static"), TEXT("B:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + (REDUCED_SCATTERING_ENTRY_WIDTH + paddingX) * 2, posY, REDUCED_SCATTERING_TEXT_WIDTH, REDUCED_SCATTERING_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += REDUCED_SCATTERING_TEXT_HEIGHT;
	reducedScatteringEditR = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX, posY, REDUCED_SCATTERING_ENTRY_WIDTH, REDUCED_SCATTERING_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	reducedScatteringEditG = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + REDUCED_SCATTERING_ENTRY_WIDTH + paddingX, posY, REDUCED_SCATTERING_ENTRY_WIDTH, REDUCED_SCATTERING_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	reducedScatteringEditB = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + (REDUCED_SCATTERING_ENTRY_WIDTH + paddingX) * 2, posY, REDUCED_SCATTERING_ENTRY_WIDTH, REDUCED_SCATTERING_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);

}

char* getReducedScatteringField(HWND* win)
{
	int iLine = SendMessage(*win, EM_LINELENGTH, 0, 0) + 1;
	TCHAR* p = new TCHAR[iLine];
	int iLength = SendMessage(*win, WM_GETTEXT, iLine, (LPARAM)p);
	char* pConvert = new char[iLine];
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, iLine, pConvert, sizeof(char)*(iLine), NULL, NULL);
	pConvert[iLine - 1] = '\0';

	delete p;
	return pConvert;
}

void updateEntityReducedScatteringData(Entity* e)
{
	if (e == NULL)
		return;

	char* pX = getAbsorptionField(&reducedScatteringEditR);
	char* pY = getAbsorptionField(&reducedScatteringEditG);
	char* pZ = getAbsorptionField(&reducedScatteringEditB);

	float x = atof(pX);
	float y = atof(pY);
	float z = atof(pZ);

	((Model_obj*)e)->reducedScattering[0] = x;
	((Model_obj*)e)->reducedScattering[1] = y;
	((Model_obj*)e)->reducedScattering[2] = z;

	delete pX;
	delete pY;
	delete pZ;
	return;
}

void populateReducedScatteringEntryFields(Entity* e)
{
	SendMessage(reducedScatteringEditR, EM_SETSEL, 0, SendMessage(reducedScatteringEditR, EM_LINELENGTH, 0, 0));
	SendMessage(reducedScatteringEditR, WM_CLEAR, 0, 0);


	SendMessage(reducedScatteringEditG, EM_SETSEL, 0, SendMessage(reducedScatteringEditG, EM_LINELENGTH, 0, 0));
	SendMessage(reducedScatteringEditG, WM_CLEAR, 0, 0);


	SendMessage(reducedScatteringEditB, EM_SETSEL, 0, SendMessage(reducedScatteringEditB, EM_LINELENGTH, 0, 0));
	SendMessage(reducedScatteringEditB, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", ((Model_obj*)e)->reducedScattering[0]);
	i = 0;
	while (fs[i] != '\0' && i < 32)
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(reducedScatteringEditR, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", ((Model_obj*)e)->reducedScattering[1]);
	i = 0;
	while (fs[i] != '\0' && i < 32)
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(reducedScatteringEditG, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", ((Model_obj*)e)->reducedScattering[2]);
	i = 0;
	while (fs[i] != '\0' && i < 32)
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(reducedScatteringEditB, EM_REPLACESEL, 0, (LPARAM)p);

	delete p;
	delete fs;
}

void reducedScatteringWindowUpdate(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	updateEntityReducedScatteringData(e);
}

void reducedScatteringWindowReset(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	populateReducedScatteringEntryFields(e);
}
#endif