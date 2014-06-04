#include "hrmMainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    hrm::hrmMainWindow hrm;

    hrm.show();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
