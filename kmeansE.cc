//clang++ -O3 -o d -Xpreprocessor -fopenmp -lomp -lzmq pp.cc
//clang++ -std=c++11 -O3 -o d -Xpreprocessor -fopenmp -lomp -lzmqpp -lzmq kmeansE.cc



#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include <iostream>
#include  <zmqpp/zmqpp.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <omp.h>

using namespace rapidjson;
using namespace zmqpp;
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

inline double distanciakmeans(DataFrame means,DataFrame data,vector<size_t> v){
	double distance = 0.0;
	for(int i=0;i<v.size();++i){
		distance += sqrt(squared_12_distance(data[i],means[v[i]]));
	}
	return distance/v.size();
}


pair<DataFrame,vector<size_t>> k_means( const DataFrame& data, size_t k, size_t number_of_iterations, double ep,const int empty, const DataFrame Imeans){
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
	size_t iteration = 0;
	while( (iteration<number_of_iterations) && (contador != k)){
		// find assignements ---- 
		#pragma omp parallel for num_threads(16)
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
		for (size_t point = 0; point < data.size(); ++point){
		    const size_t cluster = assignments[point];
		    for(size_t d = 0; d < dimensions; d++){
		    	new_means[cluster][d] += data[point][d];
		    	}			
			counts[cluster] += 1;
			}
	//------------------- divide sumas por saltos para obtener centroides
		contador = 0;
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
			}
		//-------------final de centroides nuevos---------------
		
		
		++iteration;
	}
//----------------termina iteaciones--------------------------
	return {means, assignments};
}

pair<DataFrame,vector<size_t>> kmeansOP( const DataFrame& data, size_t k, size_t number_of_iterations, double ep,const int empty, const DataFrame Imeans,int porcentaje){
	int dimensions = data[0].size();
	int DataAUX = data.size()*porcentaje/100;
	static random_device seed;
	static mt19937 random_number_generator(seed());
	uniform_int_distribution<size_t> indices(0,data.size()-1);/// change		  
	DataFrame datos(DataAUX,vector<double>(dimensions,0.0));

	for(int i=0;i<DataAUX;i++){
		size_t j = indices(random_number_generator);
		datos[i] = data[j];
	}
	DataFrame c;
	vector<size_t> a;
	tie(c,a) = k_means(datos,k,number_of_iterations,ep,0,datos);
	//imprimirkameans(a,data,numeroCluster)
	//printpointmeans(c,dimensions);
	//printpointmeans(c,2);
	//writen(c,"arrhythmiaMeans");
	tie(c,a) = k_means(data,k,number_of_iterations,ep,1,c);
	return{c,a};

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
 int  main ()
	{
	string id;
	cout<<"ingrese id"<<endl;
	cin>>id;
	// 1. Parse a JSON string into DOM.
    const char* json = "{\"op\":\"reg\"}";
    Document d;
    d.Parse(json);
    // 2. Modify it by DOM.
    Value& s = d["op"];
    //cout<<s.GetString()<<endl;
    //s.SetInt(s.GetInt() + 1);
    // 3. Stringify the DOM
    //StringBuffer buffer;
    //Writer<StringBuffer> writer(buffer);
    //d.Accept(writer);
    //Output {"project":"rapidjson","stars":11}
    //cout << buffer.GetString() << std::endl;

    context ctx;
    socket sok(ctx,socket_type::dealer);
    sok.set(socket_option::identity,id);
    //sok.identity("hol"); 
    sok.connect("tcp://localhost:4443");

    
    poller poll;
    poll.add(sok,ZMQ_POLLIN);
    //cout<< sok.get_identity();

    message mensaje;


 //    //cout <<strlen(json)<<endl;
 //    //worker.send(json,12,ZMQ_SNDMORE);
 //    int sizeM = strlen(json);
     	mensaje << json;
 		sok.send(mensaje);
 		//cout << mensaje;
    
 while(1){
     	message response;
   		int more;
  		//poll.has(sok);
        if (poll.has(sok)) {
             cout << "h1"<<endl;
             sok.receive(response);
             size_t more_size = sizeof (more);
             //worker.getsockopt(ZMQ_RCVMORE, &more, &more_size);	                           
        	const char* json1;
        	response >> json1;
         	Document doc;
     		doc.Parse(json1);
     		Value& s1 = doc["op"];
     		//cout<<s1.GetString()<<endl;
        
     		if(s1=="reg"){
     			s1.SetString("dep");
     			StringBuffer buffer;
     			Writer<StringBuffer> writer(buffer);
     			doc.Accept(writer);
     			cout<<buffer.GetString()<<endl;
    			mensaje << buffer.GetString();
     			sok.send(mensaje);

     		}
     		if(s1=="sendworker"){

     			Value& distan = doc["distancias"];
     			Value& initial = doc["inicial"];
     			Value& final = doc["final"];
     			Value& dat = doc["dataset"];
     			Value& nvar = doc["nvariables"];
     			cout << dat.GetString()<<endl;
     			string dataset= dat.GetString();
	 			int numeroVariables = nvar.GetInt();
	 			int numeroIT = 1000;
	 			double epsilon = 0.01;
	 			DataFrame data = readData(dataset,numeroVariables);
	 			DataFrame means;
	 			double dis;
	 			DataFrame c;
	 			vector<size_t> a;	
	 				for(int i=initial.GetInt();i<=final.GetInt();i++){
	 					cout << i;
	 					tie(c,a) = kmeansOP(data,i,numeroIT,epsilon,0,means,30);
	 					//c = k_means(data,i,numeroIT,epsilon,0,means);		
	 					dis = distanciakmeans(c,data,a);
	 					double punto[2]= {(double)i,dis};
						
	 					cout << dis <<endl;
	 					distan.PushBack(Value().SetDouble(dis), doc.GetAllocator());
	 					//distan.PushBack(GenericValue().SetArray(punto), doc.GetAllocator());
	 					}

	 			s1.SetString("finishwork");
	 			StringBuffer buffer;
     			Writer<StringBuffer> writer(buffer);
     			doc.Accept(writer);
     			cout<<buffer.GetString()<<endl;
     			mensaje <<buffer.GetString();
     			sok.send(mensaje);
     		}
    			
     		else
     			cout<<json1<<endl;
        	
         }
        
        


     }
    
   	return 0;
}
   