#include "Game/MapObj/AssemblyBlock.hpp"
#include "Game/LiveActor/LiveActor.hpp"
#include "Game/LiveActor/ModelObj.hpp"
#include "Game/LiveActor/Nerve.hpp"
#include "Game/Util/ActionSoundUtil.hpp"
#include "Game/Util/ActorInitUtil.hpp"
#include "Game/Util/ActorMovementUtil.hpp"
#include "Game/Util/ActorSensorUtil.hpp"
#include "Game/Util/ActorSwitchUtil.hpp"
#include "Game/Util/EffectUtil.hpp"
#include "Game/Util/JMapUtil.hpp"
#include "Game/Util/LiveActorUtil.hpp"
#include "Game/Util/MathUtil.hpp"
#include "Game/Util/MtxUtil.hpp"
#include "Game/Util/ObjUtil.hpp"
#include "Game/Util/PlayerUtil.hpp"
#include "Game/Util/StringUtil.hpp"
#include "JSystem/JGeometry/TVec.hpp"
#include "revolution/mtx.h"
#include "revolution/types.h"

namespace {
    static const s32 sAssembleDuration = 10;
    static const f32 sFloatRotSpeedNoSign = 0.1f;
    static const char* const sReturnPosName = "合体ブロック故郷点";
};

namespace NrvAssemblyBlock {
    NEW_NERVE(AssemblyBlockNrvWait, AssemblyBlock, Wait);
    NEW_NERVE(AssemblyBlockNrvAssemble, AssemblyBlock, Assemble);
    NEW_NERVE(AssemblyBlockNrvAssembleWait, AssemblyBlock, AssembleWait);
    NEW_NERVE(AssemblyBlockNrvReturn, AssemblyBlock, Return);
    NEW_NERVE(AssemblyBlockNrvTimer, AssemblyBlock, Timer);
}  // namespace NrvAssemblyBlock

AssemblyBlock::AssemblyBlock(const char* pName)
    : LiveActor(pName), mIsTimed(AssemblyBlockTimedStatus_NotTimed), mActivatonRange(-1.0f), _128(0.0f, 0.0f, 0.0f), mFloatRotSpeed(), mActivationTime(0x12c), mBloomModel(), _140() {
    mBaseMtx.identity();
    _C0.identity();
    _F0.identity();
}

void AssemblyBlock::init(const JMapInfoIter& rIter) {
    char objName[256];

    MR::initDefaultPos(this, rIter);
    MR::tryRegisterNamePosLinkObj(this, rIter, ::sReturnPosName);

    if (!MR::tryFindLinkNamePos(this, ::sReturnPosName, _F0)) {
        TVec3f trans;
        trans.add(mPosition, TVec3f(0.0f, 1000.0f, 1000.0f));
        _F0.setTrans(trans);
    }

    MR::getMapPartsObjectNameIfExistShapeID(objName, sizeof(objName), rIter);
    MR::initActor(this, rIter, objName, false);

    if (MR::isEqualString(objName, "AssemblyBlockPartsTimerA")) {
        _140 = true;
    } else {
        _140 = false;
    }

    _C0.set(getBaseMtx());
    mBaseMtx.set(_F0);
    MR::setBaseTRMtx(this, mBaseMtx);

    if (MR::isEqualSubString(objName, "PartsIce")) {
        MR::connectToSceneIndirectMapObj(this);
    } else {
        MR::connectToSceneMapObj(this);
    }

    initHitSensor(1);
    MR::addBodyMessageSensorMapObj(this);
    MR::initCollisionPartsAutoEqualScaleOne(this, objName, getSensor(0), nullptr);

    TVec3f trans_C0;
    _C0.getTrans(trans_C0);
    TVec3f trans_F0;
    _F0.getTrans(trans_F0);
    _128.add(trans_C0, trans_F0);
    _128 *= 0.5f;

    float radius;
    MR::calcModelBoundingRadius(&radius, this);
    MR::setClippingTypeSphere(this, radius + _128.distance(trans_F0), &_128);
    MR::useStageSwitchAwake(this, rIter);
    MR::getJMapInfoArg0NoInit(rIter, &mActivatonRange);

    if (mActivatonRange <= 0.0f) {
        mActivatonRange = 2.0f * radius;
    }

    MR::getJMapInfoArg1NoInit(rIter, &mActivationTime);
    MR::getJMapInfoArg7NoInit(rIter, &mIsTimed);

    if (MR::getRandom((s32)0, 2) != 0) {
        mFloatRotSpeed = ::sFloatRotSpeedNoSign;
    } else {
        mFloatRotSpeed = -::sFloatRotSpeedNoSign;
    }

    if (mIsTimed == AssemblyBlockTimedStatus_NotTimed) {
        initEffectKeeper(0, "AssemblyBlock", false);
    } else {
        initEffectKeeper(0, nullptr, false);
    }

    initSound(4, "AssemblyBlock");

    if (MR::isEqualString(objName, "AssemblyBlockPartsTimerA")) {
        mBloomModel = MR::createBloomModel2(this, getBaseMtx());
    }

    MR::tryStartAllAnim(this, "Wait");
    initNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvWait));
    makeActorAppeared();
}

