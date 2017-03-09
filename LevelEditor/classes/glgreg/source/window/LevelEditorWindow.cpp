#include "window\LevelEditorWindow.h"
#include "window\EntityBoxWindow.h"
#include "window\SpatialWindow.h"
#include "window\LightIntensityWindow.h"
#include "window\BrdfDropDownWindow.h"
#include "window\LightFovWindow.h"
#include "window\LightTypeDropDownWindow.h"
#include "window\LightAspectRatioWindow.h"
#include "window\ScatteringCoefficientWindow.h"

BasicWindow baseWindow;
HDC deviceContext = NULL;
HGLRC renderingContext = NULL;
bool windowActive = false;
bool keyPress[256];

#ifdef _EDITOR

HMENU menu = NULL;
//HWND updateSceneButton = false;
HMENU hMenuShaderType;
unsigned int windowSettingIndex;
HWND newEntityEdit;
HWND newEntity;

std::vector<BasicWindow*> allWindows;

std::vector<BasicWindow*> noLightingWindow;
std::vector<BasicWindow*> lightWindow;
std::vector<BasicWindow*> adsWindow;
std::vector<BasicWindow*> adsBrdfWindow;
std::vector<BasicWindow*> globalJacobiWindow;
std::vector<BasicWindow*> radianceCacheUGWindow;
std::vector<BasicWindow*> MonteCarloBrdfWindow;
std::vector<BasicWindow*> lightFrustumWindow;
std::vector<BasicWindow*> progressivePhotonMapWindow;

std::vector<BasicWindow*> MonteCarloBrdfWindowTorraceSparrow;
std::vector<BasicWindow*> MonteCarloBrdfWindowBeardMaxwell;
std::vector<BasicWindow*> MonteCarloBrdfWindowCookTorrance;
std::vector<BasicWindow*> MonteCarloBrdfWindowKajiya;
std::vector<BasicWindow*> MonteCarloBrdfWindowPoulinFournier;
std::vector<BasicWindow*> MonteCarloBrdfWindowHeTorranceSillionGreenberg;
std::vector<BasicWindow*> MonteCarloBrdfWindowOrenNayar;
std::vector<BasicWindow*> MonteCarloBrdfWindowCoupled;
std::vector<BasicWindow*> MonteCarloBrdfWindowAshikhminShirley;
std::vector<BasicWindow*> MonteCarloBrdfWindowGranierHeidrich;
std::vector<BasicWindow*> MonteCarloBrdfWindowMinnaert;
std::vector<BasicWindow*> MonteCarloBrdfWindowPhong;
std::vector<BasicWindow*> MonteCarloBrdfWindowBlinn;
std::vector<BasicWindow*> MonteCarloBrdfWindowLewis;
std::vector<BasicWindow*> MonteCarloBrdfWindowNeumannNeumann;
std::vector<BasicWindow*> MonteCarloBrdfWindowStrauss;
std::vector<BasicWindow*> MonteCarloBrdfWindowWard;
std::vector<BasicWindow*> MonteCarloBrdfWindowSchlick;
std::vector<BasicWindow*> MonteCarloBrdfWindowLafortune;

std::vector<std::vector<BasicWindow*>> windowSettings;

void createWindowSettings()
{
	/*Add all windows to the list*/
	allWindows.push_back(&entityBoxWindow);
	allWindows.push_back(&spatialWindow);
	allWindows.push_back(&lightIntensityWindow);
	allWindows.push_back(&brdfDropDownWindow);
	allWindows.push_back(&indexOfRefractionWindow);
	allWindows.push_back(&microfacetRoughnessWindow);
	allWindows.push_back(&lightTypeDropDownWindow);
	allWindows.push_back(&lightFovWindow);
	allWindows.push_back(&lightAspectRatioWindow);
	allWindows.push_back(&absorptionWindow);
}

