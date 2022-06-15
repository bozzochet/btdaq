#include "aallib.h"
#include "abi.h"
#include "frm.h"
#include "apid.h"
#include "cmdopt.h"
#include "al_lib.h"

#ifndef OS_ECOS

//----  Command line parameters

#define CMD_HELP      'h'
#define CMD_DEBUG     'd'
#define CMD_APID      'a'
#define CMD_CW        1000
#define CMD_BS        1001
#define CMD_RIC       1029

const char           short_options[] =  "hd:a:";
static struct _CmdOpt long_options[] =
{
  {"help",    0, 0, CMD_HELP,     "Show this listing.", NULL},
  {"debug",   1, 0, CMD_DEBUG,    "Bigger is more.", "Debug"},

  {"cw",      0, 0, CMD_CW,       "Use CCSDS Check Word.", "Options"},

  {"bs",      0, 0, CMD_BS,       "CCSDS header ByteSwap.", "Options"},

  {"apid",    1, 0, CMD_APID,     "The APID to listen to.\n"
                                  "AMSHK, AMSSCI, AMSCMD, AMSHKH, PBCKSCK, PBCKHKH, "
                                  "PTOPCMD, PTOPREPLY, ACOPCMD, ACOPHK, ACOPQC, "
                                  "APLS_C, APLS_D, ACOP_C, ACOP_D, DDRS2_C, DDRS2_D, "
                                  "FILL", "APID"},
  {"RIC",     0, 0, CMD_RIC,      "Use RIC (EXPRESS) command formats.", "RIC"},
  {0, 0, 0, 0, 0, 0}
};

int Debug = 0;
int16 APID = 0x3FF;
int32 Options = 0;
int32 FrameSize = 256;
int32 RIC = FALSE;

void usage(void)
{
  printf("Usage:\n");
  printf("Program to test the fep routines.\n");
  printf("\n");
  CmdOptUsage(short_options, long_options);
}

int main(int argc, char **argv)
{
pAMSBlock pBlock, pClone, pRequest;
int i;
int8 *p;
FRAME FRM;
int8 Buffer[4096];
FRAME dbFRM;

  printf("frmTest 1.0 Compiled: " __DATE__ " " __TIME__ "\n"
         "  Peter Dennett PADSOFT, Inc  pdennett@padsoft.com  281 334 3800\n");

  // opterr = 0; // Turn off getopts error handling

  while (1)
  {
  int option_index = 0;
  int ch;

    ch = CmdOptGet(argc, argv, short_options, long_options, &option_index);
    if (ch == -1)
      break;

    switch (ch)
    {
      case '?':
      case CMD_HELP:
        usage();
        exit(0);
        break;

      case CMD_RIC: 
        RIC = TRUE;
        break;

      case CMD_DEBUG: 
        Debug = strtol(optarg, NULL, 0);
        break;

      case CMD_APID:
        APID = apidLookupValue(optarg);

        if (APID == 0)
        {
          fprintf(stderr, "APID %s was not located\n", optarg);
          exit(1);
        }
        break;

      case CMD_CW: 
        Options |= FO_CHECKWORD;
        break;

      case CMD_BS: 
        Options |= FO_BYTESWAP;
        break;
    }
  }

  if (Debug >= 2)
    Options |= FO_DEBUG;

  if (RIC)
    Options |= FO_RIC;
  else
    Options |= FO_PLMDM;

  pBlock = abiConstruct(0x12, 0x1f0090, FrameSize+2, BT_WRITE|BT_REQUEST);
  for(i=0, p=abiDataPtr8(pBlock); i<pBlock->DataCount; i++)
    *p++ = i;
  abiPrint("pBlock", pBlock, 22);

  pClone = abiClone(pBlock, 10);
  abiReplyError(pClone, BLOCK_S_END);
  abiPrint("pClone", pClone, 22);

  abiLog(stdout, pBlock);

  frmConstruct(&FRM, APID, FrameSize, Options);

  frmConstruct(&dbFRM, APID, FrameSize, Options);

  for (i=0; i<2; i++)
  {
    while( ! frmBlock(&FRM, &Buffer[0], pBlock))
    {
      if (Debug >= 1)
      {
        printf("Frame:\n");
        HexDump(&Buffer[0], FrameSize, TRUE);
      }

      if (Options & FO_CHECKWORD)
        printf("CheckWord: 0x%04X\n", frmCheckWord(&Buffer[0], FrameSize, FALSE, FALSE));

      while (frmDeblock(&dbFRM, &Buffer[0], &pRequest))
      {
        if (pRequest)
        {
          abiPrint("Deblock", pRequest, 22);
          abiDestruct(pRequest);
        }
      }
    }
    if (pBlock)
      abiDestruct(pBlock);
    pBlock = NULL;
  }

  return 0;
}

#endif // ! OS_ECOS
