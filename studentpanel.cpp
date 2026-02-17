#include "studentpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

StudentPanel::StudentPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_userInfoLabel(nullptr)
    , m_balanceLabel(nullptr)
    , m_electricityTable(nullptr)
    , m_rechargeTable(nullptr)
    , m_webQueryBtn(nullptr)
    , m_electricityQuery(nullptr)
{
    initUI();
}

StudentPanel::~StudentPanel()
{
}

void StudentPanel::initUI()
{
    setWindowTitle("惠电 - 学生端");
    resize(900, 650);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QLabel *titleLabel = new QLabel("惠电宿舍电费充值管理系统 - 学生端");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout *userInfoLayout = new QHBoxLayout();
    
    m_userInfoLabel = new QLabel();
    m_userInfoLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    userInfoLayout->addWidget(m_userInfoLabel);
    
    userInfoLayout->addStretch();
    
    m_webQueryBtn = new QPushButton("网页电费查询");
    m_webQueryBtn->setStyleSheet("padding: 8px 16px; background-color: #9b59b6; color: white; border: none; border-radius: 4px;");
    connect(m_webQueryBtn, &QPushButton::clicked, this, &StudentPanel::onWebQueryClicked);
    userInfoLayout->addWidget(m_webQueryBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPanel::onRefreshClicked);
    userInfoLayout->addWidget(refreshBtn);
    
    QPushButton *logoutBtn = new QPushButton("退出登录");
    logoutBtn->setStyleSheet("padding: 8px 16px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
    connect(logoutBtn, &QPushButton::clicked, this, &StudentPanel::onLogoutClicked);
    userInfoLayout->addWidget(logoutBtn);
    
    mainLayout->addLayout(userInfoLayout);
    
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #ddd; } QTabBar::tab { padding: 10px 20px; }");
    
    QWidget *balanceTab = new QWidget();
    QVBoxLayout *balanceLayout = new QVBoxLayout(balanceTab);
    
    QLabel *balanceTitle = new QLabel("电费余额");
    balanceTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    balanceLayout->addWidget(balanceTitle);
    
    m_balanceLabel = new QLabel();
    m_balanceLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #27ae60; padding: 20px; background-color: #ecf0f1; border-radius: 8px;");
    m_balanceLabel->setAlignment(Qt::AlignCenter);
    balanceLayout->addWidget(m_balanceLabel);
    
    QPushButton *rechargeBtn = new QPushButton("立即充值");
    rechargeBtn->setStyleSheet("padding: 12px 30px; font-size: 16px; background-color: #27ae60; color: white; border: none; border-radius: 6px;");
    rechargeBtn->setMaximumWidth(200);
    connect(rechargeBtn, &QPushButton::clicked, this, &StudentPanel::onRechargeClicked);
    balanceLayout->addWidget(rechargeBtn, 0, Qt::AlignCenter);
    
    balanceLayout->addStretch();
    
    tabWidget->addTab(balanceTab, "余额查询");
    
    QWidget *electricityTab = new QWidget();
    QVBoxLayout *electricityLayout = new QVBoxLayout(electricityTab);
    
    QLabel *electricityTitle = new QLabel("电费使用记录");
    electricityTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    electricityLayout->addWidget(electricityTitle);
    
    m_electricityTable = new QTableWidget();
    m_electricityTable->setColumnCount(5);
    m_electricityTable->setHorizontalHeaderLabels({"时间", "宿舍", "用电量(kWh)", "费用(元)", "备注"});
    m_electricityTable->horizontalHeader()->setStretchLastSection(true);
    m_electricityTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_electricityTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    electricityLayout->addWidget(m_electricityTable);
    
    tabWidget->addTab(electricityTab, "使用记录");
    
    QWidget *rechargeTab = new QWidget();
    QVBoxLayout *rechargeLayout = new QVBoxLayout(rechargeTab);
    
    QLabel *rechargeTitle = new QLabel("充值记录");
    rechargeTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    rechargeLayout->addWidget(rechargeTitle);
    
    m_rechargeTable = new QTableWidget();
    m_rechargeTable->setColumnCount(6);
    m_rechargeTable->setHorizontalHeaderLabels({"时间", "学号", "宿舍", "充值金额(元)", "充值后余额(元)", "操作人"});
    m_rechargeTable->horizontalHeader()->setStretchLastSection(true);
    m_rechargeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rechargeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rechargeLayout->addWidget(m_rechargeTable);
    
    tabWidget->addTab(rechargeTab, "充值记录");
    
    connect(tabWidget, &QTabWidget::currentChanged, this, &StudentPanel::onTabChanged);
    
    mainLayout->addWidget(tabWidget);
}

void StudentPanel::setCurrentUser(const UserInfo& user)
{
    m_currentUser = user;
    loadUserInfo();
    loadBalanceInfo();
    loadElectricityRecords();
    loadRechargeRecords();
}

void StudentPanel::loadUserInfo()
{
    QString info = QString("欢迎, %1 | 学号: %2 | 宿舍: %3")
                       .arg(m_currentUser.name)
                       .arg(m_currentUser.studentId)
                       .arg(m_currentUser.dormitory);
    m_userInfoLabel->setText(info);
}

void StudentPanel::loadBalanceInfo()
{
    UserInfo updatedUser = DatabaseManager::instance().getUserById(m_currentUser.id);
    m_currentUser = updatedUser;
    
    QString balanceText = QString("¥ %1").arg(m_currentUser.balance, 0, 'f', 2);
    m_balanceLabel->setText(balanceText);
    
    if (m_currentUser.balance < 50) {
        m_balanceLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #e74c3c; padding: 20px; background-color: #ecf0f1; border-radius: 8px;");
    } else {
        m_balanceLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #27ae60; padding: 20px; background-color: #ecf0f1; border-radius: 8px;");
    }
}

void StudentPanel::loadElectricityRecords()
{
    QList<ElectricityRecord> records = DatabaseManager::instance().getElectricityRecordsByUser(m_currentUser.id);
    
    m_electricityTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const ElectricityRecord& record = records[i];
        m_electricityTable->setItem(i, 0, new QTableWidgetItem(record.recordTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_electricityTable->setItem(i, 1, new QTableWidgetItem(record.dormitory));
        m_electricityTable->setItem(i, 2, new QTableWidgetItem(QString::number(record.usage, 'f', 2)));
        m_electricityTable->setItem(i, 3, new QTableWidgetItem(QString::number(record.cost, 'f', 2)));
        m_electricityTable->setItem(i, 4, new QTableWidgetItem(record.remark));
    }
}

void StudentPanel::loadRechargeRecords()
{
    QList<RechargeRecord> records = DatabaseManager::instance().getRechargeRecordsByUser(m_currentUser.id);
    
    m_rechargeTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const RechargeRecord& record = records[i];
        m_rechargeTable->setItem(i, 0, new QTableWidgetItem(record.rechargeTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_rechargeTable->setItem(i, 1, new QTableWidgetItem(record.studentId));
        m_rechargeTable->setItem(i, 2, new QTableWidgetItem(record.dormitory));
        m_rechargeTable->setItem(i, 3, new QTableWidgetItem(QString::number(record.amount, 'f', 2)));
        m_rechargeTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.balanceAfter, 'f', 2)));
        m_rechargeTable->setItem(i, 5, new QTableWidgetItem(record.operatorName));
    }
}

