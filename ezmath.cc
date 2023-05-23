#include <napi.h>
#include <parsing/parser.hpp>
#include <fstream>

Napi::Value Simplify(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string arg = info[0].As<Napi::String>();

  try {
    auto tree = ezmath::parsing::ParseTree(arg);
    ezmath::tree::math::simplify(tree);
    return Napi::String::New(env, tree->ToString());
  } catch(const std::exception& ex) {
    return Napi::String::New(env, ex.what());
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "simplify"), Napi::Function::New(env, Simplify));
  return exports;
}

NODE_API_MODULE(ezmath, Init)