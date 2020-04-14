#include <iostream>
#include <pthread.h>

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


int main()
{
	cout << "enter X0, a, c, m, N" << endl;

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
 	delete []res;

	cout << "Prog finish";
	return 0;
}
