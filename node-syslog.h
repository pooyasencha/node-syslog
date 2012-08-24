#ifndef syslog_h
#define syslog_h

#include <node.h>
#include <node_object_wrap.h>
#include <node_version.h>
#include <v8.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

namespace node {


class Syslog : ObjectWrap {
    public:
	static void Initialize ( v8::Handle<v8::Object> target);
	static void send(char *name, int options, int facility, uint32_t log_level, char *msg);
	    
    protected:
	static v8::Persistent<v8::Function> constructor;
	static v8::Handle<v8::Value> setAsync (const v8::Arguments& args);
	static v8::Handle<v8::Value> New (const v8::Arguments& args);
	static v8::Handle<v8::Value> log (const v8::Arguments& args);
	static v8::Handle<v8::Value> setMask (const v8::Arguments& args);
	static v8::Handle<v8::Value> destroy (const v8::Arguments& args);
	
	Syslog () : ObjectWrap() {
	};
	
	~Syslog (){};


    private:
	static bool async;
	
	char name[1024];
	int options;
	int facility;


    // lock
    public:
	static uv_mutex_t mutex;
    private:
	static class mutex_init	{
	    public:
		mutex_init();
	} mutex_initializer;

};

}  // namespace node
#endif // syslog_h
