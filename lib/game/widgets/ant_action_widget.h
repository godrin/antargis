#ifndef __ANT_ACTION_WIDGET_H
#define __ANT_ACTION_WIDGET_H

#include "ag_widget.h"

#include <map>

class AntActionWidget:public AGWidget {
public:
    enum Action {FIGHT,HARVEST,RECRUIT,INVENT,DISMISS,REST};

    class Handler {
    public:
        virtual void actionClicked(Action a);
    };


    AntActionWidget(AGWidget *pParent,const AGRect2 &r);
    virtual ~AntActionWidget() throw();

    void setHandler(Handler *pHandler);

    void show(const std::vector<Action> &actions,const AGVector2 &target);

private:
    std::map<AGWidget*,Action> actionButtons;
    Handler *handler;
};

#endif
