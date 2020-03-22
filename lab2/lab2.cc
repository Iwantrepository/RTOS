#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define STR_LEN 100

using namespace std;

struct data{

	int X0;
	int a;
	int c;
	int m;
	int N;
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
	char buff[STR_LEN];
	FILE *fp;
	    while( ( c = getopt( argc, argv, "k:i:o:" ) )
	      != -1 ) {
	      switch( c ) {
	        case 'k': printf( "otp -> %s\n", optarg );
							fp = fopen(optarg, "rb");
							if(!fp)
								break;

							fgets(buff, STR_LEN, fp);
							printf("%s", buff);
							fclose(fp);
							break;
	        case 'i': printf( "input -> %s\n", optarg );
	        				fp = fopen(optarg, "rb");
	        	        	if(!fp)
	        	        		break;

	        	        	fgets(buff, STR_LEN, fp);
	        	        	printf("%s", buff);
	        	        	fclose(fp);
	        	        	break;
	        case 'o': printf( "output -> %s\n", optarg );
	        				fp = fopen(optarg, "rb");
	        	        	if(!fp)
	        	        		break;

	        	        	fgets(buff, STR_LEN, fp);
	        	        	printf("%s", buff);
	        	        	fclose(fp);
	        	        	break;
	        case '?': ++errflag;
	                  break;
	      }
	    }

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	    cout << "Num of CPUs " << numCPU << endl;

	pthread_barrier_t * barrier;
	int bar_start = pthread_barrier_init(barrier, NULL, numCPU);
	cout << "bar_start" << bar_start << endl;
	bar_start = pthread_barrier_destroy(barrier);
	cout << "bar_start" << bar_start << endl;


	data dt;

	dt.X0 = 7;
	dt.a = 7;
	dt.c = 7;
	dt.m = 10;
	dt.N = 15;


	LKG lkg(dt);
	int* res = lkg.start_thread();

	for(int i=0; i<dt.N; i++)
		cout << res[i] << " ";
	cout << endl;


	cout << "Prog finish";
	return 0;
}
