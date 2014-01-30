//========================================================================
// GLFW - An OpenGL library
// Platform:    Win32
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "win32_platform.h"


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

void _glfwPlatformInitDropfiles(_GLFWwindow* window)
{
    // WM_DROPFILES enable
    DragAcceptFiles(window->win32.handle, TRUE);

	window->dropfilesId = 0;
	window->dropfilesNum = 0;
	window->dropfilesString = NULL;
}

void _glfwPlatformDestroyDropfiles(_GLFWwindow* window)
{
	int i = window->dropfilesNum;
	while(i > 0) {
		--i;
		free(window->dropfilesString[i]);
	}
	window->dropfilesNum = 0;
	free(window->dropfilesString);
	window->dropfilesString = NULL;
}

void _glfwPlatformSetDropfiles(_GLFWwindow* window, HDROP hDrop)
{
	_glfwPlatformDestroyDropfiles(window);

	unsigned int num = DragQueryFile(hDrop, 0xffffffff, NULL, 0);
	if(num > 0) {
		window->dropfilesNum = num;

		char** pp = (char**)malloc(sizeof(char*) * (num + 1));
		int i;
		for(i = 0; i < num; ++i) {
			WCHAR path[256];
			path[0] = 0;
			int n = DragQueryFile(hDrop, i, path, sizeof(path));
			if(n) {
				path[n] = 0;
				pp[i] = _glfwCreateUTF8FromWideString(path);
			} else {
				pp[i] = NULL;
			}
		}
		pp[num] = NULL;
		DragFinish(hDrop);

		window->dropfilesString = pp;
		window->dropfilesId++;

		if(window->callbacks.dropfiles) {
			window->callbacks.dropfiles((GLFWwindow*)window, num, (const char**)pp);
		}
	}
}

int _glfwPlatformGetDropfilesId(_GLFWwindow* window)
{
	return window->dropfilesId & 0x7fffffff;
}

const char** _glfwPlatformGetDropfilesString(_GLFWwindow* window, int* count)
{
	if(count != NULL) {
		*count = window->dropfilesNum;
	}		
	return (const char**)window->dropfilesString;
}
