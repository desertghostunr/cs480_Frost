// header files ///////////////////////////////
#include <iostream>
#include <vector>
#include <string>
#include "engine.h"


//Global Constants //////////////////////////
const string FRAGMENT_SHADER_OPT = "-f";
const string VERTEX_SHADER_OPT = "-v";
const string HELP_OPT = "--h";

const char TAB_CHAR = '\t';

// free function prototypes ////////////////
bool ProcessCommandLineParameters( int argCount, char **argVector, 
                                   vector<pair<GLenum, string>>& shaderVector );

// main ///////////////////////////////////

int main(int argc, char **argv)
{
  // Start an engine and run it then cleanup after
  Engine *engine = new Engine("Tutorial Window Name", 800, 600);
  vector<pair<GLenum, string>> shaderVector;

  //handle cmd line parameters
  if( !ProcessCommandLineParameters( argc, argv, shaderVector ) )
  {
    cout << endl;
    cout <<"Insufficient or incorrect command line parameters to run."<< endl;
    cout <<"Terminating the program."<<endl<<endl;
    cout <<"Please ensure that both the vertex shader and the fragment shader";
    cout <<" are specified."<< endl;
    cout <<"Example usage: " << endl;
    cout <<" ./Tutorial -v shaders/cubeVertexShader.glsl ";
    cout <<" -f shaders/cubeFragmentShader.glsl" << endl << endl;

    delete engine;
    engine = NULL;
    return 1;
  }

  //initialize engine and run it
  if(!engine->Initialize())
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

@details  goes through the command line parameters and processes them

@param in: argCount: the number of arguments

@param in: argVector: the argument vector

@param out: shaderVector: a vector containing shaders and their type

@notes none

***************************************/
bool ProcessCommandLineParameters
( 
  int argCount, // the number of arguments
  char **argVector, // the argument vector
  vector<pair<GLenum, string>>& shaderVector // a vector containing shader files
)
{
  GLenum shaderType;
  string tmpStr;
  int index;
  bool vertShaderGiven = false, fragShaderGiven = false;

  for( index = 1; index < argCount; index++ )
  {
    tmpStr = argVector[ index ];

    if( ( tmpStr == VERTEX_SHADER_OPT ) || ( tmpStr == FRAGMENT_SHADER_OPT ) )
    {
      if( tmpStr == VERTEX_SHADER_OPT )
      {
        shaderType = GL_VERTEX_SHADER;
        vertShaderGiven = true;
      }
      else
      {
        shaderType = GL_FRAGMENT_SHADER;
        fragShaderGiven = true;
      }

      index++;
      tmpStr = argVector[ index ];

      if( ( tmpStr == VERTEX_SHADER_OPT ) 
          || ( tmpStr == FRAGMENT_SHADER_OPT ) 
          || ( tmpStr == HELP_OPT ) )
      {
        cout << "No shader file found after: " << argVector[ index -1 ] << endl;
        cout << "Terminating the program." <<endl;
        return false;
      }

      shaderVector.push_back( pair<GLenum, string>(shaderType, tmpStr ) );
    }
    else if( tmpStr == HELP_OPT )
    {
      cout << endl << "Help selected." << endl << endl;

      cout << "This program is ran as follows: " << endl;
      cout << " ./Tutorial -[SHADER_OPTION] [SHADER_FILE_PATH]";
      cout << " -[SHADER_OPTION] [SHADER_FILE_PATH]" << endl << endl;

      cout<< "If only one type of shader is";
      cout << " specified the program will terminate." << endl << endl;
      
      cout << "Options include: " <<endl;
      cout << TAB_CHAR << VERTEX_SHADER_OPT << " [SHADER_FILE_PATH] : ";
      cout << "specifing the vertex shader file path." << endl;
      cout << TAB_CHAR << FRAGMENT_SHADER_OPT << " [SHADER_FILE_PATH] : ";
      cout << "specifing the fragment shader file path." << endl << endl;
      
      cout << "Shader Information: " << endl;
      cout << "Typically shader files will be found in" << endl;
      cout << "[CURRENT_DIR]/shaders" <<  endl;
      cout << "When ran this must be included in the shader file path." << endl;
      cout << "The path should look like: shaders/[SHADER_FILE]" << endl << endl;

      cout <<"Example usage: " << endl;
      cout <<" ./Tutorial -v shaders/cubeVertexShader.glsl ";
      cout <<" -f shaders/cubeFragmentShader.glsl" << endl << endl;
    }
    else 
    {
      cout <<"Invalid command line parameter: "<<tmpStr<<endl;
      return false;
    }
  }

  return ( vertShaderGiven && fragShaderGiven );

}