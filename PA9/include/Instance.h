/***************************************

@file Instance.h

@brief A struct to hold an instance of an object model

@note none

***************************************/

//header files /////////////////////////
#include"ObjectModel.h"

struct Instance
{
    std::string modelPath;
    ObjectModel objModel;

    Instance( ){ }

    Instance( const std::string &path, const ObjectModel& model ):
        modelPath( path ), 
        objModel( model )
    {

    }

    Instance( const Instance& src ): 
        modelPath( src.modelPath ),
        objModel( src.objModel )
    {

    }
};
