#ifndef ChatOfPomelo_LoginScene_h
#define ChatOfPomelo_LoginScene_h

#include <cocos2d.h>
#include <cocos-ext.h>
#include <pomelo.h>
#include "VisibleRect.h"

class LoginScene:public cocos2d::Layer,public cocos2d::extension::EditBoxDelegate
{
public:
    virtual bool init();
    static cocos2d::Scene* scene();
    static LoginScene* create();
    
    //cocos2d-x schedule handle callback
    void dispatchLoginCallbacks(float delta);
    
    //cocos2d-x event callback
    void onLogin(cocos2d::Ref* pSender);
    virtual void onEnter();
    virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox,const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::EditBox* editBox);
    
    //pomelo handle
    void doLogin();
    //pomelo event callback
    static void onChatCallback(pc_client_t* client,const char* event, void* data);
    static void onAddCallback(pc_client_t* client,const char* event, void* data);
    static void onLeaveCallback(pc_client_t* client, const char* event,void* data);
    static void onDisconnectCallback(pc_client_t* client, const char* event,void* data);
    static void requestGateCallback(pc_request_t* req, int status, json_t *resp);
    static void requestConnectorCallback(pc_request_t* req, int status, json_t* resp);
private:
    std::string s_EditName;
    std::string s_EditChannel;
    cocos2d::extension::EditBox* m_pEditName;
    cocos2d::extension::EditBox* m_pEditChannel;
};

#endif
