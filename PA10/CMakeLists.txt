PROJECT(Pinball)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules")
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLM REQUIRED)
FIND_PACKAGE(Assimp REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Bullet REQUIRED)
SET(CXX11_FLAGS -std=gnu++11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
SET(TARGET_LIBRARIES "${OPENGL_LIBRARY} ${SDL2_LIBRARY} ${ASSIMP_LIBRARY} ${OpenCV_LIBS} ${BULLET_LIBRARIES}")

IF(UNIX)
  ADD_DEFINITIONS(-DUNIX)
ENDIF(UNIX)

IF(NOT APPLE)
  IF(GLEW_FOUND)
      INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIRS})
      LINK_LIBRARIES(${GLEW_LIBRARIES})
  ENDIF(GLEW_FOUND)
ENDIF(NOT APPLE)

IF(NOT ASSIMP_FOUND)
  FIND_PACKAGE(assimp REQUIRED)
ENDIF(NOT ASSIMP_FOUND)

INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIR})
LINK_LIBRARIES(${ASSIMP_LIBRARIES})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
LINK_LIBRARIES(${OpenCV_LIBS})

INCLUDE_DIRECTORIES( ${BULLET_INCLUDE_DIR})
LINK_LIBRARIES( ${BULLET_LIBRARIES})


INCLUDE_DIRECTORIES(
  "${PROJECT_SOURCE_DIR}/include"
  ${SDL2_INCLUDE_DIR}
  ${GLM_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${BULLET_INCLUDE_DIR}
)


# Set Includes
SET(INCLUDES ${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${INCLUDES})

# Set sources
FILE(GLOB_RECURSE SOURCES "src/*.cpp")
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

add_custom_target("${PROJECT_NAME}_SUCCESSFUL" ALL
                  DEPENDS ${PROJECT_NAME}
                  COMMAND ${CMAKE_COMMAND} -E echo ""
                  COMMAND ${CMAKE_COMMAND} -E echo "====================="
                  COMMAND ${CMAKE_COMMAND} -E echo "  Compile complete!"
                  COMMAND ${CMAKE_COMMAND} -E echo "====================="
                  COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_CURRENT_BINARY_DIR}"
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/shaders
                  COMMAND ${CMAKE_COMMAND} -E echo "Copied ${PROJECT_SOURCE_DIR}/shaders/ to ${CMAKE_CURRENT_BINARY_DIR}/shaders"
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/models/ ${CMAKE_CURRENT_BINARY_DIR}/models
                  COMMAND ${CMAKE_COMMAND} -E echo "Copied ${PROJECT_SOURCE_DIR}/models/ to ${CMAKE_CURRENT_BINARY_DIR}/models"
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/sounds/ ${CMAKE_CURRENT_BINARY_DIR}/sounds
                  COMMAND ${CMAKE_COMMAND} -E echo "Copied ${PROJECT_SOURCE_DIR}/sounds/ to ${CMAKE_CURRENT_BINARY_DIR}/sounds"
				  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/config/ ${CMAKE_CURRENT_BINARY_DIR}/config
                  COMMAND ${CMAKE_COMMAND} -E echo "Copied ${PROJECT_SOURCE_DIR}/config/ to ${CMAKE_CURRENT_BINARY_DIR}/config"
                 )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARY} ${SDL2_LIBRARY} ${ASSIMP_LIBRARY} ${OpenCV_LIBS} ${BULLET_LIBRARIES})
