#pragma once

#ifndef _WEB
#include <windows.h>
#include "EntityBoxWindow.h"
#include "SpatialWindow.h"
#include "BasicWindow.h"
#include "IndexOfRefractionWindow.h"
#include "MicrofacetRoughnessWindow.h"
#include "EditWindow.h"
#endif

#include "..\Scene.h"
#include <stdio.h>
#include <vector>
#include "..\LevelEditorGlobals.h"
#include "..\Controller.h"
#include "..\shading utils\ShaderUtils.h"
#include <Richedit.h>

#ifdef _EDITOR

#define UPDATE_BUTTON_HEIGHT 30 
#define UPDATE_BUTTON_WIDTH 80
#define EDIT_POPOUT_WIDTH 350
#define EDIT_POPOUT_HEIGHT 800

#define SUB_MENU_NEW_ENTITY 9001
#define SUB_MENU_EXPORT 9002
#define SUB_MENU_EXIT 9003
#define SUB_MENU_DELETE_ENTITY 9004
#define SUB_MENU_EXPORT_SCENE 9005
#define SUB_MENU_IMPORT_SCENE 9006
#define SUB_MENU_SHADING_TYPE_NO_LIGHTING 9007
#define SUB_MENU_SHADING_TYPE_ADS 9008
#define SUB_MENU_SHADING_TYPE_BRDF 9009
#define SUB_MENU_NEW_LIGHT_ENTITY 9010
#define SUB_MENU_NEW_BOUNDARY_ENTITY 9011
#define SUB_MENU_SHADING_TYPE_GLOBAL_JACOBI 9012
#define SUB_MENU_SHADING_TYPE_RADIANCE_CACHE_UNIFORM_GRID 9013
#define SUB_MENU_SHADING_TYPE_MONTE_CARLO_BRDF 9014
#define SUB_MENU_SHADING_TYPE_SHADOW_MAP 9015
#define SUB_MENU_SHADING_TYPE_PHOTON_MAP 9016
#define SUB_MENU_SHADING_TYPE_PROGRESSIVE_PHOTON_MAP 9017
#define SUB_MENU_SHADING_TYPE_SVD_BRDF 9018
#define SUB_MENU_SHADING_TYPE_SH_BRDF 9019
#define SUB_MENU_SHADING_TYPE_WAVELET_ENVIRONMENT_MAP 9020
#define SUB_MENU_SHADING_TYPE_SH_BRDF_PHOTON_MAP 9021
#define SUB_MENU_SHADING_TYPE_SHADOW_MAP_PROJECTION_TEXTURE 9022
#define SUB_MENU_SHADING_TYPE_SUBSURFACE_SCATTERING_DIFFUSE 9023
#define SUB_MENU_SHADING_TYPE_PHOTON_MAP_PROJECTION_TEXTURE 9024
#define SUB_MENU_SHADING_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE 9025
#define SUB_MENU_SHADING_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE 9026
#define SUB_MENU_SHADING_TYPE_POSITION_PROJECTION_TEXTURE 9027
#define SUB_MENU_SHADING_TYPE_REFLECTIVE_SHADOW_MAP 9028
#define SUB_MENU_SHADING_TYPE_NORMAL_PROJECTION_TEXTURE 9029
#define SUB_MENU_SHADING_TYPE_RADIANCE_SHADOW_MAP 9030
#define SUB_MENU_SHADING_TYPE_INSTANT_RADIOSITY 9031
#define SUB_MENU_SHADING_TYPE_STOCHASTIC_RAY_TRACE 9032
#define SUB_MENU_SHADING_TYPE_IRRADIANCE_CACHING 9033
#define SUB_MENU_SHADING_TYPE_POINT_CLOUD 9034

#define HWND_NEW_ENTITY_LABEL_INDEX 0
#define HWND_NEW_ENTITY_EDIT_INDEX 1
#define HWND_NEW_ENTITY_BUTTON_INDEX 2
#define UPDATE_BUTTON_INDEX 3
#define EDIT_POPOUT 4
#define EDIT_SCROLLBAR 5

extern HMENU menu;
extern HWND updateSceneButton;
extern HMENU hMenuFile;
extern HWND hwndNewEntityEdit;
extern HWND hWndNewEntity;

extern std::vector<BasicWindow*> allWindows;

extern std::vector<BasicWindow*> noLightingWindow;
extern std::vector<BasicWindow*> lightWindow;
extern std::vector<BasicWindow*> adsWindow;
extern std::vector<BasicWindow*> adsBrdfWindow;
extern std::vector<BasicWindow*> globalJacobiWindow;
extern std::vector<BasicWindow*> radianceCacheUGWindow;
extern std::vector<BasicWindow*> MonteCarloBrdfWindow;

extern std::vector<std::vector<BasicWindow*>> windowSettings;
void createWindowSettings();
void updateWindowSetting(unsigned int setting);

void addMenu(HWND hWnd);
void createTabWindow(HWND hWnd);
//void buttonHit();

LRESULT CALLBACK newEntityWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void uncheckShadingMenuItems();
void unselectEntities();
bool createNewEntity(char* filePath);
unsigned int getShaderWindowsIndex(unsigned int shaderIndex);
unsigned int getLightWindowsIndex(unsigned int lightType);

#endif

#ifndef _WEB
extern BasicWindow baseWindow;
extern HDC deviceContext;
extern HGLRC renderingContext;
#endif
extern bool windowActive;

#ifndef _WEB
bool createLevelEditorWindow(int width, int height, int bits);
LRESULT CALLBACK levelEditorWindowCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void resizeLevelEditorWindow(int width, int height);
#endif
void levelEditorUpdate(Entity* e, Scene* s);
void levelEditorReset(Entity* e, Scene* s);
