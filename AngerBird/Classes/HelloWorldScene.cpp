//
//  HelloWorldScene.cpp
//  AngerBird
//
//  Created by msp on 13-11-8.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"

using namespace CocosDenshion;


#define PTM_RATIO 32
#define FLOOR_HEIGHT 62.0f

enum
{
	kTagTileMap = 1,
	kTagSpriteManager = 1,
	kTagAnimation1 = 1,
}; 

HelloWorld::HelloWorld()
{
	this->setIsTouchEnabled( true );
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->InitPhysical();
    
    CCSprite * pBg = CCSprite::spriteWithFile("bg.png");
    pBg->setAnchorPoint(CCPointZero);
    pBg->setPosition(CCPointMake(0.0, 0.0));
    this->addChild(pBg,-1);
    
    CCSprite * pBase2 = CCSprite::spriteWithFile("catapult_base_2.png");
    pBase2->setAnchorPoint(CCPointZero);
    pBase2->setPosition(ccp(181.0f,FLOOR_HEIGHT));
    this->addChild(pBase2,0);
    
    CCSprite * pSquirrelLeft = CCSprite::spriteWithFile("squirrel_1.png");
    pSquirrelLeft->setAnchorPoint(CCPointZero);
    pSquirrelLeft->setPosition(ccp(11.0f,FLOOR_HEIGHT));
    this->addChild(pSquirrelLeft,0);
    
    CCSprite * pBase1 = CCSprite::spriteWithFile("catapult_base_1.png");
    pBase1->setAnchorPoint(CCPointZero);
    pBase1->setPosition(ccp(181.0f,FLOOR_HEIGHT));
    this->addChild(pBase1,9);
    
    CCSprite * pSquirrelRight = CCSprite::spriteWithFile("squirrel_2.png");
    pSquirrelRight->setAnchorPoint(CCPointZero);
    pSquirrelRight->setPosition(ccp(240.0f,FLOOR_HEIGHT));
    this->addChild(pSquirrelRight,9);
    
    
    
    
    //发射臂
    CCSprite * pIceGround = CCSprite::spriteWithFile("fg.png");
    pIceGround->setAnchorPoint(CCPointZero);
    this->addChild(pIceGround,10);

	CCSprite * pArm = CCSprite::spriteWithFile("catapult_arm.png");
    this->addChild(pArm,1);
    
    CCMenuItemImage * backItem = CCMenuItemImage::itemFromNormalImage("back.png", "back.png");
    backItem->setTarget(this, menu_selector(HelloWorld::backToMainMenu));
    CCMenu * pMenu = CCMenu::menuWithItem(backItem);
    pMenu->setPosition(ccp(40,300));
    this->addChild(pMenu,111);
    
    
    
    b2BodyDef armBodyDef;
    armBodyDef.type = b2_dynamicBody;
    armBodyDef.linearDamping = 1;//线性阻尼
    armBodyDef.angularDamping = 1;//角阻抗
    armBodyDef.position.Set(230.0/PTM_RATIO, (FLOOR_HEIGHT + 91.0f)/PTM_RATIO);
    armBodyDef.userData = pArm;
    m_pArmBody = m_pWorld->CreateBody(&armBodyDef);
    
    b2PolygonShape armBox;
    b2FixtureDef armBoxDef;
    armBoxDef.shape = &armBox;
    armBoxDef.density = 0.3f;
    armBox.SetAsBox(11.0f/PTM_RATIO, 91.0f/PTM_RATIO);
    m_pArmFixture = m_pArmBody->CreateFixture(&armBoxDef);
    
    //旋转关节
    b2RevoluteJointDef armJoinDef;
    armJoinDef.Initialize(m_pGroundBody, m_pArmBody, b2Vec2(233.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
    armJoinDef.enableMotor = true;
    armJoinDef.enableLimit = true;
    armJoinDef.motorSpeed = -10;
    armJoinDef.lowerAngle = CC_DEGREES_TO_RADIANS(9);
    armJoinDef.upperAngle = CC_DEGREES_TO_RADIANS(75);
    armJoinDef.maxMotorTorque = 700;//就是当关节被旋转后，使关节恢复的力，类似弹力。这个力越小，越容易旋转，恢复力也就越小。
    m_pArmJoin = (b2RevoluteJoint*)m_pWorld->CreateJoint(&armJoinDef);
    
    m_pMouseJoin = NULL;
    m_bReleaseArm = false;
    
    CCDelayTime * delayTime = CCDelayTime::actionWithDuration(0.2f);
    CCCallFunc * callFunc = CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::resetGame));
    this->runAction(CCSequence::actions(delayTime,callFunc,NULL));
    
    createTargets();
    
    m_pListener = new MyContactListener();
    m_pWorld->SetContactListener(m_pListener);
    m_pBulletBody = NULL;

    
	schedule( schedule_selector(HelloWorld::tick) );
    
}


