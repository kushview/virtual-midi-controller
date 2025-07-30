#pragma once

#include <kv/crypto/unlockstatus.hpp>

#define VMC_PUBLIC_KEY "5,7a4ad6341657cebf63bbf185e8bf5c10f61ee54b35bbb585c743c6744f04471dc3ade56c52234bd9e107666aba32d21d8d4b64d2ebb94e924dd9fce990ebe9ac00c76d06d2746d66012c25d75022e2220b245f4e8fc36f4b48fd2f2e376114872343aa6f4ad83187524de872ed8ee9bf86425063473c954b7f46631229c56ee0e29750e5c3b37f58cacdee084b2282f5d9dee450d7a057eeb154da927efd06be0aadb2d56e54e9aa745cdc8b3599b9a062ea25f988a68e885ca35c380bb8cdbf128d55af57eb173c5c9ae57953655a12fee7c1db84c52cdb89d93695749fbab06625cb5db83f02b7f1bed41b1ae56e812f7af035b8e65d53c10085179f9fa3b1"

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
