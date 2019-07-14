
//clang++  -std=c++11 -O3 -o d -Xpreprocessor -fopenmp -lomp -lzmq pp.cc
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include <iostream>
#include  "zmq.hpp"

using namespace rapidjson;


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
    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
    return 0;

	zmq::context_t context(1);
    zmq::socket_t worker(context, ZMQ_DEALER);
    worker.setsockopt(ZMQ_IDENTITY, "B", 1);  
    worker.connect("tcp://localhost:4444");
	
    
    //worker.send("{\"operacion\":\"registrar\"}",25,ZMQ_SNDMORE);
    //worker.send("{\"operacion\":\"registrar\"}",25,0);

    
}
   