HelloWorld::~HelloWorld()
{
	delete m_pWorld;
	m_pWorld = NULL;
	
	//delete m_debugDraw;
}

void HelloWorld::InitPhysical()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    b2Vec2 gravity;
    bool bSleep = true;
    gravity.Set(1.0f, -10.0f);
    m_pWorld = new b2World(gravity,bSleep);

//    m_pWorld->SetContinuousPhysics(true);
    
    b2BodyDef groundDef;
    groundDef.position.Set(0,0);
    
    m_pGroundBody = m_pWorld->CreateBody(&groundDef);
    
    //边界
    b2PolygonShape groundBox;//多边形形状
    b2FixtureDef     boxShapeDef;//边框形状定义
    
    boxShapeDef.shape = &groundBox;
    
    
    /***box2d的坐标系与cocos2d一样，都是以左下角为原点。***/
    groundBox.SetAsEdge(b2Vec2(0,FLOOR_HEIGHT/PTM_RATIO), b2Vec2(screenSize.width*2/PTM_RATIO,
                                                                 FLOOR_HEIGHT/PTM_RATIO));//下边界
    m_pGroundBody->CreateFixture(&boxShapeDef);
    
    groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(0,FLOOR_HEIGHT/PTM_RATIO));//左边界
    m_pGroundBody->CreateFixture(&boxShapeDef);
    
    groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width*2/PTM_RATIO,
                                                                      screenSize.height/PTM_RATIO));//上边界
    m_pGroundBody->CreateFixture(&boxShapeDef);
    
//    groundBox.SetAsEdge(b2Vec2(screenSize.width*2/PTM_RATIO,screenSize.height/PTM_RATIO),
//                        b2Vec2(screenSize.width*2/PTM_RATIO,0));//右边界
    m_pGroundBody->CreateFixture(&boxShapeDef);
    
}

void HelloWorld::draw()
{
//	glDisable(GL_TEXTURE_2D);
//	glDisableClientState(GL_COLOR_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	
//	glEnable(GL_TEXTURE_2D);
//	glEnableClientState(GL_COLOR_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
}


