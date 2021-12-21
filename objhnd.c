/******************************************************************************/
/*                                                                            */
/*  program: isholliday                                                       */
/*  file   : objhnd.c                                                         */
/*                                                                            */
/*  description:                                                              */
/*    this file includes all functions necessarey for creating xymon ignore   */
/*    files                                                                   */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                               I N C L U D E S                              */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <utime.h>
#include <time.h>
#include <sys/types.h>

/******************************************************************************/
/*                                                                            */
/*                               D E F I N E S                                */
/*                                                                            */
/******************************************************************************/
#define RDBUFFLNG 1024
#define FLAGDIR   "monitor/flag" 

/******************************************************************************/
/*                                                                            */
/*                                M A C R O S                                 */
/*                                                                            */
/******************************************************************************/
#define mSetEOS()       \
  loop = 1;             \
  while( loop )         \
  {                     \
    switch( *p )        \
    {                   \
      case ' ' :        \
      case '\t':        \
      {                 \
        *p = '\0' ;     \
        loop = 0 ;      \
        break ;         \
      }                 \
      default:          \
      {                 \
        break;          \
      }                 \
    }                   \
    p++;                \
  }                     \
  
#define mSkipSpace( )   \
  loop = 1;             \
  while( loop )         \
  {                     \
    switch( *p )        \
    {                   \
      case ' ' :        \
      case '\t':        \
      {                 \
        break;          \
      }                 \
      case '\0':        \
      case '\n':        \
      {                 \
        goto _error;    \
      }                 \
      default :         \
      {                 \
        loop = 0;       \
      }                 \
    }                   \
    p++;                \
  }                     \
  

/******************************************************************************/
/*                                                                            */
/*                               G L O B A L S                                */
/*                                                                            */
/******************************************************************************/
FILE *fp;
char readBuffer[RDBUFFLNG] ;

/******************************************************************************/
/*                                                                            */
/*                              F U N C T I O N S                             */
/*                                                                            */
/******************************************************************************/
void setFilePointer( FILE *_fp )
{
  fp = _fp;
}

char* readObjLine( )
{
  return fgets( readBuffer, RDBUFFLNG, fp );
}

int crtIgnoreObj( char* holliday, int timeOffset )
{
  char *app;
  char *qmgr;
  char *type;
  char *obj;
  char *attr;

  char file[PATH_MAX] ;
  struct utimbuf modTime ;
  time_t sysTime ;
  FILE *fp ;

  int loop = 1 ;
 
  char *p = readBuffer;

  // -------------------------------------------------------
  // set utime 
  // -------------------------------------------------------
  sysTime = time( NULL );
  modTime.modtime = sysTime + timeOffset ;
  modTime.actime  = sysTime ;

  // -------------------------------------------------------
  // ignore leading blanks , skip comments
  // -------------------------------------------------------
  loop = 1; 
  while( loop )         //
  {                     //
    switch( *p )        //
    {                   // ignore white spaces
      case ' ' :        //  blank
      case '\t':        //  tab
      {                 //
        p++;            //
        continue;       //
      }                 //
      case '\0' :       // empty line
      case '\n' :       // 
      case '#'  :       // comment line
      {                 //
        goto _door;     //
      }                 //
      default:          // app name found
      {                 //
        app = p ;       //
        loop = 0 ;      // break while loop
        break;          //
      }                 //
    }                   //
  }                     //

  // -------------------------------------------------------
  // find the end of app
  // -------------------------------------------------------
  mSetEOS(); 
 
  // -------------------------------------------------------
  // find qmgr
  // -------------------------------------------------------
  mSkipSpace() ;
  qmgr = p - 1 ;
  mSetEOS();
  
  // -------------------------------------------------------
  // find type
  // -------------------------------------------------------
  mSkipSpace() ;
  type = p - 1 ;
  mSetEOS();
  
  // -------------------------------------------------------
  // find obj
  // -------------------------------------------------------
  mSkipSpace() ;
  obj = p - 1 ;
  mSetEOS();

  // -------------------------------------------------------
  // handle all attributes
  // -------------------------------------------------------
  mSkipSpace();                  // skip spaces means
  attr = p -1 ;                  // move pointer to first attr
                                 // attr string looks like:
  loop=1;                        //  attr1,attr2,attr3  \n
  while( loop )                  //
  {                              //
    switch( *p )                 // check every char
    {                            //
      // -------------------------------
      // end of atttribute list
      // -------------------------------
      case ' ' :                 // blank or
      case '\t':                 // tab or
      case '\n':                 // EOL or
      case '\0':                 // or EOS -> end of last attribute reached
      {                          // go out of the loop
        loop = 0;                // no break continue with switch, 
      }                          // last attribute needs attribute handling
      // -------------------------------
      // end of single attribute
      // -------------------------------
      case ',':                  // end of attribute reached (not the last one)
      {                          //
        *p = '\0' ;              //
        sprintf( file, "%s/"FLAGDIR"/%s-%s-%s-%s-%s", getenv("HOME"), 
                                                      app           , 
                                                      qmgr          , 
                                                      type          , 
                                                      attr          , 
                                                      obj          );
        fp = fopen( file, "a" ); // open the flag file
        if( !fp )                // error handling
        {                        //
          printf( "can't open file %s\n", file );
          perror("Error :");     //
          goto _error;           //
        }                        //
        fprintf( fp, "%s\n", holliday );
        fclose( fp );            //
        utime( file, &modTime ); //
        attr = p + 1;            //
        break;                   //
      }                          //
      // -------------------------------
      // any other char / part of attr name
      // -------------------------------
      default:                   //
      {                          //
        break;                   //
      }                          //
    }                            //
    p++;                         //
  }
  _door:
  return  0;

  _error:
  if( fp ) fclose( fp ) ;
  return 1;
}

