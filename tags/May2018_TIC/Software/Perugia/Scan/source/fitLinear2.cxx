#include "TLinearFitter.h"
#include "TF1.h"
#include "TRandom.h"

void fitLinear2()
{
   //Fit a 5d hyperplane by n points, using the linear fitter directly

   //This macro shows some features of the TLinearFitter class
   //A 5-d hyperplane is fit, a constant term is assumed in the hyperplane
   //equation (y = a0 + a1*x0 + a2*x1 + a3*x2 + a4*x3 + a5*x4)

   Int_t n=100;
   Int_t i;
   TRandom rand;
   TLinearFitter *lf=new TLinearFitter(5);

   //The predefined "hypN" functions are the fastest to fit
   lf->SetFormula("hyp5"); 

   Double_t *x=new Double_t[n*10*5];
   Double_t *y=new Double_t[n*10];
   Double_t *e=new Double_t[n*10];

   //Create the points and put them into the fitter
   for (i=0; i<n; i++){
      x[0 + i*5] = rand.Uniform(-10, 10);
      x[1 + i*5] = rand.Uniform(-10, 10);
      x[2 + i*5] = rand.Uniform(-10, 10);		  
      x[3 + i*5] = rand.Uniform(-10, 10);
      x[4 + i*5] = rand.Uniform(-10, 10);
      e[i] = 0.01;
      y[i] = 4*x[0+i*5] + x[1+i*5] + 2*x[2+i*5] + 3*x[3+i*5] + 0.2*x[4+i*5]  + rand.Gaus()*e[i];
   }

   //To avoid copying the data into the fitter, the following function can be used:
   lf->AssignData(n, 5, x, y, e);
   //A different way to put the points into the fitter would be to use
   //the AddPoint function for each point. This way the points are copied and stored
   //inside the fitter

   //Perform the fitting and look at the results
   lf->Eval();
   TVectorD params;
   TVectorD errors;
   lf->GetParameters(params);
   lf->GetErrors(errors);
   for (Int_t i=0; i<6; i++)
      printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
   Double_t chisquare=lf->GetChisquare();
   printf("chisquare=%f\n", chisquare);


   //Now suppose you want to add some more points and see if the parameters will change
   for (i=n; i<n*2; i++) {
      x[0+i*5] = rand.Uniform(-10, 10);
      x[1+i*5] = rand.Uniform(-10, 10);
      x[2+i*5] = rand.Uniform(-10, 10);		  
      x[3+i*5] = rand.Uniform(-10, 10);
      x[4+i*5] = rand.Uniform(-10, 10);
      e[i] = 0.01;
      y[i] = 4*x[0+i*5] + x[1+i*5] + 2*x[2+i*5] + 3*x[3+i*5] + 0.2*x[4+i*5]  + rand.Gaus()*e[i];
   }

   //Assign the data the same way as before
   lf->AssignData(n*2, 5, x, y, e);   
   lf->Eval();
   lf->GetParameters(params);
   lf->GetErrors(errors);
   printf("\nMore Points:\n");
   for (Int_t i=0; i<6; i++)
      printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
   chisquare=lf->GetChisquare();
   printf("chisquare=%.15f\n", chisquare);


   //Suppose, you are not satisfied with the result and want to try a different formula
   //Without a constant:
   //Since the AssignData() function was used, you don't have to add all points to the fitter again
   lf->SetFormula("x0++x1++x2++x3++x4");

   lf->Eval();
   lf->GetParameters(params);
   lf->GetErrors(errors);
   printf("\nWithout Constant\n");
   for (Int_t i=0; i<5; i++)
     printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
   chisquare=lf->GetChisquare();
   printf("chisquare=%f\n", chisquare);

   //Now suppose that you want to fix the value of one of the parameters
   //Let's fix the first parameter at 4:
   lf->SetFormula("hyp5");
   lf->FixParameter(1, 4);
   lf->Eval();
   lf->GetParameters(params);
   lf->GetErrors(errors);
   printf("\nFixed Constant:\n");
   for (i=0; i<6; i++)
      printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
   chisquare=lf->GetChisquare();
   printf("chisquare=%.15f\n", chisquare);
   
   //The fixed parameters can then be released by the ReleaseParameter method
   delete lf;

}

void fitLinear3()
{
    
      Double_t *xp = new Double_t[4];
      Double_t *yp = new Double_t[4];
      Double_t *zp = new Double_t[4];
      Double_t *ey = new Double_t[4];
      double xval[4] = { 1.019178, 1.025000, 1.028648, 1.030941 };
      double yval[4] = { 1.247832, 1.250000, 1.247419, 1.250928 };
      double zval[4] = { -1087.0, 0.0, 1046.0, 1797.0 }; 
      TVectorD retpar;
      printf(" \n");
      Int_t n=4;
      for (int i=0; i<n; i++) {
         xp[i] = xval[i];
         yp[i] = yval[i];
         zp[i] = zval[i];
         ey[i] = 1.;
         printf("i %d x %f y %f z %f\n",i,xp[i],yp[i],zp[i]);
       }
      /*       for (int i=0; i<4; i++) {
	 Double_t z[1];
         z[0] = zp[i];
         Double_t x = xp[i];
         resfit->AddPoint(z,x,1.);
	 } */

       TLinearFitter *resfit = new TLinearFitter(1);
       resfit->AssignData(n,1,zp,xp,ey);
       resfit->SetFormula("pol1");
       resfit->Eval();
       //       int iret=resfit->Eval();
       //       printf("iret %d\n",iret);
       float chisqr=resfit->GetChisquare();
       resfit->PrintResults(3,0.);
       resfit->GetParameters(retpar);
       printf("chisqr %f parametres %f %f\n",chisqr,retpar[0],retpar[1]); 
       delete resfit;
}

int main(int argc,char** argv){          

  fitLinear2();  
  fitLinear3();
  
  return 0;
 
}

