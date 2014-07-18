#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__
#include <cocos2d.h>

class AppDelegate:private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    
    /**
     @brief Implement Director and Scene init code here
     @return true   Initialize success, app continue
     @return false  Initailize failed, app terminate
     */
    virtual bool applicationDidFinishLaunching();
    
    /**
     @brief The function be called when the application enter background
     @param the pointer of application
     */
    virtual void applicationDidEnterBackground();
    
    /**
     @brief The function be called when the application enter foreground
     @param the pointer of application
     */
    virtual void applicationWillEnterForeground();
};

#endif  //__APP_DELEGATE_H__