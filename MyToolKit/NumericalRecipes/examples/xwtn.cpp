#include <iostream>
#include <iomanip>
#include <cmath>
#include "nr.h"
using namespace std;

// Driver for routine wtn

int main(void)
{
        const int NX=8,NY=16,NTOT=NX*NY;
        const DP EPS=1.0e-6;
        int i,j,l,nerror=0;
        int ndim_d[2]={NX,NY};
        Vec_INT ndim(ndim_d,2);
        Vec_DP aorg(NTOT),a(NTOT);

        NR::pwtset(12);
        for (i=0;i<NX;i++)
          for (j=0;j<NY;j++) {
            l=i+j*NX;
            aorg[l]=a[l]=(i == j ? -1.0 : 1.0/sqrt(fabs(DP(i-j))));
          }
        NR::wtn(a,ndim,1,NR::pwt);
        // here, one might set the smallest components to zero,
        // encode and transmit the remaining components as a compressed
        // form of the "image"
        NR::wtn(a,ndim,-1,NR::pwt);
        for (l=0;l<NTOT;l++) {
          if (fabs(aorg[l]-a[l]) >= EPS) {
            cout << "Compare Error at element " << l << endl;
            nerror++;
          }
        }
        if (nerror != 0)
          cout << "Number of comparision errors: " << nerror << endl;
        else
          cout << "transform-inverse transform check OK" << endl;
        return nerror;
}
