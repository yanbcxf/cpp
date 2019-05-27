#include <iostream>
#include <iomanip>
#include <cmath>
#include "nr.h"
using namespace std;

// Driver for routine wt1

int main(void)
{
        const int NMAX=512, NCEN=332, NWID=33;
        bool itest;
        int i,k,nused;
        DP frac,thresh,tmp;
        Vec_DP u(NMAX),v(NMAX),w(NMAX);

        for (;;) {
          cout << "Enter k (4, -4, 12, or 20) and frac (0.0 to 1.0):" << endl;
          cout << "Termination when k=0" << endl;
          cin >> k >> frac;
          if (k == 0) return 0;
          frac=MIN(1.0,MAX(0.0,frac));
          itest=(k == -4);
          if (k < 0) k = -k;
          if (k != 4 && k != 12 && k != 20) continue;
          for (i=0;i<NMAX;i++)
            w[i]=v[i]=(i > NCEN-NWID && i < NCEN+NWID ?
              (DP(i-NCEN+NWID)*DP(NCEN+NWID-i))/(NWID*NWID) : 0.0);
          if (!itest) NR::pwtset(k);
          NR::wt1(v,1,itest ? NR::daub4 : NR::pwt);
          for (i=0;i<NMAX;i++) u[i]=fabs(v[i]);
          thresh=NR::select(int((1.0-frac)*NMAX),u);
          nused=0;
          for (i=0;i<NMAX;i++) {
            if (fabs(v[i]) <= thresh)
              v[i]=0.0;
            else
              nused++;
          }
          NR::wt1(v,-1,itest ? NR::daub4 : NR::pwt);
          for (thresh=0.0,i=0;i<NMAX;i++)
            if ((tmp=fabs(v[i]-w[i])) > thresh) thresh=tmp;
          cout << "k,NMAX,nused = " << setw(5) << k;
          cout << setw(5) << NMAX << setw(5) << nused << endl;
          cout << "discrepancy = " << setw(12) << thresh << endl << endl;
        }
}
