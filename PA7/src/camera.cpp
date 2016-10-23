#include "camera.h"

Camera::Camera()
{
	topDown = true;
	leftRight = 0.0;
	zoom = 15.0;
	upDown = 0.0;
	
    zoomMinimum = { 0.50f, 0.50f, 0.50f, 0.50f, 3.0f, 3.0f, 1.50f, 1.0f, 0.50f };
    zoomSelect = 0;
}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
    //--Init the view and projection matrices
    //    if you will be having a moving camera the view matrix will need to more dynamic
    //    ...Like you should update it before you render more dynamic 
    //    for this project having them static will be fine

    LookTopDown();

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   1000.0f); //Distance to the far plane, 
    return true;
}


void Camera::LookTopDown()
{
    zoomSelect = 0;

    leftRight = 0.0;
    upDown = 0.0;
    zoom = 15.0;
    topDown = true;
    view = glm::lookAt( glm::vec3( 0.0, zoom, 0.0), //Eye Position
                                   glm::vec3( leftRight, 0.0, upDown), //Focus point
                                   glm::vec3(0.0, 0.0, 1.0)); //Positive Y is up

}

void Camera::LookSideToSide()
{
    zoomSelect = 0;

    leftRight = 0.0;
    upDown = 0.0;
    zoom = 15.0;
    topDown = false;
    view = glm::lookAt( glm::vec3( 0.0, 0.80, zoom), //Eye Position
                                   glm::vec3( leftRight, upDown, 0.0), //Focus point
                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

}

void Camera::moveLeft()
{
   if( topDown )
   {
      leftRight = leftRight + 0.10;
   }
   else
   {
      leftRight = leftRight - 0.10;
   }
}
void Camera::moveRight()
{
   if( topDown )
   {
      leftRight = leftRight - 0.10;
   }
   else
   {
      leftRight = leftRight + 0.10;
   }
}
void Camera::moveUp()
{
   upDown = upDown + 0.10;
}
void Camera::moveDown()
{
   upDown = upDown - 0.10;
}

void Camera::zoomIn()
{
    if( topDown && zoom <= 1.0f )
    {
        return;
    }

    if( zoom <= -200.0f )
    {
        return;
    }
      
    zoom = zoom - 0.5;

    if( ( zoom >= 0.0f ) && ( zoom < zoomMinimum[ zoomSelect ] ) )
    {
        zoom = -1.0f * zoomMinimum[ zoomSelect ];
    }

}
void Camera::zoomOut()
{
    if( zoom >= 200.0f )
    {
        return;
    }
  
    zoom = zoom + 0.5;

    if( ( zoom <= 0.0f ) && ( zoom > ( -1.0f * zoomMinimum[ zoomSelect ] ) ) )
    {
        zoom = zoomMinimum[ zoomSelect ];
    }
   
}

void Camera::updateLookAt()
{
   if( topDown )
   {
      view = glm::lookAt( glm::vec3( leftRight, zoom, upDown), //Eye Position
                                   glm::vec3( leftRight, 0.0, upDown), //Focus point
                                   glm::vec3(0.0, 0.0, 1.0)); //Positive Y is up
   }
   else
   {
      view = glm::lookAt( glm::vec3( leftRight, 8.0 + upDown, zoom), //Eye Position
                                   glm::vec3( leftRight, upDown, 0.0), //Focus point
                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
   }

}

void Camera::updateCamera( bool isOn, glm::vec3 planet )
{
   if( isOn )
   {
	float newZoom = 0.0;

    if( ( zoom <= 0.0f ) && ( zoom > ( -1.0f * zoomMinimum[ zoomSelect ] ) ) )
    {
        zoom = -1.0f * zoomMinimum[ zoomSelect ];
    }
    else if( ( zoom >= 0.0f ) && ( zoom < zoomMinimum[ zoomSelect ] ) )
    {
        zoom = zoomMinimum[ zoomSelect ];
    }


	if( topDown )
    {
	    leftRight = planet.x;
	    newZoom = planet.y + zoom;
	    upDown = planet.z;
	     view = glm::lookAt( glm::vec3( leftRight, newZoom, upDown), //Eye Position
                                  planet, //Focus point
                                   glm::vec3(0.0, 0.0, 1.0)); //Positive Y is up
	}
	else
	{
	    leftRight = planet.x;
        upDown = planet.y;

        if( ( zoomSelect == 4 )
            || ( zoomSelect == 5 )
            || ( zoomSelect == 6 )
            || ( zoomSelect == 7 ) )
        {
            upDown += 0.13f;
        }

	    newZoom = planet.z + zoom;
   	    view = glm::lookAt( glm::vec3( leftRight, upDown, newZoom), //Eye Position
                                   planet, //Focus point
                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
	}

   }

}

glm::mat4 Camera::GetProjection()
{
    return projection;
}

glm::mat4 Camera::GetView()
{
    return view;
}



void Camera::setZoomSelect( unsigned int selectVal )
{
    if( selectVal < zoomMinimum.size( ) )
    {
        zoomSelect = selectVal;
    }
    else
    {
        zoomSelect = 0;
    }
}


