/*
-----------------------------------------------------------------------------
Filename:    cadogAnimationControl.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|
	  Ogre 1.7.x Application Wizard for Code::Blocks (May 2010)
	  http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/

#include "cadogApplication.h"
#include "cadog.h"
#include "cadogAnimationControl.h"

AnimationControl::~AnimationControl()
{
}

void AnimationControl::init(Ogre::Entity* ent, Ogre::SceneNode* node,const char* state)
{
    mEntity=ent;
    mNode=node;

    if(state!=0){
        mAnimationState=ent->getAnimationState(state);
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
    }

    // Set default values for variables
    mWalkSpeed = 0;
    mWaitTime = 0.6;
    accRate=2.0;

    mDirection = Ogre::Vector3::ZERO;
    mDistance=0.0;


    Ogre::Vector3 p=mNode->getPosition();
    mDestination=p;

    ho=p.y;
    hmax=1;
    frequence=2;
    tAcc=0;
}

void AnimationControl::setSpeed(Ogre::Real speed){
    mWalkSpeed=speed;
}

void AnimationControl::move(Ogre::Vector3 mDest){
    mDestination=mDest;
    mDirection=mDest-mNode->getPosition();
    mDistance=mDirection.normalise();
    mWalkSpeed=mDistance/mWaitTime;
    ho=mDestination.y;
}

void AnimationControl::acelerate(){
    mWaitTime=0.3;
}

void AnimationControl::reduce(){
    mWaitTime=0.6;
}

void AnimationControl::showMove(const Ogre::FrameEvent &evt,bool& movComp,const char* ws, const char* is1, const char* is2){

    if(mDistance<=0.001){
        if(is1!=0)
            mAnimationState = mEntity->getAnimationState(is1);

        mNode->setPosition(mDestination);
        movComp=true;
    }else{
        movComp=false;
        Ogre::Real move=mWalkSpeed*evt.timeSinceLastFrame;
        mDistance-=move;
        if(mDistance>0.001){
            mNode->translate(mDirection*move);
            if(ws!=0)
                mAnimationState = mEntity->getAnimationState(ws);
        }else{
            if(is2!=0)
                mAnimationState = mEntity->getAnimationState(is2);
            mNode->setPosition(mDestination);
            movComp=true;
        }
    }

    if(is1!=0 && is2!=0 && ws!=0){
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
        mAnimationState->addTime(evt.timeSinceLastFrame);
    }

}

void AnimationControl::showMoveSin(const Ogre::FrameEvent &evt,bool& movComp,const char* ws, const char* is1, const char* is2){
    Ogre::Real h;
    tAcc+=evt.timeSinceLastFrame;

    if(mDistance<=0.001){
        if(is1!=0)
            mAnimationState = mEntity->getAnimationState(is1);

        h=hmax*sin(2*acos(-1)*frequence*tAcc);
        mDestination.y=h+ho;
        mNode->setPosition(mDestination);
        movComp=true;

    }else{
        Ogre::Real move=mWalkSpeed*evt.timeSinceLastFrame;

        h=hmax*sin(2*acos(-1)*frequence*tAcc);
        mDestination.y=h+ho;

        mDistance-=move;

        if(mDistance>0.001){

            mNode->translate(mDirection*move);
            mNode->translate(0,h,0);

            if(ws!=0)
                mAnimationState = mEntity->getAnimationState(ws);
        }else{
            if(is2!=0)
                mAnimationState = mEntity->getAnimationState(is2);
            mNode->setPosition(mDestination);
            movComp=true;
        }
    }

    if(is1!=0 && is2!=0 && ws!=0){
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
        mAnimationState->addTime(evt.timeSinceLastFrame);
    }
}
