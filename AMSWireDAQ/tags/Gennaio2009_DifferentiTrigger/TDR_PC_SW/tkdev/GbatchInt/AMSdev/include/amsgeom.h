//  $Id: amsgeom.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void magnetgeom02d(AMSgvolume &);
extern void magnetgeom02r(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
#ifdef __G4AMS__
 extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void pshgeom02(AMSgvolume &);
extern void richgeom02(AMSgvolume &, float );
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float );
extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume * , int);
extern void ext1structure02(AMSgvolume &);
extern void ext2structure02(AMSgvolume &);
};
