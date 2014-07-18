#include "ChatScene.h"
#include "LoginScene.h"
#include "UserScene.h"

USING_NS_CC;
using namespace std;

#define LINE_SPACE      50
#define TESTS_COUNT     5
#define FONT_NAME       "Thonburi"
#define FONT_SIZE       36
#define CHAT_LIMIT      20

std::string global_text[] =
{
    "Welcome To Pomelo Cocos2d-x Chat Demo",
    "This is a simple chat demo based on libpomelo and cocos2d-x",
    "You can send broadcast message directly",
    "You can also send private message by @user at the head of your messages",
    "Enjoy with it"
};

static Point s_tCurPos = Vec2::ZERO;
static int cur = 0;

void onSendCallback(pc_request_t* req,int status,json_t* resp)
{
    
}

bool ChatScene::init()
{
    if(!Layer::init())
    {
        return false;
    }
    Rect visibleRect = VisibleRect::getVisibleRect();
    m_tBeginPoint = Vec2(0, VisibleRect::top().y-50);
    auto backLabel = LabelTTF::create("Back", "Arial", 25);
    MenuItem* pBackMenuItem = MenuItemLabel::create(backLabel, std::bind(&ChatScene::backCallback, this, std::placeholders::_1));
    Menu* pBackMenu = Menu::create(pBackMenuItem, NULL);
    pBackMenu->setAnchorPoint(Point::ZERO);
    pBackMenu->setPosition(Vec2(VisibleRect::right().x+35, VisibleRect::top().y-20));
    this->addChild(pBackMenu, 10);
    
    auto pUserLabel = LabelTTF::create("Users", "Arial", 25);
    MenuItemLabel* pUserMenuItem = MenuItemLabel::create(pUserLabel, std::bind(&ChatScene::userCallback, this, std::placeholders::_1));
    Menu* pUserMenu = Menu::create(pUserMenuItem, NULL);
    pUserMenu->setAnchorPoint(Vec2::ZERO);
    pUserMenu->setPosition(Vec2(VisibleRect::right().x -40,VisibleRect::top().y -20));
    this->addChild(pUserMenu, 10);
    
    auto pSendLabel = LabelTTF::create("Send", "Arial", 25);
    MenuItemLabel* pSendMenuItem = MenuItemLabel::create(pSendLabel, std::bind(&ChatScene::sendCallback, this, std::placeholders::_1));
    Menu* pSendMenu = Menu::create(pSendMenuItem, NULL);
    pSendMenu->setAnchorPoint(Vec2::ZERO);
    pSendMenu->setPosition(Vec2(VisibleRect::right().x-45, 25));
    addChild(pSendMenu,10);
    
    this->m_pChannelText = LabelTTF::create(s_channel.c_str(), "Arial", 25);
    this->m_pChannelText->setPosition(Vec2(VisibleRect::center().x, VisibleRect::top().y - 20));
    addChild(this->m_pChannelText, 10);
    
    m_layer = Layer::create();
    
    for (int i = 0; i<visibleRect.size.height/LINE_SPACE; ++i) {
        LabelTTF* pLabel;
        if(i<TESTS_COUNT)
        {
            pLabel = LabelTTF::create(global_text[i].c_str(), "Arial", 23);
            pLabel->setDimensions(Size(visibleRect.size.width, 50));
        }
        else
        {
            pLabel = LabelTTF::create("", "Arial", 23);
            pLabel->setDimensions(Size(visibleRect.size.width, 50));
        }
        pLabel->setColor(Color3B(32, 32, 32));
        pLabel->setAnchorPoint(Vec2(0, 0));
        pLabel->setPosition(Vec2(VisibleRect::left().x+10, (m_tBeginPoint.y-(i+1)*LINE_SPACE)));
        m_layer->addChild(pLabel, 10, i);
    }
    
    m_layer->setContentSize(Size(Vec2(VisibleRect::getVisibleRect().size.width-30, (TESTS_COUNT+1)*(LINE_SPACE))));
    m_layer->setPosition(s_tCurPos);
    addChild(m_layer,2);
    
    LayerColor* pLayer1 = LayerColor::create(Color4B(255, 255, 255, 255), visibleRect.size.width, visibleRect.size.height);
    pLayer1->setCascadeColorEnabled(false);
    pLayer1->setPosition(Vec2(0,0));
    addChild(pLayer1,1);
    
    LayerColor* pLayer2 = LayerColor::create(Color4B(0, 0, 0, 255), visibleRect.size.width, 40);
    pLayer2->setCascadeColorEnabled(false);
    pLayer2->setPosition(Vec2(0, VisibleRect::top().y-40));
    addChild(pLayer2,8);
    
    LayerColor* pLayer3 = LayerColor::create(Color4B(72, 72, 72, 255), visibleRect.size.width, 50);
    pLayer3->setCascadeColorEnabled(false);
    pLayer3->setPosition(Vec2(0, 0));
    addChild(pLayer3,8);
    
    m_pTextField = TextFieldTTF::textFieldWithPlaceHolder("", FONT_NAME, FONT_SIZE);
    m_pTextField->setAnchorPoint(Vec2::ZERO);
    m_pTextField->setPosition(VisibleRect::left().x+10,5);
    //m_pTextField->setColorSpaceHolder(Color3B(255,0,0));
    m_pTextField->setString("1111111111111111111111111111111111111111111111111111111111");
    addChild(m_pTextField,12);
    
    m_pTextField->setDelegate(this);
    
    return true;
}

