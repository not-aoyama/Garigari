#pragma once

#include <revolution/types.h>
#include <JSystem/JGeometry/TVec.hpp>

class LiveActor;

namespace SpinDriverUtil {
    void setShadowAndClipping(LiveActor*, TVec3f*, f32, f32, f32*);
};
