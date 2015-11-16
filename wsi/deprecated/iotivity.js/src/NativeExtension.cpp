#include <node.h>

void findResourceFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void observeValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void cancelObserveValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void postValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void putValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void getValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void deleteValueFunc(const v8::FunctionCallbackInfo<v8::Value>& info);
void init_iotivity();

void Init(v8::Local<v8::Object> exports) {
    NODE_SET_METHOD(exports, "findResource", findResourceFunc);
    NODE_SET_METHOD(exports, "observe", observeValueFunc);
    NODE_SET_METHOD(exports, "cancelObserve", cancelObserveValueFunc);
    NODE_SET_METHOD(exports, "post", postValueFunc);
    NODE_SET_METHOD(exports, "put", putValueFunc);
    NODE_SET_METHOD(exports, "get", getValueFunc);
    NODE_SET_METHOD(exports, "delete", deleteValueFunc);


    init_iotivity();
}

NODE_MODULE(NativeExtension, Init)
