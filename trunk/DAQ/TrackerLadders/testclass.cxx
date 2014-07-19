#include <stdio.h>
#include "TrackerLadders.h"

int main() {

  TrackerLadders InnerTracker;

//   for (int i=0; i<MAXLAD+1; i++) {
//     Ladder *aladder=InnerTracker.GetLadder(i);
   
//     if (aladder==0) printf("sorry, ladder=0\n");
//     printf("lad=%3d %d %d %d %s %d %d %d %d %lf %lf\n", i,aladder->GetX(), aladder->GetLayer(), aladder->GetSlot(), aladder->GetName(), aladder->GetXtant(), aladder->GetPwg(), aladder->GetPwgpos(), aladder->GetTdr(), aladder->GetLc80(), aladder->GetLc90());

//   }

  InnerTracker.ReadFile("ladlist.txt");

  printf("%d ladders found\n", InnerTracker.GetNlads());


//   for (int i=0; i<MAXLAD+1; i++) {
//     Ladder *aladder=InnerTracker.GetLadder(i);
   
//     if (aladder==0) printf("sorry, ladder=0\n");
//     printf("lad=%3d %d %d %d %s %d %d %d %d %lf %lf\n", i,aladder->GetX(), aladder->GetLayer(), aladder->GetSlot(), aladder->GetName(), aladder->GetXtant(), aladder->GetPwg(), aladder->GetPwgpos(), aladder->GetTdr(), aladder->GetLc80(), aladder->GetLc90());

//   }

  for (int i=0; i<InnerTracker.GetNlads(); i++) {
    TkLadder *aladder=InnerTracker.GetLadder(i);
   
    if (aladder==0) printf("sorry, ladder=0\n");
    printf("%d %d %d %s %d %d %d %d %lf %lf\n", aladder->GetX(), aladder->GetLayer(), aladder->GetSlot(), aladder->GetName(), aladder->GetXtant(), aladder->GetPwg(), aladder->GetPwgpos(), aladder->GetTdr(), aladder->GetLc80(), aladder->GetLc90());

  }

  for (int i=0; i<InnerTracker.GetNlads(); i++) {
    if (!InnerTracker.CheckTdrAssignment(i)) printf("check ladder %3d\n",i);
    
    
  }

 //  for (int i=0; i<MAXLAD+1; i++) {
//     Ladder *aladder=InnerTracker.GetLadder(i);
   
//     if (aladder==0) printf("sorry, ladder=0\n");
//     printf("lad=%3d %d %d %d %s %d %d %d %d %lf %lf\n", i,aladder->GetX(), aladder->GetLayer(), aladder->GetSlot(), aladder->GetName(), aladder->GetXtant(), aladder->GetPwg(), aladder->GetPwgpos(), aladder->GetTdr(), aladder->GetLc80(), aladder->GetLc90());

//   }

  printf("bye...\n");

}
