#include <bits/stdc++.h>

using namespace std;

const int N = 581012;
const int D = 55;

vector<int> data[N+5];
pair<double,double> musigma[D+5];

void getMuSigma(int id){
	double mu = 0.0 , sigma = 0.0;
	for(int i=0;i<N;i++){
		mu += data[i][id];
	}
	mu /= N;
	for(int i=0;i<N;i++)
		sigma += (data[i][id] - mu) * (data[i][id] - mu);
	sigma /= N;
	sigma = sqrt(sigma);
	musigma[id] = {mu,sigma};
}

int main(){
	FILE* fp = fopen("setwise_covtype.data","r");
	for(int i=0;i<N;i++){
		for(int j=0;j<=D;j++){
			int a; 
			char c;
			fscanf(fp,"%d%c",&a,&c);
			data[i].push_back(a);
		}
	}
	fclose(fp);
	fp = fopen("normalized.data","w");
	for(int j=0;j<D;j++) getMuSigma(j);
	for(int i=0;i<N;i++){
		fprintf(fp,"%d ",data[i][0]);
		for(int j=1;j<10;j++){
			double temp = (data[i][j] - musigma[j].first) 
			/ musigma[j].second;
			fprintf(fp,"%lf ",temp);
		}
		for(int j=10;j<D;j++)
			fprintf(fp,"%d ",data[i][j]);
		fprintf(fp,"%d\n",data[i][D]);
	}
	fclose(fp);
	return 0;
}
