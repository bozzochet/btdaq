#include "aallib.h"
#include "abi.h"

#ifndef OS_ECOS

int main(int argc, char **argv)
{
pAMSBlock pBlock, pClone;
int i;
int8 *p;

  printf("abiTest\n");

  pBlock = abiConstruct(0x12, 0x90, 101, BT_WRITE|BT_REQUEST);
  for(i=0, p=abiDataPtr8(pBlock); i<pBlock->DataCount; i++)
    *p++ = i;
  abiPrint("pBlock", pBlock, 22);

  pClone =abiClone(pBlock, 10);
  abiPrint("pClone", pClone, 22);

  //  abiLog(stdout, pBlock);

  return 0;
}

#endif // ! OS_ECOS
