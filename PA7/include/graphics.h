#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <vector>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"

#include "GraphicsInfo.h"
#include "ObjectTable.h"

#include"Instance.h"

class Graphics
{
    public:
        static const int INCREASE_SPEED = 0;
        static const int DECREASE_SPEED = 1;
        static const int RESET_SPEED = 2;

        Graphics();
        ~Graphics();
        bool Initialize(int width, int height, 
                                        const GraphicsInfo& progInfo );
        void Update(unsigned int dt);
        void Render();

        bool updateList( unsigned int objectID, unsigned int dt );

        bool updateChildren( unsigned int objectID, unsigned int dt );

        bool updateRotation( unsigned int objectID, float speedOfRotation );
        bool updateOrbit( unsigned int objectID, float speedOfOrbit );

        bool toggleRotationDirection( unsigned int objectID );
        bool toggleRotationPaused( unsigned int objectID );
        
        bool toggleOrbitDirection( unsigned int objectID );
        bool toggleOrbitPaused( unsigned int objectID );

        bool toggleAllPaused( unsigned int objectID );
        void toggleAllObjectsPaused( );

        void changeOrbitSpeed( unsigned int opCode );

        void changeRotSpeed( unsigned int opCode );


	void ChangePerspectiveStatic( int position );

	void cameraLeftOrRight( bool left );
	void cameraUpOrDown( bool up);
	void cameraZoomInOrOut(bool in);

	void startTracking( int planet );



    private:
        std::string ErrorString(GLenum error);
	
	bool cameraTracking;
	int planetIndex;

        Camera *m_camera;
        Shader *m_shader;

        GLint m_projectionMatrix;
        GLint m_viewMatrix;
        GLint m_modelMatrix;

        ObjectTable objectRegistry; //holds the scene's objects

        std::vector<Instance> modelRegistry;

        std::vector<unsigned int> planetRegistry;
};

#endif /* GRAPHICS_H */