void HelloWorld::tick(ccTime dt)
{
		
	int velocityIterations = 8;
	int positionIterations = 1;
    
	
	m_pWorld->Step(dt, velocityIterations, positionIterations);
	
    //更新位置和角度
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL)
        {
            CCSprite * actor = (CCSprite*)b->GetUserData();
            actor->setPosition(ccp(b->GetPosition().x*PTM_RATIO,b->GetPosition().y*PTM_RATIO));
            actor->setRotation(-1*CC_RADIANS_TO_DEGREES(b->GetAngle()));
    
        }	
	}
    
    
    //发射子弹
    if(m_bReleaseArm && m_pBulletJoint != NULL)
    {
        if(m_pArmJoin->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10))//差不多回到原始位置再发射子弹
        {
            m_bReleaseArm = false;
            m_pWorld->DestroyJoint(m_pBulletJoint);
            m_pBulletJoint = NULL;
            
            CCDelayTime *delayAction = CCDelayTime::actionWithDuration(5.0f);
            CCCallFunc *callSelectorAction = CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::resetBullet));
            this->runAction(CCSequence::actions(delayAction, callSelectorAction, NULL));
            
        }
    }
    
    //镜头跟随
    if(m_pBulletBody && m_pBulletJoint == NULL)
    {
        b2Vec2 position = m_pBulletBody->GetPosition();
        CCPoint myPosition = this->getPosition();
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        
        if(position.x > screenSize.width/2.0f/PTM_RATIO)
        {
            myPosition.x = -min(screenSize.width, position.x*PTM_RATIO);
            this->setPosition(myPosition);
        }
    }
    
    //消灭敌人
    std::set<b2Body*>::iterator pos;
    for(pos = m_pListener->contracts.begin();pos != m_pListener->contracts.end();)
    {
        b2Body * body = *pos;
        CCSprite * enemy = (CCSprite*)body->GetUserData();
        
        
        //爆炸效果
        CCPoint position = ccp(body->GetWorldCenter().x*PTM_RATIO,body->GetWorldCenter().y*PTM_RATIO);
        CCParticleSun *explosion = CCParticleSun::node();
		explosion->retain();
		explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
		explosion->initWithTotalParticles(20);
		explosion->setIsAutoRemoveOnFinish(true);
		explosion->setStartSizeVar(10.0f);
		explosion->setSpeed(70.0f);
		explosion->setAnchorPoint(ccp(0.5f, 0.5f));
		explosion->setPosition(position);
		explosion->setDuration(1.0f);
		addChild(explosion, 11);
		explosion->release();

        

        //移除
        this->removeChild(enemy, true);
        m_pWorld->DestroyBody(body);
        body = NULL;
        
        m_pListener->contracts.erase(*pos);
        pos++;
        
        
    }
    
    
  
}

#pragma mark - touch

void HelloWorld::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
   if(m_pMouseJoin != NULL)
   {
       return;
   }
    
    CCTouch * touch = (CCTouch*)pTouches->anyObject();
    CCPoint location = touch->locationInView(touch->view());
    location = CCDirector::sharedDirector()->convertToGL(location);
    b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO,location.y/PTM_RATIO);
    if(locationWorld.x < m_pArmBody->GetWorldCenter().x + 50.0f/PTM_RATIO && locationWorld.x > m_pArmBody->GetWorldCenter().x - 50.0f/PTM_RATIO)
    {
        b2MouseJointDef mouseJoinDef;
        mouseJoinDef.bodyA = m_pGroundBody;
        mouseJoinDef.bodyB = m_pArmBody;
        mouseJoinDef.target = locationWorld;
        mouseJoinDef.maxForce = 2000;//施加给加节的力。只有当它大小旋转关节上的maxMotorTorque，才能使关切旋转
        m_pMouseJoin = (b2MouseJoint*)m_pWorld->CreateJoint(&mouseJoinDef);
    }
}

void HelloWorld::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
        CCLOG("touch moved");
    if(m_pMouseJoin == NULL)
    {
        return;
    }
    
    CCTouch * touch = (CCTouch*)pTouches->anyObject();
    CCPoint location = touch->locationInView(touch->view());
    location = CCDirector::sharedDirector()->convertToGL(location);
    b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO,location.y/PTM_RATIO);
    
    m_pMouseJoin->SetTarget(locationWorld);
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    CCLOG("touch end");
    if(m_pMouseJoin != NULL)
    {
        if(m_pArmJoin->GetJointAngle() >= CC_DEGREES_TO_RADIANS(5))//偏离大于5度才发射子弹
        {
            m_bReleaseArm = true;
        }
        
        m_pWorld->DestroyJoint(m_pMouseJoin);
        m_pMouseJoin = NULL;
    }
}

#pragma mark -

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::node();
    
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