void updateWindowSetting(unsigned int setting)
{
	
	for (int i = 0; i < staticEditWindows.size(); i++)
	{
		ShowWindow(staticEditWindows[i]->win, SW_HIDE);
	}
	for (int i = 0; i < nonstaticEditWindows.size(); i++)
	{
		//ShowWindow(nonstaticEditWindows[i]->win, SW_HIDE);
	}

	RECT baseRect;
	GetWindowRect(editWindow.win, &baseRect);
	//int w = baseRect.right - baseRect.left;
	int h = abs(baseRect.top - baseRect.bottom) - UPDATE_BUTTON_HEIGHT;
	int wtf = baseRect.top - baseRect.bottom;
	long wtfl = baseRect.top - baseRect.bottom;
	int baseRectW = abs(baseRect.right - baseRect.left);

	int padding = 12; // (h - totalChildrenHeight) / ((int)windowSettings[setting].size() + 1);

	int x = 0;
	int y = padding;
	for (int i = 0; i < staticEditWindows.size(); i++)
	{
		GetWindowRect(staticEditWindows[i]->win, &baseRect);
		int childW = abs(baseRect.right - baseRect.left);
		int childH = abs(baseRect.top - baseRect.bottom);
		x = (baseRectW - childW) / 2;
		MoveWindow(staticEditWindows[i]->win, x, y, childW, childH, false);
		ShowWindow(staticEditWindows[i]->win, SW_SHOW);
		y += childH + padding;
	}

	for (int i = 0; i < nonstaticEditWindows.size(); i++)
	{
		GetWindowRect(nonstaticEditWindows[i]->win, &baseRect);
		int childW = abs(baseRect.right - baseRect.left);
		int childH = abs(baseRect.top - baseRect.bottom);
		x = (baseRectW - childW) / 2;
		MoveWindow(nonstaticEditWindows[i]->win, x, y, childW, childH, false);
		//ShowWindow(nonstaticEditWindows[currentSelectedParameter]->win, SW_SHOW);
	}
	//ShowWindow(nonstaticEditWindows[0]->win, SW_SHOW);
	levelEditorReset(currentSelectedEntity, levelEditorScene);

}

#endif

