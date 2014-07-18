#include "UserScene.h"

bool UserScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size winSize = CCDirector::getInstance()->getWinSize();
    
    TableView *tableView = TableView::create(this, Size(VisibleRect::getVisibleRect().size.width, 60));
    
    tableView = TableView::create(this, Size(VisibleRect::getVisibleRect().size.width, VisibleRect::getVisibleRect().size.height));
    tableView->setDirection(ScrollView::Direction::VERTICAL);
    tableView->setPosition(Point::ZERO);
    tableView->setDelegate(this);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    this->addChild(tableView);
    tableView->reloadData();
    
    // Back Menu
    MenuItem* itemBack = MenuItemFont::create("Back",std::bind(&UserScene::backCallback, this,std::placeholders::_1));
    itemBack->setPosition(Point(VisibleRect::rightBottom().x - 50, VisibleRect::rightBottom().y + 25));
    Menu *menuBack = Menu::create(itemBack, NULL);
    menuBack->setPosition(Point::ZERO);
    addChild(menuBack);
    
    return true;
}

void UserScene::backCallback(cocos2d::Ref *sender)
{
    Director::getInstance()->popScene();
}

void UserScene::tableCellTouched(cocos2d::extension::TableView *table, cocos2d::extension::TableViewCell *cell)
{
    //...
}

Size UserScene::cellSizeForTable(cocos2d::extension::TableView *table)
{
    return Size(60,60);
}

TableViewCell* UserScene::tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)
{
    std::string string = m_userQueue->at(idx);
    
    TableViewCell *cell = table->dequeueCell();
    if (!cell)
    {
        cell = new TableViewCell();
        cell->autorelease();
        Sprite *sprite = Sprite::create("Icon.png");
        sprite->setAnchorPoint(Point::ZERO);
        sprite->setPosition(Point::ZERO);
        cell->addChild(sprite);
        
        auto *label = LabelTTF::create(string.c_str(), "Helvetica", 20.0);
        label->setPosition(Point::ZERO);
        label->setAnchorPoint(Point::ZERO);
        label->setTag(123);
        cell->addChild(label);
    }
    else
    {
        auto* label = (LabelTTF*)cell->getChildByTag(123);
        label->setString(string.c_str());
    }
    
    return cell;
}

ssize_t UserScene::numberOfCellsInTableView(cocos2d::extension::TableView *table)
{
    return m_userQueue->size();
}

void UserScene::setUserQueue(std::vector<std::string> *queue)
{
    m_userQueue = queue;
}