void HelloWorld::createBullets(int count)
{
    m_iCurrentBullet = 0;
    float pos = 62.0f;//第一颗子弹的x位置
    
    if(count > 0)
    {
        //delta 是子弹排列的间隔
        //165是最后一颗子弹的x位置
        //30是子弹大小
        
        float fDelta = (count > 1)?((165.0f - 62.0f - 30)/(count-1)):0;
        for(int i = 0;i < count;i++,pos += fDelta)
        {
            
            CCSprite * pBullet = CCSprite::spriteWithFile("acorn.png");
            this->addChild(pBullet,1);
            
            b2BodyDef bulletBodyDef;
            bulletBodyDef.type = b2_dynamicBody;
            bulletBodyDef.bullet = true;
            bulletBodyDef.position.Set(pos/PTM_RATIO, (FLOOR_HEIGHT+15.0)/PTM_RATIO);
            bulletBodyDef.userData = pBullet;
            b2Body * bulletBody = m_pWorld->CreateBody(&bulletBodyDef);
            bulletBody->SetActive(false);
            
            b2CircleShape circle;
            circle.m_radius = 15.0/PTM_RATIO;
            
            b2FixtureDef ballShapeDef;
            ballShapeDef.shape = &circle;
            ballShapeDef.density = 0.8f;
            ballShapeDef.restitution = 0.2f;
            ballShapeDef.friction = 0.99f;
            bulletBody->CreateFixture(&ballShapeDef);
            
            m_pBullets.push_back(bulletBody);
            
            
        }
    }
}


bool HelloWorld::attachBullet()
{
    if(m_iCurrentBullet < m_pBullets.size())
    {
        m_pBulletBody = (b2Body*)m_pBullets.at(m_iCurrentBullet++);
        m_pBulletBody->SetTransform(b2Vec2(230.0f/PTM_RATIO, (155.0f+FLOOR_HEIGHT)/PTM_RATIO), 0.0f);
        m_pBulletBody->SetActive(true);
        
        b2WeldJointDef weldJointDef;
        weldJointDef.Initialize(m_pBulletBody, m_pArmBody, b2Vec2(230.0f/PTM_RATIO,(155.0f+FLOOR_HEIGHT)/PTM_RATIO));
        weldJointDef.collideConnected = false;
        
        m_pBulletJoint = (b2WeldJoint*)m_pWorld->CreateJoint(&weldJointDef);
        return true;
    }
    
    return false;
}


void HelloWorld::resetGame( )
{
    this->createBullets(5);
    this->attachBullet();
    
    //镜头移动
    CCFiniteTimeAction * action1 = CCMoveTo::actionWithDuration(1.5f, ccp(-480.0f, 0.0f));
    CCDelayTime *action3 = CCDelayTime::actionWithDuration(1.0f);
    CCFiniteTimeAction *action4 = CCMoveTo::actionWithDuration(1.5f, CCPointZero);
    runAction(CCSequence::actions(action1,action3, action4, NULL));

}


void HelloWorld::resetBullet()
{
    if(!this->attachBullet())
    {
        m_pBulletBody->SetTransform(b2Vec2(0, 0), 0);
        m_pBulletBody->SetActive(false);
    }
    CCAction *action = CCMoveTo::actionWithDuration(0.2f, CCPointZero);
    runAction(action);
    
}


