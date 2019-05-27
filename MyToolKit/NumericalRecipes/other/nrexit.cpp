
#include <iostream>
using namespace std;

// Function used to stall exit in Borland C++Builder

void nrexit(void)
{
        cin.get();
        return;
}

#pragma exit nrexit
