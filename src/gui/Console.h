#ifndef HRM_CONSOLE_H
#define HRM_CONSOLE_H

#include <QPlainTextEdit>
#include <QString>

namespace hrm
{

    class Console : public QPlainTextEdit
    {
            Q_OBJECT

        public:
            Console(QWidget *parent = 0);
            virtual ~Console();

            void print(const QString &string);
    };

}

#endif
