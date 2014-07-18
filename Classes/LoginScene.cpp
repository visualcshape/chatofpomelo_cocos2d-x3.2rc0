#include "LoginScene.h"
#include "ChatScene.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#define GATE_HOST "127.0.0.1"
#define GATE_PORT 3014

static const char* connectorHost = "";
static int connectorPort = 0;
static string username = "";
static string channel = "";
static pc_client_t* pomelo_client = NULL;
static std::vector<string>* messageQueue = NULL;
static std::vector<string>* userQueue = NULL;
static int error = 0;
static json_t* userList = NULL;

static void on_close(pc_client_t* client,const char* event,void* data);

LoginScene* LoginScene::create()
{
    LoginScene* pRet = new LoginScene();
    if(pRet&&pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

void LoginScene::onChatCallback(pc_client_t *client, const char *event, void *data)
{
    json_t* json = (json_t*)data;
    const char* pMsg = json_dumps(json, 0);
    CCLOG("%s %s",event,pMsg);
    userQueue->push_back(string(pMsg));
}

void LoginScene::onAddCallback(pc_client_t *client, const char *event, void *data)
{
    if(!userQueue)
    {
        return;
    }
    json_t* json = (json_t*)data;
    json_t* user = json_object_get(json, "user");
    const char* pMsg = json_string_value(user);
    CCLOG("%s %s",event,pMsg);
    userQueue->push_back(string(pMsg));
    return;
}

void LoginScene::onLeaveCallback(pc_client_t *client, const char *event, void *data)
{
    if(!userQueue)
    {
        return;
    }
    json_t* json = (json_t*)data;
    json_t* user = json_object_get(json, "user");
    const char* pMsg = json_string_value(user);
    CCLOG("%s %s",event,pMsg);
    for(unsigned int i = 0 ; i<userQueue->size() ; i++)
    {
        string cstr = userQueue->at(i);
        if(strcmp(cstr.c_str(), pMsg))
        {
            userQueue->erase(userQueue->begin()+i);
            break;
        }
    }
    return;
}

void LoginScene::onDisconnectCallback(pc_client_t *client, const char *event, void *data)
{
    CCLOG("%s",event);
    return;
}

void LoginScene::requestGateCallback(pc_request_t *req, int status, json_t *resp)
{
    if(status==-1)
    {
        CCLOG("Faild to send request to server.\n");
    }
    else if(status==0)
    {
        connectorHost = json_string_value(json_object_get(resp, "host"));
        connectorPort = json_number_value(json_object_get(resp, "port"));
        CCLOG("%s %d",connectorHost,connectorPort);
        pc_client_t* client = pc_client_new();
        
        struct sockaddr_in address;
        
        memset(&address, 0, sizeof(struct sockaddr_in));
        address.sin_family = AF_INET;
        address.sin_port = htons(connectorPort);
        address.sin_addr.s_addr = inet_addr(connectorHost);
        
        //add pomelo event listener
        void (*on_disconnect)(pc_client_t* client,const char* event,void* data) = &LoginScene::onDisconnectCallback;
        void (*on_chat)(pc_client_t* client,const char* event,void* data) = &LoginScene::onChatCallback;
        void (*on_add)(pc_client_t* client,const char* event,void* data) = &LoginScene::onAddCallback;
        void (*on_leave)(pc_client_t* client,const char* event,void* data) = &LoginScene::onLeaveCallback;
        
        pc_add_listener(client, "disconnect", on_disconnect);
        pc_add_listener(client, "onChat", on_chat);
        pc_add_listener(client, "onAdd", on_add);
        pc_add_listener(client, "onLeave", on_leave);
        
        //try to connect to server
        if(pc_client_connect(client, &address))
        {
            CCLOG("Fail to connect server.\n");
            pc_client_destroy(client);
            return;
        }
        
        const char* route = "connector.entryHandler.enter";
        json_t* msg = json_object();
        json_t* str = json_string(username.c_str());
        json_t* channel_str = json_string(channel.c_str());
        json_object_set(msg, "username", str);
        json_object_set(msg, "rid", channel_str);
        //decref for json object
        json_decref(str);
        json_decref(channel_str);
        
        pc_request_t* request = pc_request_new();
        void (*connect_cb)(pc_request_t* req,int status,json_t* resp) = &LoginScene::requestConnectorCallback;
        pc_request(client,request,route,msg,connect_cb);
    }
    
    //release relative resource with pc_request_t
    json_t* pc_msg = req->msg;
    pc_client_t* pc_client = req->client;
    json_decref(pc_msg);
    pc_request_destroy(req);
    
    pc_client_stop(pc_client);
}

void LoginScene::requestConnectorCallback(pc_request_t *req, int status, json_t *resp)
{
    error = 0;
    if(status == -1)
    {
        CCLOG("Fail to send request to server.\n");
    }
    else if(status==0)
    {
        char* json_str = json_dumps(resp, 0);
        CCLOG("server response:%s \n",json_str);
        json_t* users = json_object_get(resp,"users");
        if (json_object_get(resp, "error")) {
            error =1;
            CCLOG("Connect error %s",json_str);
            free(json_str);
            return;
        }
        pomelo_client = req->client;
        for (unsigned int i = 0; i<json_array_size(users); i++) {
            json_t* val = json_array_get(users, i);
            userQueue->push_back(string(json_string_value(val)));
        }
    }
    
    //release relatice resource with pc_request_t
    json_t* msg = req->msg;
    json_decref(msg);
    pc_request_destroy(req);
}

//disconnect event callback
void on_close(pc_client_t* client,const char* event,void* data)
{
    CCLOG("Client closed:%d.\n",client->state);
}

void LoginScene::doLogin()
{
    const char *ip = GATE_HOST;
    int port = GATE_PORT;
    
    pc_client_t *client = pc_client_new();
    
    struct sockaddr_in address;
    
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);
    
    // try to connect to server.
    if(pc_client_connect(client, &address)) {
        CCLOG("fail to connect server.\n");
        pc_client_destroy(client);
        return ;
    }
	
	// add some event callback.
	pc_add_listener(client, PC_EVENT_DISCONNECT, on_close);
    
    const char *route = "gate.gateHandler.queryEntry";
    json_t *msg = json_object();
    json_t *str = json_string(username.c_str());
    json_object_set(msg, "uid", str);
    // decref for json object
    json_decref(str);
    
    pc_request_t *request = pc_request_new();
    void (*on_request_gate_cb)(pc_request_t *req, int status, json_t *resp) = &LoginScene::requestGateCallback;
    pc_request(client, request, route, msg, on_request_gate_cb);
    
    // main thread has nothing to do and wait until child thread return.
    pc_client_join(client);
    
    // release the client
    pc_client_destroy(client);
}

Scene* LoginScene::scene()
{
    Scene* scene = NULL;
    
    do
    {
        scene = Scene::create();
        
        LoginScene* layer = LoginScene::create();
        
        scene->addChild(layer);
    }while (0);
    
    return scene;
}

void LoginScene::onEnter()
{
    Layer::onEnter();
    CCLOG("Login onEnter");
    pomelo_client = NULL;
    messageQueue = new std::vector<string>;
    
    userQueue = new std::vector<string>;
    
    Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginScene::dispatchLoginCallbacks), this, 0, false);
}


