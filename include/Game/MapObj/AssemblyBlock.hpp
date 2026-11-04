#pragma once

#include "Game/LiveActor/LiveActor.hpp"
#include <JSystem/JGeometry/TMatrix.hpp>

class ModelObj;

class AssemblyBlock : public LiveActor {
public:
    enum AssemblyBlockTimedStatus {
        AssemblyBlockTimedStatus_NotTimed = -1,
        AssemblyBlockTimedStatus_IsTimed = 0,
    };

    /// @brief Creates a new `AssemblyBlock`.
    /// @param pName A pointer to the null-terminated name of the object.
    AssemblyBlock(const char* pName);

    virtual void init(const JMapInfoIter&);
    virtual void calcAndSetBaseMtx();

    /// @brief If the player is alive and within this block's activation distance, makes the block start assembling.
    /// @return `true` if this function make the block start assembling (i.e. if the player is alive and within activation distance). `false` otherwise.
    bool tryStartAssemble();

    /// @brief If the player is alive and not within this block's activation distance plus 50, and if this block is not timed, makes the block start returning to its spawn point.
    /// @return `true` if this function made the block start returning (i.e. if the player is alive and far enough away and the block isn't timed). `false` otherwise.
    bool tryStartReturn();

    /// @brief This function is executed every frame while the block is in its default, un-assembled state. Each time this function is called, the
    /// block rotates, and it will start to assemble if the player is near enough.
    void exeWait();

    /// @brief This function is executed every frame while the block is in the process of assembling. After it executes 10 times, the block will be
    /// fully assembled and remain in place.
    void exeAssemble();

    /// @brief This function is executed every frame while the block has already been assembled and is now stationary, and no timer has started. If
    /// this block is timed, this function will start the timer.
    void exeAssembleWait();

    /// @brief This function is executed every frame while the block is in the process of disassembling and returning to its spawn point. After it
    /// executes 10 times, the block will be fully disassembled and go back to floating at its spawn point.
    void exeReturn();

    /// @brief If this is a timed block, this function is executed every frame while the block has been assembled and the timer is running. If it's
    /// 100 frames before the timer runs out, this function will make the "Disappear" animation start playing. If the timer is up, this function will
    /// make the block disappear.
    void exeTimer();

    /* 0x090 */ TPos3f mBaseMtx;
    /* 0x0C0 */ TPos3f _C0;
    /* 0x0F0 */ TPos3f _F0;

    /// @brief If this value is 0, the block will be activated for a set amount of time only. If it's -1, it won't be timed.
    /* 0x120 */ s32 mIsTimed;

    /// @brief Specifies the activation range to the player's position. If set to -1, the model's diameter is used instead.
    /* 0x124 */ f32 mActivatonRange;

    /* 0x128 */ TVec3f _128;

    /// @brief The amount of degrees per second the block rotates per frame while it's floating and unassembled.
    /* 0x134 */ f32 mFloatRotSpeed;

    /// @brief If `mIsTimed` is 0, this specifies the timer until the block disappears again.
    /* 0x138 */ s32 mActivationTime;

    /* 0x13C */ ModelObj* mBloomModel;
    /* 0x140 */ bool _140;
};
