#include <iostream>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

int main(){
	const gsl_rng_type * T;
	gsl_rng *random_gen;
	gsl_rng_env_setup();
	T = gsl_rng_rand48;
	random_gen = gsl_rng_alloc(T);
	double mu = 1.0;
	int N = 100000000;
	FILE* fp = fopen("inter_arrival_time.data","w");
	for(int i=0;i<N;i++){
		double time = gsl_ran_exponential( random_gen , mu);
		fprintf(fp,"%lf\n",time);
	}
	fclose(fp);
	return 0;
}