bool createLevelEditorWindow(int width, int height, int bits)
{
	unsigned int pixelFormat;
	WNDCLASS wndclass;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;
	windowRect.left = (long)0;
	windowRect.right = (long)width;
	windowRect.top = (long)0;
	windowRect.bottom = (long)height;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = levelEditorWindowCallback;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = TEXT("LevelEditor");


#ifdef _EDITOR
	WNDCLASS wndClassNewEnt;
	wndClassNewEnt.style = CS_HREDRAW | CS_VREDRAW;
	wndClassNewEnt.lpfnWndProc = newEntityWindowCallback;
	wndClassNewEnt.cbClsExtra = 0;
	wndClassNewEnt.cbWndExtra = 0;
	wndClassNewEnt.hInstance = hInstance;
	wndClassNewEnt.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClassNewEnt.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassNewEnt.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClassNewEnt.lpszMenuName = NULL;
	wndClassNewEnt.lpszClassName = TEXT("newEntity");

	if (!RegisterClass(&wndClassNewEnt))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
#endif

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_BORDER;
#ifdef _EDITOR
	createWindowSettings();
	windowSettingIndex = 0;
#endif
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
	baseWindow.update = levelEditorUpdate;
	baseWindow.reset = levelEditorReset;
	if (!(baseWindow.win = CreateWindowEx(dwExStyle,
		TEXT("LevelEditor"),
#ifdef _EDITOR
		TEXT("gregjksmith Engine Editor"),
#else
		TEXT("gregjksmith Engine Viewer"),
#endif

		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL
		)))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Window Creation Error."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,(byte)0,
		(byte)0,
		(byte)0,
		(byte)0,
		(byte)0,(byte)0,(byte)0,(byte)0,
		(byte)16,
		(byte)0,
		(byte)0,
		PFD_MAIN_PLANE,
		(byte)0,
		(byte)0,(byte)0,(byte)0
	};

	if (!(deviceContext = GetDC(baseWindow.win)))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Can't Create A GL Device Context"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(pixelFormat = ChoosePixelFormat(deviceContext, &pfd)))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Can't Find A Suitable PixelFormat"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!SetPixelFormat(deviceContext, pixelFormat, &pfd))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Can't Set The PixelFormat"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(renderingContext = wglCreateContext(deviceContext)))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Can't Create A GL Rendering Context"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!wglMakeCurrent(deviceContext, renderingContext))
	{
		//killGLWindow();
		MessageBox(NULL, TEXT("Can't Activate The GL Rendering Context"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
	}

#ifdef _EDITOR
	updateWindowSetting(windowSettingIndex);
#endif
}



void resizeLevelEditorWindow(int width, int height)
{

	MoveWindow(baseWindow.win, 0, 0, width, height, true);
}

LRESULT CALLBACK levelEditorWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
#ifdef _EDITOR
			
			addMenu(hWnd);
			createEditWindow(hWnd);
#endif		
			break;
		}
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				windowActive = TRUE;
			}
			else
			{
				windowActive = false;
			}

			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;

		}
		case WM_KEYDOWN:
		{
			keyPress[wParam] = true;
			return 0;
		}
		case WM_KEYUP:
		{
			keyPress[wParam] = false;
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
#ifdef _EDITOR
			getControllerInstance()->setStarting();
#endif
			getControllerInstance()->rightMouseDown = true;
			return 0;
		}
		case WM_RBUTTONUP:
		{
			getControllerInstance()->rightMouseDown = false;
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
#ifdef _EDITOR
			Entity* e;
			for (int i = 0; i < levelEditorScene->numEntities(); i++)
			{
				if (levelEditorScene->getEntity(i)->highlighted)
				{
					e = levelEditorScene->getEntity(i);
					e->selected = true;
					//levelEditorScene->getEntity(i)->selected = true
				}
				else
					levelEditorScene->getEntity(i)->selected = false;
			}

			for (int i = 0; i < levelEditorScene->numLightEntities(); i++)
			{
				if (levelEditorScene->getLightEntity(i)->highlighted)
					levelEditorScene->getLightEntity(i)->selected = true;
				else
					levelEditorScene->getLightEntity(i)->selected = false;
			}

			levelEditorReset(currentSelectedEntity, levelEditorScene);
#endif
			SetFocus(baseWindow.win);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			//scene->clearSelection();
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
#ifdef _EDITOR
			getControllerInstance()->setStarting();
#endif
			getControllerInstance()->middleMouseDown = true;
			return 0;
		}
		case WM_MBUTTONUP:
		{
			getControllerInstance()->middleMouseDown = false;
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
#ifdef _EDITOR
			//getControllerInstance()->moveTowardsOrigin(-(float)HIWORD(wParam));
#endif
			return 0;
		}
		case WM_MOUSEMOVE:
		{

#ifdef _EDITOR
			getControllerInstance()->setMousePosition(LOWORD(lParam), HIWORD(lParam));
#endif

#ifdef _VIEWER
			getControllerInstance()->setMousePosition(LOWORD(lParam) - SCREEN_WIDTH / 2, HIWORD(lParam));
#endif
			return 0;
		}
		case WM_COMMAND:
		{

#ifdef _EDITOR
			if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == UPDATE_BUTTON_INDEX)
			{
				//buttonHit();
			}

			if (LOWORD(wParam) == SUB_MENU_NEW_ENTITY)
			{
				newEntity = CreateWindowEx(0,
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

			if (LOWORD(wParam) == SUB_MENU_DELETE_ENTITY)
			{
				if (currentSelectedEntity == NULL)
				{
					MessageBox(hWnd, TEXT("No Entity Selected"), TEXT("Error"), MB_OK);
				}
				else
				{

					if (MessageBox(hWnd, TEXT("Are you sure?"), TEXT("Delete?"), MB_YESNOCANCEL) == IDYES)
					{
						levelEditorScene->removeEntity(currentSelectedEntity);
						delete currentSelectedEntity;
						currentSelectedEntity = NULL;
						//populateLists(scene);
						levelEditorReset(currentSelectedEntity, levelEditorScene);
					}
				}
			}

			else if (LOWORD(wParam) == SUB_MENU_EXPORT_SCENE)
			{
				if (!levelEditorScene->exportScene("../bin/level.scene"))
					MessageBox(hWnd, TEXT("Could Not Locate File"), TEXT("ERROR"), MB_OK);
				else
					MessageBox(hWnd, TEXT("Export Successful"), TEXT("SUCCESS"), MB_OK);
			}

			else if (LOWORD(wParam) == SUB_MENU_IMPORT_SCENE)
			{
				Scene* newScene = importScene("../bin/level.scene", getControllerInstance()->camera());
				if (newScene == NULL)
				{
					MessageBox(hWnd, TEXT("Could Not Locate File"), TEXT("ERROR"), MB_OK);
				}
				else
				{
					delete levelEditorScene;
					levelEditorScene = newScene;
				}
			}

			else if (LOWORD(wParam) == SUB_MENU_NEW_BOUNDARY_ENTITY)
			{
				SphereBoundaryEntity* be = new SphereBoundaryEntity();
				levelEditorScene->addBoundaryEntity(be);
			}

			else if (LOWORD(wParam) == SUB_MENU_NEW_LIGHT_ENTITY)
			{
				LightEntity* le = new LightEntity(POINT_LIGHT);
				levelEditorScene->addLightEntity(le);
			}

			else if (LOWORD(wParam) == SUB_MENU_EXIT)
			{
				if (MessageBox(hWnd, TEXT("Are you sure you want to exit?"), TEXT("EXIT"), MB_YESNOCANCEL) == IDYES)
					done = true;

			}



			if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_NO_LIGHTING)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NO_LIGHTING, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_RASTERIZE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_ADS)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_ADS, MF_CHECKED);
				//windowSettingIndex = ADS_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_ADS;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_BRDF)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_BRDF, MF_CHECKED);
				//windowSettingIndex = ADS_BRDF_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_ADS_BRDF;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_GLOBAL_JACOBI)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_GLOBAL_JACOBI, MF_CHECKED);
				//windowSettingIndex = GLOBAL_JACOBI_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_RADIANCE_CACHE_UNIFORM_GRID)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_RADIANCE_CACHE_UNIFORM_GRID, MF_CHECKED);
				//windowSettingIndex = RADIANCE_CACHE_UNIFORM_GRID_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_MONTE_CARLO_BRDF)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_MONTE_CARLO_BRDF, MF_CHECKED);
				//windowSettingIndex = MONTE_CARLO_BRDF_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_MONTE_CARLO_BRDF;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SHADOW_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SHADOW_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SHADOW_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_PHOTON_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_PHOTON_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_PROGRESSIVE_PHOTON_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PROGRESSIVE_PHOTON_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_PROGRESSIVE_PHOTON_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SVD_BRDF)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SVD_BRDF, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SVD_BRDF;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SH_BRDF)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SH_BRDF, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SH_BRDF;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_WAVELET_ENVIRONMENT_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_WAVELET_ENVIRONMENT_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_WAVELET_ENVIRONMENT_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SH_BRDF_PHOTON_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SH_BRDF_PHOTON_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SH_BRDF_PHOTON_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SHADOW_MAP_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SHADOW_MAP_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_SUBSURFACE_SCATTERING_DIFFUSE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SUBSURFACE_SCATTERING_DIFFUSE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_PHOTON_MAP_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_POSITION_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_POSITION_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_POSITION_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_REFLECTIVE_SHADOW_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_REFLECTIVE_SHADOW_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_REFLECTIVE_SHADOW_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_NORMAL_PROJECTION_TEXTURE)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NORMAL_PROJECTION_TEXTURE, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_NORMAL_PROJECTION_TEXTURE;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_RADIANCE_SHADOW_MAP)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_RADIANCE_SHADOW_MAP, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_RADIANCE_SHADOW_MAP;
				updateWindowSetting(windowSettingIndex);
			}
			else if (LOWORD(wParam) == SUB_MENU_SHADING_TYPE_INSTANT_RADIOSITY)
			{
				uncheckShadingMenuItems();
				CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_INSTANT_RADIOSITY, MF_CHECKED);
				//windowSettingIndex = NO_LIGHTING_WINDOW_INDEX;
				levelEditorScene->shadingType = SHADER_TYPE_INSTANT_RADIOSITY;
				updateWindowSetting(windowSettingIndex);
			}
