#include <QApplication>
#include <QGridLayout>
#include <QMainWindow>
#include <memory>

#include "view/MainWindow.h"

// #include "dummypresenter.h"
// #include "dummyview.h"

// std::pair<std::unique_ptr<DummyPresenter>, DummyView*> createDummyWindow(void) {
//     auto view = std::make_unique<DummyView>();
//     auto viewPtr = view.get();
//     auto model = std::make_unique<DummyModel>();
//     auto presenter = std::make_unique<DummyPresenter>(std::move(view), std::move(model));

//     // NOTE: viewPtr remains valid even though view is moved, this is because it is a
//     //       pointer to the object on the heap, not to unique_ptr itself.
//     //       doing view.get() in the call to make_pair would however,
//     //       be undefined behaviour because the unique_ptr view has been moved
//     //       'use after move' is not supposed to be well-defined.

//     return std::make_pair(std::move(presenter), viewPtr);
// } // namespace

auto main(int argc, char *argv[]) -> int {
    QApplication app(argc, argv);

    // auto window_ = createDummyWindow();
    auto window = std::make_unique<MainWindow>();
    auto *window_ptr = window.get();

    window_ptr->show();

    return QApplication::exec();
}
