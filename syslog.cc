#include "node-syslog.h"

using namespace v8;
using namespace node;

void
Syslog::Initialize ( Handle<Object> target)
{
	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	t->SetClassName(String::NewSymbol("Syslog"));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	
	// Prototype
	NODE_SET_PROTOTYPE_METHOD(t, "log", Syslog::log);
	NODE_SET_PROTOTYPE_METHOD(t, "setMask", Syslog::setMask);
	
	Persistent<Function> constructor = Persistent<Function>::New(t->GetFunction());
	target->Set(String::NewSymbol("Syslog"), constructor);
}

Handle<Value>
Syslog::New ( const Arguments& args)
{
	HandleScope scope;

	if (args.Length() == 0 || !args[0]->IsString()) {
		return ThrowException(Exception::Error(
			String::New("Must give daemonname string as argument")));
	}
	
	if (args.Length() < 3 ) {
		return ThrowException(Exception::Error(
			String::New("Must have atleast 3 params as argument")));
	}
	
	Syslog* obj = new Syslog();
	args[0]->ToString()->WriteAscii((char*) obj->name, 0, sizeof(obj->name) - 1);
	obj->name[sizeof(obj->name) - 1] = '\0';
	obj->options = args[1]->ToInt32()->Value();
	obj->facility = args[2]->ToInt32()->Value();
	
	obj->Wrap(args.This());
	
	return args.This();
}

struct log_request {
	Persistent<Function> cb;
	char *name;
	int options;
	int facility;
	char *msg;
	uint32_t log_level;
};

static void UV_AfterLog(uv_work_t *req) {
	struct log_request *log_req = (struct log_request *)(req->data);

	log_req->cb.Dispose(); // is this necessary?
	free(log_req->msg);
	free(log_req);
	delete req;
}

static void UV_Log(uv_work_t *req) {
	struct log_request *log_req = (struct log_request *)(req->data);
	uv_mutex_lock(&Syslog::mutex);
	openlog( log_req->name, log_req->options, log_req->facility );
	syslog(log_req->log_level, "%s", log_req->msg);
	closelog();
	uv_mutex_unlock(&Syslog::mutex);
	return;
}

Handle<Value>
Syslog::log ( const Arguments& args)
{
	HandleScope scope;
	Local<Function> cb = Local<Function>::Cast(args[3]);
	
	struct log_request * log_req = (struct log_request *)
		calloc(1, sizeof(struct log_request));
	
	if(!log_req) {
		V8::LowMemoryNotification();
		return ThrowException(Exception::Error(
			String::New("Could not allocate enought memory")));
	}
	
	String::AsciiValue msg(args[1]);
	uint32_t log_level = args[0]->Int32Value();
	
	log_req->cb = Persistent<Function>::New(cb);
	
	Syslog* obj = ObjectWrap::Unwrap<Syslog>(args.This());
	log_req->name = obj->name;
	log_req->options = obj->options;
	log_req->facility = obj->facility;
	log_req->msg = strdup(*msg);
	log_req->log_level = log_level;

	uv_work_t *work_req = new uv_work_t();
	work_req->data = log_req;
	uv_queue_work(uv_default_loop(), work_req, UV_Log, UV_AfterLog);

	return scope.Close(Undefined());
}

Handle<Value>
Syslog::destroy ( const Arguments& args)
{
	Syslog* obj = ObjectWrap::Unwrap<Syslog>(args.This());
	free(obj->name);
	return Undefined();
}

Handle<Value>
Syslog::setMask ( const Arguments& args)
{
	bool upTo = false;
	int mask, value, ret;
	HandleScope scope;
	
	if (args.Length() < 1) {
		return ThrowException(Exception::Error(String::New("You must provide an mask")));
	}
	
	if (!args[0]->IsNumber()) {
		return ThrowException(Exception::Error(String::New("First parameter (mask) should be numeric")));
	}
	
	if (args.Length() == 2 && !args[1]->IsBoolean()) {
		return ThrowException(Exception::Error(String::New("Second parameter (upTo) should be boolean")));
	}
	
	if (args.Length() == 2 && args[1]->IsBoolean()) {
		upTo = true;
	}
	
	value = args[0]->Int32Value();
	if(upTo) {
		mask = LOG_UPTO(value);
	} else {
		mask = LOG_MASK(value);
	}
	
	uv_mutex_lock(&Syslog::mutex);
	ret = setlogmask(mask);
	uv_mutex_unlock(&Syslog::mutex);
	return scope.Close(Integer::New( ret ));
}

// lock
Syslog::mutex_init::mutex_init() {
	uv_mutex_init(&mutex);
}
uv_mutex_t Syslog::mutex;
Syslog::mutex_init Syslog::mutex_initializer;

NODE_MODULE(syslog, Syslog::Initialize);
