#include <napi.h>

namespace sdf
{
    Napi::Object Init(Napi::Env env, Napi::Object exports);

    template<typename D, unsigned int stride>
    void generate(D* dst, uint8_t* src, int src_w, int src_h, int channel, int threshold, float spread);

    Napi::Value GenerateWrapped(const Napi::CallbackInfo &info);
} // namespace functionexample