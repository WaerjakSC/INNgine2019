#include "registry.h"

Registry *Registry::mInstance = nullptr;

Registry::Registry() {
}
Registry *Registry::instance() {
    if (!mInstance)
        mInstance = new Registry();
    return mInstance;
}
