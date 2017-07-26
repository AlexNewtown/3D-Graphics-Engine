#ifdef _EDITOR

#include "window\SpatialWindow.h"

BasicWindow spatialWindow;

HWND spatialEditPositionHeader = NULL;
HWND spatialEditXText = NULL;
HWND spatialEditX = NULL;
HWND spatialEditYText = NULL;
HWND spatialEditY = NULL;
HWND spatialEditZText = NULL;
HWND spatialEditZ = NULL;

HWND spatialRotationHeader = NULL;
HWND spatialRotXText = NULL;
HWND spatialRotX = NULL;
HWND spatialRotYText = NULL;
HWND spatialRotY = NULL;
HWND spatialRotZText = NULL;
HWND spatialRotZ = NULL;

HWND spatialScaleHeader = NULL;
HWND spatialScaleXText = NULL;
HWND spatialScaleX = NULL;
HWND spatialScaleYText = NULL;
HWND spatialScaleY = NULL;
HWND spatialScaleZText = NULL;
HWND spatialScaleZ = NULL;

bool createSpatialWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = spatialWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("SpatialWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	spatialWindow.win = CreateWindow(TEXT("SpatialWindow"), TEXT("SW"), WS_CHILD, SPATIAL_WINDOW_X, SPATIAL_WINDOW_Y,
		SPATIAL_WINDOW_WIDTH, SPATIAL_WINDOW_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	spatialWindow.update = spatialWindowUpdate;
	spatialWindow.reset = spatialWindowReset;
	ShowWindow(spatialWindow.win, SW_SHOW);
}

