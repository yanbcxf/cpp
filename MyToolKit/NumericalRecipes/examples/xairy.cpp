#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "nr.h"
using namespace std;

// Driver for routine airy

int main(void)
{
        string txt;
        int i,nval;
        DP ai,bi,aip,bip,x,xai,xbi,xaip,xbip;
        ifstream fp("fncval.dat");

        if (fp.fail())
          NR::nrerror("Data file fncval.dat not found");
        getline(fp,txt);
        while (txt.find("Airy Functions")) {
          getline(fp,txt);
          if (fp.eof()) NR::nrerror("Data not found in fncval.dat");
        }
        fp >> nval;
        getline(fp,txt);
        cout << endl << "Airy Functions" << endl;
        cout << setw(5) << "x" << endl;
        cout << setw(14) << "ai" << setw(16) << "bi";
        cout << setw(16) << "aip" << setw(16) << "bip" << endl;
        cout << setw(14) << "xai" << setw(16) << "xbi";
        cout << setw(16) << "xaip" << setw(16) << "xbip" << endl;
        cout << fixed << setprecision(6);
        for (i=0;i<nval;i++) {
          fp >> x >> ai >> bi >> aip >> bip;
          getline(fp,txt);
          NR::airy(x,xai,xbi,xaip,xbip);
          cout << setw(10) << x << endl;
          cout << setw(16) << ai << setw(16) << bi;
          cout << setw(16) << aip << setw(16) << bip << endl;
          cout << setw(16) << xai << setw(16) << xbi;
          cout << setw(16) << xaip << setw(16) << xbip << endl;
        }
        fp.close();
        return 0;
}
