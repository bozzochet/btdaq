/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2013 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                           61 Harbor Lane                                  */
/*                         Kemah, Texas 77565                                */
/*                          +1 713 899 6100                                  */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  commreport.c    Create a communications report for specific day

//  Revision History
//    Nov 2013 by Peter Dennett
//      - Initial version at JSC and PADSOFT.  Thanks to A.Basili

// To get strptime defined:
#define _GNU_SOURCE 1

#include "aallib.h"
#include "commreport.h"

#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include <string.h>

//~============================================================================

static char *WebText = NULL;
static size_t WebTextLen = 0;

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
size_t realsize = size * nmemb;
 
  WebText = (char *)realloc(WebText, WebTextLen + realsize + 1);
  if (WebText) 
  {
    memcpy(&(WebText[WebTextLen]), ptr, realsize);
    WebTextLen += realsize;
    WebText[WebTextLen] = 0;
  }

   return realsize;
 }

//~============================================================================

static time_t ConvertTime(int year, char *c, struct tm *t) 
{
struct tm tt;

  if (!t)
    t = &tt;

  if ( ! c) 
    return (time_t) -1;

  strptime(c, "%j/%H:%M:%S", t);

  t->tm_year = year - 1900;
  t->tm_mon  = 0;
  t->tm_mday = t->tm_yday + 1;

  return mktime(t);
}

