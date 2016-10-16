// header files ///////////////////////////////
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "rapidxml/rapidxml.hpp"
#include "engine.h"

//Helper Data Structures /////////////////////
struct NormalizationData
{
    float diameter;
    glm::vec2 scale;
    float rotDivider;
    float orbitDivider;
    float sunMultiplier;

    NormalizationData
    ( 
        float diam, //diameter
        glm::vec2 scaleFact, //scale
        float rot, //rotation divider
        float orbit, //orbit divider
        float sunMult //sun multiplier
    ):
        diameter( diam ),
        scale( scaleFact ),
        rotDivider( rot ),
        orbitDivider( orbit ),
        sunMultiplier( sunMult )
    {
        //done in intializers
    }

    NormalizationData
    (
        const NormalizationData& normData
    ) :
        diameter( normData.diameter ),
        scale( normData.scale ),
        rotDivider( normData.rotDivider ),
        orbitDivider( normData.orbitDivider ),
        sunMultiplier( normData.sunMultiplier )
    {
        //done in intializers
    }
};

//Global Constants //////////////////////////
//frag options
const string FRAGMENT_SHADER_OPT = "-f";
const string VERTEX_SHADER_OPT = "-v";

//model options
const string MODEL_PATH_OPT = "-m";

//config options
const string CONFIG_OPT = "-c";

//help options
const string HELP_OPT = "--h";

//general characters
const char TAB_CHAR = '\t';

//xml types
const string NORM_FACT = "NormalizationFactor";
const string OBJECT = "Object";
const string SHADER = "Shader";
const string DIAMETER = "Diameter";
const string X_SCALE = "xScale";
const string Y_SCALE = "yScale";
const string Z_SCALE = "zScale";
const string ROTATION_DIVIDER = "RotationDiv";
const string ORBIT_DIVIDER = "OrbitDiv";
const string SUN = "Sun";
const string PLANET = "Planet";
const string MOON = "Moon";
const string X_ORBIT_RADIUS = "xOrbitRadius";
const string Y_ORBIT_RADIUS = "yOrbitRadius";
const string ORBIT_RATE = "OrbitRate";
const string ORBIT_TILT = "OrbitTilt";
const string ROTATION_RATE = "RotationRate";
const string TILT = "Tilt";
const string VERTEX = "Vertex";
const string FRAGMENT = "Fragment";

// free function prototypes ////////////////
bool ProcessCommandLineParameters( int argCount, char **argVector, 
                                   GraphicsInfo& progInfo );

bool ReadConfigurationFile( const std::string& fileName, GraphicsInfo& progInfo );

bool ProcessConfigurationFile( rapidxml::xml_node<> *rootNode, 
                               GraphicsInfo& progInfo );

bool ProcessConfigurationFileHelper( rapidxml::xml_node<> *parentNode,
                                     GraphicsInfo& progInfo,
                                     const NormalizationData& normData );
// main ///////////////////////////////////

#if defined( _WIN64 ) || ( _WIN32 )
    #undef main
#endif

