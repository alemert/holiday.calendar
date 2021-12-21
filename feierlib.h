/*
int schaltjahr(int jahr);
int tag_im_jahr(int jahr, int n);
int wochentag_im_jahr(int jahr, int n);
int monat_im_jahr(int jahr, int n);
*/

long jdatum(int jahr, int monat, int tag);
void gdatum(long jd, int *jahr, int *monat, int *tag);
int tagesnummer(int jahr, int monat, int tag);
char *datum_zu_text(char *s, int jahr, int vgltnum, int zeile);

