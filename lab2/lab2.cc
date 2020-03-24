#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <limits.h>
#include <bitset>

#define STR_LEN 100

using namespace std;

struct data{

	unsigned int X0;
	unsigned int a;
	unsigned int c;
	unsigned int m;
	unsigned int N;
};

struct bar_data{
	char key[256];
	char text[256];
	char res[256];
};


class LKG
{
public:
	LKG(data par) : tid(0), param(par){}
	~LKG(){}

private:

	static void* calc(void *temp)
	{
		LKG* cur = (LKG*) temp;
		return cur->calculation(cur);
	}

	void* calculation(LKG* cur)
        {
		data curPar = cur->getParam();
                int *rez = new int [curPar.N];
                rez[0] = curPar.X0;
                for (int i = 1; i < curPar.N; i++)
                {
                        rez[i] = (curPar.a*curPar.X0 + curPar.c) % curPar.m;
                	curPar.X0 = rez[i];
		}

                return rez;
        };

public:
	int* start_thread()
	{
		void *per;
		int *sum;
		pthread_create(&tid, NULL, calc, this);
		int buf;
		buf = pthread_join(tid, &per);

		cout << "pthread_join returned: " << buf << endl;

		sum = (int*)per;

		return sum;
	};

	data getParam()
	{
		return param;
	}



private:
	pthread_t tid;
	data param;

};


int main( int argc, char* argv[] )
{
	cout << "Prog start" << endl;
	int c, errflag = 0;
	char buff_k[STR_LEN];
	char buff_i[STR_LEN];
	string s_k, s_i, s_o;

	int text_len;

	FILE *fp_k, *fp_i, *fp_o;
	    while( ( c = getopt( argc, argv, "k:i:o:" ) )
	      != -1 ) {
	      switch( c ) {
	        case 'k': printf( "otp -> %s\n", optarg );
							fp_k = fopen(optarg, "rb");
							if(!fp_k)
								return 0;

							fgets(buff_k, STR_LEN, fp_k);
							printf("%s", buff_k);
							break;
	        case 'i': printf( "input -> %s\n", optarg );
	        				fp_i = fopen(optarg, "rb");
	        	        	if(!fp_i)
	        	        		return 0;

	        	        	fgets(buff_i, STR_LEN, fp_i);
	        	        	text_len = strlen(buff_i);
	        	        	buff_i[text_len-1] = '\0';
	        	        	printf("%s - (%d)\n", buff_i, text_len);
	        	        	fclose(fp_i);
	        	        	break;
	        case 'o': printf( "output -> %s\n", optarg );
	        				fp_o = fopen(optarg, "rb");
	        	        	if(!fp_o)
	        	        		return 0;
	        	        	break;
	        case '?': ++errflag;
	                  break;
	      }
	    }

	    data dt;

	    dt.X0 = 1;
	    dt.a = 1664525;
	    dt.c = 1013904223;
	    dt.m = INT_MAX;
	    dt.N = text_len*8;


	    LKG lkg(dt);
	    int* res = lkg.start_thread();

	    for(int i=0; i<dt.N; i++)
	    	cout << res[i]%2 << "";
	    cout << endl;



	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	    cout << "Num of CPUs " << numCPU << endl;

	numCPU = 1;

	static pthread_barrier_t barrier;
	int status = pthread_barrier_init(&barrier, NULL, numCPU);
	cout << "Bar init " << status << endl;

	bar_data bdt;

	status = pthread_barrier_wait(&barrier);
	cout << "Bar wait " << status << endl;
	status = pthread_barrier_destroy(&barrier);
	cout << "Bar destroy " << status << endl;


	cout << "Prog finish " << /*bitset<8>*/(char)(('a'^'x')^'x') << endl;

	fclose(fp_k);
	fclose(fp_o);
	return 0;
}
