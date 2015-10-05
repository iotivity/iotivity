#include <nan.h>

std::string findResource(std::string resType);
std::string postResource(std::string resourceId,std::string property,std::string value);

void findResourceFunc(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if(info.Length() < 2) {
        Nan::ThrowTypeError("Wrong number of arguments");
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsFunction()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    v8::String::Utf8Value arg0(info[0]->ToString());
    std::string resType = std::string(*arg0);
    v8::Local<v8::Function> callbackHandle = info[1].As<v8::Function>();

    std::string result = findResource(resType);

    v8::Local<v8::Value> argv[] = { Nan::New(result).ToLocalChecked() };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callbackHandle, 1, argv);
}

void postResourceFunc(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if(info.Length() < 4) {
        Nan::ThrowTypeError("Wrong number of arguments");
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString() || !info[3]->IsFunction()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    v8::String::Utf8Value arg0(info[0]->ToString());
    std::string resourceId = std::string(*arg0);
    v8::String::Utf8Value arg1(info[1]->ToString());
    std::string property = std::string(*arg1);
    v8::String::Utf8Value arg2(info[2]->ToString());
    std::string value = std::string(*arg2);

    v8::Local<v8::Function> callbackHandle = info[3].As<v8::Function>();

    std::string result = postResource(resourceId,property,value);

    v8::Local<v8::Value> argv[] = { Nan::New(result).ToLocalChecked() };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callbackHandle, 1, argv);
}

void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("findResource").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(findResourceFunc)->GetFunction());
    exports->Set(Nan::New("postResource").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(postResourceFunc)->GetFunction());
}

NODE_MODULE(NativeExtension, Init)
