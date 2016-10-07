/***************************************

@file ObjectTable.h 

@brief A class to hold all of the program's objects

@note none

***************************************/

// headers ////////////////////////////////

#include <vector>
#include "object.h"


class ObjectTable
{
    public: 

        //constructors
        ObjectTable( );
        ObjectTable( const ObjectTable& src );

        //destructor
        ~ObjectTable( );

        //set methods
        unsigned int addObject( unsigned int parent = -1 );

        bool setChild( unsigned int objID, unsigned int parentToBeID );

        //get methods
        unsigned int getSize( );

        Object& operator [ ] ( unsigned int index );

        const Object& operator [ ] ( unsigned int index ) const;

    private:
        std::vector<Object> data;

};
