#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <complex>
#include "nr.h"
using namespace std;

// Driver for routine cisi

int main(void)
{
        string txt;
        int i,nval;
        DP x,xci,xsi;
        complex<DP> cs;
        ifstream fp("fncval.dat");

        if (fp.fail())
          NR::nrerror("Data file fncval.dat not found");
        getline(fp,txt);
        while (txt.find("Cosine and Sine Integrals")) {
          getline(fp,txt);
          if (fp.eof()) NR::nrerror("Data not found in fncval.dat");
        }
        fp >> nval;
        getline(fp,txt);
        cout << endl << "Cosine and Sine Integrals" << endl;
        cout << setw(9) << "x" << setw(14) << "actual" << setw(12) <<"ci(x)";
        cout << setw(12) << "actual" << setw(12) << "si(x)" << endl << endl;
        cout << fixed << setprecision(6);
        for (i=0;i<nval;i++) {
          fp >> x >> xci >> xsi;
          NR::cisi(x,cs);
          cout << setw(12) << x << setw(12) << xci << setw(12) << real(cs);
          cout << setw(12) << xsi << setw(12) << imag(cs) << endl;
        }
        return 0;
}