//~============================================================================
pCOMMREPORT GetCommReport(int year, int jday, char *host, char *user, 
                          char *pass, int Debug)
{
CURL *curl;
CURLcode res;
char b[512];
char c[512];
enum _CprState 
{
  S_INIT=0,
  S_ROW=1,
  S_HTML=2,
  S_TEXT=3,
} CprState;
char *cp;
char text[1024];
char *fld[10];
//int index = 0;
int texti = 0;
int fldi = 0;
int fldf = 0;
pCOMMREPORT pCR = NULL;
time_t start, end;
int i;

// Use curl to get a copy of the web page in memory

  sprintf(b, "https://%s/apps/ostpv/CommSummaryReport.asp?GMT_year=%d&gmtDay=%d", 
          host, year, jday);
  sprintf(c, "%s:%s", user, pass);

  curl = curl_easy_init();
  if (Debug > 2) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_URL, b);
  curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM); 
  curl_easy_setopt(curl, CURLOPT_USERPWD, c);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM );
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  res = curl_easy_perform(curl);
  if ((Debug) && (res != CURLE_OK))
    fprintf(stderr, "curl_easy_perform() failed %s\n", 
            curl_easy_strerror(res));
  curl_easy_cleanup(curl);

  if (! WebText)
  {
    if (Debug)
      printf("GetCommReport: no web text collected\n");
    return NULL;
  }

  // Parse the web page table rows

  pCR = (pCOMMREPORT) malloc(sizeof (struct _COMMREPORT));
  if ( ! pCR)
  {
    if (Debug)
      printf("GetCommReport: Out of memory\n");
    return NULL;
  }
  memset(pCR, 0, sizeof (struct _COMMREPORT));

  pCR->year = year;
  pCR->jday = jday;
  pCR->passes = 0;

  // First pass gets the S and Ku passes into the table.  We will need a
  // second pass to get the right TDRS names on the Ku passes

  cp = WebText;
  //??memset(fld, 0, sizeof fld);
  CprState = S_INIT;

  for (cp = WebText; *cp; cp++)
  {
    switch (CprState)
    {
      case S_INIT:
        if (strncmp(cp, "<tr align=", 10) == 0)
        {
          CprState = S_ROW;
          texti = 0;
          fldf = 0;
          fldi = 0;
          memset(fld, 0, sizeof fld);
        }
        break;

      case S_ROW:
        if (strncmp(cp, "</tr>", 5) == 0)
        {
          CprState = S_INIT;

          if (strcasestr(fld[1], "TDRS") && strcasestr(fld[5], "AVAIL"))
          {
            if (strcasestr(fld[2], "ALL") && (strcasestr(fld[5], "KU") || 
                                              strcasestr(fld[5], "S1") ||
                                              strcasestr(fld[5], "S2")))
            {
              start = ConvertTime(year, fld[3], NULL);
              end = ConvertTime(year, fld[4], NULL);

              pCR = (pCOMMREPORT) realloc(pCR, sizeof (struct _COMMREPORT) + 
                                          pCR->passes * 
                                          (sizeof pCR->commpass[0]));
              if ( ! pCR)
              {
                if (Debug)
                  printf("GetCommReport: Out of memory %d\n", 
                         sizeof (struct _COMMREPORT) + pCR->passes * 
                                          (sizeof pCR->commpass[0]));
                return NULL;
              }
              memset(&pCR->commpass[pCR->passes], 0,
                     sizeof (pCR->commpass[pCR->passes]));
              strncpy(pCR->commpass[pCR->passes].orbit, fld[0], 
                      sizeof pCR->commpass[pCR->passes].orbit);
              strncpy(pCR->commpass[pCR->passes].band, fld[5], 
                      sizeof pCR->commpass[pCR->passes].band);
              pCR->commpass[pCR->passes].commservice =
                strcasestr(fld[5], "KU") ? comm_ku : comm_s;
              pCR->commpass[pCR->passes].start = start;
              pCR->commpass[pCR->passes].end = end;
              pCR->passes++;
            }
          }
        }
        else if (strncmp(cp, "<td ", 4) == 0)
          CprState = S_HTML;
        break;

      case S_HTML:
      case S_TEXT:
        if (strncmp(cp, "</td>", 5) == 0)
        {
          CprState = S_ROW;

          text[texti++] = 0;
          fld[fldi++] = &(text[fldf]);
          fldf = texti;
        }
        else if (*cp == '>')
          CprState = S_TEXT;
        else if (*cp == '<')
          CprState = S_HTML;
        else if (CprState == S_TEXT)
          text[texti++] = *cp;
        
        break;
    }
  }

  // The second pass gets the right TDRS names on the Ku passes.  The same 
  // parsing just handle the tags differently.

  cp = WebText;
  CprState = S_INIT;

  for (cp = WebText; *cp; cp++)
  {
    switch (CprState)
    {
      case S_INIT:
        if (strncmp(cp, "<tr align=", 10) == 0)
        {
          CprState = S_ROW;
          texti = 0;
          fldf = 0;
          fldi = 0;
          memset(fld, 0, sizeof fld);
        }
        break;

      case S_ROW:
        if (strncmp(cp, "</tr>", 5) == 0)
        {
          CprState = S_INIT;

          if (strcasestr(fld[1], "TDRS") && strcasestr(fld[5], "AVAIL"))
          {
            if (strcasestr(fld[2], "ALL") && (strcasestr(fld[5], "KU") || 
                                              strcasestr(fld[5], "S1") ||
                                              strcasestr(fld[5], "S2")))
            {
              ;
            }
            else if (strcasestr(fld[5], "KU"))
            {
              start = ConvertTime(year, fld[3], NULL);
              end = ConvertTime(year, fld[4], NULL);

              for (i=0; i<pCR->passes; i++)
              {
                if ((pCR->commpass[i].commservice == comm_ku) &&
                    (start == pCR->commpass[i].start) &&
                    (end == pCR->commpass[i].end))
                  strncpy(pCR->commpass[i].tdrs, fld[2],
                          sizeof pCR->commpass[i].tdrs);
              }
            }
            else if (strcasestr(fld[5], "S1") ||
                     strcasestr(fld[5], "S2"))
            {
              start = ConvertTime(year, fld[3], NULL);
              end = ConvertTime(year, fld[4], NULL);

              for (i=0; i<pCR->passes; i++)
              {
                if ((pCR->commpass[i].commservice == comm_s) &&
                    (start == pCR->commpass[i].start) &&
                    (end == pCR->commpass[i].end))
                  strncpy(pCR->commpass[i].tdrs, fld[2],
                          sizeof pCR->commpass[i].tdrs);
              }
            }
          }
        }
        else if (strncmp(cp, "<td ", 4) == 0)
          CprState = S_HTML;
        break;

      case S_HTML:
      case S_TEXT:
        if (strncmp(cp, "</td>", 5) == 0)
        {
          CprState = S_ROW;

          text[texti++] = 0;
          fld[fldi++] = &(text[fldf]);
          fldf = texti;
        }
        else if (*cp == '>')
          CprState = S_TEXT;
        else if (*cp == '<')
          CprState = S_HTML;
        else if (CprState == S_TEXT)
          text[texti++] = *cp;
        
        break;
    }
  }

  // Done with pages

  free(WebText);

  return pCR;
}

