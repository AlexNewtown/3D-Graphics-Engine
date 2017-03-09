#pragma once
#include <windows.h>
#include "entities/Entity.h"
#include "Scene.h"

class BasicWindow
{
public:	
	HWND win;

	/* 
		UPDATE functions read data from the GUIs and write
		them to the global variables
	*/
	void(*update)(Entity* e, Scene* s);


	/*
		RESET functions read data from the global variables
		and writes them to the GUIs
	*/
	void(*reset)(Entity* e, Scene* s);
};
