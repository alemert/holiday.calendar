/*

   DATEINFO.C

   geschrieben von Oliver Kraus

   Veroeffentlichung in c't 15/97
                  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*---- Algorithmen ----*/

/*
   Eingabe: Jahr
   Ausgabe: 0 kein Schaltjahr       1 Schaltjahr
*/
int schaltjahr(int jahr)
{
   if ( ((jahr % 4 == 0) && (jahr % 100 != 0)) 
      || (jahr % 400 == 0) )
      return 1;
   return 0;
}

/*
   Eingabe: Jahr
            Monat (1=Jan, 2=Feb, ... 12 = Dec)
            Tag   (1...31)
   Ausgabe: Tagesnummer rel. zum Jahresanfang 
            (1=1.1.,2=2.1.,...365/366=31.12)
   Algorithmus von Robertson
*/
int tagesnummer(int jahr, int monat, int tag)
{
   int d, e;
   d = (monat+10)/13;
   e = tag + (611*(monat+2))/20 - 2*d - 91;
   return e + schaltjahr(jahr)*d;
}

/*
   Eingabe: Jahr
            Tagesnummer rel. zum Jahresanfang 
            (1=1.1.,2=2.1.,...365/366=31.12)
   Ausgabe: Monat (1=Jan, 2=Feb, ... 12 = Dec)
   Algorithmus von R. A. Stone
*/
int monat_im_jahr(int jahr, int n)
{
   int a;
   a = schaltjahr(jahr);
   if ( n > 59+a )
      n += 2-a;
   n += 91;
   return (20*n)/611 - 2;
}

/*
   Eingabe: Jahr
            Tagesnummer rel. zum Jahresanfang 
            (1=1.1.,2=2.1.,...365/366=31.12)
   Ausgabe: Tag (1..31)
   Algorithmus von R. A. Stone
*/
int tag_im_jahr(int jahr, int n)
{
   int a, m;
   a = schaltjahr(jahr);
   if ( n > 59+a )
      n += 2-a;
   n += 91;
   m = (20*n)/611;
   return n - (611*m)/20;
}

/*
   Eingabe: Jahr
            Tagesnummer rel. zum Jahresanfang 
            (1=1.1.,2=2.1.,...365/366=31.12)
   Ausgabe: Wochentag (0=So, 1=Mo,..., 6=Sa)
   Algorithmus von Zeller
*/
int wochentag_im_jahr(int jahr, int n)
{
   int j,c;
   j = (jahr-1) % 100;
   c = (jahr-1) / 100;
   return (28+j+n+(j/4)+(c/4)+5*c) % 7;
}

/*
   Eingabe: Jahr
   Ausgabe: Tagesnummer des Ostersonntag, 
            rel. zum Jahresanfang.
   Algorithmus "Computus ecclesiasticus"

   325 n.Chr. wurde Ostern auf den Sonntag nach 
   dem ersten Fruehlingsvollmond festgelegt. Damit 
   liegt Ostern zwischen dem 22. Maerz und
   dem 25. April.
*/
int ostersonntag(int jahr)
{
   int gz, jhd, ksj, korr, so, epakte, n;
   gz = (jahr%19)+1;
   jhd = jahr/100+1;
   ksj = (3*jhd)/4-12;
   korr = (8*jhd+5)/25-5;
   so = (5*jahr)/4-ksj-10;
   epakte = (11*gz+20+korr-ksj) % 30;
   if ( (epakte == 25 && gz > 11) || epakte == 24 )
      epakte++;
   n = 44-epakte;
   if ( n < 21 )
      n = n + 30;
   n = n + 7 - (so+n) % 7;
   n += schaltjahr(jahr);
   return n+59;
}

/*
   Eingabe: Jahr
            Monat (1=Jan, 2=Feb, ... 12 = Dec)
            Tag   (1...31)
   Ausgabe: Julianisches Datum 
            (Tage seit dem 1.1.4713 v.Chr.)
   Algorithmus von R. G. Tantzen
*/
long jdatum(int jahr, int monat, int tag)
{
   long c, y;
   if ( monat>2 )
   {
      monat -= 3;
   }
   else
   {
      monat += 9;
      jahr--;
   }
   tag += (153*monat+2)/5;
   c = (146097L*(((long)jahr) / 100L))/4L;
   y =   (1461L*(((long)jahr) % 100L))/4L;
   return c+y+(long)tag+1721119L;
}

/*
   Eingabe: Julianisches Datum 
            (Tage seit dem 1.1.4713 v.Chr.)
   Ausgabe: Jahr
            Monat (1=Jan, 2=Feb, ... 12 = Dec)
            Tag   (1...31)
   Modifizierter Algorithmus von R. G. Tantzen
*/
void gdatum(long jd, int *jahr, int *monat, int *tag)
{
   long j,m,t;
   jd -= 1721119L;

   j  = (4L*jd-1L) / 146097L;
   jd = (4L*jd-1L) % 146097L;
   t  = jd/4L;

   jd = (4L*t+3L) / 1461L;
   t  = (4L*t+3L) % 1461L;
   t  = (t+4L)/4L;

   m  = (5L*t-3L) / 153L;
   t  = (5L*t-3L) % 153L;
   t  = (t+5L)/5L;

   j  = 100L*j + jd;
   if ( m < 10L )
   {
      m+=3;
   }
   else
   {
      m-=9;
      j++;
   }
   *jahr  = (int)j;
   *monat = (int)m;
   *tag   = (int)t;
}

/*---- Programmteil ----*/