int main(int argc, char **argv)
{
    // Start an engine and run it then cleanup after
    Engine *engine = new Engine("Tutorial Window Name", 800, 600);
    GraphicsInfo progInfo;

    //handle cmd line parameters
    if( !ProcessCommandLineParameters( argc, argv, progInfo ) )
    {
        cout << endl;
        cout <<"Insufficient or incorrect command line parameters to run."<< endl;
        cout <<"Terminating the program."<<endl<<endl;
        cout <<"Please ensure that both the vertex shader and the fragment shader";
        cout <<" are specified."<< endl;
        cout <<"Example usage: " << endl;
        cout <<" ./Tutorial -v shaders/textureVertexShader.glsl ";
        cout <<"-f shaders/textureFragmentShader.glsl ";
        cout <<"-m models/BoxT.obj" << endl << endl;

        cout <<"Run ./Tutorial --h for help with using the program."<<endl<<endl;

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
    GLenum shaderType;
    string tmpStr;
    int index;

    bool successFlag = true;

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
            
            // debug only //////////////////////////////////////////////////////
            unsigned int tIndex, t2Index;
            for( tIndex = 0; tIndex < progInfo.modelVector.size( ); tIndex++ )
            {
                cout << "Model "<< tIndex << ": " <<progInfo.modelVector[ tIndex ] << endl;
            }
            for( tIndex = 0; tIndex < progInfo.planetData.size( ); tIndex++ )
            {                
                for( t2Index = 0; t2Index < progInfo.planetData[ tIndex ].childID.size( ); t2Index++ )
                {
                    cout << "Parent: " << tIndex << ", Child: " << progInfo.planetData[ tIndex ].childID[ t2Index ] << " model:" << progInfo.modelVector[ progInfo.planetData[ progInfo.planetData[ tIndex ].childID[ t2Index ] ].modelID ] << endl;
                }
            }
            ///////////////////////////////////////////////////////////////////
        }
        else if( ( tmpStr == VERTEX_SHADER_OPT ) || ( tmpStr == FRAGMENT_SHADER_OPT ) )
        {
            if( tmpStr == VERTEX_SHADER_OPT )
            {
                shaderType = GL_VERTEX_SHADER;
            }
            else
            {
                shaderType = GL_FRAGMENT_SHADER;
            }

            index++;
            tmpStr = argVector[ index ];

            if( ( tmpStr == VERTEX_SHADER_OPT ) 
                    || ( tmpStr == FRAGMENT_SHADER_OPT ) 
                    || ( tmpStr == HELP_OPT ) )
            {
                cout << "No shader file found after: " << argVector[ index - 1 ] << endl;
                cout << "Terminating the program." <<endl;
                return false;
            }

            progInfo.shaderVector.push_back( pair<GLenum, string>(shaderType, tmpStr ) );
        }
        else if( tmpStr == MODEL_PATH_OPT )
        {
            index++;
            tmpStr = argVector[ index ];

            if( ( tmpStr == VERTEX_SHADER_OPT ) 
                    || ( tmpStr == FRAGMENT_SHADER_OPT ) 
                    || ( tmpStr == HELP_OPT )
                    || ( tmpStr == MODEL_PATH_OPT ) )
            {
                cout << "No model file found after: " << argVector[ index - 1 ] << endl;
                cout << "Terminating the program." <<endl;
                return false;
            }

            progInfo.modelVector.push_back( tmpStr );
            
        }
        else if( tmpStr == HELP_OPT )
        {
            cout << endl << "Help selected." << endl << endl;

            cout << "This program is ran as follows: " << endl;
            cout << " ./Tutorial -[SHADER_OPTION] [SHADER_FILE_PATH]";
            cout << " -[SHADER_OPTION] [SHADER_FILE_PATH]";
            cout << " -[MODEL_OPTION] [MODEL_FILE_PATH]" << endl << endl;

            cout<< "If only one type of shader is specified or the model isn't";
            cout << " specified the program will terminate." << endl << endl;
            
            cout << "Options include: " <<endl;
            cout << TAB_CHAR << VERTEX_SHADER_OPT << " [SHADER_FILE_PATH] : ";
            cout << "specifing the vertex shader file path." << endl;
            cout << TAB_CHAR << FRAGMENT_SHADER_OPT << " [SHADER_FILE_PATH] : ";
            cout << "specifing the fragment shader file path." << endl << endl;
            cout << TAB_CHAR << MODEL_PATH_OPT << " [MODEL_FILE_PATH] : ";
            cout << "specifing the model file path." << endl << endl;
            
            cout << "Shader Information: " << endl;
            cout << "Typically shader files will be found in" << endl;
            cout << "[CURRENT_DIR]/shaders" <<    endl;
            cout << "When ran this must be included in the shader file path." << endl;
            cout << "The path should look like: shaders/[SHADER_FILE]" << endl << endl;

            cout << "Model Information: " << endl;
            cout << "Typically model files will be found in" << endl;
            cout << "[CURRENT_DIR]/models" <<    endl;
            cout << "When ran this must be included in the model file path." << endl;
            cout << "The path should look like: models/[MODEL_FILE]" << endl << endl;

            cout <<"Example usage: " << endl;
            cout <<" ./Tutorial -v shaders/textureVertexShader.glsl ";
            cout <<"-f shaders/textureFragmentShader.glsl ";
            cout <<"-m models/BoxT.obj" << endl << endl;

            cout<<"Please see README.md in the project directory for any additional";
            cout<<" information."<<endl<<endl;
        }
        else 
        {
            cout <<"Invalid command line parameter: "<<tmpStr<<endl;
            return false;
        }
    }

    return ( ( progInfo.shaderVector.size( ) > 1 ) 
             && ( progInfo.modelVector.size( ) > 0 ) && successFlag );

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

    rootNode = doc.first_node( "SolarSystem" );

    return ProcessConfigurationFile( rootNode, progInfo );
}

