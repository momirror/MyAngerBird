
//
//  File.cpp
//  AngerBird
//
//  Created by msp on 13-11-12.
//
//

#include "MainScene.h"
#include "HelloWorldScene.h"

MainScene::~MainScene()
{
    
}

MainScene::MainScene()
{
    CCSprite * bg = CCSprite::spriteWithFile("bg0.png");
    bg->setAnchorPoint(ccp(0,0));
    this->addChild(bg,-1);
//
//    CCSprite * itemSprite = CCSprite::spriteWithFile("ball.png");
//    CCSprite * itemSprite2 = CCSprite::spriteWithFile("ball.png");
//    CCSprite * itemSprite3 = CCSprite::spriteWithFile("ball.png");
//    CCMenuItemSprite * item = CCMenuItemSprite::itemFromNormalSprite(itemSprite, itemSprite2, itemSprite3, this, menu_selector(MainScene::StartGame));
//    
//    CCMenu * menu = CCMenu::menuWithItem(item);
//    menu->alignItemsHorizontally();
//    menu->setPosition(ccp(100,100));
//    this->addChild(menu,0);
//    CCLog("%f,%f",menu->getPosition().x,menu->getPosition().y);
    
    CCLabelTTF * pStartLabel = CCLabelTTF::labelWithString("开始", "Marker Felt", 20);
    pStartLabel->setColor(ccc3(22,22,22));
    CCMenuItemLabel * pStartItem = CCMenuItemLabel::itemWithLabel(pStartLabel, this, menu_selector(MainScene::StartGame));
    
    CCLabelTTF * pContinuteLabel = CCLabelTTF::labelWithString("继续", "Marker Felt", 20);
    pContinuteLabel->setColor(ccc3(22,22,22));    
    CCMenuItemLabel * pContinuteItem = CCMenuItemLabel::itemWithLabel(pContinuteLabel, this, menu_selector(MainScene::ContinuteGame));
    
    CCLabelTTF * pSettingLabel = CCLabelTTF::labelWithString("设置", "Marker Felt", 20);
    pSettingLabel->setColor(ccc3(22,22,22));    
    CCMenuItemLabel * pSettingItem = CCMenuItemLabel::itemWithLabel(pSettingLabel, this, menu_selector(MainScene::Setting));
    
    CCMenu * pMenu = CCMenu::menuWithItems(pStartItem,pContinuteItem,pSettingItem,NULL);
    pMenu->alignItemsVertically();
    this->addChild(pMenu);
    
}

CCScene* MainScene::scene()
{
    CCScene * scene = CCScene::node();
    CCLayer * layer = new MainScene();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

void MainScene::StartGame()
{
    static int i = 0;
    CCLog("start game:%d",i++);
    
    CCScene * scene = HelloWorld::scene();
//    CCDirector::sharedDirector()->replaceScene(scene);//正常切换
    
//    CCDirector::sharedDirector()->replaceScene(CCTransitionJumpZoom::transitionWithDuration(2,scene));//弹跳切换
    
//    CCDirector::sharedDirector()->replaceScene(CCTransitionFlipX::transitionWithDuration(2, scene));//X轴翻转切换
    
//    CCDirector::sharedDirector()->replaceScene(CCTransitionFlipY::transitionWithDuration(2, scene));//Y轴翻转切换
    
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::transitionWithDuration(2, scene));//淡出淡入切换
    
//    CCDirector::sharedDirector()->replaceScene(CCTransitionShrinkGrow::transitionWithDuration(2, scene));//交错切换
    
//     CCDirector::sharedDirector()->replaceScene(CCTransitionMoveInL::transitionWithDuration(2, scene));//从左进入覆盖切换
    
//    CCDirector::sharedDirector()->replaceScene(CCTransitionSlideInL::transitionWithDuration(2, scene));//从左进入推出切换
    
}


void MainScene::ContinuteGame()
{
    CCLog("Continute game");
}


void MainScene::Setting()
{
    CCLog("setting");
}