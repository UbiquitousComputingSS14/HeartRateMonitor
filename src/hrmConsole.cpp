#include "hrmConsole.h"

#include <QScrollBar>

namespace hrm
{

    hrmConsole::hrmConsole(QWidget *parent) :
        QPlainTextEdit(parent)
    {
        document()->setMaximumBlockCount(100);
        QPalette p = palette();
        p.setColor(QPalette::Base, Qt::black);
        p.setColor(QPalette::Text, Qt::white);
        setPalette(p);
    }

    hrmConsole::~hrmConsole()
    {
    }

    void hrmConsole::print(const QString &string)
    {
        insertPlainText(string);

        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

}
