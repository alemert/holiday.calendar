/******************************************************************************/
/*                                                                            */
/*  holliday - main                                                           */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                               I N C L U D E                                */
/*                                                                            */
/******************************************************************************/

// ---------------------------------------------------------
// system
// ---------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// ---------------------------------------------------------
// own
// ---------------------------------------------------------
#include "feierlib.h"
#include "objhnd.h"

/******************************************************************************/
/*                                                                            */
/*                               D E F I N E S                                */
/*                                                                            */
/******************************************************************************/
#define BUFFLNG 1024

/******************************************************************************/
/*                                                                            */
/*                                   M A I N                                  */
/*                                                                            */
/******************************************************************************/
int main( int argc, char* argv[] )
{
  FILE *holFP ;   // holliday file pointer
  FILE *objFP ;   // object file pointer
                  //
  const char *holName;  // holliday file name
  const char *objName;  // object file name
  int    timeOffset ;   // time offset in seconds
  char*  timeOffsetBuff;//
                        //
  time_t epTime ;       // epoch time
  struct tm *hrTime;    // human readable time
  long julianDay;       // days in julian calander
  int year;             //
  int month;            //
  int day;              //
  int yday;             // days since 1.1 this year
                        //
  char buff[BUFFLNG];   // string buffer
  char *rcStr;          // return code string

  // -------------------------------------------------------
  // check command line attributes
  // -------------------------------------------------------
  switch( argc )
  {
    case 0:
    case 1:
    {
      printf( "holliday file not specified\n");
      goto _door;
    }
    case 2:
    { 
      printf( "object file not specified\n");
      goto _door;
    } 
    case 3: 
    { 
      printf( "time offset not specified\n");
      goto _door;
    } 
    case 4: break;
    default:
    {
      printf( "object file not specified\n");
      goto _door;
    }
  }

  // -------------------------------------------------------
  // set command line attributes
  // -------------------------------------------------------
  holName = argv[1] ;
  objName = argv[2] ;

  timeOffsetBuff = argv[3] ;
  timeOffset = atoi( timeOffsetBuff ) ;
  
  // -------------------------------------------------------
  // open configuration files
  // -------------------------------------------------------
  holFP = fopen( holName, "r" );
  if( holFP == NULL )
  {
    printf( "can't open file %s\n", holName );
    goto _door ;
  }
  objFP = fopen( objName, "r" );
  if( objFP == NULL )
  {
    printf( "can't open file %s\n", objName );
    goto _door ;
  }

  // -------------------------------------------------------
  // check if today is a holliday
  // -------------------------------------------------------
  time( &epTime );                         // today unix epoch time
  hrTime = localtime( &epTime );           // 
  year = hrTime->tm_year + 1900 ;          //
                                           //
  julianDay = jdatum( year             ,   // days since 1.1.4713 BC
                      hrTime->tm_mon+1 ,   //
                      hrTime->tm_mday  );  //
                                           //
  gdatum( julianDay, 
          &year    ,
          &month   ,
          &day     );

  yday = tagesnummer( year, month, day );


  int cnt = 0;
  while( fgets( buff, BUFFLNG, holFP ) != NULL )
  {
    cnt++;
    rcStr = datum_zu_text( buff, year, yday, cnt );
    if( rcStr != NULL )
    {
      setFilePointer( objFP );
      while( readObjLine() )
      {
        crtIgnoreObj( rcStr, timeOffset );
      }
      break;
    }
  }

  // -------------------------------------------------------
  // exit / error handling
  // -------------------------------------------------------
  _door :

  if( objFP != NULL ) { fclose( objFP ); }
  if( holFP != NULL ) { fclose( holFP ); }

  return 1;
}

