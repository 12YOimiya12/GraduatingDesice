#include "loding_wd.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFormLayout>

loding_wd::loding_wd(QWidget *parent)
    : QMainWindow(parent)
    , m_usernameEdit(nullptr)
    , m_passwordEdit(nullptr)
    , m_loginBtn(nullptr)
    , m_studentPanel(nullptr)
    , m_adminPanel(nullptr)
{
    initUI();
    
    if (!DatabaseManager::instance().initDatabase()) {
        QMessageBox::critical(this, "错误", "数据库初始化失败！");
        return;
    }
    
    DatabaseManager::instance().initSampleData();
}

loding_wd::~loding_wd()
{
}

void loding_wd::initUI()
{
    setWindowTitle("惠电 - 登录");
    resize(450, 350);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    QLabel *titleLabel = new QLabel("惠电宿舍电费充值管理系统");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    QLabel *subtitleLabel = new QLabel("用户登录");
    subtitleLabel->setStyleSheet("font-size: 16px; color: #7f8c8d;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(30);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    
    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("请输入用户名");
    m_usernameEdit->setText("2021001"); // 设置默认用户名
    m_usernameEdit->setStyleSheet("padding: 10px; font-size: 14px; border: 1px solid #bdc3c7; border-radius: 5px;");
    formLayout->addRow("用户名:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setText("123456"); // 设置默认密码
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setStyleSheet("padding: 10px; font-size: 14px; border: 1px solid #bdc3c7; border-radius: 5px;");
    formLayout->addRow("密码:", m_passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    mainLayout->addSpacing(20);
    
    m_loginBtn = new QPushButton("登录");
    m_loginBtn->setStyleSheet("padding: 12px; font-size: 16px; background-color: #3498db; color: white; border: none; border-radius: 5px;");
    connect(m_loginBtn, &QPushButton::clicked, this, &loding_wd::onLoginClicked);
    mainLayout->addWidget(m_loginBtn);
    
    QLabel *hintLabel = new QLabel("测试账号: 学生 2021001/123456, 管理员 admin/admin123");
    hintLabel->setStyleSheet("font-size: 12px; color: #95a5a6;");
    hintLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(hintLabel);
    
    mainLayout->addStretch();
}

void loding_wd::onLoginClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名和密码！");
        return;
    }
    
    UserInfo user;
    if (DatabaseManager::instance().authenticateUser(username, password, user)) {
        hide();
        
        if (user.role == 0) {
            m_adminPanel = new AdminPanel();
            m_adminPanel->setCurrentUser(user);
            connect(m_adminPanel, &AdminPanel::logout, this, &loding_wd::onShowLoginAgain);
            m_adminPanel->show();
        } else {
            m_studentPanel = new StudentPanel();
            m_studentPanel->setCurrentUser(user);
            connect(m_studentPanel, &StudentPanel::logout, this, &loding_wd::onShowLoginAgain);
            m_studentPanel->show();
        }
    } else {
        QMessageBox::warning(this, "错误", "用户名或密码错误！");
    }
}

void loding_wd::onShowLoginAgain()
{
    m_usernameEdit->clear();
    m_passwordEdit->clear();
    show();
}
