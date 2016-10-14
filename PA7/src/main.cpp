// header files ///////////////////////////////
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "rapidxml/rapidxml.hpp"
#include "engine.h"


//Global Constants //////////////////////////
//frag options
const string FRAGMENT_SHADER_OPT = "-f";
const string VERTEX_SHADER_OPT = "-v";

//object options
const string PLANET_OPT = "-p";

const string CHILD_OPT = "-c";

const string MODEL_PATH_OPT = "-m";

const string SCALE_OPT = "-s";

const string ROT_RATE_OPT = "-r";

const string ORBIT_RATE_OPT = "-o";

const string LOC_LOCAL_OPT = "-l";

const string TILT_OPT = "-t";

const size_t CONFIG_OPT_SIZE = 3;


const string PRIMARY_OPTIONS[ ] = { PLANET_OPT,
                                    VERTEX_SHADER_OPT,
                                    FRAGMENT_SHADER_OPT };
const unsigned int P_OPT_SIZE = 3;

const unsigned int PLANET_INDEX = 0;

const unsigned int VERTEX_SHADER_INDEX = 1;

const unsigned int FRAGMENT_SHADER_INDEX = 2;


//help options
const string HELP_OPT = "--h";

//general characters
const char TAB_CHAR = '\t';

// free function prototypes ////////////////
bool ProcessCommandLineParameters( int argCount, char **argVector, 
                                   GraphicsInfo& progInfo );

bool ProcessLine( const std::string& line, GraphicsInfo& progInfo );

bool ProcessConfigurationFile( const std::string& fileName, 
                               GraphicsInfo& progInfo );

bool ReadConfigurationFile( const std::string& fileName, GraphicsInfo& progInfo );

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

        if( ( tmpStr == VERTEX_SHADER_OPT ) || ( tmpStr == FRAGMENT_SHADER_OPT ) )
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

// PROCESS LINE //////////
/***************************************

@brief ProcessLine

@details processes a single string of configuration information

@param in: line: the line to process

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ProcessLine( const std::string & line, GraphicsInfo & progInfo )
{
   
    bool successFlag = true;
    GLenum shaderType;

    unsigned int optIndex;

    size_t strIndex;

    for( optIndex = 0; optIndex < P_OPT_SIZE; optIndex++ )
    {
        strIndex = line.find( PRIMARY_OPTIONS[ optIndex ] );

        if( strIndex != string::npos )
        {
            if( optIndex == PLANET_INDEX )
            {

            }
            else if( optIndex == VERTEX_SHADER_INDEX )
            {

            }
            else if( optIndex == FRAGMENT_SHADER_INDEX )
            {
                
            }
        }
    }

    return successFlag;
}

// PROCESS CONFIGURATION FILE //////////
/***************************************

@brief ProcessConfigurationFile

@details processes the configuration file specified in the command prompt parameters

@param in: fileName: the name of the file to load

@param out: progInfo: a struct containing program information

@notes none

***************************************/
bool ProcessConfigurationFile
( 
    const std::string & fileName, //the configuration file
    GraphicsInfo & progInfo //the program information to pass to the engine
)
{
    bool successFlag = true;

    //to do: implement

    return successFlag;
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

    if( fileOpen.fail( ) )
    {
        std::cout << "Failure reading in the file: " << fileName << "!!!" << std::endl;

        fileOpen.close( );

        return false;
    }

    buffer = vector<char>( istreambuf_iterator<char>( fileOpen ),
                           istreambuf_iterator<char>( ) );

    buffer.push_back( '\0' );


    fileOpen.close( );

    return true;
}


