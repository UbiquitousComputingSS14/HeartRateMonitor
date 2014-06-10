#ifndef HRM_CONSOLE_H
#define HRM_CONSOLE_H

#include <QPlainTextEdit>
#include <QString>

namespace hrm
{

    class hrmConsole : public QPlainTextEdit
    {
            Q_OBJECT

        public:
            hrmConsole(QWidget *parent = 0);
            virtual ~hrmConsole();

            void print(const QString &string);
    };

}

#endif
