#pragma once

#include <kv/crypto/unlockstatus.hpp>

#define VMC_PUBLIC_KEY ""

namespace vmc
{

    class UnlockStatus : public kv::KushviewUnlockStatus
    {
    public:
        UnlockStatus() = default;
        juce::String getProductID() override { return juce::String(157); }
        juce::RSAKey getPublicKey() override { return juce::RSAKey(VMC_PUBLIC_KEY); }
    };

}