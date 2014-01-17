#include "jsapi.h"
#include <sstream>
#include <iostream>
#include <jsperf.h>
#include <list>
#include <time.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/appender.h>
#include <log4cplus/consoleappender.h>



JSBool compileAndRepeat(JSContext *cx, JSObject *global,const char * script,const char *filename);

/* The class of the global object. */
static JSClass global_class = { "global",
	JSCLASS_NEW_RESOLVE | JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	NULL,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
	fprintf(stderr, "%s:%u:%s\n",
		report->filename ? report->filename : "<no filename="">",
		(unsigned int) report->lineno,
		message);
}

int run(JSContext *cx) {
	/* Enter a request before running anything in the context */
	JSAutoRequest ar(cx);

	/* Create the global object in a new compartment. */
	JSObject *global = JS_NewCompartmentAndGlobalObject(cx, &global_class,NULL);
	if (global == NULL)
		return 1;

	/* Set the context's global */
	JSAutoEnterCompartment ac;
	ac.enter(cx, global);
	JS_SetGlobalObject(cx, global);

	/* Populate the global object with the standard globals, like Object and Array. */
	if (!JS_InitStandardClasses(cx, global))
		return 1;
	if (!JS::RegisterPerfMeasurement(cx, global))
		return 1 ;
	JS_WrapObject(cx,&global);
	/* Your application code here. This may include JSAPI calls to create your own custom JS objects and run scripts. */
	const char *script = "x=(function(a, b){return a * b;})(15, 6);"; 
	jsval rval; 
	JS_EvaluateScript(cx, global, script, strlen(script), NULL, 0, &rval); 
	script = "\"value=\"+x;";
	for(long i=0; i<20;i++){
		//JSBool status = compileAndRepeat(cx,global,script,"");
		JSBool status = JS_EvaluateScript(cx, global, script, strlen(script), NULL, 0, &rval); 

		if (status == JS_TRUE){ 
			JSString *d; 
			//JS_NewExternalString(cx,);
			d =JS_ValueToString(cx, rval); 
			
			int length  = JS_GetStringEncodingLength(cx,d);
			char* bytes ;//=  new char[length+1];
			bytes = JS_EncodeString(cx,d);
			//JS_EncodeStringToBuffer(d,bytes,length);
			//bytes[length] =0;

			//std::cout << "the result=" << bytes <<std::endl;
			//delete[] bytes;
			JS_free(cx,bytes);
			
		} 
	}
	return 0;
}

log4cplus::Logger loger;
int main(int argc, const char *argv[]) {
	log4cplus::initialize();
	log4cplus::SharedAppenderPtr _append(new log4cplus::ConsoleAppender());
	_append->setName("append test");
	std::string pattern = "%d{%m/%d/%y %H:%M:%S}  - %m [%l]%n";    
	//log4cplus::Layout _layout(new PatternLayout(pattern));   
	/* step 3: Attach the layout object to the appender */    
	//_append->setLayout( _layout );   
	loger= log4cplus::Logger::getInstance("console");
	loger.addAppender(_append);

	/* Create a JS runtime. */
	JSRuntime *rt = JS_NewRuntime(320L * 1024L * 1024L);
	if (rt == NULL)
		return 1;
	std::list<JSContext *> contexts;

	time_t nowtime;
	nowtime=time(NULL);//��ȡ����ʱ��??
	std::cout<<nowtime<<std::endl;//���nowtim

	for (long i =0 ;i < 100; i++)
	{
		/* Create a context. */
		JSContext *cx = JS_NewContext(rt, 8192);
		if (cx == NULL)
			return 1;
		JS_SetOptions(cx, JSOPTION_VAROBJFIX);
		JS_SetErrorReporter(cx, reportError);

		int status = run(cx);
		contexts.push_front(cx);
		//JS_GC(cx);
	}

	nowtime=time(NULL);//��ȡ����ʱ��??
	std::cout<< "100:" << nowtime<<std::endl;//���nowtim
	for (std::list<JSContext *>::iterator it = contexts.begin(); it != contexts.end();it++)
	{
		//JS_GC(*it);
	}

	nowtime=time(NULL);//��ȡ����ʱ��??
	std::cout<< "GC:" << nowtime<<std::endl;//���nowtim

	while(contexts.size()>0){
		 nowtime=time(NULL);//��ȡ����ʱ��??
		 std::cout<< "DestroyContext"<<nowtime<<std::endl;//���nowtim
		 JS_DestroyContext(contexts.front());
		 contexts.pop_front();
	 }
	 nowtime=time(NULL);//��ȡ����ʱ��??
	 std::cout<<nowtime<<std::endl;//���nowtim

	 for (long i =0 ;i < 1; i++)
	 {
		 /* Create a context. */
		 JSContext *cx = JS_NewContext(rt, 8192);
		 if (cx == NULL)
			 return 1;
		 JS_SetOptions(cx, JSOPTION_VAROBJFIX);
		 JS_SetErrorReporter(cx, reportError);

		 int status = run(cx);
		 contexts.push_front(cx);
		 JS_GC(cx);

	 }

	 nowtime=time(NULL);//��ȡ����ʱ��??
	 std::cout<<nowtime<<std::endl;//���nowtim

	 while(contexts.size()){
		 JS_DestroyContext(contexts.front());
		 contexts.pop_front();
	 }
	 nowtime=time(NULL);//��ȡ����ʱ��??
	 std::cout<<nowtime<<std::endl;//���nowtim

	 int i;
	 std::cin >> i;

	 JS_DestroyRuntime(rt);
	 JS_ShutDown();
	
	
	 return 0;
}

/*
 * Compile a script and execute it repeatedly until an
 * error occurs.  (If this ever returns, it returns false.
 * If there's no error it just keeps going.)
 */
JSBool compileAndRepeat(JSContext *cx, JSObject *global,const char * content,const char *filename)
{
    JSObject *scriptObj;

	scriptObj = JS_CompileScript(cx, global, content,strlen(content),filename,0);
	if (scriptObj == NULL)
        return JS_FALSE;   /* compilation error */

	
    if (!JS_AddNamedObjectRoot(cx, &scriptObj, "compileAndRepeat script object"))
        return JS_FALSE;

    for (long i =0; i < 10; i++) {
        jsval result;
        if (!JS_ExecuteScript(cx, global, scriptObj, &result))
            break;
        JS_MaybeGC(cx);
    }

    JS_RemoveObjectRoot(cx, &scriptObj);  /* scriptObj becomes unreachable
                                             and will eventually be collected. */
    return JS_FALSE;
}