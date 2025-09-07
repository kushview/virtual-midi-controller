# libkv-auth Stub Implementation

This directory contains stub implementations for the private `libkv-auth` library.

The stub files allow the Virtual MIDI Controller to build successfully without access to the private licensing library. In this configuration:

- The unlock functionality is disabled
- The application behaves as if it's always in an unlocked state
- No licensing validation is performed

## Files

- `include/kv/crypto/unlockstatus.hpp` - Stub unlock status classes
- `include/kv/ui/unlockoverlay.hpp` - Stub unlock overlay component
- `include/kv/ui/unlockform.hxx` - Stub unlock form component

These stubs provide the minimum interface needed for the main application to compile and link successfully.