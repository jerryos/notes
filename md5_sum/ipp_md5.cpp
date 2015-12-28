#include <stdio.h>
#include <string.h>
#include "ipp.h"
#include <stdlib.h>

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	if(argc <=1)
	{
		cout<<"Usage: commamd agr[1]"<<endl;
		exit(-1);
	}

    int len = 0;
//    FILE *pFile = fopen ("test.db", "rb");
    FILE *pFile = fopen (argv[1], "rb");


    // start clock
    clock_t start, finish;
    start = clock();	
    
    int size;
    IppsMD5State *ctx;
    ippsMD5GetSize(&size);
    ctx = (IppsMD5State*)malloc(size);
    unsigned char buffer[1024] = {0};
    IppStatus st = ippsMD5Init(ctx);
    while ((len = fread (buffer, 1, 1024, pFile)) > 0)
    {   
    	ippsMD5Update((const Ipp8u *)buffer, len, ctx);
    }   
	// degisting result
    static Ipp8u digest[16];
    st = ippsMD5Final(digest, ctx);
    
    //end clock
    finish = clock();
    double duration= (double)(finish - start)/CLOCKS_PER_SEC;
    printf("Time: %lf s\n",duration);
//  printf("Time: %lf s\nAvg: %lf s\n",duration, (double)(duration/num));

	//print result
    cout<<"MD5: ";
	int k=0;
	for(; k<16; k++)
	{
		cout.fill('0');
		cout.width(2);
		cout<<hex<<(int)digest[k];
	}
	
	cout<<endl;
	
	free(ctx);
    fclose(pFile); 
    return 0;
}

//g++ ipp_md5.cpp -I /opt/intel/compilers_and_libraries_2016.1.150/linux/ipp/include/  /opt/intel/compilers_and_libraries_2016.1.150/linux/ipp/lib/intel64_lin/nonpic/libippcp.a /opt/intel/compilers_and_libraries_2016.1.150/linux/ipp/lib/intel64_lin/libippcore.a

