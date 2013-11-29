//
//  File.h
//  AngerBird
//
//  Created by msp on 13-11-12.
//
//

#ifndef __AngerBird__File__
#define __AngerBird__File__

#include <iostream>
#include "cocos2d.h"
using namespace cocos2d;

class MainScene:cocos2d::CCLayer {
    
    
public:
    ~MainScene();
    MainScene();
    
    static CCScene * scene();
    void StartGame();
    void ContinuteGame();
    void Setting();

};

#endif /* defined(__AngerBird__File__) */
