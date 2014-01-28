//========================================================================
// GLFW - An OpenGL library
// Platform:    Any
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

#include <stdlib.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////


GLFWAPI int glfwGetDropfilesId(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(-1);
    return _glfwPlatformGetDropfilesId(window);
}

GLFWAPI const char** glfwGetDropfilesString(GLFWwindow* handle, int* count)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return _glfwPlatformGetDropfilesString(window, count);
}

GLFWAPI GLFWdropfilesfun glfwSetDropfilesCallback(GLFWwindow* handle, GLFWdropfilesfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    GLFWdropfilesfun previous;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    previous = window->callbacks.dropfiles;
    window->callbacks.dropfiles = cbfun;
    return previous;
}

