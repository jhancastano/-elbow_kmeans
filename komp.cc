//#include "timer.hh"
//#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <omp.h>

using namespace std;


using Point = vector<double>;
using DataFrame = vector<Point>;

inline double square(double value){
	return value * value;
}

inline double squared_12_distance(const Point& first,const Point& second){
	double d = 0.0;
	for(size_t dim = 0; dim < first.size();dim++){
		d += square(first[dim]-second[dim]);
	}
	return d;
}




DataFrame k_means( const DataFrame& data, size_t k, size_t number_of_iterations, double ep,const int empty, const DataFrame Imeans){
	size_t dimensions = data[0].size();
	static random_device seed;
	static mt19937 random_number_generator(seed());
	uniform_int_distribution<size_t> indices(0,data.size()-1);/// change		  
	// pick centroids as random points from the dataset
	DataFrame means(k);// 
	double distanciaepsilon;
	int contador = 0;
	size_t epsilon = numeric_limits<size_t>::max();
//------------------asignacion de primeros cluster--------------------------	
	if(empty == 0){
	for (Point& cluster : means){ // cluster -> means
		size_t i = indices(random_number_generator);
		cluster = data[i];//cluster inicial		
		}}
	if(empty == 1)
		means = Imeans;
	vector<size_t> assignments(data.size());
	
//--------------------------ciclo de kmeans-------------------------------
	for(size_t iteration = 0; iteration < number_of_iterations; ++iteration){
		// find assignements ---- 
		#pragma omp parallel for //num_threads(8)
		for (size_t point = 0; point < data.size() ; ++point){
			double best_distance = numeric_limits<double>::max();// variable mejor distacia, inicializada con la maxima
			size_t best_cluster = 0; // variable mejor cluster, inicializada con 0
			for (size_t cluster = 0; cluster < k; cluster++){
				const double distance = squared_12_distance(data[point], means[cluster]);
				if(distance < best_distance){
				    best_distance = distance;
				    best_cluster = cluster;
					}
				}
			assignments[point] = best_cluster;
			}
		DataFrame new_means(k,vector<double>(dimensions,0.0));// means nuevo
		DataFrame new_meansaux(k,vector<double>(dimensions,0.0));//means actual
		vector<size_t> counts(k, 0);
	//----------------------- asigna cluster a los puntos----------------
		//#pragma omp parallel for
		for (size_t point = 0; point < data.size(); ++point){
		    const size_t cluster = assignments[point];
		    for(size_t d = 0; d < dimensions; d++){
		    	new_means[cluster][d] += data[point][d];
		    	}			
			counts[cluster] += 1;
			}
	//------------------- divide sumas por saltos para obtener centroides
		//#pragma omp parallel for
		for (size_t cluster = 0; cluster < k; ++cluster){
			const size_t count = max<size_t>(1, counts[cluster]);
			for(size_t d = 0; d < dimensions;d++){
				new_meansaux[cluster][d] = means[cluster][d];
				means[cluster][d] = new_means[cluster][d] / count;
				}
			distanciaepsilon = squared_12_distance(new_meansaux[cluster],means[cluster]);
			if(distanciaepsilon < ep){
				contador++;
				}
			if(distanciaepsilon > ep){
				contador--;
				}	
			}
		//-------------final de centroides nuevos---------------
	//-------------retorno si los centroides no cambian o el cambio es menor a epsilon
		if(contador == k ){
			return means;
		}
		contador = 0;
	//--------------fin retorno---------------------

	}
//----------------termina iteaciones--------------------------
	return means;
}

void writen(DataFrame data,string nombre){
	ofstream archivo;
	archivo.open(nombre,ios::out);
	if(archivo.fail()){
		cout<<"error"<<endl;
		exit(1);
	}
	for(int i=0;i< data.size();++i){
		for(int j=0; j<data[i].size();++j){
			archivo << data[i][j]<<' ';
			}
		archivo<<endl;
	}
	archivo.close();
}


DataFrame readData(string File,int nVariables ){
	DataFrame data;
	ifstream input(File);
	string line;
	while(getline(input,line)){
		istringstream iss(line);
		double v;
		Point p;
		for(int i = 0;i < nVariables; i++){
			iss >> v;
			p.push_back(v);
			}
		data.push_back(p);
		}
		cout << data.size() << endl;
		return data;
}


void imprimirkameans(vector<size_t> m,DataFrame data,int k){
	vector<int> v(k);
	for(int i = 0; i < data.size(); i++) {
  		v[m[i]]++;
  		}
  	for(int x = 0; x<v.size(); x++){
  		cout << "k_means" << x << " -> "<<v[x] <<endl;
  		}
}

void printpointmeans(DataFrame means,int nVariables){
	for(int i=0;i<means.size();i++ ){
		cout<<'(';
		for(int j=0;j<nVariables;j++)
			cout << means[i][j]<<',';
		cout <<')'<<endl;
	}
}






int main(){
	// main
	string dataset;
	int numeroVariables;
	int numeroCluster;
	int numeroIT;
	double epsilon;

	//cout << "ingrese nombre dataset"<<endl;
	//cin >>dataset;
	//cout << "ingrese numero variables dataset"<<endl;
	//cin >> numeroVariables;
	//cout << "ingrese numero de cluster o k"<<endl;
	//cin >> numeroCluster;
	//cout << "ingrese numero de iteraciones"<<endl;
	//cin >> numeroIT;
	//cout << "ingrese epsilon de convergencia ej(0.1)"<<endl;
	//cin >> epsilon;
	dataset= "arrhythmia.dat";
	numeroVariables = 279;
	numeroCluster = 40;
	numeroIT = 1000;
	epsilon = 0.001;

	DataFrame data = readData(dataset,numeroVariables);
	DataFrame means = readData("arrhythmiaMeans",numeroVariables);
	DataFrame c;
	vector<size_t> a;
	
		//ofstream archivo;
		//archivo.open("tiemposkompOP.csv",ios::out);
		//if(archivo.fail()){
		//	cout<<"error"<<endl;
		//	exit(1);
		//}
		for(int i=0;i<10;i++){
			//ScopedTimer t;
			c = k_means(data,numeroCluster,numeroIT,epsilon,0,means);
			//archivo<<t.elapsed()<<endl;
			//cout <<  " tiempo : " << t.elapsed()<< "ms" << endl;
		//printpointmeans(c,numeroVariables);
		//imprimirkameans(a,data,numeroCluster);
		}
	//tie(c,a) = kmeansOP(data,numeroCluster,numeroIT,epsilon,0,c,60);
	//printpointmeans(c,numeroVariables);
	//imprimirkameans(a,data,numeroCluster);
	
	return 0;

}