void addSpatialItems(HWND hWnd)
{
	int paddingX = (SPATIAL_WINDOW_WIDTH - 3* SPATIAL_ENTRY_WIDTH) / 2;
	int paddingY = (SPATIAL_WINDOW_HEIGHT - 3 * SPATIAL_TEXT_HEIGHT - 3 * SPATIAL_ENTRY_HEIGHT - 3 * SPATIAL_HEADER_TEXT_HEIGHT ) / 2;

	int posX = 0;
	int posY = 0;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	spatialEditPositionHeader = CreateWindow(TEXT("Static"), TEXT("Position:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd,NULL, hInstance, NULL);

	posY += SPATIAL_HEADER_TEXT_HEIGHT;
	spatialEditXText = CreateWindow(TEXT("Static"), TEXT("X:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialEditYText = CreateWindow(TEXT("Static"), TEXT("Y:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialEditZText = CreateWindow(TEXT("Static"), TEXT("Z:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX)*2, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += SPATIAL_TEXT_HEIGHT;
	spatialEditX = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialEditY = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialEditZ = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX)*2, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);


	posY += SPATIAL_ENTRY_HEIGHT + paddingY;
	spatialRotationHeader = CreateWindow(TEXT("Static"), TEXT("Rotation:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += SPATIAL_HEADER_TEXT_HEIGHT;
	spatialRotXText = CreateWindow(TEXT("Static"), TEXT("X:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialRotYText = CreateWindow(TEXT("Static"), TEXT("Y:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialRotZText = CreateWindow(TEXT("Static"), TEXT("Z:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX) * 2, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += SPATIAL_TEXT_HEIGHT;
	spatialRotX = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialRotY = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialRotZ = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX) * 2, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);



	posY += SPATIAL_ENTRY_HEIGHT + paddingY;
	spatialScaleHeader = CreateWindow(TEXT("Static"), TEXT("Scale:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += SPATIAL_HEADER_TEXT_HEIGHT;
	spatialScaleXText = CreateWindow(TEXT("Static"), TEXT("X:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialScaleYText = CreateWindow(TEXT("Static"), TEXT("Y:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialScaleZText = CreateWindow(TEXT("Static"), TEXT("Z:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX) * 2, posY, SPATIAL_TEXT_WIDTH, SPATIAL_TEXT_HEIGHT,
		hWnd, NULL, hInstance, NULL);

	posY += SPATIAL_TEXT_HEIGHT;
	spatialScaleX = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialScaleY = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + SPATIAL_ENTRY_WIDTH + paddingX, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
	spatialScaleZ = CreateWindow(TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		posX + (SPATIAL_ENTRY_WIDTH + paddingX) * 2, posY, SPATIAL_ENTRY_WIDTH, SPATIAL_ENTRY_HEIGHT,
		hWnd, NULL, hInstance, NULL);
}

char* getEditField(HWND *win)
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

void updateEntitySpatialData(Entity* e)
{
	if (e == NULL)
		return;

	char* pX = getEditField(&spatialEditX);
	char* pY = getEditField(&spatialEditY);
	char* pZ = getEditField(&spatialEditZ);

	char* pRX = getEditField(&spatialRotX);
	char* pRY = getEditField(&spatialRotY);
	char* pRZ = getEditField(&spatialRotZ);

	char* pSX = getEditField(&spatialScaleX);
	char* pSY = getEditField(&spatialScaleY);
	char* pSZ = getEditField(&spatialScaleZ);

	float x = atof(pX);
	float y = atof(pY);
	float z = atof(pZ);

	float rx = atof(pRX);
	float ry = atof(pRY);
	float rz = atof(pRZ);

	float sx = atof(pSX);
	float sy = atof(pSY);
	float sz = atof(pSZ);

	e->moveTo(x, y, z);
	e->scaleTo(sx, sy, sz);
	e->rotateTo(rx, ry, rz);

	delete pX;
	delete pY;
	delete pZ;
	delete pRX;
	delete pRY;
	delete pRZ;
	delete pSX;
	delete pSY;
	delete pSZ;
	return;
}

void populateSpatialEntryFields(Entity* e)
{
	SendMessage(spatialEditX, EM_SETSEL, 0, SendMessage(spatialEditX, EM_LINELENGTH, 0, 0));
	SendMessage(spatialEditX, WM_CLEAR, 0, 0);


	SendMessage(spatialEditY, EM_SETSEL, 0, SendMessage(spatialEditY, EM_LINELENGTH, 0, 0));
	SendMessage(spatialEditY, WM_CLEAR, 0, 0);


	SendMessage(spatialEditZ, EM_SETSEL, 0, SendMessage(spatialEditZ, EM_LINELENGTH, 0, 0));
	SendMessage(spatialEditZ, WM_CLEAR, 0, 0);




	SendMessage(spatialRotX, EM_SETSEL, 0, SendMessage(spatialRotX, EM_LINELENGTH, 0, 0));
	SendMessage(spatialRotX, WM_CLEAR, 0, 0);


	SendMessage(spatialRotY, EM_SETSEL, 0, SendMessage(spatialRotY, EM_LINELENGTH, 0, 0));
	SendMessage(spatialRotY, WM_CLEAR, 0, 0);


	SendMessage(spatialRotZ, EM_SETSEL, 0, SendMessage(spatialRotZ, EM_LINELENGTH, 0, 0));
	SendMessage(spatialRotZ, WM_CLEAR, 0, 0);




	SendMessage(spatialScaleX, EM_SETSEL, 0, SendMessage(spatialScaleX, EM_LINELENGTH, 0, 0));
	SendMessage(spatialScaleX, WM_CLEAR, 0, 0);


	SendMessage(spatialScaleY, EM_SETSEL, 0, SendMessage(spatialScaleY, EM_LINELENGTH, 0, 0));
	SendMessage(spatialScaleY, WM_CLEAR, 0, 0);


	SendMessage(spatialScaleZ, EM_SETSEL, 0, SendMessage(spatialScaleZ, EM_LINELENGTH, 0, 0));
	SendMessage(spatialScaleZ, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", e->x());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialEditX, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->y());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialEditY, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->z());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialEditZ, EM_REPLACESEL, 0, (LPARAM)p);


	sprintf(fs, "%0.3f", e->rotX());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialRotX, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->rotY());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialRotY, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->rotZ());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialRotZ, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->scaleX());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialScaleX, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->scaleY());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialScaleY, EM_REPLACESEL, 0, (LPARAM)p);

	sprintf(fs, "%0.3f", e->scaleZ());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(spatialScaleZ, EM_REPLACESEL, 0, (LPARAM)p);

	delete p;
	delete fs;
}

LRESULT CALLBACK spatialWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		addSpatialItems(hWnd);

		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void spatialWindowUpdate(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	updateEntitySpatialData(e);
	
}

void spatialWindowReset(Entity* e, Scene *s)
{
	if (e == NULL)
		return;

	populateSpatialEntryFields(e);
}

#endif