#include "window\EntityBoxWindow.h"
#include "window\LevelEditorWindow.h"

#ifdef _EDITOR

BasicWindow entityBoxWindow;
HWND hWndNewEntity;
HWND entityBox_entityText = NULL;
HWND entityBox_entityList = NULL;
HWND entityBox_selectedEntityText = NULL;
HWND entityBox_selectedEntityList = NULL;

bool createEntityBoxWindow(HWND parent)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = createEntityBoxWindowCallback;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("EntityBoxWindow");

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	entityBoxWindow.win = CreateWindow(TEXT("EntityBoxWindow"), TEXT("EBW"), WS_CHILD, ENTITY_BOX_WINDOW_X, ENTITY_BOX_WINDOW_Y,
		ENTITY_BOX_WINDOW_WIDTH, ENTITY_BOX_WINDOW_HEIGHT, parent, NULL, GetModuleHandle(NULL), NULL);
	entityBoxWindow.update = entityBoxWindowUpdate;
	entityBoxWindow.reset = entityBoxWindowReset;
	ShowWindow(entityBoxWindow.win, SW_SHOW);
}

LRESULT CALLBACK createEntityBoxWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
		int boxX, boxY;
		int textBoxX, textBoxY;

		case WM_CREATE:
		{
			boxX = (ENTITY_BOX_WINDOW_WIDTH - ENTITY_BOX_LIST_WIDTH * 2) / 3;
			boxY = (ENTITY_BOX_WINDOW_HEIGHT - ENTITY_BOX_LIST_HEIGHT - ENTITY_BOX_TEXT_HEIGHT) / 2;
	
			textBoxX = boxX;
			textBoxY = boxY;
			boxY += ENTITY_BOX_LIST_HEIGHT + ENTITY_BOX_TEXT_HEIGHT;

			entityBox_entityList = CreateWindow(TEXT("listBox"),
				NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, boxX, boxY - ENTITY_BOX_LIST_HEIGHT, ENTITY_BOX_LIST_WIDTH, ENTITY_BOX_LIST_HEIGHT,
				hWnd, (HMENU)ENTITY_LIST_BOX_INDEX, GetModuleHandle(NULL), NULL);

			entityBox_entityText = CreateWindow(TEXT("Static"), TEXT("Entities:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				textBoxX, textBoxY, ENTITY_BOX_TEXT_WIDTH, ENTITY_BOX_TEXT_HEIGHT,
				hWnd, (HMENU)NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			boxX = ENTITY_BOX_WINDOW_WIDTH - ENTITY_BOX_LIST_WIDTH - (ENTITY_BOX_WINDOW_WIDTH - ENTITY_BOX_LIST_WIDTH * 2) / 3;
			textBoxX = boxX;

			entityBox_selectedEntityList = CreateWindow(TEXT("listBox"),
				NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, boxX, boxY - ENTITY_BOX_LIST_HEIGHT, ENTITY_BOX_LIST_WIDTH, ENTITY_BOX_LIST_HEIGHT,
				hWnd, (HMENU)SELECTED_ENTITY_LIST_BOX_INDEX, GetModuleHandle(NULL), NULL);

			entityBox_selectedEntityText = CreateWindow(TEXT("Static"), TEXT("Selected Entities:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				textBoxX, textBoxY, ENTITY_BOX_TEXT_WIDTH, ENTITY_BOX_TEXT_HEIGHT,
				hWnd, (HMENU)NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			break;
		}
		
		case WM_COMMAND:
		{

			if (LOWORD(wParam) == SUB_MENU_NEW_ENTITY)
			{
				HWND hWndNewEntity = CreateWindowEx(0,
					TEXT("newEntity"),
					TEXT("newEntity"),
					WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE,
					150, 150,
					400,
					300,
					NULL,
					NULL,
					GetModuleHandle(NULL),
					NULL
					);

				return 0;
			}

			if (HIWORD(wParam) == LBN_SELCHANGE)
			{

				unselectEntities();
				if (lParam == (LPARAM)entityBox_entityList) //listBoxIndex == ENTITY_LIST_BOX_INDEX)
				{
					int i = SendMessage(entityBox_entityList, LB_GETCURSEL, 0, 0);
					int totalEntities = levelEditorScene->numEntities() + levelEditorScene->numLightEntities() + levelEditorScene->numPointCloudEntities();
					if (i >= levelEditorScene->numEntities() + levelEditorScene->numLightEntities())
					{
						i = i - levelEditorScene->numEntities() - levelEditorScene->numLightEntities();
						populateEntryFields(levelEditorScene->getPointCloudEntity(i), &currentSelectedEntity);
					}
					else if (i >= levelEditorScene->numEntities())
					{
						i = i - levelEditorScene->numEntities();
						populateLightEntryFields(levelEditorScene->getLightEntity(i), &currentSelectedEntity);
						//windowSettingIndex = getLightWindowsIndex(levelEditorScene->getLightEntity(i)->lightType);
						updateWindowSetting(windowSettingIndex);
					}
					else
					{
						populateEntryFields(levelEditorScene->getEntity(i), &currentSelectedEntity);
						//windowSettingIndex = getShaderWindowsIndex(levelEditorScene->shadingType);
						updateWindowSetting(windowSettingIndex);
					}
				}
				else if (lParam == (LPARAM)entityBox_selectedEntityList) //listBoxIndex == SELECTED_ENTITY_LIST_BOX_INDEX)
				{
					int i = SendMessage(entityBox_selectedEntityText, LB_GETCURSEL, 0, 0);
					/*
					int selectionIndex = 0;
					int j = 0;
					while (selectionIndex <= i)
					{
						if (scene->getEntity(j)->selected)
						{
							selectionIndex++;
						}
						j++;
					}
					*/
					Entity* e = NULL;
					int selectionIndex = 0;
					for (int j = 0; j < levelEditorScene->numEntities(); j++)
					{
						if (levelEditorScene->getEntity(j)->selected)
						{
							if (selectionIndex == i)
							{
								e = levelEditorScene->getEntity(j);
								//windowSettingIndex = getShaderWindowsIndex(levelEditorScene->shadingType);
							}
							selectionIndex++;
						}
					}
					for (int j = 0; j < levelEditorScene->numLightEntities(); j++)
					{
						if (levelEditorScene->getLightEntity(j)->selected)
						{
							if (selectionIndex == i)
							{
								e = levelEditorScene->getLightEntity(j);
								//windowSettingIndex = LIGHT_WINDOW_INDEX;
							}
							selectionIndex++;
						}
					}
					for (int j = 0; j < levelEditorScene->numPointCloudEntities(); j++)
					{
						if (levelEditorScene->getPointCloudEntity(j)->selected)
						{
							if (selectionIndex == i)
							{
								e = levelEditorScene->getPointCloudEntity(j);
								//windowSettingIndex = LIGHT_WINDOW_INDEX;
							}
							selectionIndex++;
						}
					}

					populateEntryFields(e, &currentSelectedEntity);
					updateWindowSetting(windowSettingIndex);
				}
			}
		}
		
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void entityBoxWindowUpdate(Entity* e, Scene* s)
{
	if (e == NULL)
		return;
}

void entityBoxWindowReset(Entity* e, Scene* s)
{
	if(s != NULL)
		populateEntityListBox(s);

	if (e == NULL)
		return;

}

void populateEntityListBox(Scene* s)
{

	SendMessage(entityBox_entityList, LB_RESETCONTENT, 0, 0);
	SendMessage(entityBox_selectedEntityList, LB_RESETCONTENT, 0, 0);

	int numE = s->numEntities();
	int selectedIndex = 0;
	int entityListIndex = 0;
	Model_obj* m;
	for (int i = 0; i < numE; i++)
	{
		m = (Model_obj*)s->getEntity(i);
		std::string eName = m->entityName();
		TCHAR* p = new TCHAR[eName.size() + 1];
		for (int j = 0; j < eName.size(); j++)
		{
			p[j] = eName[j];
		}
		p[eName.size()] = '\0';
		SendMessage(entityBox_entityList, LB_INSERTSTRING, entityListIndex, (LPARAM)p);
		entityListIndex++;
		if (m->selected)
		{
			SendMessage(entityBox_selectedEntityList, LB_INSERTSTRING, selectedIndex, (LPARAM)p);
			selectedIndex++;
		}

		free(p);
	}

	numE = s->numLightEntities();
	for (int i = 0; i < numE; i++)
	{
		m = (Model_obj*)s->getLightEntity(i);
		std::string eName = m->entityName();
		TCHAR* p = new TCHAR[eName.size() + 1];
		for (int j = 0; j < eName.size(); j++)
		{
			p[j] = eName[j];
		}
		p[eName.size()] = '\0';
		SendMessage(entityBox_entityList, LB_INSERTSTRING, entityListIndex, (LPARAM)p);
		entityListIndex++;
		if (m->selected)
		{
			SendMessage(entityBox_selectedEntityList, LB_INSERTSTRING, selectedIndex, (LPARAM)p);
			selectedIndex++;
		}

		free(p);
	}

	numE = s->numBoundaryEntities();
	for (int i = 0; i < numE; i++)
	{
		m = (Model_obj*)s->getBoundaryEntity(i);
		std::string eName = m->entityName();
		TCHAR* p = new TCHAR[eName.size() + 1];
		for (int j = 0; j < eName.size(); j++)
		{
			p[j] = eName[j];
		}
		p[eName.size()] = '\0';
		SendMessage(entityBox_entityList, LB_INSERTSTRING, entityListIndex, (LPARAM)p);
		entityListIndex++;
		if (m->selected)
		{
			SendMessage(entityBox_selectedEntityList, LB_INSERTSTRING, selectedIndex, (LPARAM)p);
			selectedIndex++;
		}

		free(p);
	}

	numE = s->numPointCloudEntities();
	for (int i = 0; i < numE; i++)
	{
		PointCloudEntity* pce = s->getPointCloudEntity(i);
		std::string eName = pce->entityName();
		TCHAR* p = new TCHAR[eName.size() + 1];
		for (int j = 0; j < eName.size(); j++)
		{
			p[j] = eName[j];
		}
		p[eName.size()] = '\0';
		SendMessage(entityBox_entityList, LB_INSERTSTRING, entityListIndex, (LPARAM)p);
		entityListIndex++;
		if (pce->selected)
		{
			SendMessage(entityBox_selectedEntityList, LB_INSERTSTRING, selectedIndex, (LPARAM)p);
			selectedIndex++;
		}

		free(p);
	}

}

void populateEntryFields(Entity* e, Entity** populated)
{
	if (e == NULL)
		return;

	*populated = e;
	e->selected = true;

	populateSpatialEntryFields(e);
}

void populateLightEntryFields(LightEntity* e, Entity** populated)
{
	populateEntryFields(e,populated);

	populateSpatialEntryFields(e);

	/*
	SendMessage(lightIntensityText, EM_SETSEL, 0, SendMessage(lightIntensityText, EM_LINELENGTH, 0, 0));
	SendMessage(lightIntensityText, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;
	sprintf(fs, "%0.3f", e->intensity);
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(lightIntensityText, EM_REPLACESEL, 0, (LPARAM)p);
	*/
}

void populateBoundaryEntryFields(SphereBoundaryEntity* e, Entity** populated)
{
	populateEntryFields(e,populated);
	/*();
	showBoundaryRadiusField();

	SendMessage(hwndEditBoundaryRadiusText, EM_SETSEL, 0, SendMessage(hwndEditBoundaryRadiusText, EM_LINELENGTH, 0, 0));
	SendMessage(hwndEditBoundaryRadiusText, WM_CLEAR, 0, 0);

	TCHAR* p = new TCHAR[32];
	char* fs = new char[32];
	int i = 0;

	sprintf(fs, "%0.3f", e->radius());
	i = 0;
	while (fs[i] != '\0')
	{
		p[i] = fs[i];
		i++;
	}
	p[i] = '\0';
	SendMessage(hwndEditBoundaryRadiusText, EM_REPLACESEL, 0, (LPARAM)p);

	delete p;
	delete fs;
	*/
}

#endif