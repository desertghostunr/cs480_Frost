# - Try to find OpenCV
# Once done this will define
#
#  OPENCV_FOUND - system has OpenCV
#  OPENCV_INCLUDE_DIR - the OpenCV include directory
#  OPENCV_LIBRARIES - Link these to use OpenCV
#

message("<FindOpenCV.cmake>")

SET(OPENCV "libopencv_core")

FIND_PATH(OPENCV_INCLUDE_DIR NAMES opencv2
  PATHS
  /usr/include
  /usr/local/include
  /opt/local/include
  NO_DEFAULT_PATH
)
 

FIND_LIBRARY(LIBOPENCV
  NAMES 
  ${OPENCV}
  PATHS
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  NO_DEFAULT_PATH
)

SET(OPENCV_LIBRARIES
  ${LIBOPENCV} 
)

IF(OPENCV_INCLUDE_DIR AND OPENCV_LIBRARIES)
   SET(OPENCV_FOUND TRUE)
ENDIF(OPENCV_INCLUDE_DIR AND OPENCV_LIBRARIES)

IF(OPENCV_FOUND)
  MARK_AS_ADVANCED(OPENCV_INCLUDE_DIR OPENCV_LIBRARIES )
ENDIF(OPENCV_FOUND)

message("</FindOpenCV.cmake>")