void leerzeichen(char **s)
{
   while((**s) > '\0' && (**s) <= ' ')
      (*s)++;
}

int lese_zahl(char **s)
{
   int z = 0;
   while( (**s) >= '0' && (**s) <= '9' )
   {
      z = z*10 + (int)(**s) - '0';
      (*s)++;
   }
   return z;
}

int lese_wochentag(char **s)
{
   int wt = -1;
   if ( strncmp(*s, "so", 2) == 0 )
      wt = 0;
   else if ( strncmp(*s, "mo", 2) == 0 )
      wt = 1;
   else if ( strncmp(*s, "di", 2) == 0 )
      wt = 2;
   else if ( strncmp(*s, "mi", 2) == 0 )
      wt = 3;
   else if ( strncmp(*s, "do", 2) == 0 )
      wt = 4;
   else if ( strncmp(*s, "fr", 2) == 0 )
      wt = 5;
   else if ( strncmp(*s, "sa", 2) == 0 )
      wt = 6;
   if ( wt >= 0 )
      (*s)+=2;
   return wt;
}

#define OSTR "ostersonntag"
#define MSTR "muttertag"

char *datum_zu_text(char *s, 
   int jahr, int vgltnum, int zeile)
{
   int tag, monat;
   int tnum;
   int vorzeichen;
   int wt, wij;
   leerzeichen(&s);
   if ( *s == '\0' ) 
      return NULL;
   if ( *s == ';' )     /* Kommentarzeichen */
      return NULL;
   if ( strncmp(s, OSTR, strlen(OSTR)) == 0 )
   {
      tnum = ostersonntag(jahr);
      s+=strlen(OSTR);
   }
   else if ( strncmp(s, MSTR, strlen(MSTR)) == 0 )
   {
      tnum = tagesnummer(jahr, 4, 30);
      tnum = tnum - wochentag_im_jahr(jahr, tnum) + 14;
      if ( tnum == ostersonntag(jahr)+49 )
         tnum -= 7;
      s+=strlen(MSTR);
   }
   else
   {
      tag=lese_zahl(&s);
      if ( *s != '.' )
      {
         printf(
            "Falsches Datumsformat in Zeile %d"
            " (erwartet: tt.mm oder '" OSTR "')", zeile);
         return NULL;
      }
      s++;
      monat=lese_zahl(&s);
      tnum = tagesnummer(jahr, monat, tag);
   }
   for(;;)
   {
      leerzeichen(&s);
      if ( *s == '+' || *s == '-' )
      {
         vorzeichen = 1;
         if ( *s == '-' )
            vorzeichen = -1;
         s++;
         leerzeichen(&s);
         wt = lese_wochentag(&s);
         if ( wt >= 0 )
         {
            wij = wochentag_im_jahr(jahr, tnum);
            if ( vorzeichen > 0 )
            {
               /* wochentag NACH/GLEICH dem Datum */
               if ( wij > wt )
                  tnum = tnum - wij + 7 + wt;
               else
                  tnum = tnum - wij + wt;
            }
            else
            {
               /* wochentag VOR dem angegeben Datum */
               if ( wij > wt )
                  tnum = tnum - wij + wt;
               else
                  tnum = tnum - wij - 7 + wt;
            }
         }
         else
         {
            tnum += vorzeichen*lese_zahl(&s);
         }
         leerzeichen(&s);
      }
      else if ( *s == ':' )
      {
         s++;
         if ( vgltnum == tnum )
            return s;            
         break;
      }
      else
      {
         printf(
            "Falscher Operator in Zeile %d"
            " (erwartet: '+', '-' oder ':')\n", 
            zeile);
         break;
      }
   }
   return NULL;
}

char *wochentage[] = 
{
   "Sonntag", 
   "Montag",
   "Dienstag",
   "Mittwoch",
   "Donnerstag",
   "Freitag",
   "Samstag"
};

int main(int argc, char *argv[])
{
   static char s[1024];
   int jahr = 1996;
   int tnum;
   int zeile;
   int rc=1;
   char *erg;
   FILE *fp;
   struct tm *tms;
   time_t curr_time;
   long jd;
   int monat, tag;

   if ( argc < 2 )
   {
      puts("Benutzung: dateinfo Feiertagsdatei [Jahr]");
      return 1;
   }

   fp = fopen(argv[1], "rt");
   if ( fp == NULL )
   {
      printf("Feiertagsdatei '%s' konnte nicht "
             "geoeffnet werden\n", argv[1]);
      return 1;
   }

   
      time(&curr_time);
      tms = localtime(&curr_time);
      jahr = tms->tm_year+1900;
      jd = jdatum(jahr, tms->tm_mon+1, tms->tm_mday);
      gdatum(jd, &jahr, &monat, &tag);
      tnum = tagesnummer(jahr, monat, tag);
         gdatum(jd, &jahr, &monat, &tag);
         tnum = tagesnummer(jahr, monat, tag);
         zeile = 0;
         fseek(fp, 0L, SEEK_SET);
         while( fgets(s, 1024, fp) != NULL )
         {
            zeile++;
            erg = datum_zu_text(s, jahr, tnum, zeile);
            if ( erg != NULL ){
               /*printf("%-10s %02d.%02d.%04d %s", 
                  wochentage[wochentag_im_jahr(jahr, tnum)],
                  tag_im_jahr(jahr, tnum),
                  monat_im_jahr(jahr, tnum), 
                  jahr, erg ); */
	       rc=0;
            }
         }

   fclose(fp);

   return rc;
}

