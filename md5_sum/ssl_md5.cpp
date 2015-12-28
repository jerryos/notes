#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{

 if(argc <=1)
    {
        cout<<"Usage: commamd argv[1]"<<endl;
        exit(-1);
    }

//    FILE *pFile = fopen ("test.db", "rb");
    FILE *pFile = fopen (argv[1], "rb");


	MD5_CTX ctx;
	int len = 0;
	unsigned char buffer[1024] = {0};
	unsigned char digest[16] = {0};

	// start clock
	clock_t start, finish;
 	start = clock();
	
	MD5_Init (&ctx);

	while ((len = fread (buffer, 1, 1024, pFile)) > 0)
	{
		MD5_Update (&ctx, buffer, len);
	}

	MD5_Final (digest, &ctx);

	//end clock
	finish = clock();
 	double duration= (double)(finish - start)/CLOCKS_PER_SEC;
 	printf("Time: %lf s\n",duration);
// 	printf("Time: %lf s\nAvg: %lf s\n",duration, (double)(duration/num));

	
	fclose(pFile);
	

	int i = 0;
	char buf[33] = {0};
    char tmp[3] = {0};
    for(i = 0; i < 16; i++ )
	{
		sprintf(tmp,"%02x", digest[i]); 
		strcat(buf, tmp);
    }

		
	cout <<"MD5: "<< buf << endl;

    return 0;
}


// g++ ssl_md5.cpp -o ssl_md5  -lcrypto -lssl
