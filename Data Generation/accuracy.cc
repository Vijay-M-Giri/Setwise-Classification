#include <bits/stdc++.h>

using namespace std;

const int N = 300;
const int E = 1500;

map<int,int> mp;

int main(){
	FILE* fp = fopen("Data Generation/entity_class.data","r");
	for(int i=0;i<E;i++){
		int a,b;
		fscanf(fp,"%d %d",&a,&b);
		mp[a] = b;
	}
	fclose(fp);
	fp = fopen("Data Generation/result.data","r");
	int cnt = 0;
	for(int i=0;i<N;i++){
		int a,b;
		fscanf(fp,"%d %d",&a,&b);
		if(mp[a] == b)
			cnt++;
	}
	cout << "count : " << cnt << endl;
	double accuracy = 100.0 * cnt / N ;
	printf("Accuracy : %lf \n",accuracy);
	return 0;
}