#endif
		}
			
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#ifdef _EDITOR
void addMenu(HWND hWnd)
{
	HMENU hMenuFile;
	menu = CreateMenu();

	hMenuFile = CreatePopupMenu();
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)hMenuFile, (LPCWSTR)TEXT("File"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_IMPORT_SCENE, (LPCWSTR)TEXT("Import Scene"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_NEW_LIGHT_ENTITY, (LPCWSTR)TEXT("New Light Entity"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_NEW_BOUNDARY_ENTITY, (LPCWSTR)TEXT("New Boundary Entity"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_NEW_ENTITY, (LPCWSTR)TEXT("New Entity"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_DELETE_ENTITY, (LPCWSTR)TEXT("Delete Entity"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_EXPORT_SCENE, (LPCWSTR)TEXT("Export Scene"));
	AppendMenu(hMenuFile, MF_STRING, SUB_MENU_EXIT, (LPCWSTR)TEXT("Exit"));

	hMenuShaderType = CreatePopupMenu();
	AppendMenu(menu, MF_STRING | MF_POPUP | MF_USECHECKBITMAPS, (UINT)hMenuShaderType, (LPCWSTR)TEXT("Shading Type"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_NO_LIGHTING, (LPCWSTR)TEXT("No Lighting"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_ADS, (LPCWSTR)TEXT("Ambient/Diffuse/Specular (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_BRDF, (LPCWSTR)TEXT("Ambient/Diffuse/Specular + BRDF (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_GLOBAL_JACOBI, (LPCWSTR)TEXT("Global Illumination Jacobi (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_RADIANCE_CACHE_UNIFORM_GRID, (LPCWSTR)TEXT("Radiance Caching with Uniform Grids (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_MONTE_CARLO_BRDF, (LPCWSTR)TEXT("Monte Carlo BRDF (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_PROGRESSIVE_PHOTON_MAP, (LPCWSTR)TEXT("Progressive Photon Map (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SVD_BRDF, (LPCWSTR)TEXT("SVD BRDF (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SH_BRDF, (LPCWSTR)TEXT("SH BRDF (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_WAVELET_ENVIRONMENT_MAP, (LPCWSTR)TEXT("Wavelet Environment Map (deprecated)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SH_BRDF_PHOTON_MAP, (LPCWSTR)TEXT("SHBRDF Photon Map (deprecated)"));


	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SHADOW_MAP_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Shadow Map Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_PHOTON_MAP_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Photon Map Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Normal Depth Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Photon Map Transmittance Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_POSITION_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Position Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_NORMAL_PROJECTION_TEXTURE, (LPCWSTR)TEXT("Normal Projection Texture (deferred shader)"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_RADIANCE_SHADOW_MAP, (LPCWSTR)TEXT("Radiance Shadow Map (deferred shader)"));

	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SHADOW_MAP, (LPCWSTR)TEXT("Shadow Map"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_PHOTON_MAP, (LPCWSTR)TEXT("Photon Map"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_SUBSURFACE_SCATTERING_DIFFUSE, (LPCWSTR)TEXT("Subsurface Scattering Diffuse"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_REFLECTIVE_SHADOW_MAP, (LPCWSTR)TEXT("Reflective Shadow Map"));
	AppendMenu(hMenuShaderType, MF_STRING, SUB_MENU_SHADING_TYPE_INSTANT_RADIOSITY, (LPCWSTR)TEXT("Instant Radiosity"));
	uncheckShadingMenuItems();

	SetMenu(hWnd, menu);
}

LRESULT CALLBACK newEntityWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			HWND label = CreateWindow(TEXT("Static"), TEXT("Enter File Path for New Entity:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				20, 20, 200, 20,
				hWnd, (HMENU)HWND_NEW_ENTITY_LABEL_INDEX, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			newEntityEdit = CreateWindow(TEXT("Edit"), TEXT("../bin/assets/obj/"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				20, 2 * 20 + 20, 350, 20,
				hWnd, (HMENU)HWND_NEW_ENTITY_EDIT_INDEX, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			HWND okButton = CreateWindow(TEXT("Button"), TEXT("Create"), WS_CHILD | WS_VISIBLE | SS_LEFT,
				20, 5 * 20 + 20, 80, 55,
				hWnd, (HMENU)HWND_NEW_ENTITY_BUTTON_INDEX, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

			return 0;
		}
		case WM_CLOSE:
			DestroyWindow(hWndNewEntity);
			return 0;

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == HWND_NEW_ENTITY_BUTTON_INDEX)
			{
				char* px = getEditField(&newEntityEdit);
				if (createNewEntity(px))
				{
					DestroyWindow(newEntity);
				}

			}
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool createNewEntity(char* filePath)
{
	//ifstream myFile;
	//myFile.open(__filePath.c_str());
	FILE* file = fopen((const char*)filePath, "r");
	if (!file)
	{
		MessageBox(hWndNewEntity, TEXT("File Does Not Exist"), TEXT("ERROR"), MB_OK);
		return false;
	}
	fclose(file);

	RastShader* s = new RastShader();
	s->setShaderType(SHADER_TYPE_RASTERIZE);
	Model_obj* m = new Model_obj((const char*)filePath, s, false);
	s->addUniform((void*)m->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, true);
	s->addUniform((void*)getControllerInstance()->pm()->data(), "projectionMatrix", UNIFORM_MAT_4, false);
	s->addUniform((void*)m->nm()->data(), "normalMatrix", UNIFORM_MAT_4, true);
	levelEditorScene->addEntity(m);
	levelEditorReset(currentSelectedEntity, levelEditorScene);
	return true;
}

#endif

void levelEditorUpdate(Entity* e, Scene* s)
{
#ifdef _EDITOR
	/*
	for (int i = 0; i < windowSettings[windowSettingIndex].size(); i++)
	{
		windowSettings[windowSettingIndex][i]->update(e, s);
	}
	*/
	updateEditWindow(e, s);
#endif
}

void levelEditorReset(Entity* e, Scene* s)
{
#ifdef _EDITOR
	/*
	for (int i = 0; i < windowSettings[windowSettingIndex].size(); i++)
	{
		windowSettings[windowSettingIndex][i]->reset(e, s);
	}
	*/
	resetEditWindow(e, s);
#endif
}

#ifdef _EDITOR
void unselectEntities()
{
	for (int i = 0; i < levelEditorScene->numEntities(); i++)
	{
		levelEditorScene->getEntity(i)->selected = false;
	}

	for (int i = 0; i < levelEditorScene->numLightEntities(); i++)
	{
		levelEditorScene->getLightEntity(i)->selected = false;
	}

	for (int i = 0; i < levelEditorScene->numBoundaryEntities(); i++)
	{
		levelEditorScene->getBoundaryEntity(i)->selected = false;
	}
}

void uncheckShadingMenuItems()
{
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_BRDF, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NO_LIGHTING, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_ADS, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_GLOBAL_JACOBI, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_RADIANCE_CACHE_UNIFORM_GRID, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_MONTE_CARLO_BRDF, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SHADOW_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PROGRESSIVE_PHOTON_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SVD_BRDF, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SH_BRDF, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_WAVELET_ENVIRONMENT_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SH_BRDF_PHOTON_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SHADOW_MAP_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_SUBSURFACE_SCATTERING_DIFFUSE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_POSITION_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_REFLECTIVE_SHADOW_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_NORMAL_PROJECTION_TEXTURE, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_RADIANCE_SHADOW_MAP, MF_UNCHECKED);
	CheckMenuItem(hMenuShaderType, SUB_MENU_SHADING_TYPE_INSTANT_RADIOSITY, MF_UNCHECKED);
}

unsigned int getLightWindowsIndex(unsigned int lightType)
{
	switch (lightType)
	{
	case POINT_LIGHT:
		return LIGHT_WINDOW_INDEX;
		break;
	case SURFACE_LIGHT:
		return LIGHT_WINDOW_INDEX;
		break;
	case DIRECTIONAL_LIGHT:
		return LIGHT_WINDOW_INDEX;
		break;
	case FRUSTUM_LIGHT:
		return LIGHT_WINDOW_FRUSTUM_INDEX;
		break;
	}
	return LIGHT_WINDOW_INDEX;
}

unsigned int getShaderWindowsIndex(unsigned int shaderIndex)
{
	switch (shaderIndex)
	{
	case SHADER_TYPE_RASTERIZE:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_RAY_TRACE:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_RADIOSITY:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_TREE_BOUNDARY_OUTLINE:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_ADS:
		return ADS_WINDOW_INDEX;
		break;

	case SHADER_TYPE_ADS_BRDF:
		return ADS_BRDF_WINDOW_INDEX;
		break;

	case SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI:
		return GLOBAL_JACOBI_WINDOW_INDEX;
		break;

	case SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID:
		return RADIANCE_CACHE_UNIFORM_GRID_WINDOW_INDEX;
		break;

	case SHADER_TYPE_RAIDANCE_MAP:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_PHOTON_MAP:
		return NO_LIGHTING_WINDOW_INDEX;
		break;

	case SHADER_TYPE_MONTE_CARLO_BRDF:
		
		if (currentSelectedEntity == NULL)
			return MONTE_CARLO_BRDF_WINDOW_INDEX;

		switch (((Model_obj*)currentSelectedEntity)->brdfType)
		{
			case BRDF_TORRANCE_SPARROW:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_TORRANCE_SPARROW;
				break;

			case BRDF_BEARD_MAXWELL:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_BEARD_MAXWELL;
				break;

			case BRDF_COOK_TORRANCE:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_COOK_TORRANCE;
				break;

			case BRDF_KAJIYA:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_KAJIYA;
				break;

			case BRDF_POULIN_FOURNIER:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_POULIN_FOURNIER;
				break;

			case BRDF_HE_TORRANCE_SILLION_GREENBERG:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_HE_TORRANCE_SILLION_GREENBERG;
				break;

			case BRDF_OREN_NAYAR:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_OREN_NAYAR;
				break;

			case BRDF_COUPLED:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_COUPLED;
				break;

			case BRDF_ASHIKHMIN_SHIRLEY:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_ASHIKHMIN_SHIRLEY;
				break;

			case BRDF_GRANIER_HEIDRICH:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_GRANIER_HEIDRICH;
				break;

			case BRDF_MINNAERT :
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_MINNAERT;
				break;

			case BRDF_PHONG:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_PHONG;
				break;

			case BRDF_BLINN:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_BLINN;
				break;

			case BRDF_LEWIS:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_LEWIS;
				break;

			case BRDF_NEUMANN_NEUMANN:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_NEUMANN_NEUMANN;
				break;

			case BRDF_STRAUSS:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_STRAUSS;
				break;

			case BRDF_WARD:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_WARD;
				break;

			case BRDF_SCHLICK:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_SCHLICK;
				break;

			case BRDF_LAFORTUNE:
				return MONTE_CARLO_BRDF_WINDOW_INDEX_BRDF_LAFORTUNE;
				break;

		}
		return MONTE_CARLO_BRDF_WINDOW_INDEX;
		break;
	}
}

#endif