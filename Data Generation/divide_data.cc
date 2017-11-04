#include <bits/stdc++.h>

using namespace std;

const int N = 581012;
const int M = 100000;
const int D = 55;

set<int> s;
vector<int> data[N+5];

int main(){
	srand(time(NULL));
	FILE* fp = fopen("setwise_covtype.data","r");
	while(s.size() < 300){
		int r = rand() % 1500 + 1;
		s.insert(r);
	}
	for(int i=0;i<N;i++){
		for(int j=0;j<=D;j++){
			int a;
			fscanf(fp,"%d",&a);
			data[i].push_back(a);
		}
	}
	FILE* fw = fopen("initial_training.data","w");
	FILE* sm = fopen("stream.data","w");
	FILE* tst = fopen("test_entities.data","w");
	fprintf(fw,"%d %d 7 1500\n",M,D+1);
	int cnt = 0;
	for(int i=0;i<N && cnt < M;i++) if(s.count(data[i][D]) == 0){
		cnt++;
		for(int j=0;j<D;j++){
			fprintf(fw,"%d ",data[i][j]);
		}
		fprintf(fw,"%d\n",data[i][D]);
	}
	for(int i=cnt;i<N;i++){
		for(int j=0;j<D-1;j++){
			fprintf(sm,"%d ",data[i][j]);
		}
		if(s.count(data[i][D]))
			fprintf(sm,"-1 ");
		else 
			fprintf(sm,"%d ",data[i][D-1]);
		fprintf(sm,"%d\n",data[i][D]);
	}
	for(int it : s) 
		fprintf(tst,"%d\n",it);
	fclose(fp);
	fclose(fw);
	fclose(sm);
	fclose(tst);
	return 0;
}
