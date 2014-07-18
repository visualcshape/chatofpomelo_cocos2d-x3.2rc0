#include "AppDelegate.h"
#include <cocos2d.h>
#include <CCGLView.h>
#include "LoginScene.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
    
}

AppDelegate::~AppDelegate()
{
    
}

bool AppDelegate::applicationDidFinishLaunching()
{
    //init director
    Director* pDirector = Director::getInstance();
    GLView* pGLView = pDirector->getOpenGLView();
    if(!pGLView){
        pGLView = GLView::create("ChatOfPomelo");
        pDirector->setOpenGLView(pGLView);
    }
    
    //turn on display FPS
    pDirector->setDisplayStats(true);
    
    //set FPS
    pDirector->setAnimationInterval(1.0/60);
    
    //Create a Scene
    Scene* pScene = LoginScene::scene();
    
    //run
    pDirector->runWithScene(pScene);
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director* pDirector = Director::getInstance();
    pDirector->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director* pDirector = Director::getInstance();
    pDirector->startAnimation();
}