bool LoginScene::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!Layer::init());
        Point visibleOrigin = Director::getInstance()->getOpenGLView()->getVisibleOrigin();
        Size visibleSize = Director::getInstance()->getOpenGLView()->getVisibleSize();
        //top
        Size editBoxSize = Size(visibleSize.width-100, 60);
        m_pEditName = EditBox::create(editBoxSize, Scale9Sprite::create("green_edit.png"));
        m_pEditName->setPosition(Vec2(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height*3/4));
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        m_pEditName->setFont("Paint Boy", 25);
#else
        m_pEditName->setFont("Arial",25);
#endif
        m_pEditName->setColor(Color3B::BLACK);
        m_pEditName->setPlaceHolder("Name");
        m_pEditName->setPlaceholderFontColor(Color3B::WHITE);
        m_pEditName->setMaxLength(8);
        m_pEditName->setReturnType(EditBox::KeyboardReturnType::DONE);
        m_pEditName->setDelegate(this);
        this->addChild(m_pEditName,1);
        //middle
        m_pEditChannel = EditBox::create(editBoxSize, Scale9Sprite::create("green_edit.png"));
        m_pEditChannel->setPosition(Vec2(visibleOrigin.x+visibleSize.width/2, visibleOrigin.y+visibleSize.height/2));
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        m_pEditChannel->setFont("American Typewriter", 30);
#else
        m_pEditChannel->setFont("Arial",25);
