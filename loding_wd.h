#ifndef LODING_WD_H
#define LODING_WD_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "databasemanager.h"
#include "studentpanel.h"
#include "adminpanel.h"

/**
 * @brief 登录窗口类
 * 
 * 提供用户登录界面，主要功能包括：
 * - 用户名和密码输入
 * - 用户身份验证
 * - 根据用户角色跳转到相应界面（学生端/管理员端）
 * - 支持重新显示登录界面
 */
class loding_wd : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit loding_wd(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~loding_wd();

private slots:
    /**
     * @brief 登录按钮点击槽函数
     */
    void onLoginClicked();
    
    /**
     * @brief 重新显示登录界面槽函数
     */
    void onShowLoginAgain();

private:
    /**
     * @brief 初始化用户界面
     */
    void initUI();
    
    QLineEdit *m_usernameEdit;    // 用户名输入框
    QLineEdit *m_passwordEdit;    // 密码输入框
    QPushButton *m_loginBtn;      // 登录按钮
    StudentPanel *m_studentPanel; // 学生端面板
    AdminPanel *m_adminPanel;     // 管理员端面板
};

#endif // LODING_WD_H
