#ifndef ChatOfPomelo_ChatScene_h
#define ChatOfPomelo_ChatScene_h

#include <cocos2d.h>
#include <cocos-ext.h>
#include "VisibleRect.h"
#include <pomelo.h>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class ChatScene : public cocos2d::Layer,public cocos2d::extension::EditBoxDelegate, public cocos2d::TextFieldDelegate
{
public:
    virtual bool init();
    //static coocs2d::Scene* scene();
    void menuCallback(cocos2d::Ref* sender);
    void backCallback(cocos2d::Ref* sender);
    void userCallback(cocos2d::Ref* sender);
    void sendCallback(cocos2d::Ref* sender);
    
    void updateUICallback(float delta);
    void addMessage(const char* event,void* data);
    static cocos2d::Layer* sharedMItemLayer();
    
    CREATE_FUNC(ChatScene);
    
    void setChannel(string channel);
    void setUser(string user);
    void setClient(pc_client_t* _client);
    void setMessageQueue(std::vector<string>* queue);
    void setUserQueue(std::vector<string>* queue);
    
    virtual void onEnter();
    //Depercated sooner or later
    virtual bool onTouchBegan(Touch* pTouch,Event* pEvent);
    virtual void onTouchMove(Touch* pTouch,Event* pEvent);
    virtual void onTouchEnded(Touch* pTouch,Event* pEvent);
    //New
    void setEventHadlers();
    
    virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox,const std::string& text);
    virtual void editBoxReturn(cocos2d::extension::EditBox* editBox);
    
    //TextFieldDelegate
    virtual bool onTextFieldAttachWithIME(TextFieldTTF* pSender);
    virtual bool onTextFieldDetachWithIME(TextFieldTTF* pSender);
    virtual bool onTextFieldInsertText(TextFieldTTF* pSender,const char* text,int nLen);
    virtual bool onTextFieldDeleteBackward(TextFieldTTF* pSender,const char* delText,int nLen);
private:
    Point m_tBeginPoint;
    Menu* m_pItemMenu;
    Layer* m_layer;
    TextFieldTTF* m_pTextField;
    cocos2d::LabelTTF* m_pLabelShowEditReturn;
    cocos2d::extension::EditBox* m_pEditName;
    cocos2d::LabelTTF* m_pChannelText;
    string s_text;
    string s_channel;
    string s_user;
    pc_client_t* client;
    std::vector<string>* messageQueue;
    std::vector<string>* userQueue;
};

#endif
