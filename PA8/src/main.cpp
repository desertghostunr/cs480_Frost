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
const string OBJECT = "Object";
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

// free function prototypes ////////////////
bool ProcessCommandLineParameters( int argCount, char **argVector, 
                                   GraphicsInfo& progInfo );

bool ReadConfigurationFile( const std::string& fileName, GraphicsInfo& progInfo );

bool ProcessConfigurationFile( rapidxml::xml_node<> *rootNode, 
                               GraphicsInfo& progInfo );

bool ProcessConfigurationFileHelper( rapidxml::xml_node<> *parentNode,
                                     GraphicsInfo& progInfo );
// main ///////////////////////////////////

#if defined( _WIN64 ) || ( _WIN32 )
    #undef main
#endif

int main(int argc, char **argv)
{
    // Start an engine and run it then cleanup after
    Engine *engine = new Engine("Tutorial Window Name", 1200, 760);
    GraphicsInfo progInfo;
    
    //handle cmd line parameters
    if( !ProcessCommandLineParameters( argc, argv, progInfo ) )
    {
        cout << endl;
        cout <<"Insufficient or incorrect command line parameters to run."<< endl;
        cout <<"Terminating the program."<<endl<<endl;

        cout <<"Run ./SolarSystem --h for help with using the program."<<endl<<endl;

        delete engine;
        engine = NULL;
        return 1;
    }

    //initialize engine and run it
    
    if(!engine->Initialize(progInfo))
    {
        printf("The engine failed to start.\n");
        delete engine;
        engine = NULL;
        return 1;
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
            cout << "./SolarSystem -c config/SolarSystemConfig.xml" << endl << endl;

            cout<< "If only one type of shader is specified or the model isn't";
            cout << " specified the program will terminate." << endl << endl;
            
            cout << "Options include: " <<endl;
            cout << CONFIG_OPT << " [CONFIG_FILE_PATH]" << endl <<endl;

            cout <<"Example usage: " << endl;
            cout <<"./SolarSystem -c config/SolarSystemConfig.xml" << endl << endl;

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
    rapidxml::xml_node<> *parentNode;
    bool noError = true, vertShader = false, fragShader = false;
    string tempStr;

    for( parentNode = rootNode->first_node( 0 ); 
         parentNode; parentNode = parentNode->next_sibling( ) )
    {
        
        if( parentNode->name( ) == OBJECT ) 
        {
            noError = ( noError && ProcessConfigurationFileHelper( parentNode, 
                                                                   progInfo ) );
	  
        }
        else if( parentNode->name( ) == SHADER )
        {
            if( parentNode->first_attribute( "name" )->value( ) == VERTEX )
            {
                vertShader = true;
                
                tempStr = parentNode->value( );
                progInfo.shaderVector.push_back( 
                      std::pair<GLenum, string>( GL_VERTEX_SHADER, tempStr ) );
		
            }
            else if( parentNode->first_attribute( "name" )->value( ) == FRAGMENT )
            {
                fragShader = true;

                tempStr = parentNode->value( );
                progInfo.shaderVector.push_back( 
                      std::pair<GLenum, string>( GL_FRAGMENT_SHADER, tempStr ) );
		
            }
         
        }
        else
        {
            //do nothing
        }
    }


	
    return ( noError && vertShader && fragShader && !progInfo.modelVector.empty( ) );
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
        else if( childNode->name( ) == OBJECT )
        {
            progInfo.objectData[ pIndex ].childID.push_back( progInfo.objectData.size( ) );

            noError = ( noError && ProcessConfigurationFileHelper( childNode, progInfo ) );
        }
    }

    return noError;
}



