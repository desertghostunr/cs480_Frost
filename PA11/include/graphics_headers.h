#ifndef GRAPHICS_HEADERS_H
#define GRAPHICS_HEADERS_H

#include <iostream>

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/gl3.h>
    #include <OpenGL/GLU.h>
#else //linux as default
    #include <GL/glew.h>
#endif

// GLM for matricies
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <btBulletDynamicsCommon.h>

#define INVALID_UNIFORM_LOCATION 0x7fffffff

#if defined( _WIN64 ) || defined( _WIN32 )
	#define M_PI 3.14159265358979323846264338327950288
#endif

struct Vertex
{
    glm::vec3 vertex;
    glm::vec2 uv;
    glm::vec3 normal;

    Vertex(glm::vec3 v, glm::vec2 u, glm::vec3 n): vertex(v), uv(u), normal(n) {}
};

#endif /* GRAPHICS_HEADERS_H */
