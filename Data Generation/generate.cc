#include <bits/stdc++.h>

using namespace std;

const int N = 581012;
const int D = 55;

vector<int> data[N+5];
int bin[N+5];
double buf[] = {-1.3,-0.7,-0.3,-0.1,0,0.1,0.3,0.7,1.3};
int cnt[12] , cur[12] , sum[12];
map<int,int> mp;

int main(){
	FILE* fp = fopen("covtype.data","r");
	double mu, sigma;
	mu = sigma = 0.0;
	for(int i=0;i<N;i++){
		for(int j=0;j<D;j++){
			int a; 
			char c;
			fscanf(fp,"%d%c",&a,&c);
			data[i].push_back(a);
		}
		mu += data[i][0];
	}
	mu /= N;
	for(int i=0;i<N;i++){
		sigma += (data[i][0] - mu) * (data[i][0] - mu);
	}
	sigma /= N;
	sigma = sqrt(sigma);
	cout << mu << ' ' << sigma << endl;
	for(int i=0;i<N;i++){
		int id = -1;
		for(int j=0;j<8;j++){
			if(data[i][0] >= mu + buf[j] * sigma && data[i][0] < mu + buf[j+1] * sigma){
				id = j+1;
				break;
			}
		}
		if(id == -1){
			if(data[i][0] < mu + buf[0] * sigma) id = 0;
			else id = 9;
		}
		bin[i] = id;
		cnt[id]++;
	}
	for(int i=0;i<9;i++) cout << cnt[i] << ' ';
	cout << endl;
	fclose(fp);
	fp = fopen("setwise_covtype.data","w");
	for(int i=0;i<N;i++){
		int entity = cur[bin[i]] * 150.0 / cnt[bin[i]];
		if(entity >= 150) entity = 149;
		cur[bin[i]]++;
		entity = 150 * bin[i] + entity + 1;
		data[i].push_back(entity);
	}
	random_shuffle(data,data+N);
	for(int i=0;i<N;i++){
		for(int j=0;j<D;j++)
			fprintf(fp,"%d ",data[i][j]);
		fprintf(fp,"%d\n",data[i][D]);
		mp[data[i][D]] = data[i][D-1];
	}
	fclose(fp);
	fp = fopen("entity_class.data","w");
	for(auto it : mp){
		fprintf(fp,"%d %d\n",it.first,it.second);
	}
	fclose(fp);
	return 0;
}