void ChatScene::onEnter()
{
    Layer::onEnter();
    CCLOG("Chat Scene onEnter");
    Director::getInstance()->getScheduler()->schedule(schedule_selector(ChatScene::updateUICallback), this, 0, false);
}

void ChatScene::updateUICallback(float delta)
{
    if (!messageQueue) {
        return;
    }
    unsigned int count = 0;
    bool flag = false;
    
    for (unsigned int i = cur; i < messageQueue->size(); i++,cur++) {
        std::string cstr = messageQueue->at(i);
        json_error_t error;
        json_t* pJson = json_loads(cstr.c_str(), JSON_DECODE_ANY, &error);
        std::string msg = json_string_value(json_object_get(pJson, "msg"));
        std::string from = json_string_value(json_object_get(pJson, "from"));
        if(from == s_user)
        {
            continue;
        }
        std::string target = json_string_value(json_object_get(pJson, "target"));
        if(target == "*")
        {
            target = "all";
        }
        std::string content = from + std::string(" says to ")+target+std::string(" :") + msg;
        
        auto pLabel = LabelTTF::create(content.c_str(), "Arial", 25);
        pLabel->setDimensions(Size(VisibleRect::getVisibleRect().size.width, 50));
        pLabel->setColor(Color3B(32, 32, 32));
        pLabel->setAnchorPoint(Vec2(0, 0));
        count = m_layer->getChildrenCount();
        pLabel->setPosition(Vec2(VisibleRect::left().x+10, (VisibleRect::top().y-(count+2)*LINE_SPACE)));
        m_layer->addChild(pLabel,10);
        flag = true;
    }
    if(flag)
    {
        m_layer->setPosition(Vec2(0, (count+2)*LINE_SPACE-VisibleRect::getVisibleRect().size.height+50));
        m_layer->setContentSize(Size(Vec2(VisibleRect::getVisibleRect().size.width-20, (count+1)*(LINE_SPACE))));
    }
}

void ChatScene::menuCallback(Ref* sender)
{
    Scene* pScene = LoginScene::scene();
    Director::getInstance()->replaceScene(pScene);
}

void ChatScene::backCallback(Ref* sender)
{
    Director::getInstance()->getScheduler()->pauseTarget(this);
    pc_client_destroy(client);
    //NOTE
    messageQueue->clear();
    userQueue->clear();
    cur = 0;
    Scene* pScene = LoginScene::scene();
    Director::getInstance()->replaceScene(pScene);
}

void ChatScene::userCallback(Ref* sender)
{
    Director::getInstance()->getScheduler()->pauseTarget(this);
    Scene* pScene = Scene::create();
    UserScene* pLayer = new UserScene();
    pLayer->setUserQueue(userQueue);
    if(pLayer&&pLayer->init())
    {
        pScene->addChild(pLayer);
        CCLOG("Director pushScene");
        Director::getInstance()->pushScene(pScene);
    }
    else
    {
        delete pLayer;
        pLayer = NULL;
    }
}

void ChatScene::sendCallback(Ref* sender)
{
    //CCLOG("%zd", m_layer->getChildrenCount());
    ssize_t count = m_layer->getChildrenCount();
    string text = m_pTextField->getString();
    string target = "";
    string content = "";
    if(text.length() > 1 && text[0]=='@')
    {
        for (unsigned int i =1; i<text.length(); i++) {
            if (text[i] != ' ')
            {
                target += text[i];
            }
            else
            {
                content = text.substr(i);
                break;
            }
        }
    }
    
    if(target == "")
    {
        target = "*";
        content = s_user + " says to all:" +text;
    }
    else
    {
        content = s_user + " says to " + target + ":" + content;
    }
    
    auto pLable = LabelTTF::create(content.c_str(), "Arial", 25);
    pLable->setDimensions(Size(VisibleRect::getVisibleRect().size.width, 50));
    pLable->setAnchorPoint(Vec2::ZERO);
    pLable->setPosition(Vec2(VisibleRect::getVisibleRect().size.width-20, (count+1)*(LINE_SPACE)));
    
    m_layer->addChild(pLable,10,count);
    m_layer->setPosition(Vec2(0, ((count+2)*LINE_SPACE-VisibleRect::getVisibleRect().size.height+50)));
    m_layer->setContentSize(Size(VisibleRect::getVisibleRect().size.width-20, (count+1)*(LINE_SPACE)));
    
    const char* route = "chat.chatHandler.send";
    json_t* pMsg = json_object();
    json_t* pStr = json_string(content.c_str());
    json_object_set(pMsg, "content", pStr);
    json_object_set(pMsg, "rid", json_string(s_channel.c_str()));
    json_object_set(pMsg, "from", json_string(s_user.c_str()));
    json_object_set(pMsg, "target", json_string(target.c_str()));
    
    m_pTextField->setString("");
    
    pc_request_t* request = pc_request_new();
    void (*on_send_cb)(pc_request_t* req,int status,json_t* resp) = onSendCallback;
    pc_request(client,request,route,pMsg,on_send_cb);
}

