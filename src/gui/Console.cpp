#include "Console.h"

#include <QScrollBar>

namespace hrm
{

    const QString Console::INFO_HTML = "<font color=\"Aqua\">";
    const QString Console::END_HTML = "</font><br/>";

    Console::Console(QWidget *parent) :
        QPlainTextEdit(parent)
    {
        document()->setMaximumBlockCount(100);
        QPalette p = palette();
        p.setColor(QPalette::Base, Qt::black);
        p.setColor(QPalette::Text, Qt::white);
        setPalette(p);

        setReadOnly(true);
        setTextInteractionFlags(Qt::NoTextInteraction);
    }

    Console::~Console()
    {
    }

    void Console::print(const QString &string)
    {
        insertPlainText('\n' + string);

        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

    void Console::printInfo(const QString& string)
    {
        QString str = INFO_HTML + string + END_HTML;

        appendHtml(str);

        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

}
