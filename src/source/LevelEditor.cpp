/**********************************************************************
* FILENAME :        LevelEditor.cpp
*
* DESCRIPTION :
*       Entry point for the Engine/level editor. int WINAPI WinMain
*		Contains two builds:
*			Editor and Viewer.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/


#ifndef _WEB
#include <windows.h>
#endif
#include <GL\glew.h>
#include <GL\glut.h>
#include <stdio.h>

#ifdef _WEB
#include "classes\glgreg\header\window\LevelEditorWindow.h"
#include "classes\glgreg\header\Timer.h"
#include "classes\glgreg\header\OpenGLInit.h"
#include "classes\glgreg\header\Controller.h"
#include "classes\glgreg\header\Model.h"
#include "classes\glgreg\header\shader\RastShader.h"
#include "classes\glgreg\header\Scene.h"
#include "classes\glgreg\header\AABBTree.h"
#include "classes\glgreg\header\LightEntity.h"
#include "classes\glgreg\header\BoundaryEntity.h"
#include "classes\glgreg\header\levelEditorGlobals.h"
#include "classes\glgreg\header\Renderbuffer.h"
#else
#include "data structures/Timer.h"
#include "Controller.h"
#include "entities/Model.h"
#include "shader\RastShader.h"
#include "Scene.h"
#include "data structures/AABBTree.h"
#include "entities/LightEntity.h"
#include "entities/BoundaryEntity.h"
#include "window\LevelEditorWindow.h"
#include "levelEditorGlobals.h"
#include "openGL/Renderbuffer.h"
#include "shader/FilterBlurShader.h"
#include <CL\cl.h>
#endif


#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_SPACEBAR 32
#define KEY_SHIFT 16

#ifdef _EDITOR
#define GL_VIEWPORT_POSITION_X 0
#define GL_VIEWPORT_POSITION_Y 0
#else
#define GL_VIEWPORT_POSITION_X 0
#define GL_VIEWPORT_POSITION_Y 0
#endif
#ifdef _EDITOR
#define MOVE_SPEED 1.0f
#endif
#ifdef _VIEWER
#define MOVE_SPEED 3.0f
#endif
#define OBJ_FILE_PATH_CRATE "../bin/assets/obj/crate/Crate.obj"
#define OBJ_FILE_PATH_CORNELL_BOX "../bin/assets/obj/CornellBox/cornellbox.obj"
//#define OBJ_FILE_PATH_CORNELL_BOX "../bin/assets/obj/CornellBox/cornellbox.obj"

/*base window instance*/
#ifndef _WEB
HWND hWnd = NULL;
#endif

/*global variables : done, levelEditorScene*/
bool done = false;
Scene* levelEditorScene = NULL;
static int glViewportWidth = SCREEN_WIDTH - GL_VIEWPORT_POSITION_X;
static int glViewportHeight = SCREEN_HEIGHT - GL_VIEWPORT_POSITION_Y;
Controller* controller;
//FilterBlurShader* filterBlurShader;
Timer renderTimer;
Timer updateTimer;
#ifdef _EDITOR
Entity* currentSelectedEntity;
#endif

RenderbufferMS* rbms;

/*
WINDOW ROUTINES
*/
#ifndef _WEB
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
#endif

int drawGLScene();
void killGLWindow();
void resizeGLScene(int width, int height);
void initGL();
void init();
void initShaders();
void loadScene(const char* path);

/*
void loadScene
@param const char* path

imports and populates a scene from a file path "path"
*/
void loadScene(const char* path)
{
	/*load the scene*/
	Scene* newScene = importScene(path, controller->camera());

	if (newScene == NULL)
	{
#ifndef _WEB
		MessageBox(hWnd, TEXT("Could Not Locate File"), TEXT("ERROR"), MB_OK);
#endif
	}
	else
	{
		/* deletes the old scene */
		delete levelEditorScene;
		levelEditorScene = newScene;
	}
}

