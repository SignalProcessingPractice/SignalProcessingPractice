///
/// @file main.cpp
///

#include <QApplication>

#include "presenter/MainPresenter.h"
#include "view/MainWindow.h"

auto main(int argc, char* argv[]) -> int {
    QApplication app(argc, argv);

    MainWindow window;
    MainPresenter presenter(&window);

    window.show();

    return QApplication::exec();
}
