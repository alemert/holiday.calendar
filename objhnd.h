/******************************************************************************/
/*                                                                            */
/*  program: isholliday                                                       */
/*  file   : objhnd.h                                                         */
/*                                                                            */
/*  description:                                                              */
/*    this file includes all functions necessarey for creating xymon ignore   */
/*    files                                                                   */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                              P R O T O T Y P E                             */
/*                                                                            */
/******************************************************************************/
void setFilePointer( FILE *_fp );
char* readObjLine( );
int crtIgnoreObj( char* str, int offset );

