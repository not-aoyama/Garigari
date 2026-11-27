#pragma once

#include <JSystem/JGeometry/TVec.hpp>

class ParabolicPath {
public:
    ParabolicPath();

    void initFromMaxHeight(const TVec3f& rStart, const TVec3f& rEnd, const TVec3f& rUp);
    void initFromUpVector(const TVec3f& rStart, const TVec3f& rEnd, const TVec3f& rUp, f32 max);
    void initFromUpVectorAddHeight(const TVec3f& rStart, const TVec3f& rEnd, const TVec3f& rUp, f32 max);
    void calcPosition(TVec3f*, f32) const;
    void calcDirection(TVec3f*, f32, f32) const;
    f32 getLength(f32 startParam, f32 endParam, s32 numSegments) const;
    f32 getTotalLength(s32 numSegments) const;
    f32 calcPathSpeedFromAverageSpeed(f32 avgSpeed) const;

    /* 0x00 */ TVec3f mPosition;
    /* 0x0C */ TVec3f mAxisY;
    /* 0x18 */ TVec3f mAxisZ;
    /* 0x24 */ f32 mAccel;
    /* 0x28 */ f32 mVelocity;
    /* 0x2C */ f32 mScalar;
};
