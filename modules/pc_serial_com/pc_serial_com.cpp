//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"

#include "siren.h"
#include "fire_alarm.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "event_log.h"

//=====[Declaration of private defines]========================================

#define MAX_TIME_READ_STEPS 1000
#define TIME_READ_STEP 10

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
    PC_SERIAL_SAVE_DATE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static pcSerialReadStatus_t pcSerialComStringRead( char* str, int strLength );

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );

static void pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();
static void commandShowCurrentAlarmState();
static void commandShowCurrentGasDetectorState();
static void commandShowCurrentOverTemperatureDetectorState();
static void commandEnterCodeSequence();
static void commandEnterNewCode();
static void commandShowCurrentTemperatureInCelsius();
static void commandShowCurrentTemperatureInFahrenheit();
static void commandSetDateAndTime();
static void commandShowDateAndTime();
static void commandShowStoredEvents();

static void clearArray(char** str);

static void pcSerialComSaveDate(char);


//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_DATE:
                pcSerialComSaveDate( receivedChar);
            break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void pcSerialComGetCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeChars] = receivedChar;
    pcSerialComStringWrite( "*" );
    numberOfCodeChars++;
   if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        codeComplete = true;
        numberOfCodeChars = 0;
    } 
}

static void pcSerialComSaveNewCodeUpdate( char receivedChar )
{
    static char newCodeSequence[CODE_NUMBER_OF_KEYS];

    newCodeSequence[numberOfCodeChars] = receivedChar;
    pcSerialComStringWrite( "*" );
    numberOfCodeChars++;
    if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeChars = 0;
        codeWrite( newCodeSequence );
        pcSerialComStringWrite( "\r\nNew code configured\r\n\r\n" );
    } 
}

static void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {
        case '1': commandShowCurrentAlarmState(); break;
        case '2': commandShowCurrentGasDetectorState(); break;
        case '3': commandShowCurrentOverTemperatureDetectorState(); break;
        case '4': commandEnterCodeSequence(); break;
        case '5': commandEnterNewCode(); break;
        case 'c': case 'C': commandShowCurrentTemperatureInCelsius(); break;
        case 'f': case 'F': commandShowCurrentTemperatureInFahrenheit(); break;
        case 's': case 'S': commandSetDateAndTime(); break;
        case 't': case 'T': commandShowDateAndTime(); break;
        case 'e': case 'E': commandShowStoredEvents(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "Available commands:\r\n" );
    pcSerialComStringWrite( "Press '1' to get the alarm state\r\n" );
    pcSerialComStringWrite( "Press '2' to get the gas detector state\r\n" );
    pcSerialComStringWrite( "Press '3' to get the over temperature detector state\r\n" );
    pcSerialComStringWrite( "Press '4' to enter the code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press '5' to enter a new code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    pcSerialComStringWrite( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    pcSerialComStringWrite( "Press 's' or 'S' to set the date and time\r\n" );
    pcSerialComStringWrite( "Press 't' or 'T' to get the date and time\r\n" );
    pcSerialComStringWrite( "Press 'e' or 'E' to get the stored events\r\n" );
    pcSerialComStringWrite( "\r\n" );
}

static void commandShowCurrentAlarmState()
{
    if ( sirenStateRead() ) {
        pcSerialComStringWrite( "The alarm is activated\r\n");
    } else {
        pcSerialComStringWrite( "The alarm is not activated\r\n");
    }
}

static void commandShowCurrentGasDetectorState()
{
    if ( gasDetectorStateRead() ) {
        pcSerialComStringWrite( "Gas is being detected\r\n");
    } else {
        pcSerialComStringWrite( "Gas is not being detected\r\n");
    }    
}

static void commandShowCurrentOverTemperatureDetectorState()
{
    if ( overTemperatureDetectorStateRead() ) {
        pcSerialComStringWrite( "Temperature is above the maximum level\r\n");
    } else {
        pcSerialComStringWrite( "Temperature is below the maximum level\r\n");
    }
}

static void commandEnterCodeSequence()
{
    if( sirenStateRead() ) {
        pcSerialComStringWrite( "Please enter the four digits numeric code " );
        pcSerialComStringWrite( "to deactivate the alarm: " );
        pcSerialComMode = PC_SERIAL_GET_CODE;
        codeComplete = false;
        numberOfCodeChars = 0;
    } else {
        pcSerialComStringWrite( "Alarm is not activated.\r\n" );
    }
}

static void commandEnterNewCode()
{
    pcSerialComStringWrite( "Please enter the new four digits numeric code " );
    pcSerialComStringWrite( "to deactivate the alarm: " );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;

}

static void commandShowCurrentTemperatureInCelsius()
{
    char str[100] = "";
    sprintf ( str, "Temperature: %.2f \xB0 C\r\n",
                    temperatureSensorReadCelsius() );
    pcSerialComStringWrite( str );  
}

static void commandShowCurrentTemperatureInFahrenheit()
{
    char str[100] = "";
    sprintf ( str, "Temperature: %.2f \xB0 C\r\n",
                    temperatureSensorReadFahrenheit() );
    pcSerialComStringWrite( str );  
}

static void commandSetDateAndTime()
{
    pcSerialComMode = PC_SERIAL_SAVE_DATE;
    pcSerialComStringWrite("\r\nType four digits for the current year (YYYY): ");

}

static void pcSerialComSaveDate(char receivedChar) {
/**
* La función bloqueante se llamaba desde esta función. Se agregó el estado PC_SERIAL_SAVE_DATE
* al enum pcSerialComMode_t. Cuando se escriba una "S", se cambiará el estado y cada vez que
* se reciba un char por puerto serie, se guardará en alguna de las variables de esta función.
* Para garantizar que se guarden se utilizaron variables static.
*/
    static char year[5] = "";
    static char month[3] = "";
    static char day[3] = "";
    static char hour[3] = "";
    static char minute[3] = "";
    static char second[3] = "";
    static int i = 0;
    pcSerialReadStatus_t status = PC_SERIAL_READ_STATUS_ERROR;


    if(0<= i && i < 4) {
        year[i] = receivedChar;
        if(i == 3) {
            year[4] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Type two digits for the current month (01-12): ");
        }
    }

    if(4<= i && i < 6) {
        month[i-4] = receivedChar;
        if(i-4 == 1) {
            month[2] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Type two digits for the current day (01-31): ");
        }
        
    }

    if(6<= i && i < 8) {
        day[i-6] = receivedChar;
        if(i-6 == 1) {
            day[2] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Type two digits for the current hour (00-23): ");
        }
        
    }

    if(8<= i && i < 10) {
        hour[i-8] = receivedChar;
        if(i-8 == 1) {
            hour[2] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Type two digits for the current minutes (00-59): ");
        }
        
    }

    if(10<= i && i < 12) {
        minute[i-10] = receivedChar;
        if(i-10 == 1) {
            minute[2] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Type two digits for the current seconds (00-59): ");
        }
        
    }

    if(12<= i && i < 14) {
        second[i-12] = receivedChar;
        if(i-12 == 1) {
            second[2] = '\0';
            pcSerialComStringWrite("\r\n");
            pcSerialComStringWrite("Date and time has been set\r\n");
            dateAndTimeWrite( atoi(year), atoi(month), atoi(day), 
            atoi(hour), atoi(minute), atoi(second) );
            i = -1;
            pcSerialComMode = PC_SERIAL_COMMANDS;
        }
        
    }
    i++;
}

static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}

static void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH] = "";
    int i;
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        pcSerialComStringWrite( str );   
        pcSerialComStringWrite( "\r\n" );                    
    }
}
