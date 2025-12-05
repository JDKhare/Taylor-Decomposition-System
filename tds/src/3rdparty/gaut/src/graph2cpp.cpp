#include <fstream>
#include "ac_fixed.h"
using namespace std;

#define iteration 10
#define verbose   0
#define trace     1


//predefined or user defined  operation
//constant
//input
ac_int<32,true> a[iteration] = {0,1,2,3,4,5,6,7,8,9};
ac_int<32,true> b[iteration] = {0,1,2,3,4,5,6,7,8,9};
//output
ac_int<32,true> o = 0;
//variable
ac_int<32,true> tmp_id6 = 0;
ac_int<32,true> id3 = 0;
ac_int<32,true> id5 = 0;

// process
int main (int argc, char *argv[])
{
	int it;
	for (it=0; it<iteration; it++)
 	{
		if(trace) cout <<  "Iteration : " << it << endl;
		tmp_id6 = id3 * id5;
		if (trace)
		{
			cout << "tmp_id6[ " << it << "] = " << tmp_id6 << "(" << tmp_id6.to_string(AC_BIN) << ")" << endl;
		}
		o = tmp_id6;
			cout << "o[ " << it << "] = " << o << endl;
		id3 = a; // aging
		id5 = b; // aging
  }
  return 0;
}
