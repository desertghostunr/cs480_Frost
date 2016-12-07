#include "camera.h"

Camera::Camera()
{
	topDown = true;
	leftRight = 0.0;
	zoom = 150.0;
	upDown = 0.0;
	
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

    leftRight = 0.0f;
    upDown = 0.0;
    zoom = 255.0;
    topDown = false;
    view = glm::lookAt( glm::vec3( leftRight, 147.22  + upDown, zoom ), //Eye Position
                        glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                        glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.15f, //Distance to the near plane, normally a small value like this
                                   6000.0f); //Distance to the far plane, 
    return true;
}


void Camera::LookTopDown()
{
    leftRight = 0.0;
    upDown = 0.0;
    zoom = 150.0;
    topDown = true;
    view = glm::lookAt( glm::vec3( 0.0, zoom, 0.0), //Eye Position
                                   glm::vec3( leftRight, 0.0, upDown), //Focus point
                                   glm::vec3(0.0, 0.0, 1.0)); //Positive Y is up

}

void Camera::LookSideToSide()
{
    leftRight = 0.0;
    upDown = 0.0;
    zoom = 255.0;
    topDown = false;
    view = glm::lookAt( glm::vec3( 0.0, 147.22, zoom), //Eye Position
                                   glm::vec3( leftRight, upDown, 0.0), //Focus point
                                   glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

}

void Camera::moveLeft()
{
   if( topDown )
   {
      leftRight = leftRight + 1.0;
   }
   else
   {
      leftRight = leftRight - 1.0;
   }

   view = glm::lookAt( glm::vec3( leftRight, 147.22 + upDown, 255 ), //Eye Position
                       glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                       glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}
void Camera::moveRight()
{
   if( topDown )
   {
      leftRight = leftRight - 1.0;
   }
   else
   {
      leftRight = leftRight + 1.0;
   }

   view = glm::lookAt( glm::vec3( leftRight, 147.22 + upDown, 255 ), //Eye Position
                       glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                       glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}
void Camera::moveUp()
{
   upDown = upDown + 1.0;
   view = glm::lookAt( glm::vec3( leftRight, 147.22 + upDown, zoom ), //Eye Position
                       glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                       glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}
void Camera::moveDown()
{
   upDown = upDown - 1.0;
   view = glm::lookAt( glm::vec3( leftRight, 147.22 + upDown, zoom ), //Eye Position
                       glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                       glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}

void Camera::zoomIn()
{
    if( topDown && zoom <= 10.0f )
    {
        return;
    }
      
    zoom = zoom - 5;

}
void Camera::zoomOut()
{
  
    zoom = zoom + 5;
   
}

void Camera::resetView( )
{
    upDown = 0.0f;
    leftRight = 0.0f;
    view = glm::lookAt( glm::vec3( leftRight, 147.22 + upDown, zoom ), //Eye Position
                        glm::vec3( 0.0, 0.0, 0.0 ), //Focus point
                        glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}

void Camera::followShip( glm::vec3 shipPosition, glm::vec3 cameraPosition, glm::vec3 leftView, glm::vec3 rightView, int viewSelect )
{
	glm::vec3 focalPoint;

	if( viewSelect == LOOK_LEFT )
	{
		focalPoint = leftView;
	}
	else if( viewSelect == LOOK_RIGHT )
	{
		focalPoint = rightView;
	}
	else
	{
		focalPoint = shipPosition;
	}

	view = glm::lookAt( cameraPosition, //Eye Position
						focalPoint, //Focus point
						glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
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
        upDown = planet.y + 50;
	    newZoom = planet.z + 160;
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

void Camera::rotate( glm::vec3 position, glm::vec4 whereToLook )
{
   
   view = glm::lookAt( glm::vec3( whereToLook.x + position.x, position.y, whereToLook.z + position.z), //Eye Position
                       glm::vec3( whereToLook.x , whereToLook.y, whereToLook.z ), //Focus point
                       glm::vec3( 0.0, 1.0, 0.0 ) ); //Positive Y is up
}


