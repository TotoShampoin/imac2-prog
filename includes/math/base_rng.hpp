#pragma once

namespace Random {

template <typename Type>
class BaseRng {
public:
    virtual Type operator()() = 0;
};

} // namespace Random
