#ifndef ChatOfPomelo_UserScene_h
#define ChatOfPomelo_UserScene_h

#include <cocos2d.h>
#include <cocos-ext.h>
#include "VisibleRect.h"

USING_NS_CC;
USING_NS_CC_EXT;

class UserScene: public cocos2d::Layer, public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
{
public:
    virtual bool init();
    
    void backCallback(cocos2d::Ref* sender);
    
    CREATE_FUNC(UserScene);
    
    void setUserQueue(std::vector<std::string>* queue);
    //Implementation
    virtual void scrollViewDidScroll(ScrollView* view) {};
    virtual void scrollViewDidZoom(ScrollView* view) {};
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
    virtual Size cellSizeForTable(TableView* table);
    virtual TableViewCell* tableCellAtIndex(TableView* table,ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView* table);
private:
    std::vector<std::string>* m_userQueue;
};

#endif
