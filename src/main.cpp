/* cppsrc/main.cpp */
#include <napi.h>
#include "sdf.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    return sdf::Init(env, exports);
}

NODE_API_MODULE(testaddon, InitAll)