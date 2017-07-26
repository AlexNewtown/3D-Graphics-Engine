#include "window\ParameterBoxWindow.h"

#ifdef _EDITOR

BasicWindow parameterBoxWindow;
HWND parameterBox = NULL;
HWND parameterBoxText = NULL;
int currentSelectedParameter = NULL;
std::vector<std::string> parameterStrings;

bool createParameterBoxWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = createParameterBoxWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("parameterBoxWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	parameterBoxWindow.win = CreateWindow(TEXT("parameterBoxWindow"), TEXT("PBW"), WS_CHILD, PARAMETER_BOX_WINDOW_X, PARAMETER_BOX_WINDOW_Y,
		PARAMETER_BOX_WINDOW_WIDTH, PARAMETER_BOX_WINDOW_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	parameterBoxWindow.update = parameterBoxWindowUpdate;
	parameterBoxWindow.reset = parameterBoxWindowReset;
	ShowWindow(parameterBoxWindow.win, SW_SHOW);
}

LRESULT CALLBACK createParameterBoxWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		int boxX, boxY;
		int textBoxX, textBoxY;
		case WM_CREATE:
		{

			textBoxX = (PARAMETER_BOX_WINDOW_WIDTH - PARAMETER_BOX_LIST_WIDTH) / 2;
			textBoxY = (PARAMETER_BOX_WINDOW_HEIGHT - PARAMETER_BOX_LIST_HEIGHT - PARAMETER_BOX_TEXT_HEIGHT) / 2;
			boxX = textBoxX;
			boxY = textBoxY + PARAMETER_BOX_TEXT_HEIGHT;

			parameterBoxText = CreateWindow(TEXT("Static"), TEXT("Parameters:"), WS_CHILD | WS_VISIBLE | SS_LEFT ,
				textBoxX, textBoxY, PARAMETER_BOX_TEXT_WIDTH, PARAMETER_BOX_TEXT_HEIGHT,
				hWnd, (HMENU)NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			parameterBox = CreateWindow(TEXT("listBox"),
				NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_STANDARD, boxX, boxY, PARAMETER_BOX_LIST_WIDTH, PARAMETER_BOX_LIST_HEIGHT,
				hWnd, (HMENU)PARAMETER_LIST_BOX_INDEX, GetModuleHandle(NULL), NULL);

			
		}
		case WM_COMMAND:
		{

			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				if (lParam == (LPARAM)parameterBox)
				{
					currentSelectedParameter = SendMessage(parameterBox, LB_GETCURSEL, 0, 0);
					for (int i = 0; i < nonstaticEditWindows.size(); i++)
					{
						ShowWindow(nonstaticEditWindows[i]->win, SW_HIDE);
					}
					ShowWindow(nonstaticEditWindows[currentSelectedParameter]->win, SW_SHOW);
					nonstaticEditWindows[currentSelectedParameter]->reset(currentSelectedEntity, levelEditorScene);
				}
			}


			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void parameterBoxWindowUpdate(Entity* e, Scene* s)
{

}

void parameterBoxWindowReset(Entity* e, Scene* s)
{
	populateParameterListBox();
}

void populateParameterListBox()
{
	//parameterStrings
	if (SendMessage(parameterBox, LB_GETCOUNT, 0, 0) > 0)
		SendMessage(parameterBox, LB_RESETCONTENT, 0, 0);

	int numP = parameterStrings.size();
	
	for (int i = 0; i < numP; i++)
	{
		std::string st = parameterStrings[i];
		TCHAR* p = new TCHAR[st.size() + 1];
		for (int j = 0; j < st.size(); j++)
		{
			p[j] = st[j];
		}
		p[st.size()] = '\0';
		LRESULT aa = SendMessage(parameterBox, LB_INSERTSTRING, i, (LPARAM)p);

		delete(p);
	}
}

#endif