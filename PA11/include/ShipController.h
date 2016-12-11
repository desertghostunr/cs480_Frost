#ifndef SHIPCONTROLLER_H
#define SHIPCONTROLLER_H

#include "graphics_headers.h"

struct ShipController
{
    static const float MAX_SPEED; //value defined in graphics.cpp
    static const float MAX_ROT; //value defined in graphics.cpp
    static const float STD_FORCE; //value defined in graphics.cpp
    static const float STD_REVERSE; //value defined in graphics.cpp
    static const float STD_TORQUE; //value defined in graphics.cpp

    static const int MAX_HEALTH = 100;

    static const int RELOAD_TIME_MS = 15000;

    static const float CAMERA_FOLLOW_DISTANCE; //value defined in graphics.cpp

    static const float CAMERA_FOLLOW_HEIGHT; //value defined in graphics.cpp

    size_t index;

    btVector3 force;
    bool forceOn;
    bool slowDown;

    bool shipReversed;
    int shipReverseCounter;

    btVector3 torque;
    bool torqueOn;
    float torqueAcc;
    bool slowRotDown;

    btVector3 rightHit;
    btVector3 leftHit;

    int healthPoints;

    bool firingLeft;
    bool firingRight;

    int leftReloadTime;
    int rightReloadTime;

    glm::vec3 cameraPosition;

    bool lookingLeft;
    bool lookingRight;

    float waveCycle;

    bool waveUp;

    float cameraHeight;

    float cameraDistance;

    unsigned int skyIndex;


    ShipController( ) :
        index( 0 ),
        force( btVector3( 0, 0, 0 ) ),
        forceOn( false ),
        slowDown( false ),
        shipReversed( false ),
        shipReverseCounter( 0 ),
        torque( btVector3( 0, 0, 0 ) ),
        torqueOn( false ),
        torqueAcc( 0 ),
        slowRotDown( false ),
        rightHit( 0, 0, 0 ),
        leftHit( 0, 0, 0 ),
        healthPoints( MAX_HEALTH ),
        firingLeft( false ),
        firingRight( false ),
        leftReloadTime( 0 ),
        rightReloadTime( 0 ),
        cameraPosition( 0, 0, 0 ),
        lookingLeft( false ),
        lookingRight( false),
        waveCycle( 0 ),
        waveUp( true ),
        cameraHeight( 0 ),
        cameraDistance( 0 ),
        skyIndex( -1 )
    {
    }

    ShipController( size_t newIndex ) :
        index( newIndex ),
        force( btVector3( 0, 0, 0 ) ),
        forceOn( false ),
        slowDown( false ),
        shipReversed( false ),
        shipReverseCounter( 0 ),
        torque( btVector3( 0, 0, 0 ) ),
        torqueOn( false ),
        torqueAcc( 0 ),
        slowRotDown( false ),
        rightHit( 0, 0, 0 ),
        leftHit( 0, 0, 0 ),
        healthPoints( MAX_HEALTH ),
        firingLeft( false ),
        firingRight( false ),
        leftReloadTime( 0 ),
        rightReloadTime( 0 ),
        cameraPosition( 0, 0, 0 ),
        lookingLeft( false ),
        lookingRight( false),
        waveCycle( 0 ),
        waveUp( true ),
        cameraHeight( 0 ),
        cameraDistance( 0 ),
        skyIndex( -1 )
        
    {
    }

    ShipController( const ShipController& src ) :
        index( src.index ),
        force( src.force ),
        forceOn( src.forceOn ),
        slowDown( src.slowDown ),
        shipReversed( src.shipReversed ),
        shipReverseCounter( src.shipReverseCounter ),
        torque( src.torque ),
        torqueOn( src.torqueOn ),
        torqueAcc( src.torqueAcc ),
        slowRotDown( src.slowRotDown ),
        rightHit( src.rightHit ),
        leftHit( src.leftHit ),
        healthPoints( src.healthPoints ),
        firingLeft( src.firingLeft ),
        firingRight( src.firingRight ),
        leftReloadTime( src.leftReloadTime ),
        rightReloadTime( src.rightReloadTime ),
        cameraPosition( src.cameraPosition ),
        lookingLeft( src.lookingLeft ),
        lookingRight( src.lookingRight ),
        waveCycle( src.waveCycle ),
        waveUp( src.waveUp ),
        cameraHeight( src.cameraHeight ),
        cameraDistance( src.cameraDistance ),
        skyIndex( src.skyIndex )
    {
    }

    const ShipController& operator=( const ShipController& src )
    {
        if( this != &src )
        {
            index = src.index;
            force = src.force;
            forceOn = src.forceOn;
            slowDown = src.slowDown;
            shipReversed = src.shipReversed;
            shipReverseCounter = src.shipReverseCounter;
            torque = src.torque;
            torqueOn = src.torqueOn;
            torqueAcc = src.torqueAcc;
            slowRotDown = src.slowRotDown;
            rightHit = src.rightHit;
            leftHit = src.leftHit;
            healthPoints = src.healthPoints;
            firingLeft = src.firingLeft;
            firingRight = src.firingRight;
            leftReloadTime = src.leftReloadTime;
            rightReloadTime = src.rightReloadTime;
            cameraPosition = src.cameraPosition;
            lookingLeft = src.lookingLeft;
            lookingRight = src.lookingRight;
            waveCycle = src.waveCycle;
            waveUp = src.waveUp;
            cameraHeight = src.cameraHeight;
            cameraDistance = src.cameraDistance;
            skyIndex = src.skyIndex;
        }

        return *this;
    }
};

struct HUD
{
    unsigned int windHud;

    HUD( ) :windHud( -1 )
    {

    }

    HUD( const HUD& src ) : 
        windHud( src.windHud )
    {

    }
};


#endif // !SHIPCONTROLLER_H