void AssemblyBlock::exeWait() {
    if (MR::isFirstStep(this)) {
        if (MR::isEffectValid(this, "Blur")) {
            MR::deleteEffect(this, "Blur");
        }

        MR::validateCollisionParts(this);
        MR::validateHitSensors(this);
    }

    MR::rotateMtxLocalXDegree(mBaseMtx, mFloatRotSpeed);
    MR::rotateMtxLocalYDegree(mBaseMtx, mFloatRotSpeed);
    MR::rotateMtxLocalZDegree(mBaseMtx, mFloatRotSpeed);

    f32 scalar = MR::sinDegree(getNerveStep());

    TVec3f trans;
    mBaseMtx.getTrans(trans);
    trans.scaleAdd(scalar, mGravity, trans);
    mBaseMtx.setTrans(trans);
    tryStartAssemble();
}

void AssemblyBlock::exeAssemble() {
    // FIXME
    // decomp.me: https://decomp.me/scratch/Bhyay
    // The wrong numbers are being added to the address stored in r31
    
    if (MR::isFirstStep(this)) {
        MR::emitEffect(this, "OjAssembleBlockEnd");

        if (_140) {
            MR::startActionSound(this, "OjAssembleBlockRetF");
        } else {
            MR::startActionSound(this, "OjAssembleBlockReturn");
        }

        MR::invalidateCollisionParts(this);
        MR::invalidateHitSensors(this);
    }

    MR::blendMtx(_F0, _C0, MR::calcNerveRate(this, ::sAssembleDuration), mBaseMtx);

    if (!MR::isStep(this, ::sAssembleDuration)) {
        return;
    }

    if (_140) {
        MR::startActionSound(this, "Disappear");
    } else {
        MR::startActionSound(this, "OjAssembleBlockVanF");
    }

    setNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvAssembleWait));
}

void AssemblyBlock::exeAssembleWait() {
    if (MR::isFirstStep(this)) {
        if (MR::isEffectValid(this, "Blur")) {
            MR::deleteEffect(this, "Blur");
        }

        MR::tryRumblePadWeak(this, 0);
        MR::validateCollisionParts(this);
        MR::validateHitSensors(this);
    }

    if (mIsTimed == AssemblyBlockTimedStatus_IsTimed) {
        setNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvTimer));
    } else {
        tryStartReturn();
    }
}

void AssemblyBlock::exeReturn() {
    // FIXME
    // decomp.me: https://decomp.me/scratch/yMYHP
    // The wrong numbers are being added to the address stored in r31
    
    if (MR::isFirstStep(this)) {
        MR::tryRumblePadWeak(this, 0);
        MR::emitEffect(this, "Blur");

        if (_140) {
            MR::startActionSound(this, "OjAssembleBlockRetF");
        } else {
            MR::startActionSound(this, "OjAssembleBlockReturn");
        }

        MR::invalidateCollisionParts(this);
        MR::invalidateHitSensors(this);
    }

    MR::blendMtx(_C0, _F0, MR::calcNerveRate(this, ::sAssembleDuration), mBaseMtx);

    if (MR::isStep(this, ::sAssembleDuration)) {
        setNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvWait));
    }
}

void AssemblyBlock::exeTimer() {
    // FIXME
    // decomp.me: https://decomp.me/scratch/WspLF
    // The wrong numbers are being added to the address stored in r31

    if (MR::isStep(this, mActivationTime - 100)) {
        MR::tryStartAllAnim(this, "Disappear");
    }

    if (MR::isStep(this, mActivationTime)) {
        if (_140) {
            MR::startActionSound(this, "OjAssembleBlockVanF");
        } else {
            MR::startActionSound(this, "OjAssembleBlockVanish");
        }

        if (mBloomModel != nullptr) {
            mBloomModel->kill();
        }

        kill();
    }
}

void AssemblyBlock::calcAndSetBaseMtx() {
    MR::setBaseTRMtx(this, mBaseMtx);
}

bool AssemblyBlock::tryStartAssemble() {
    bool isNotNear = !MR::isNearPlayerAnyTime(this, mActivatonRange);

    if (isNotNear) {
        return false;
    }

    if (MR::isPlayerDead()) {
        return false;
    }

    setNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvAssemble));
    return true;
}

bool AssemblyBlock::tryStartReturn() {
    if (MR::isNearPlayerAnyTime(this, 50.0f + mActivatonRange)) {
        return false;
    }

    if (MR::isPlayerDead()) {
        return false;
    }

    setNerve(GET_NERVE(AssemblyBlock, AssemblyBlockNrvReturn));
    return true;
}