void HelloWorld::createTargets()
{
    char a[12] = "brick_1.png";
    char b[12] = "brick_2.png";
    char c[12] = "brick_3.png";
    char brick_platfor[19] = "brick_platform.png";
    char dog[13] = "head_dog.png";
    char cat[13] = "head_cat.png";
    
    createTarget(b, CCPointMake(675.0, FLOOR_HEIGHT), 0.0f, false, false, false);
    createTarget(a, CCPointMake(741.0, FLOOR_HEIGHT), 0.0f, false, false, false);
    createTarget(a, CCPointMake(741.0, FLOOR_HEIGHT+23.0), 0.0f, false, false, false);
    createTarget(c, CCPointMake(673.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false);
    createTarget(a, CCPointMake(707.0, FLOOR_HEIGHT+58.0), 0.0f, false, false, false);
    createTarget(a, CCPointMake(707.0, FLOOR_HEIGHT+81.0), 0.0f, false, false, false);
    
    createTarget(dog, CCPointMake(704, FLOOR_HEIGHT), 0.0f, true, false, true);
    createTarget(cat, CCPointMake(680.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true);
    createTarget(dog, CCPointMake(740.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true);
    
    // 2 bricks at the right of the first block
    createTarget(b, CCPointMake(770.0, FLOOR_HEIGHT), 0.0f, false, false, false);
    createTarget(b, CCPointMake(770.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false);
    
    // The dog between the blocks
    createTarget(dog, CCPointMake(830.0, FLOOR_HEIGHT), 0.0f, true, false, true);
    
    // Second block
    createTarget(brick_platfor, CCPointMake(839.0, FLOOR_HEIGHT), 0.0f, false, true, false);
    createTarget(b, CCPointMake(854.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false);
    createTarget(b, CCPointMake(854.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false);
    createTarget(cat, CCPointMake(881.0, FLOOR_HEIGHT+28.0), 0.0f, true, false, true);
    createTarget(b, CCPointMake(909.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false);
    createTarget(a, CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false);
    createTarget(a, CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0+23.0), 0.0f, false, false, false);
    createTarget(b, CCPointMake(882.0, FLOOR_HEIGHT+108.0), 90.0f, false, false, false);

    
}


void HelloWorld::createTarget(char * imageName,CCPoint position,float rotation,bool isCircle,bool isStatic,bool isEnemy)
{
    CCSprite * sprite = CCSprite::spriteWithFile(imageName);
    this->addChild(sprite,20);
    if(isEnemy)
    {
        sprite->setTag(ENEMY_TAT);
    }
    
    
    
    b2BodyDef bodyDef;
    bodyDef.type = isStatic?b2_staticBody:b2_dynamicBody;

    //以精灵中心为位置
    bodyDef.position.Set((position.x + sprite->getContentSize().width/2.0)/PTM_RATIO, (position.y + sprite->getContentSize().
                                                                                       height/2.0)/PTM_RATIO);
    CCLog("%f,%f",bodyDef.position.x,bodyDef.position.y);
    bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation);
    bodyDef.userData = sprite;

    b2Body * body = m_pWorld->CreateBody(&bodyDef);
    
    CCLog("%f,%f",body->GetPosition().x,body->GetPosition().y);
    b2FixtureDef fixtureDef;
    b2Fixture * fixture;

    if(isCircle)
    {
        b2CircleShape circle;
        fixtureDef.shape = &circle;
        circle.m_radius = sprite->getContentSize().width/2.0/PTM_RATIO;
        fixture = body->CreateFixture(&circle,0.5);
        m_pTargets.push_back(body);
    }
    else
    {
        b2PolygonShape box;
        fixtureDef.shape = &box;
        box.SetAsBox(sprite->getContentSize().width/2.0/PTM_RATIO, sprite->getContentSize().height/2.0/PTM_RATIO);
        body->CreateFixture(&box,0.5f);
        m_pTargets.push_back(body);
    }
    
    if(isEnemy)
    {
        fixture->SetUserData((void*)1);//??
        m_pEnemies.push_back(body);
    }
    
}


bool HelloWorld::eliminateEnery(b2Body*enemyBody)
{
    CCLog("%d",m_pWorld->GetBodyCount());
    for(b2Body * body = m_pWorld->GetBodyList();body;body = body->GetNext())
    {
        if(enemyBody == body)
        {
            CCSprite * sprite = (CCSprite*)body->GetUserData();
            CCLog("%d,%d",sprite->getTag(),sprite->retainCount());
            this->removeChild(sprite, false);
            sprite = NULL;
            
            m_pWorld->DestroyBody(body);
            body = NULL;
            
            break;
        }
    }
    
 
    
    return true;
}


void HelloWorld::backToMainMenu()
{
    CCScene * scence = MainScene::scene();
    CCDirector::sharedDirector()->replaceScene(CCTransitionJumpZoom::transitionWithDuration(2,scence));
    
}












