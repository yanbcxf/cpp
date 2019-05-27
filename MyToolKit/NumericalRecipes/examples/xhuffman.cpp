#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "nr.h"
using namespace std;

// Driver for routines hufmak, hufenc, hufdec

int main(void)
{
        const unsigned int MC=512,MQ=MC+MC-1;
        unsigned long i,j,ilong,n,nb,nlong,nt,lcode,nch;
        int k;
        string mess,ness,code;
        huffcode hcode(MQ,MQ,MQ,MQ);
        Vec_ULNG nfreq(256);
        ifstream fp("text.dat");

        for (i=0;i<MQ;i++)
          hcode.icod[i]=hcode.ncod[i]=0;
        // construct a letter frequency table from the file text.dat
        if (fp.fail()) NR::nrerror("Input file text.dat not found.");
        for (i=0;i<256;i++)
          nfreq[i]=0;
        while (!fp.eof()) {
          getline(fp,mess);
          for (k=0;k<mess.length();k++)
            if (mess[k] >= 32) nfreq[mess[k]-32]++;
        }
        fp.close();
        nch=96;
        // here is the initialization that constructs the code
        NR::hufmak(nfreq,nch,ilong,nlong,hcode);
        cout << "ind char  nfreq  ncod  icod" << endl;
        for (j=0;j<nch;j++)
          if (nfreq[j] != 0) {
            cout << setw(3) << j << "   " << char(j+32);
            cout << setw(8) << nfreq[j] << setw(6) << hcode.ncod[j];
            cout << setw(6) << hcode.icod[j] << endl;
          }
        for (;;) {
          // now ready to prompt for lines to encode
          cout << endl << "Enter a line:" << endl;
          getline(cin,mess);
          n=mess.length();
          if (n == 0) break;
          // shift from 256 character alphabet to 96 printing characters
          for (j=0;j<n;j++) mess[j] -= 32;
          // here we Huffman encode mess[0..n-1]
          nb=0;
          code.erase();
          for (j=0;j<n;j++) NR::hufenc(mess[j],code,nb,hcode);
          lcode=(nb>>3)+1;
          // message termination (encode a single int character)
          NR::hufenc(ilong,code,nb,hcode);
          // here we decode the message to get the original back
          nb=0;
          ness.erase();
          // decode up to 10 chars more than should be needed
          for (j=0;j<n+10;j++) {
            NR::hufdec(i,code,lcode,nb,hcode);
            if (i == nch) break;
            else ness += (unsigned char) (i+32);
          }
          if (j >= n+10) NR::nrerror("Huffman coding: Never get here");
          cout << "Length of line input,coded= " << n << " " << lcode << endl;
          cout << "Decoded output:" << endl << ness << endl;
          nt=ness.length();
          if (nt != n)  cout << "Error! :  n decoded != n input" << endl;
          if (nt-n == 1) cout << "May be harmless spurious character." << endl;
        }
        return 0;
}
