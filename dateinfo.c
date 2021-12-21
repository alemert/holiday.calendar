/******************************************************************************/
/*                                                                            */
/*  DATEINFO.C                                                                */
/*                                                                            */
/*  geschrieben von Oliver Kraus                                              */
/*  Veroeffentlichung in c't 15/97                                            */
/*                                                                            */
/*  original description from c't:                                            */
/*  dateinfo.c gibt alle besonderen Tage eines ganzen Jahres aus oder die der */
/*  naehsten 30 Tage. Die Feiertage stehen nicht fest kodiert im Listing,     */
/*  sondern werden aus einer ASCII-Datei (dateinfo.dat) eingelesen. Damit ist */
/*  es besonders einfach, das Programm um persoenliche Feiertage oder Termine */
/*  zu ergaenzen. Das Programm erwartet als erstes Argument den Namen der     */
/*  Feiertagsdatei. Wenn noch eine Jahresnummer folgt, gibt es die Feiertage  */
/*  des betreffenden Jahres aus. Die Feiertagsdatei enthaelt entweder         */
/*  Leerzeilen, Kommentare (eingeleitet mit einem Strichpunkt) oder einen     */
/*  Feiertag. Ein Feiertag beginnt immer mit einem Datum (ohne Jahreszahl)    */
/*  oder den Texten "ostersonntag" oder alternativ "muttertag". Mit +nummer   */
/*  oder -nummer weist man das Programm an, Tage zu addieren oder zu          */
/*  subtrahieren.                                                             */
/*                                                                            */
/******************************************************************************/

#include "feierlib.h"
#include "dateinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/******************************************************************************/
/*      main      */
/******************************************************************************/
int main(int argc, char *argv[])
{
  static char s[1024];
  int jahr = 1996;
  int tnum;
  int zeile;
  char *erg;
  FILE *fp;
  struct tm *tms;
  time_t curr_time;
  long jd, jstart, jend;
  int monat, tag;

  if ( argc < 2 )
  {
    puts("Benutzung: dateinfo Feiertagsdatei [Jahr]");
    return 1;
  }

  // -------------------------------------------------------
  // open file in "(r)ead (t)ext modous
  // -------------------------------------------------------
  fp = fopen(argv[1], "rt");
  if ( fp == NULL )
  {
    printf("Feiertagsdatei '%s' konnte nicht "
           "geoeffnet werden\n", argv[1]);
    return 1;
  }

  // -------------------------------------------------------
  // 2nd attribute on cmdln -> display all holliday in one year 
  //  according to configuration file
  //
  // to through the whole config file for each day of the year
  //  checking if the actual day is a holliday
  //
  // --------------------------------------------------------
  if ( argc > 2 )                   //
  {                                 //
    jahr = atoi(argv[2]);           //
    printf("Jahr: %d\n", jahr);     // go through all days of the year
    for ( tnum = 1; tnum <= 365+schaltjahr(jahr); tnum++ )
    {                               //
      fseek(fp, 0L, SEEK_SET);      // go back to the start of the file
      zeile = 0;                    //
      while( fgets(s, 1024, fp) != NULL )
      {                             // and read file line by line
        zeile++;                    //
        erg = datum_zu_text(s, jahr, tnum, zeile);
        if ( erg != NULL )          //
        {                           //
          printf( "%-10s %02d.%02d.%04d %s"                    , 
                      wochentage[wochentag_im_jahr(jahr, tnum)],
                      tag_im_jahr(jahr, tnum)                  ,
                      monat_im_jahr(jahr, tnum)                , 
                      jahr          , 
                      erg           );
        }                           //
      }                             //
    }                               //
  }                                 //
  // -------------------------------------------------------
  // chek if today is a holliday
  // -------------------------------------------------------
  else                              //
  {                                 //
    time(&curr_time);               // get current local time
    tms = localtime(&curr_time);    // in human readeble format
    jahr = tms->tm_year+1900;       // 
                                                // days since 1.1.4713 BC 
    jd=jdatum(jahr,tms->tm_mon+1,tms->tm_mday); // Julian calender: 
    gdatum(jd, &jahr, &monat, &tag);            // Gregorian calender
    tnum = tagesnummer(jahr, monat, tag);       // days since 1.1 this year
    printf("Heute ist %s, der %02d.%02d.%04d\n"         , 
               wochentage[wochentag_im_jahr(jahr, tnum)],
               tag_im_jahr(jahr, tnum)   ,
               monat_im_jahr(jahr, tnum) , 
               jahr                      );
    jstart = jd - 7L;  // start of the period = today - one week
    jend = jd + 30L;   // end of the period = today + one month
    for( jd = jstart; jd <= jend; jd++ )  // following loop just like
    {                                     // whole year
      gdatum(jd, &jahr, &monat, &tag);    // 
      tnum = tagesnummer(jahr, monat, tag);
      zeile = 0;
      fseek(fp, 0L, SEEK_SET);
      while( fgets(s, 1024, fp) != NULL )
      {
        zeile++;
            erg = datum_zu_text(s, jahr, tnum, zeile);
            if ( erg != NULL )
               printf("%-10s %02d.%02d.%04d %s", 
                  wochentage[wochentag_im_jahr(jahr, tnum)],
                  tag_im_jahr(jahr, tnum),
                  monat_im_jahr(jahr, tnum), 
                  jahr, erg );
         }
      }
   }

   fclose(fp);

   return 0;
}

