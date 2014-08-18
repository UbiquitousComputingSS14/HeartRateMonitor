#ifndef HRM_CONSOLE_H
#define HRM_CONSOLE_H

#include <QPlainTextEdit>
#include <QString>

namespace hrm
{

    class Console : public QPlainTextEdit
    {
            Q_OBJECT

        private:
            static const QString INFO_HTML;
            static const QString END_HTML;

        public:
            Console(QWidget *parent = 0);
            virtual ~Console();

            void print(const QString &string);
            void printInfo(const QString &string);
    };

}

#endif
