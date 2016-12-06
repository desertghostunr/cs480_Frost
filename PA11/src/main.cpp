// header files ///////////////////////////////
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "rapidxml/rapidxml.hpp"
#include "engine.h"

//Global Constants //////////////////////////
//config options
const string CONFIG_OPT = "-c";

//help options
const string HELP_OPT = "--h";

//general characters
const char TAB_CHAR = '\t';

//xml types
const string SHADER = "Shader";
const string X_SCALE = "xScale";
const string Y_SCALE = "yScale";
const string Z_SCALE = "zScale";
const string BULLET_X_SCALE = "xBScale";
const string BULLET_Y_SCALE = "yBScale";
const string BULLET_Z_SCALE = "zBScale";
const string X_POS = "xPosition";
const string Y_POS = "yPosition";
const string Z_POS = "zPosition";
const string X_ROT_AXIS = "xRotationAxis";
const string Y_ROT_AXIS = "yRotationAxis";
const string Z_ROT_AXIS = "zRotationAxis";
const string X_ROT_ANGLE = "xRotationAngle";
const string Y_ROT_ANGLE = "yRotationAngle";
const string Z_ROT_ANGLE = "zRotationAngle";
const string VERTEX = "Vertex";
const string FRAGMENT = "Fragment";
const string NAME = "name";
const string LIGHT = "Light";
const string SPOT_LIGHT = "SpotLight";
const string AMBIENT = "Ambient";
const string CONE_ANGLE = "ConeAngle";
const string SPOT_HEIGHT = "SpotHeight";
const string FOLLOW_OBJ = "FollowMe";
const string WINDOW_INFO = "Window";
const string WIDTH = "width";
const string HEIGHT = "height";
const string WIND_DIRECTION = "Wind_Direction";

// free function prototypes ////////////////
bool ProcessCommandLineParameters( int argCount, char **argVector, 
                                   GraphicsInfo& progInfo );

bool ReadConfigurationFile( const std::string& fileName, GraphicsInfo& progInfo );

bool ProcessConfigurationFile( rapidxml::xml_node<> *rootNode, 
                               GraphicsInfo& progInfo );

bool ProcessConfigurationFileHelper( rapidxml::xml_node<> *parentNode,
                                     GraphicsInfo& progInfo );

bool ProcessConfigShader( rapidxml::xml_node<> *parentNode, GraphicsInfo& progInfo );

bool ProcessConfigLight( rapidxml::xml_node<> *parentNode, 
						 GraphicsInfo& progInfo, bool spotLight = false );

bool ProcessConfigAmbient( rapidxml::xml_node<> *parentNode, 
						   GraphicsInfo& progInfo, bool spotLight = false );

bool ProcessWind( rapidxml::xml_node<> *parentNode, GraphicsInfo& progInfo );

bool IsObjectType( const string& type );

// main ///////////////////////////////////

#if defined( _WIN64 ) || ( _WIN32 )
    #undef main
#endif

int main(int argc, char **argv)
{
    // Start an engine and run it then cleanup after
	Engine *engine = NULL;
    GraphicsInfo progInfo;

	progInfo.windowName = "Tutorial";
	progInfo.windowSize = glm::vec2( 1200, 760 );
    
    //handle cmd line parameters
    if( !ProcessCommandLineParameters( argc, argv, progInfo ) )
    {
        cout << endl;
        cout <<"Insufficient or incorrect command line parameters to run."<< endl;
        cout <<"Terminating the program."<<endl<<endl;

        cout <<"Run ./Pinball --h for help with using the program."<<endl<<endl;
        return -1;
    }

	engine = new Engine( progInfo.windowName, progInfo.windowSize.x, progInfo.windowSize.y );

	if( engine == NULL )
	{
		printf( "Failed to allocate memory for the engine.\n" );
		return -1;
	}

    //initialize engine and run it
    
    if(!engine->Initialize(progInfo))
    {
        printf("The engine failed to start.\n");
        delete engine;
        engine = NULL;
        return -1;
    }
    
    engine->Run();
    delete engine;
    engine = NULL;
    return 0;
}

