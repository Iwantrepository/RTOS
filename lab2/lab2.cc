#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <limits.h>
#include <bitset>
#include <time.h>

#define STR_LEN 100

using namespace std;

pthread_barrier_t barrier;

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

void* xoring (void* arg)
{
	bar_data * b = (bar_data*)arg;
	int term;
	for(int i = 0; b->text[i] != '\0'; i++)
	{
		b->res[i] = (b->text[i])^(b->key[i]);
		//cout << b->res[i];
		term = i;
	}
	b->res[term+1] = '\0';
	//cout << endl;

	//sleep(1);

	pthread_barrier_wait(&barrier);

	return NULL;
}


int main( int argc, char* argv[] )
{
	cout << "Prog start" << endl;
	int c, errflag = 0;
	char buff_k[STR_LEN];
	char buff_i[STR_LEN];
	char s_k[256], s_i[256], s_o[256];

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
							fclose(fp_k);
							fp_k = fopen(optarg, "wb");
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
	        				fp_o = fopen(optarg, "wb");
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
	    dt.N = text_len;


	    LKG lkg(dt);
	    int* res = lkg.start_thread();

	    for(int i=0; i<dt.N; i++)
	    {
	    	cout << (int)(res[i]%256) << " ";
	    	buff_k[i] = (char)(res[i]%256);
	    }
	    buff_k[dt.N] = '\0';
	    cout << endl;



	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	    cout << "Num of CPUs " << numCPU << endl;
//numCPU = 5;


	pthread_t * threads;
	threads = new pthread_t[numCPU];
	bar_data * args;
	args = new bar_data[numCPU];

	int status = pthread_barrier_init(&barrier, NULL, numCPU+1);
	cout << "Bar init " << ((status == 0)?"Yes":"No") << endl;

	int sym_lim = (text_len / numCPU) + 1;

	for(int i = 0; i < numCPU; i++)
	{
		for(int j = 0; (j < sym_lim)&&(buff_i[i*sym_lim + j] != '\0') ; j++)
		{
			args[i].text[j] = buff_i[i*sym_lim + j];
			args[i].key[j] = buff_k[i*sym_lim + j];
		}
	}

	for(int i = 0; i < numCPU; i++)
	{
		status = pthread_create(&threads[i], NULL, xoring, (void*) &args[i]);
		cout << "Pthread [" << i <<"] init - " << ((status == 0)?"Yes":"No") << endl;
	}

	status = pthread_barrier_wait(&barrier);
	cout << "Main bar wait - " << ((status == 0)?"Yes":"No") << endl;

	for (int i=0; i < numCPU; i++) {
	    pthread_join(threads[i], NULL);
	}


	for(int i = 0; i < numCPU; i++)
		{
			for(int j = 0; (j < sym_lim)&&(args[i].res[j] != '\0') ; j++)
			{
				cout << args[i].res[j];
				s_o[i*sym_lim + j] = args[i].res[j];
				s_o[i*sym_lim + j+1] = '\0';
			}
		}
	cout << endl;

	bar_data bdt;

	status = pthread_barrier_destroy(&barrier);
	cout << "Bar destroy - " << ((status == 0)?"Yes":"No") << endl;


	fprintf(fp_k, "%s", buff_k);
	fprintf(fp_o, "%s", s_o);

	fclose(fp_k);
	fclose(fp_o);

	cout << "Prog finish " << bitset<8>(('a'^'x')^'x') << endl;
	return 0;
}
