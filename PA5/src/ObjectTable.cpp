/***************************************

@file ObjectTable.cpp

@brief A class to hold all of the program's objects

@note none

***************************************/

// headers ////////////////////////////////

#include"ObjectTable.h"

//constructors


// CONSTRUCTOR //////////////////
/***************************************

@brief ObjectTable

@details creates an empty table

@param None

@notes None

***************************************/
ObjectTable::ObjectTable( ): data( )
{
    //nothing to do
}

// COPY CONSTRUCTOR //////////////////
/***************************************

@brief ObjectTable

@details creates a table from a pre-existing one

@param in: src: the src table

@notes None

***************************************/
ObjectTable::ObjectTable( const ObjectTable& src ): data( src.data )
{
    //nothing to do
}

//destructor
// DESTRUCTOR //////////////////
/***************************************

@brief ~ObjectTable

@details deallocates the table

@param None

@notes None

***************************************/
ObjectTable::~ObjectTable( )
{
    data.clear( );
}

//set methods
// ADD OBJECT //////////////////
/***************************************

@brief addObject

@details adds an object and assigns a parent id ( optional )

@param in: parent: the parent id ( optional )

@notes parentId must be valid, returns the object's id

***************************************/
unsigned int ObjectTable::addObject(    unsigned int parent )
{
    if( (    ( int ) parent < -1 ) &&    ( parent < data.size( ) ) )
    {
        return -1;
    }

    data.push_back( Object( ) );

    if( ( int ) parent == -1 )
    {
        //do nothing, not a child
    }
    else 
    {
        data[ data.size( ) - 1 ].addParent( parent );

        data[ parent ].addChild( data.size( ) - 1 );
    }

    return ( data.size( ) - 1 );
}

// SET CHILD //////////////////
/***************************************

@brief setChild

@details set an obj as a child

@param in: objID: the    obj being made into a child

@param in: parentToBeID: the parent id

@notes parent id and child id must be valid

***************************************/
bool ObjectTable::setChild( unsigned int objID, unsigned int parentToBeID )
{
    if( (    ( int ) parentToBeID >= 0 ) &&    ( parentToBeID < data.size( ) ) )
    {
        return false;
    }

    if( (    ( int ) objID >= 0 ) &&    ( objID < data.size( ) ) )
    {
        return false;
    }

    data[ parentToBeID ].addChild( objID );

    data[ objID ].addParent( parentToBeID );

    return true;
}

//get methods
// GET SIZE //////////////////
/***************************************

@brief getSize

@details get the number of objects in the table

@param None

@notes None

***************************************/
unsigned int ObjectTable::getSize( )
{
    return ( unsigned int ) data.size( );
}

// [ ] //////////////////
/***************************************

@brief [ ]

@details allows access to the object at this index ( id )

@param in: index: the object we are accessing

@notes None

***************************************/
Object& ObjectTable::operator [ ] ( unsigned int index )
{
    return data[ index ];
}

// [ ] //////////////////
/***************************************

@brief [ ]

@details allows access to the object at this index ( id )

@param in: index: the object we are accessing

@notes None

***************************************/
const Object& ObjectTable::operator [ ] ( unsigned int index ) const 
{
    return data[ index ];
}
