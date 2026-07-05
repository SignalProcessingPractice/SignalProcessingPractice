///
/// @file MainWindow.h
///
#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    auto operator=(const MainWindow&) -> MainWindow& = delete;
    MainWindow(MainWindow&&) = delete;
    auto operator=(MainWindow&&) -> MainWindow& = delete;

private:
    Ui::MainWindow* ui;
};
