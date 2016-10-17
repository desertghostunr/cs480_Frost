#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
    public:
        Camera();
        ~Camera();
        bool Initialize(int w, int h);
	void LookTopDown();
	void LookSideToSide();
	void moveLeft();
 	void moveRight();
	void moveUp();
	void moveDown();	
	void zoomIn();
	void zoomOut();
 	void updateLookAt();
        glm::mat4 GetProjection();
        glm::mat4 GetView();

    private:
	bool topDown;
	float upDown;
	float leftRight;
	float zoom;

        glm::mat4 projection;
        glm::mat4 view;
};

#endif /* CAMERA_H */
