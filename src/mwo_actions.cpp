#include "mwo_actions.h"

MwoActions::MwoActions() : BaseActions()
{
    // category()返回一个QAction对象实例
    category()->setText(QCoreApplication::tr("Test"));
    category()->setIcon(QIcon(QPixmap(":/images/test.png")));
}

QAction* MwoActions::action(const MwoActionsType action)
{
    for(int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (actions_.at(i)->data().toInt() == action)
        {
            return actions_.at(i).get();
        }
    }
    return 0;
}

void MwoActions::generateActions()
{

    actions_.clear();
    shared_ptr<QAction> test1(new QAction(exclusiveGroup()));
    test1->setCheckable(true);
    test1->setText(QCoreApplication::tr("Test1"));
    test1->setIcon(QIcon(QPixmap(":/images/guake.png")));
    test1->setData(TEST1);
    actions_.push_back(test1);

    shared_ptr<QAction> test2(new QAction(exclusiveGroup()));
    test2->setCheckable(true);
    test2->setText(QCoreApplication::tr("Test2"));
    test2->setIcon(QIcon(QPixmap(":/images/guake.png")));
    test2->setData(TEST2);
    actions_.push_back(test2);

    shared_ptr<QAction> test3(new QAction(exclusiveGroup()));
    test3->setCheckable(true);
    test3->setText(QCoreApplication::tr("Test3"));
    test3->setIcon(QIcon(QPixmap(":/images/guake.png")));
    test3->setData(TEST3);
    actions_.push_back(test3);

    shared_ptr<QAction> dither(new QAction(exclusiveGroup()));
    dither->setCheckable(true);
    dither->setText(QCoreApplication::tr("Dither"));
    dither->setIcon(QIcon(QPixmap(":/images/guake.png")));
    dither->setData(DITHER);
    actions_.push_back(dither);
}

MwoActionsType MwoActions::selected()
{
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<MwoActionsType>(act->data().toInt());
    }
    return INVALID;
}