#endif
        m_pEditChannel->setFontColor(Color3B::BLACK);
        m_pEditChannel->setPlaceHolder("Channel:");
        m_pEditChannel->setPlaceholderFontColor(Color3B::WHITE);
        m_pEditChannel->setMaxLength(8);
        m_pEditChannel->setReturnType(EditBox::KeyboardReturnType::DONE);
        m_pEditChannel->setDelegate(this);
        this->addChild(m_pEditChannel, 1);
        // 1. Add a menu item with "X" image, which is clicked to quit the program.
        auto label = LabelTTF::create("Login", "Arial", 50);
        //#endif
        MenuItem *pMenuItem = MenuItemLabel::create(label, std::bind(&LoginScene::onLogin, this,std::placeholders::_1));
        Menu *pMenu = Menu::create(pMenuItem, NULL);
        pMenu->setPosition( Point::ZERO );
        pMenuItem->setPosition( Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 4) );
        //m_pEditEmail->setAnchorPoint(ccp(0.5, 1.0f));
        this->addChild(pMenu, 1);
        auto pLabel = LabelTTF::create("pomelo-cocos2dchat", "Arial", 30);
        //CC_BREAK_IF(! pLabel);
        // Get window size and place the label upper.
        Size size = Director::getInstance()->getWinSize();
        pLabel->setPosition(Vec2(size.width / 2, size.height - 30));
        // Add the label to HelloWorld layer as a child layer.
        this->addChild(pLabel, 1);
        // 3. Add add a splash screen, show the cocos2d splash image.
        //Sprite *pSprite = Sprite::create("HelloWorld.png");
        //CC_BREAK_IF(! pSprite);
        // Place the sprite on the center of the screen
        //pSprite->setPosition(Vec2(size.width / 2, size.height / 2));
        // Add the sprite to HelloWorld layer as a child layer.
        //this->addChild(pSprite, 0);
        
        bRet = true;
    }while (0);
    return bRet;
}

void LoginScene::dispatchLoginCallbacks(float delta)
{
    if(pomelo_client == NULL || error == 1) {
        return;
    }
    
    Director::getInstance()->getScheduler()->pauseTarget(this);
    
    Scene* pScene = Scene::create();
    ChatScene* pLayer = new ChatScene();
    pLayer->setChannel(channel);
    pLayer->setUser(username);
    pLayer->setClient(pomelo_client);
    pLayer->setUserQueue(userQueue);
    pLayer->setMessageQueue(messageQueue);
    
    CCLOG("init player");
    if(pLayer && pLayer->init()) {
        //pLayer->autorelease();
        pScene->addChild(pLayer);
        CCLOG("director replaceScene");
        CCDirector::getInstance()->replaceScene(TransitionFade::create(1, pScene));
    } else {
        delete pLayer;
        pLayer = NULL;
    }
}

void LoginScene::onLogin(cocos2d::Ref *pSender)
{
    doLogin();
}

void LoginScene::editBoxEditingDidBegin(cocos2d::extension::EditBox *editBox)
{
    CCLOG("...");
}

void LoginScene::editBoxEditingDidEnd(cocos2d::extension::EditBox *editBox)
{
    CCLOG("...");
}

void LoginScene::editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string &text)
{
    if (editBox == m_pEditName) {
        username = text;
        CCLOG("name editBox %p TextChanged, text: %s ", editBox, text.c_str());
    } else {
        channel = text;
        CCLOG("channel editBox %p TextChanged, text: %s ", editBox, text.c_str());
    }
}

void LoginScene::editBoxReturn(cocos2d::extension::EditBox *editBox)
{
    CCLOG("...");
}