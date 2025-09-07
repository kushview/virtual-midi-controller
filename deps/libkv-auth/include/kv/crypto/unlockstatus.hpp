#pragma once

#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>

// Stub header for libkv-auth when not available
// This allows the project to build without the private libkv-auth dependency

namespace kv { namespace crypto {

enum class UnlockStatus
{
    locked = 0,
    unlocked = 1
};

}}

namespace kv {

class KushviewUnlockStatus 
{
public:
    KushviewUnlockStatus() = default;
    virtual ~KushviewUnlockStatus() = default;
    
    virtual juce::String getProductID() { return {}; }
    virtual juce::RSAKey getPublicKey() { return {}; }
    
    bool isUnlocked() const { return true; } // Always unlocked in stub
    void save() {} // No-op in stub
    void load() {} // No-op in stub
};

}