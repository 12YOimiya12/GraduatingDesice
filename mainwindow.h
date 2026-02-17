#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief 主窗口类
 * 
 * 应用程序的主窗口，负责：
 * - 初始化应用程序界面
 * - 显示登录窗口
 * - 管理应用程序生命周期
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();

private:
    Ui::MainWindow *ui; // UI界面对象
};
#endif // MAINWINDOW_H
