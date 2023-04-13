//=====[Libraries]=============================================================

#include "mbed.h"

#include "gas_sensor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalIn mq2(PE_12);


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void gasSensorInit()
{
    // Se define el pin mq2 como PullUp para usar un pulsador en lugar del sensor
    mq2.mode(PullUp);
}

void gasSensorUpdate()
{
}

bool gasSensorRead()
{
    return mq2.read();
}

//=====[Implementations of private functions]==================================