void StudentPanel::onLogoutClicked()
{
    emit logout();
    close();
}

void StudentPanel::onRechargeClicked()
{
    bool ok;
    double amount = QInputDialog::getDouble(this, "充值", "请输入充值金额(元):", 50, 10, 10000, 2, &ok);
    
    if (ok && amount > 0) {
        if (DatabaseManager::instance().recharge(m_currentUser.id, amount, m_currentUser.name)) {
            QMessageBox::information(this, "成功", QString("充值成功! 充值金额: ¥%1").arg(amount, 0, 'f', 2));
            loadBalanceInfo();
            loadRechargeRecords();
        } else {
            QMessageBox::warning(this, "失败", "充值失败,请稍后重试!");
        }
    }
}

void StudentPanel::onRefreshClicked()
{
    loadBalanceInfo();
    loadElectricityRecords();
    loadRechargeRecords();
    QMessageBox::information(this, "刷新", "数据已刷新!");
}

void StudentPanel::onTabChanged(int index)
{
    if (index == 1) {
        loadElectricityRecords();
    } else if (index == 2) {
        loadRechargeRecords();
    }
}

void StudentPanel::onWebQueryClicked()
{
    if (!m_electricityQuery) {
        m_electricityQuery = new ElectricityQuery();
    }
    m_electricityQuery->show();
}
