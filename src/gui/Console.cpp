#include "Console.h"

#include <QScrollBar>

namespace hrm
{

    Console::Console(QWidget *parent) :
        QPlainTextEdit(parent)
    {
        document()->setMaximumBlockCount(100);
        QPalette p = palette();
        p.setColor(QPalette::Base, Qt::black);
        p.setColor(QPalette::Text, Qt::white);
        setPalette(p);

        setMaximumHeight(200);

        setReadOnly(true);
        setTextInteractionFlags(Qt::NoTextInteraction);
    }

    Console::~Console()
    {
    }

    void Console::print(const QString &string)
    {
        insertPlainText(string + '\n');

        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

}
