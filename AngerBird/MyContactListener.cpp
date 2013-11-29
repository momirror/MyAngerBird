//
//  MyContactListener.cpp
//  篮球-cocos2dx-box2d
//
//  Created by msp on 13-11-8.
//
//

#include "MyContactListener.h"
#include "Definition.h"



/**
	碰撞开始
	@param contact 碰撞   
 */
void MyContactListener:: BeginContact(b2Contact* contact)

{//1
//    CCLOG("BeginContact");
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    CCSprite* spriteA = (CCSprite*)bodyA->GetUserData();
    CCSprite* spriteB = (CCSprite*)bodyB->GetUserData();
    
}

/**
	碰撞结束
	@param contact 碰撞
 */
void MyContactListener::EndContact(b2Contact* contact)

{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    CCSprite* spriteA = (CCSprite*)bodyA->GetUserData();
    CCSprite* spriteB = (CCSprite*)bodyB->GetUserData();
//    CCLOG("EndContact");
    
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
    //2
//    CCLOG("PreSolve");
    //b2Manifold结构含有一个法向量和最多两个的接触点。向量和接触点都是相对于局部坐标系。为方便接触求解器处理，每个接触点都存储了法向冲量和切向(摩擦）冲量。
}

void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    //
    /*
     impluse冲量是一个数组，具有两个元素，normalImpulses和tangentImpulses，都是矢量Vector类型
     normalImpulses 碰撞通俗合力产生的冲量
     tangentImpulses 模仿切线标的目标摩擦力所产生的冲量
     */
//    CCLOG("PostSolve");
    
//    delegate->bomb(CCPointZero);
    int32 count = contact->GetManifold()->pointCount;
    float32 maxImpulse = 0.0f;
    for(int i = 0;i < count;i++)
    {
        maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
    }
    
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    CCSprite* spriteA = (CCSprite*)bodyA->GetUserData();
    CCSprite* spriteB = (CCSprite*)bodyB->GetUserData();
    if(spriteA != NULL && spriteB != NULL)
    {
        if(spriteA->getTag() == ENEMY_TAT || spriteB->getTag() == ENEMY_TAT)
        {
            if(maxImpulse > 1)
            {
                CCLog("tag:A->%d,B->%d",spriteA->getTag(),spriteB->getTag());
                CCLog("碰撞冲量为：%f,%f",impulse->normalImpulses[0],impulse->normalImpulses[1]);
//                delegate->eliminateEnery((spriteA->getTag() == ENEMY_TAT)?bodyA:bodyB);
                contracts.insert((spriteA->getTag() == ENEMY_TAT)?bodyA:bodyB);
            }
        }
    }
    

}