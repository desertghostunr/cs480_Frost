#include "..\include\PObject.h"

PObject::PObject( ): Object( )
{
	//nothing to do
}

PObject::~PObject( )
{
	//nothing to do
}

int PObject::getObjectType( )
{
	return P_OBJECT;
}