bool ChatScene::onTouchBegan(Touch* pTouch, Event* pEvent)
{
    m_tBeginPoint = pTouch->getLocation();
    return true;
}


void ChatScene::onTouchMove(Touch* pTouch, Event* pEvent)
{
    ssize_t count = m_layer->getChildrenCount();
    Point touchLocation = pTouch->getLocation();
    float nMoveY = touchLocation.y - m_tBeginPoint.y;
    Point curPos = m_layer->getPosition();
    Point nextPos = Point(curPos.x, curPos.y+nMoveY);
    if(nextPos.y<0.0f)
    {
        m_layer->setPosition(Vec2::ZERO);
    }
    if(nextPos.y>((count+1)*LINE_SPACE-VisibleRect::getVisibleRect().size.height))
    {
        m_layer->setPosition(Vec2(0,((count+1)*LINE_SPACE-VisibleRect::getVisibleRect().size.height+50)));
        return;
    }
    m_layer->setPosition(nextPos);
    m_tBeginPoint = touchLocation;
    s_tCurPos = nextPos;
}

void ChatScene::onTouchEnded(Touch* pTouch, Event* pEvent)
{
    Point endPoint = pTouch->getLocation();
    float delta = 0.0f;
    if(endPoint.x>delta && endPoint.x - VisibleRect::getVisibleRect().size.width < delta && endPoint.y -50 <delta)
    {
        m_pTextField->attachWithIME();
    }
    else
    {
        m_pTextField->detachWithIME();
    }
}

void ChatScene::setEventHadlers()
{
    EventDispatcher* pDispatcher = Director::getInstance()->getEventDispatcher();
    EventListenerTouchOneByOne* pEventListener = EventListenerTouchOneByOne::create();
    pEventListener->setSwallowTouches(true);
    
    pEventListener->onTouchBegan = CC_CALLBACK_2(ChatScene::onTouchBegan, this);
    pEventListener->onTouchMoved = CC_CALLBACK_2(ChatScene::onTouchMoved, this);
    pEventListener->onTouchEnded = CC_CALLBACK_2(ChatScene::onTouchEnded, this);
    
    pDispatcher->addEventListenerWithSceneGraphPriority(pEventListener, this);
}

void ChatScene::editBoxEditingDidBegin(EditBox* editBox)
{
    CCLOG("editBox %p DidBegin !",editBox);
}

void ChatScene::editBoxEditingDidEnd(cocos2d::extension::EditBox *editBox)
{
    CCLOG("Editbox %p DidEnd !",editBox);
}

void ChatScene::editBoxTextChanged(cocos2d::extension::EditBox *editBox, const std::string &text)
{
    s_text = text;
    CCLOG("Editbox %p TextChanged, text: %s",editBox, text.c_str());
}

void ChatScene::editBoxReturn(cocos2d::extension::EditBox *editBox)
{
    CCLOG("Editbox %p was returned !",editBox);
}

void ChatScene::setChannel(string channel)
{
    s_channel = channel;
}

void ChatScene::setUser(string user)
{
    s_user = user;
}

void ChatScene::setClient(pc_client_t* _client)
{
    client = _client;
}

void ChatScene::setMessageQueue(std::vector<string> *queue)
{
    messageQueue = queue;
}

void ChatScene::setUserQueue(std::vector<string> *queue)
{
    userQueue = queue;
}

bool ChatScene::onTextFieldAttachWithIME(cocos2d::TextFieldTTF *pSender)
{
    return false;
}


bool ChatScene::onTextFieldDetachWithIME(cocos2d::TextFieldTTF *pSender)
{
    return false;
}

bool ChatScene::onTextFieldInsertText(cocos2d::TextFieldTTF *pSender, const char *text, int nLen)
{
    if('\n' == *text)
    {
        this->sendCallback(NULL);
        return false;
    }
    
    if(pSender->getCharCount()>=CHAT_LIMIT)
    {
        return true;
    }
    
    return false;
}

bool ChatScene::onTextFieldDeleteBackward(cocos2d::TextFieldTTF *pSender, const char *delText, int nLen)
{
    return false;
}