//~============================================================================
void PrintCommReportGnuPlotKu(pCOMMREPORT pCR)
{
int i;
char start_s[40], end_s[40], boxstart_s[40];
time_t boxstart, duration;
char tdrs[12];

  if ( ! pCR)
    return;

  printf("# Ku Pass Data.   GMT %4.4d-%3.3d\n", pCR->year, pCR->jday);
  printf("# %-6s%-4s %-17s %-17s %-17s %s\n", "Orbit", "TDRS", "Start", 
         "End", "BoxStart", "Duration"); 
  for (i=0; i<pCR->passes; i++)
  {
    if (pCR->commpass[i].commservice == comm_ku)
    {
      strftime(start_s, sizeof start_s, "%Y-%j.%H:%M:%S",
               localtime(&pCR->commpass[i].start));
      strftime(end_s, sizeof end_s, "%Y-%j.%H:%M:%S",
               localtime(&pCR->commpass[i].end));
      duration = pCR->commpass[i].end - pCR->commpass[i].start;
      boxstart = pCR->commpass[i].start + duration/2;
      strftime(boxstart_s, sizeof boxstart_s, "%Y-%j.%H:%M:%S",
               localtime(&boxstart));
      if ((*pCR->commpass[i].tdrs == 0) || (*pCR->commpass[i].tdrs == ' '))
        strcpy(tdrs, "-");
      else
        strncpy(tdrs, pCR->commpass[i].tdrs+5, sizeof pCR->commpass[i].tdrs);
      printf("%-8s %-3s %-17s %-17s %-17s %d\n", pCR->commpass[i].orbit, tdrs, 
             start_s, end_s, boxstart_s, duration); 
    }
  }

  return;
}


//~============================================================================
void PrintCommReportGnuPlotSband(pCOMMREPORT pCR)
{
int i;
char start_s[40], end_s[40], boxstart_s[40];
time_t boxstart, duration;
char tdrs[12];

  if ( ! pCR)
    return;

  printf("# S-band Pass Data.   GMT %4.4d-%3.3d\n", pCR->year, pCR->jday);
  printf("# %-6s%-4s %-17s %-17s %-17s %s\n", "Orbit", "TDRS", "Start", 
         "End", "BoxStart", "Duration"); 
  for (i=0; i<pCR->passes; i++)
  {
    if (pCR->commpass[i].commservice == comm_s)
    {
      strftime(start_s, sizeof start_s, "%Y-%j.%H:%M:%S",
               localtime(&pCR->commpass[i].start));
      strftime(end_s, sizeof end_s, "%Y-%j.%H:%M:%S",
               localtime(&pCR->commpass[i].end));
      duration = pCR->commpass[i].end - pCR->commpass[i].start;
      boxstart = pCR->commpass[i].start + duration/2;
      strftime(boxstart_s, sizeof boxstart_s, "%Y-%j.%H:%M:%S",
               localtime(&boxstart));
      if ((*pCR->commpass[i].tdrs == 0) || (*pCR->commpass[i].tdrs == ' '))
        strcpy(tdrs, "-");
      else
        strncpy(tdrs, pCR->commpass[i].tdrs+5, sizeof pCR->commpass[i].tdrs);
      printf("%-8s %-3s %-17s %-17s %-17s %d\n", pCR->commpass[i].orbit, tdrs, 
             start_s, end_s, boxstart_s, duration); 
    }
  }

  return;
}

//~============================================================================
void PrintCommReport(pCOMMREPORT pCR)
{
int i;
char start_s[40], end_s[40];

  if ( ! pCR)
    return;

  printf("# Pass Data.   GMT %4.4d-%3.3d\n", pCR->year, pCR->jday);
  for (i=0; i<pCR->passes; i++)
  {
    strftime(start_s, sizeof start_s, "%Y-%j.%H:%M:%S",
             localtime(&pCR->commpass[i].start));
    strftime(end_s, sizeof end_s, "%Y-%j.%H:%M:%S",
             localtime(&pCR->commpass[i].end));
    printf("%3d '%s' '%s' '%s' '%s' '%s'\n", i, pCR->commpass[i].orbit,
           pCR->commpass[i].band, pCR->commpass[i].tdrs, start_s, end_s); 
  }

  return;
}