// free function implementation /////////////
// PROCESS COMMAND LINE PARAMETERS //////////
/***************************************

@brief ProcessCommandLineParameters

@details goes through the command line parameters and processes them

@param in: argCount: the number of arguments

@param in: argVector: the argument vector

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ProcessCommandLineParameters
( 
    int argCount, // the number of arguments
    char **argVector, // the argument vector
    GraphicsInfo& progInfo // a struct containing the info
)
{
    string tmpStr;
    int index;

    if( argCount == 1 )
    {
        return false;
    }
    
    for( index = 1; index < argCount; index++ )
    {
        tmpStr = argVector[ index ];
        if( tmpStr == CONFIG_OPT )
        {
            index++;
            tmpStr = argVector[ index ];
            
            if( !ReadConfigurationFile( tmpStr, progInfo ) )
            {
                cout << "Failure processing the configuration file!" << std::endl;
                return false;
            }
        }
        else if( tmpStr == HELP_OPT )
        {
            cout << endl << "Help selected." << endl << endl;

            cout << "This program is ran as follows: " << endl;
            cout << "./Pinball -c config/Bullet.xml" << endl << endl;

            cout<< "If only one type of shader is specified or the model isn't";
            cout << " specified the program will terminate." << endl << endl;
            
            cout << "Options include: " <<endl;
            cout << CONFIG_OPT << " [CONFIG_FILE_PATH]" << endl <<endl;

            cout <<"Example usage: " << endl;
            cout <<"./Pinball -c config/Bullet.xml" << endl << endl;

            cout<<"Please see README.md in the project directory for any additional";
            cout<<" information."<<endl<<endl;

            return false;
        }
        else 
        {
            cout <<"Invalid command line parameter: "<<tmpStr<<endl;
            return false;
        }
    }

    return true;

}

// READ CONFIGURATION FILE //////////
/***************************************

@brief ReadConfigurationFile

@details Reads the configuration file using rapidxml

@param in: fileName: the name of the file to load

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ReadConfigurationFile( const std::string & fileName, GraphicsInfo & progInfo )
{
    ifstream fileOpen( fileName.c_str( ) );
    vector<char> buffer;
    
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *rootNode;  
    
    if( fileOpen.fail( ) )
    {
        std::cout << "Failure reading in the file: " << fileName << "!!!" << std::endl;

        fileOpen.close( );

        return false;
    }

    buffer = vector<char>( istreambuf_iterator<char>( fileOpen ),
                           istreambuf_iterator<char>( ) );

    fileOpen.close( );

    buffer.push_back( '\0' );

    doc.parse<0>( &buffer[ 0 ] );

    rootNode = doc.first_node( "Configuration" );
    
    return ProcessConfigurationFile( rootNode, progInfo );
}

// PROCESS CONFIGURATION FILE //////////
/***************************************

@brief ProcessConfigurationFile

@details Processes the configuration file using rapidxml

@param in: rootNode: the rootNode in the xml doc

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ProcessConfigurationFile
( 
    rapidxml::xml_node<> *rootNode, 
    GraphicsInfo & progInfo
)
{
    rapidxml::xml_node<> *parentNode, *childNode;
    bool noError = true;
    string tempStr;
	std::stringstream strStream;

    int sIndex;

    for( parentNode = rootNode->first_node( 0 ); 
         parentNode; parentNode = parentNode->next_sibling( ) )
    {        
        if( IsObjectType( parentNode->name( ) ) )
        {
            noError = ( noError && ProcessConfigurationFileHelper( parentNode, progInfo ) );	  
        }
        else if( parentNode->name( ) == SHADER )
        {
            noError = ( noError && ProcessConfigShader( parentNode, progInfo ) );
        }
        else if( parentNode->name( ) == LIGHT )
        {
            noError = ( noError && ProcessConfigLight( parentNode, progInfo ) );
        }
        else if( parentNode->name( ) == SPOT_LIGHT )
        {           

            progInfo.spotLight.push_back( SpotLight( ) );
            
            sIndex = progInfo.spotLight.size( ) - 1;

            for( childNode = parentNode->first_node( 0 );
                 childNode; childNode = childNode->next_sibling( ) )
            {
                tempStr = childNode->value( );

                std::stringstream strStream( tempStr );

                if( childNode->name( ) == CONE_ANGLE )
                {
                    strStream >> progInfo.spotLight[ sIndex ].coneAngle;
                }
                else if( childNode->name( ) == SPOT_HEIGHT )
                {
                    strStream >> progInfo.spotLight[ sIndex ].spotHeight;
                }
                else if( childNode->name( ) == FOLLOW_OBJ )
                {
                    strStream >> progInfo.spotLight[ sIndex ].objectToFollow;
                }
                else if( childNode->name( ) == LIGHT )
                {
                    
                    noError = ( noError && ProcessConfigLight( childNode, progInfo, true ) );
                }
            }
        }
		else if( parentNode->name( ) == WINDOW_INFO )
		{
			strStream.clear( );
			strStream.str( std::string( ) );

			if( parentNode->first_attribute( WIDTH.c_str( ) ) )
			{
				tempStr = parentNode->first_attribute( WIDTH.c_str( ) )->value( );

				strStream << tempStr;

				strStream >> progInfo.windowSize.x;
			}

			strStream.clear( );
			strStream.str( std::string( ) );

			if( parentNode->first_attribute( HEIGHT.c_str( ) ) )
			{
				tempStr = parentNode->first_attribute( HEIGHT.c_str( ) )->value( );

				strStream << tempStr;

				strStream >> progInfo.windowSize.y;
			}

			tempStr = parentNode->value( );

			if( !tempStr.empty( ) )
			{
				progInfo.windowName = tempStr;
			}
		}
		else if( parentNode->name( ) == WIND_DIRECTION )
		{
			noError = ( noError && ProcessWind( parentNode, progInfo ) );
		}
        else
        {
            //do nothing
        }
    }
	
    return ( noError && !progInfo.modelVector.empty( ) );
}


// PROCESS CONFIGURATION FILE HELPER //////////
/***************************************

@brief ProcessConfigurationFile

@details Processes the configuration file using rapidxml

@param in: parentNode: the parentNode in the xml doc

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ProcessConfigurationFileHelper
( 
    rapidxml::xml_node<>* parentNode, 
    GraphicsInfo & progInfo
)
{
    rapidxml::xml_node<> *childNode;
    bool noError = true;
    string tempStr;

    unsigned int pIndex = 0, mIndex = 0;

    if( parentNode == NULL )
    {
        return true;
    }

    tempStr = parentNode->first_attribute( "path" )->value( );

    if( tempStr.empty( ) )
    {
        return false;
    }

    progInfo.objectData.push_back( ObjectInfo( ) );
    pIndex = progInfo.objectData.size( ) - 1;

	progInfo.objectData[ pIndex ].type = parentNode->name( );

    for( mIndex = 0; mIndex < progInfo.modelVector.size( ); mIndex++ )
    {
        if( tempStr == progInfo.modelVector[ mIndex ] )
        {
            progInfo.objectData[ pIndex ].modelID = mIndex;
        }
    }
    if( ( int )progInfo.objectData[ pIndex ].modelID == -1 )
    {
        progInfo.modelVector.push_back( tempStr );

        progInfo.objectData[ pIndex ].modelID = progInfo.modelVector.size( ) - 1;
    }    

    for( childNode = parentNode->first_node( 0 ); childNode;
         childNode = childNode->next_sibling( ) )
    {
        tempStr = childNode->value( );

        std::stringstream strStream( tempStr );

        if( childNode->name( ) == NAME )
        {
            strStream >> progInfo.objectData[ pIndex ].name;
        }
        else if( childNode->name( ) == X_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].scale.x;
        }
        else if( childNode->name( ) == Y_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].scale.y;
        }
        else if( childNode->name( ) == Z_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].scale.z;
        }
        else if( childNode->name( ) == BULLET_X_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].bScale.x;
        }
        else if( childNode->name( ) == BULLET_Y_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].bScale.y;
        }
        else if( childNode->name( ) == BULLET_Z_SCALE )
        {
            strStream >> progInfo.objectData[ pIndex ].bScale.z;
        }
        else if( childNode->name( ) == X_POS )
        {
            strStream >> progInfo.objectData[ pIndex ].position.x;
        }
        else if( childNode->name( ) == Y_POS )
        {
            strStream >> progInfo.objectData[ pIndex ].position.y;
        }
        else if( childNode->name( ) == Z_POS )
        {
            strStream >> progInfo.objectData[ pIndex ].position.z;
        }
        else if( childNode->name( ) == X_ROT_AXIS )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAxes.x;
        }
        else if( childNode->name( ) == Y_ROT_AXIS )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAxes.y;
        }
        else if( childNode->name( ) == Z_ROT_AXIS )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAxes.z;
        }
        else if( childNode->name( ) == X_ROT_ANGLE )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAngles.x;
        }
        else if( childNode->name( ) == Y_ROT_ANGLE )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAngles.y;
        }
        else if( childNode->name( ) == Z_ROT_ANGLE )
        {
            strStream >> progInfo.objectData[ pIndex ].rotationAngles.z;
        }
        else if( IsObjectType( childNode->name( ) ) )
        {
            progInfo.objectData[ pIndex ].childID.push_back( progInfo.objectData.size( ) );

            noError = ( noError && ProcessConfigurationFileHelper( childNode, progInfo ) );
        }
    }

    return noError;
}

bool ProcessConfigShader( rapidxml::xml_node<>* parentNode, GraphicsInfo & progInfo )
{
    rapidxml::xml_node<> *childNode;
    bool vertShader = false, fragShader = false;

    string tempStr;

    unsigned int pIndex = 0;

    progInfo.shaderVector.push_back( std::vector<std::pair<GLenum, std::string>>( ) );

    pIndex = progInfo.shaderVector.size( ) - 1;

    for( childNode = parentNode->first_node( 0 ); childNode;
         childNode = childNode->next_sibling( ) )
    {
        if( childNode->name( ) == VERTEX )
        {
            vertShader = true;

            tempStr = childNode->value( );
            progInfo.shaderVector[ pIndex ].push_back( 
				std::pair<GLenum, string>( GL_VERTEX_SHADER, tempStr ) );

        }
        else if( childNode->name( ) == FRAGMENT )
        {
            fragShader = true;

            tempStr = childNode->value( );
            progInfo.shaderVector[ pIndex ].push_back( 
				std::pair<GLenum, string>( GL_FRAGMENT_SHADER, tempStr ) );

        }
    }

    return ( vertShader && fragShader );
}

bool ProcessConfigLight( rapidxml::xml_node<>* parentNode, GraphicsInfo & progInfo, bool spotLight )
{
    rapidxml::xml_node<> *childNode;
    bool lightX = false, lightY = false, lightZ = false, noError = false;

    string tempStr;

    unsigned int pIndex = 0;

    if( !spotLight )
    {
        progInfo.lights.push_back( Light( ) );
        pIndex = progInfo.lights.size( ) - 1;
    }
    else
    {
        pIndex = progInfo.spotLight.size( ) - 1;
    }   

    

    for( childNode = parentNode->first_node( 0 ); childNode;
         childNode = childNode->next_sibling( ) )
    {
        tempStr = childNode->value( );

        std::stringstream strStream( tempStr );

        if( childNode->name( ) == X_POS )
        {
            lightX = true;
            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].incoming.x;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].incoming.x;
            }
            
        }
        else if( childNode->name( ) == Y_POS )
        {
            lightY = true;
            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].incoming.y;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].incoming.y;
            }
            
        }
        else if( childNode->name( ) == Z_POS )
        {
            lightZ = true;

            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].incoming.z;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].incoming.z;
            }

            
        }
        else if( childNode->name( ) == AMBIENT )
        {
            noError = ( ProcessConfigAmbient( childNode, progInfo, spotLight ) );
        }
    }

    return lightX && lightY && lightZ && noError;
}


bool ProcessConfigAmbient( rapidxml::xml_node<>* parentNode, GraphicsInfo & progInfo, bool spotLight )
{
    rapidxml::xml_node<> *childNode;
    bool lightX = false, lightY = false, lightZ = false;

    string tempStr;

    unsigned int pIndex = 0;

    if( !spotLight )
    {
        pIndex = progInfo.lights.size( ) - 1;
    }
    else
    {
        pIndex = progInfo.spotLight.size( ) - 1;
    }
    

    for( childNode = parentNode->first_node( 0 ); childNode;
         childNode = childNode->next_sibling( ) )
    {
        tempStr = childNode->value( );

        std::stringstream strStream( tempStr );

        if( childNode->name( ) == X_POS )
        {

            lightX = true;
            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].ambient.x;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].ambient.x;
            }
            
        }
        else if( childNode->name( ) == Y_POS )
        {
            lightY = true;
            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].ambient.y;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].ambient.y;
            }
            
        }
        else if( childNode->name( ) == Z_POS )
        {
            lightZ = true;
            if( spotLight )
            {
                strStream >> progInfo.spotLight[ pIndex ].ambient.z;
            }
            else
            {
                strStream >> progInfo.lights[ pIndex ].ambient.z;
            }
            
        }
    }

    return lightX && lightY && lightZ;
}

bool ProcessWind( rapidxml::xml_node<>* parentNode, GraphicsInfo & progInfo )
{
	rapidxml::xml_node<> *childNode;

	string tempStr;

	for( childNode = parentNode->first_node( 0 ); childNode;
		 childNode = childNode->next_sibling( ) )
	{
		tempStr = childNode->value( );

		std::stringstream strStream( tempStr );

		if( childNode->name( ) == X_POS )
		{
			strStream >> progInfo.windDirection.x;

		}
		else if( childNode->name( ) == Y_POS )
		{
			strStream >> progInfo.windDirection.y;

		}
		else if( childNode->name( ) == Z_POS )
		{
			strStream >> progInfo.windDirection.z;

		}
		else if( childNode->name( ) == X_SCALE )
		{
			strStream >> progInfo.windForce;

		}
	}
	return true;
}

bool IsObjectType( const string & type )
{
	if( ( type == OBJECT_TYPE ) 
		|| ( type == P_OBJECT_TYPE ) 
		|| ( type == P_C_OBJECT_TYPE ) )
	{
		return true;
	}

	return false;
}
