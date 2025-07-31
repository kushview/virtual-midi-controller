#pragma once

#include <kv/crypto/unlockstatus.hpp>

#define VMC_PUBLIC_KEY "5,857c5bd1dd5cc4d913749d04df29d8ac6896031c0049681b918135cd813bf4e01e431945f8d29d1878476ec36bb5c467063950e7fba68cf3616dd46657f6be818f058e53812b23c4d0974a2c8f886f3d7d94b3aae684f9584eeed953ea27774c47be9b93b83a8118e89b748d65ed67658f7782cc27005b2023df25982c59114b13de14b958675ebc3e17d6bade45f1d7340cc88cefc2fa0f2aca4bab3bdff3f7c52e14c30f8df3b071889e406ff21b2367c3c251fb079249a3b13e7d79c137712d5aae47511daf0796aad220de6ba75721919e1dc15a109ec7cad4125f2854f5dd4f60d3731821d02c00bb49242c7c828f76e40ec2513e880ca7d1b1af2d0bdf"

namespace vmc
{
    class UnlockStatus : public kv::KushviewUnlockStatus
    {
    public:
        UnlockStatus() = default;
        juce::String getProductID() override { return juce::String(159); }
        juce::RSAKey getPublicKey() override { return juce::RSAKey(VMC_PUBLIC_KEY); }
    };
}