/*
void engineLoop
heart of the engine. function is called after initialization.
Exited when the program is terminated.
*/
void engineLoop()
{
#ifndef _WEB
	MSG msg;
#endif
	/*reset the timers*/
	renderTimer.start();
	updateTimer.start();
	
	/*setting done to true terminates the program.
	continue while done == false;
	*/
	while (!done)
	{

#ifndef _WEB
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
#endif
		{
			if (windowActive)
			{
#ifndef _WEB
				if (keyPress[VK_ESCAPE])
				{
					done = true;
				}
				else
#endif
				{
					/*render time elapsed*/
					if (renderTimer.isTimeOut(MILLISECONDS_PER_FRAME))
					{
						/*draw the scene*/
						drawGLScene();
#ifndef _WEB
						/*swap the buffers*/
						SwapBuffers(deviceContext);
#endif
						/*reset the render timer*/
						renderTimer.start();
					}

				}

				/*update timer elapsed*/
				if (updateTimer.isTimeOut(MILLISECONDS_PER_UPDATE))
				{
					/*update the camera position*/
					if (keyPress[KEY_W])
					{
#ifdef _EDITOR
						controller->camera()->moveGlobal(0, 0, -1.5);
#endif

#ifdef _VIEWER
						controller->camera()->moveLocal(0.0f, 0.0f, MOVE_SPEED);
#endif
					}

					if (keyPress[KEY_A])
					{
#ifdef _EDITOR
						controller->camera()->moveGlobal(-1.5, 0, 0);
#endif

#ifdef _VIEWER
						controller->camera()->moveLocal(-MOVE_SPEED, 0.0f, 0.0f);
#endif
					}

					if (keyPress[KEY_S])
					{
#ifdef _EDITOR
						controller->camera()->moveGlobal(0, 0, 1.5);
#endif

#ifdef _VIEWER
						controller->camera()->moveLocal(0.0f, 0.0f, -MOVE_SPEED);
#endif
					}

					if (keyPress[KEY_D])
					{
#ifdef _EDITOR
						controller->camera()->moveGlobal(1.5, 0, 0);
#endif

#ifdef _VIEWER
						controller->camera()->moveLocal(MOVE_SPEED, 0.0f, 0.0f);
#endif
					}

#ifdef _VIEWER
					if (keyPress[KEY_SPACEBAR])
					{
						controller->camera()->moveLocal(0.0f, MOVE_SPEED, 0.0f);
					}

					if (keyPress[KEY_SHIFT])
					{
						controller->camera()->moveLocal(0.0f, -MOVE_SPEED, 0.0f);
					}
#endif

					controller->update();
					updateTimer.start();
				}
			}

		}
	}
}

#ifdef _WEB
int main(int argc, char **argv)
{
	resizeGLScene(SCREEN_WIDTH, SCREEN_HEIGHT);
	initGL();
	init();
}
#endif

/*
int WINAPI WinMain
entry point for the level editor.
*/
#ifndef _WEB
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*create the level editor window*/
	createLevelEditorWindow(SCREEN_WIDTH, SCREEN_HEIGHT
#ifdef _EDITOR
		+ GetSystemMetrics(SM_CYMENU)
#endif	
		,16);
	ShowWindow(baseWindow.win, SW_SHOW);
	SetForegroundWindow(baseWindow.win);
	SetFocus(baseWindow.win);
	resizeGLScene(SCREEN_WIDTH, SCREEN_HEIGHT

#ifdef _EDITOR
		+ GetSystemMetrics(SM_CYMENU)
#endif		
		);

	MSG msg;
	/*initialize*/
	initGL();
	init();

	/*call the engine loop*/
	engineLoop();

	return 0;
}
#endif


/*
	@function drawGLScene()

	@params:

	Envokes all entity updates
	Envokes entity collision detection
	Draws all entities in the scene, ommits all lights.
*/
int drawGLScene()
{
#if POST_PROCESSING
	rbms->bindFramebuffer();
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	controller->updateCameraMatrix();
	levelEditorScene->updateScene();
	levelEditorScene->detectCollisions();

#ifdef _EDITOR
	levelEditorScene->mousePick((controller->mouseX() - 0.5*SCREEN_WIDTH) / (0.5*SCREEN_WIDTH), (controller->mouseY() - 0.5*SCREEN_HEIGHT) / (0.5*SCREEN_HEIGHT));
#endif
	levelEditorScene->render();
	

#if POST_PROCESSING
	rbms->unbindFramebuffer();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, rbms->framebuffer->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glFlush();
#endif

	return 0;
}


/*
	@function killGLWindow()

	@params:

	callback, exit window.
*/
void killGLWindow()
{

}

void initGL()
{
	GLclampf x = 0.25;
	glClearColor(0.0f, 0.0f, 0.0, 1.0f);
	glewInit();
}
void resizeGLScene(int width, int height)
{
#ifdef _EDITOR
	glViewportWidth = width;
	glViewportHeight = height;
#endif

#ifdef _VIEWER
	glViewportWidth = width;
	glViewportHeight = height;
	glScissor(0, 0, width, height);
	glViewport(0, 0, width, height);
#endif
}

void init()
{	
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	controller = new Controller(SCREEN_WIDTH - GL_VIEWPORT_POSITION_X, SCREEN_HEIGHT - GL_VIEWPORT_POSITION_Y, NEAR_PLANE, FAR_PLANE);

	initShaders();
	glDepthRange(1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	levelEditorScene->preRender();
	rbms = new RenderbufferMS(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);
}

void initShaders()
{
	initTex();
	levelEditorScene = new Scene(controller->camera());
#ifdef _EDITOR
	RastShader* rastShaderC = new RastShader();
	//Model_obj* cornellBox = new Model_obj(OBJ_FILE_PATH_CORNELL_BOX, rastShaderC, false);
	//cornellBox->scaleTo(32, 32, 32);

	LightEntity* light = new LightEntity(SURFACE_LIGHT);
	light->moveTo(0.0, 27.0, 30.0);
	//light->scale(3.5, 3.5, 3.5);
	light->intensity = 8.0;
	//levelEditorScene->addEntity(cornellBox);
	levelEditorScene->addLightEntity(light);
	levelEditorUpdate(currentSelectedEntity, levelEditorScene);
	updateWindowSetting(windowSettingIndex);
#endif

#ifdef _VIEWER
	loadScene("../bin/level.scene");
#endif
}