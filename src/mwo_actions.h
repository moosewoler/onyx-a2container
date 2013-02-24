#ifndef _MWO_ACTIONS_H_
#define _MWO_ACTIONS_H_

#include "onyx/base/base.h"
#include "onyx/ui/context_dialog_base.h"

using namespace ui;

enum MwoActionsType{INVALID=-1, TEST1=0, TEST2=1, TEST3=2, DITHER=3};

class MwoActions :public BaseActions
{

public:
    MwoActions();
    ~MwoActions(){}
    void generateActions();
    QAction * action(const MwoActionsType action);
    MwoActionsType selected();
};
#endif
