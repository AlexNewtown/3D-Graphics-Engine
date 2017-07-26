#include "window\BrdfDropDownWindow.h"

#ifdef _EDITOR

BasicWindow brdfDropDownWindow;
HWND brdfDropDown;

bool createBrdfDropDownWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = brdfDropDownWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("BrdfDropDownBox");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	brdfDropDownWindow.win = CreateWindow(TEXT("BrdfDropDownBox"), NULL, WS_VISIBLE | WS_CHILD,
		0, 0, 200, 23, parent, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(brdfDropDownWindow.win, SW_HIDE);
	brdfDropDownWindow.update = updateBrdfDropDownWindow;
	brdfDropDownWindow.reset = resetBrdfDropDownWindow;
	return true;
}

LRESULT CALLBACK brdfDropDownWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{

			brdfDropDown = CreateWindow(TEXT("ComboBox"), NULL,
				CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL,
				0, 0, 200, 80, hWnd, NULL, GetModuleHandle(NULL), NULL);

			TCHAR A[40];
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Torrance-Sparrow"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_TORRANCE_SPARROW, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Beard-Maxwell"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_BEARD_MAXWELL, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Cook-Torrance"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_COOK_TORRANCE, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Kajiya"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_KAJIYA, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Poulin-Fournier"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_POULIN_FOURNIER, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("He-Torrance-Sillion-Greenberg"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_HE_TORRANCE_SILLION_GREENBERG, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Oren-Nayar"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_OREN_NAYAR, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Coupled"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_COUPLED, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Ashikhmin-Shirley"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_ASHIKHMIN_SHIRLEY, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Granier-Heidrich"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_GRANIER_HEIDRICH, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Minnaert"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_MINNAERT, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Phong"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_PHONG, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Blinn"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_BLINN, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Lewis"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_LEWIS, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Neumann-Neumann"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_NEUMANN_NEUMANN, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Strauss"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_STRAUSS, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Ward"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_WARD, (LPARAM)A);
			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Schlick"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_SCHLICK, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Lafortune"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_LAFORTUNE, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Lambertian"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_LAMBERTIAN, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("Fresnel"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_FRESNEL, (LPARAM)A);

			wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)TEXT("PHBeckmann"));
			SendMessage(brdfDropDown, (UINT)CB_ADDSTRING, (WPARAM)BRDF_PHBECKMANN, (LPARAM)A);

			SendMessage(brdfDropDown, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			break;
		}
		case WM_COMMAND:
		{
			if (lParam == (LPARAM)brdfDropDown)
			{
				if (currentSelectedEntity == NULL)
					break;

				int itemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
					(WPARAM)0, (LPARAM)0);

				switch (itemIndex)
				{
					case BRDF_TORRANCE_SPARROW:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_TORRANCE_SPARROW;
						break;
					case BRDF_BEARD_MAXWELL:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_BEARD_MAXWELL;
						break;
					case BRDF_COOK_TORRANCE:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_COOK_TORRANCE;
						break;
					case BRDF_KAJIYA:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_KAJIYA;
						break;
					case BRDF_POULIN_FOURNIER:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_POULIN_FOURNIER;
						break;
					case BRDF_HE_TORRANCE_SILLION_GREENBERG:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_HE_TORRANCE_SILLION_GREENBERG;
						break;
					case BRDF_OREN_NAYAR:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_OREN_NAYAR;
						break;
					case BRDF_COUPLED:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_COUPLED;
						break;
					case BRDF_ASHIKHMIN_SHIRLEY:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_ASHIKHMIN_SHIRLEY;
						break;
					case BRDF_GRANIER_HEIDRICH:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_GRANIER_HEIDRICH;
						break;
					case BRDF_MINNAERT:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_MINNAERT;
						break;
					case BRDF_PHONG:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_PHONG;
						break;
					case BRDF_BLINN:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_BLINN;
						break;
					case BRDF_LEWIS:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_LEWIS;
						break;
					case BRDF_NEUMANN_NEUMANN:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_NEUMANN_NEUMANN;
						break;
					case BRDF_STRAUSS:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_STRAUSS;
						break;
					case BRDF_WARD:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_WARD;
						break;
					case BRDF_SCHLICK:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_SCHLICK;
						break;
					case BRDF_LAFORTUNE:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_LAFORTUNE;
						break;
					case BRDF_LAMBERTIAN:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_LAMBERTIAN;
						break;
					case BRDF_FRESNEL:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_FRESNEL;
						break;
					case BRDF_PHBECKMANN:
						((Model_obj*)currentSelectedEntity)->brdfType = BRDF_PHBECKMANN;
						break;

				}
			}
		}

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void updateBrdfDropDownWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;
	
	float f = e->rotX();

	return;
}
void resetBrdfDropDownWindow(Entity* e, Scene* s)
{
	if (e == NULL)
		return;

	return;
}

#endif