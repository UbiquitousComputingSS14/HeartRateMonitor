#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    hrm::MainWindow hrm;

    hrm.show();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
