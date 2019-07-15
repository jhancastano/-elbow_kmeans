
//clang++ -O3 -o d -Xpreprocessor -fopenmp -lomp -lzmq pp.cc
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include <iostream>
#include  "zmq.hpp"

using namespace rapidjson;
using namespace std;

 int  main ()
	{
	// 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);
    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);
    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    //Output {"project":"rapidjson","stars":11}
    cout << buffer.GetString() << std::endl;

	zmq::context_t context(1);
    zmq::socket_t worker(context, ZMQ_DEALER);
    worker.setsockopt(ZMQ_IDENTITY, "B", 1);  
    worker.connect("tcp://localhost:4443");

    zmq::pollitem_t items [] = {
        { static_cast<void*>(worker), 0, ZMQ_POLLIN, 0 },
    };
    zmq::message_t message;

    worker.send("{\"op\":\"reg\"}",12,ZMQ_SNDMORE);
    worker.send("{\"op\":\"reg\"}",12,0);

    
    //worker.recv(&message,12);
    //cout<< message;


    zmq::message_t reply;
	//worker.recv(&reply,12);
	//string rpl = string(static_cast<char*>(reply.data()), reply.size());
	//cout << rpl << endl;

    
    while(1){
    	zmq::message_t message;
    	int more;
    	zmq::poll (&items[0], 1, -1);

    	//cout<<"jsd"<<endl;
        if (items[0].revents & ZMQ_POLLIN) {
            cout<<"h1"<<endl;
            bool mensaje = worker.recv(&message);
            size_t more_size = sizeof (more);
            worker.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        	cout<<message<<endl;
        	cout<<"h2"<<endl;
            //worker.recv(&message);
            //  Process task            
        }


       
    }
   	return 0;
}
   