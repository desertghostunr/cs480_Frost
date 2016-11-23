#ifndef P_OBJECT_H
#define P_OBJECT_H

#include "object.h"
#include "graphics_headers.h"

class PObject : Object
{
	public:
		const int P_OBJECT = 1;

		PObject( );
		~PObject( );

		int getObjectType( );

	private:
};


#endif // !P_OBJECT_H

