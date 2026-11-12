#pragma once

#include "Game/LiveActor/LiveActor.hpp"

class QuestionCoin : public LiveActor {
public:
    /// @brief Creates a new `QuestionCoin`.
    /// @param pName Pointer to a null-terminated string that will be used as a name for the Question Coin.
    QuestionCoin(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void initAfterPlacement();
    virtual void appear();
    virtual void makeActorAppeared();
    virtual void kill();
    virtual void startClipped();
    virtual void endClipped();
    virtual void calcAndSetBaseMtx();
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

    void initFromJMapInfoAr(const JMapInfoIter& rIter);
    void initShadow();
    void updateActorMtx();

    void exeWait();
    void exeThrowActor();
    void exeCaught();
};
