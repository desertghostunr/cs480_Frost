#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"
#include <vector>

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
        void updateCamera( bool isOn, glm::vec3 planet );
        glm::mat4 GetProjection();
        glm::mat4 GetView();

        void setZoomSelect( unsigned int selectVal );

    private:
        bool topDown;
        float upDown;
        float leftRight;
        float zoom;

        std::vector<float> zoomMinimum;
        unsigned int zoomSelect;

        glm::mat4 projection;
        glm::mat4 view;
};

#endif /* CAMERA_H */