// PROCESS CONFIGURATION FILE //////////
/***************************************

@brief ProcessConfigurationFile

@details Processes the configuration file using rapidxml

@param in: buffer: the buffer to process

@param out: progInfo: a struct containing program information

@param in: normData: a struct of normalization data

@param out: normData: a struct of normalization data

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
    stringstream strStream;

    NormalizationData normData( 1.0f, glm::vec2( 1.0f, 1.0f ), 1.0f, 1.0f, 1.0f );

    for( parentNode = rootNode->first_node( 0 ); 
         parentNode; parentNode = parentNode->next_sibling( ) )
    {
        if( parentNode->name( ) == NORM_FACT )
        {
            tempStr = parentNode->value( );
            strStream = stringstream( tempStr );

            if( parentNode->first_attribute( "name" )->value( ) == DIAMETER )
            {
                strStream >> normData.diameter;
            }
            else if( parentNode->first_attribute( "name" )->value( ) == X_SCALE )
            {
                strStream >> normData.scale.x;
            }
            else if( parentNode->first_attribute( "name" )->value( ) == Y_SCALE )
            {
                strStream >> normData.scale.y;
            }
            else if( parentNode->first_attribute( "name" )->value( ) == ROTATION_DIVIDER )
            {
                strStream >> normData.rotDivider;
            }
            else if( parentNode->first_attribute( "name" )->value( ) == ORBIT_DIVIDER )
            {
                strStream >> normData.orbitDivider;
            }
            else if( parentNode->first_attribute( "name" )->value( ) == SUN )
            {
                strStream >> normData.sunMultiplier;
            }
        }
        else if( parentNode->name( ) == OBJECT ) 
        {
            noError = ( noError && ProcessConfigurationFileHelper( parentNode, 
                                                                   progInfo, 
                                                                   normData ) );
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

bool ProcessConfigurationFileHelper
( 
    rapidxml::xml_node<>* parentNode, 
    GraphicsInfo & progInfo, 
    const NormalizationData & normData 
)
{
    rapidxml::xml_node<> *childNode;
    bool noError = true;
    string tempStr;
    stringstream strStream;

    unsigned int pIndex = 0, mIndex = 0;

    if( parentNode == NULL )
    {
        return true;
    }

    if( !( parentNode->first_attribute( "name" )->value( ) == PLANET )
        && !( parentNode->first_attribute( "name" )->value( ) == SUN ) 
        && !( parentNode->first_attribute( "name" )->value( ) == MOON ) )
    {
        return true;
    }

    tempStr = parentNode->first_attribute( "path" )->value( );

    if( tempStr.empty( ) )
    {
        return false;
    }

    progInfo.planetData.push_back( PlanetInfo( ) );
    pIndex = progInfo.planetData.size( ) - 1;

    for( mIndex = 0; mIndex < progInfo.modelVector.size( ); mIndex++ )
    {
        if( tempStr == progInfo.modelVector[ mIndex ] )
        {
            progInfo.planetData[ pIndex ].modelID = mIndex;
        }
    }
    if( ( int )progInfo.planetData[ pIndex ].modelID == -1 )
    {
        progInfo.modelVector.push_back( tempStr );

        progInfo.planetData[ pIndex ].modelID = progInfo.modelVector.size( ) - 1;
    }
    

    for( childNode = parentNode->first_node( 0 ); childNode;
         childNode = childNode->next_sibling( ) )
    {
        tempStr = childNode->value( );

        strStream = stringstream( tempStr );

        if( childNode->name( ) == X_SCALE )
        {
            strStream >> progInfo.planetData[ pIndex ].scale.x;
        }
        else if( childNode->name( ) == Y_SCALE )
        {
            strStream >> progInfo.planetData[ pIndex ].scale.y;
        }
        else if( childNode->name( ) == Z_SCALE )
        {
            strStream >> progInfo.planetData[ pIndex ].scale.z;
        }
        else if( childNode->name( ) == X_ORBIT_RADIUS )
        {
            strStream >> progInfo.planetData[ pIndex ].orbitRad.x;
        }
        else if( childNode->name( ) == Y_ORBIT_RADIUS )
        {
            strStream >> progInfo.planetData[ pIndex ].orbitRad.y;
        }
        else if( childNode->name( ) == ORBIT_RATE )
        {
            strStream >> progInfo.planetData[ pIndex ].orbitRate;
        }
        else if( childNode->name( ) == ORBIT_TILT )
        {
            strStream >> progInfo.planetData[ pIndex ].orbitTilt;
        }
        else if( childNode->name( ) == ROTATION_RATE )
        {
            strStream >> progInfo.planetData[ pIndex ].rotRate;
        }
        else if( childNode->name( ) == TILT )
        {
            strStream >> progInfo.planetData[ pIndex ].tilt;
        }
        else if( childNode->name( ) == OBJECT )
        {
            progInfo.planetData[ pIndex ].childID.push_back( progInfo.planetData.size( ) );

            noError = ( noError && ProcessConfigurationFileHelper( childNode,
                                                                   progInfo,
                                                                   normData ) );
        }
    }

    return noError;
}



