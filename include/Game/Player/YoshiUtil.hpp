#pragma once

#include <revolution/types.h>

class YoshiLockOnTarget;

namespace YoshiFunction {
    bool tryYoshiEat(YoshiLockOnTarget*);
};

namespace MR {
    bool isMsgYoshiStartTongue(u32);
};
