#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
    public:

		static const int LOOK_AT_SHIP = 0;
		static const int LOOK_LEFT = 1;
		static const int LOOK_RIGHT = 2;

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
        void rotate( glm::vec3 position, glm::vec4 whereToLook );

        void resetView( );

		void followShip( glm::vec3 shipPosition, glm::vec3 cameraPosition, 
						 glm::vec3 leftView, glm::vec3 rightView, int viewSelect );
		
 	void updateLookAt();
	void updateCamera( bool isOn, glm::vec3 planet );
        void changePerspective( int width, int height );

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
