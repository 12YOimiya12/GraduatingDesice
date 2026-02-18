#ifndef STUDENTPANEL_H
#define STUDENTPANEL_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QTabWidget>
#include <QPushButton>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
#include "databasemanager.h"
#include "electricityquery.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StudentPanel; }
QT_END_NAMESPACE

/**
 * @brief 学生端面板类
 * 
 * 提供学生用户的主要功能界面，包括：
 * - 个人信息和余额显示
 * - 电费充值
 * - 用电记录查询
 * - 充值记录查询
 * - 网页电费查询
 */
class StudentPanel : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit StudentPanel(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~StudentPanel();
    
    /**
     * @brief 设置当前登录用户
     * @param user 用户信息
     */
    void setCurrentUser(const UserInfo& user);
    
signals:
    /**
     * @brief 登出信号
     */
    void logout();
    
private slots:
    /**
     * @brief 登出按钮点击槽函数
     */
    void onLogoutClicked();
    
    /**
     * @brief 充值按钮点击槽函数
     */
    void onRechargeClicked();
    
    /**
     * @brief 刷新按钮点击槽函数
     */
    void onRefreshClicked();
    
    /**
     * @brief 标签页切换槽函数
     * @param index 标签页索引
     */
    void onTabChanged(int index);
    
    /**
     * @brief 网页查询按钮点击槽函数
     */
    void onWebQueryClicked();
    
private:
    /**
     * @brief 初始化用户界面
     */
    void initUI();
    
    /**
     * @brief 加载用户信息
     */
    void loadUserInfo();
    
    /**
     * @brief 加载余额信息
     */
    void loadBalanceInfo();
    
    /**
     * @brief 加载用电记录
     */
    void loadElectricityRecords();
    
    /**
     * @brief 加载充值记录
     */
    void loadRechargeRecords();
    
    /**
     * @brief 加载电费变动记录
     */
    void loadElectricityChangeRecords();
    
    /**
     * @brief 加载电费度数变动记录
     */
    void loadElectricityKwhChangeRecords();
    
    /**
     * @brief 更新电费变化图表
     */
    void updateBalanceChart();
    
    /**
     * @brief 更新度数变化图表
     */
    void updateKwhChart();
    
    QLabel *m_userInfoLabel;            // 用户信息标签
    QLabel *m_balanceLabel;              // 余额标签
    QLabel *m_remainingKwhLabel;         // 剩余度数标签
    QTableWidget *m_electricityTable;    // 用电记录表格
    QTableWidget *m_rechargeTable;       // 充值记录表格
    QTableWidget *m_changeRecordsTable;  // 电费变动记录表格
    QTableWidget *m_kwhChangeRecordsTable; // 电费度数变动记录表格
    QChartView *m_balanceChartView;      // 电费变化图表视图
    QChartView *m_kwhChartView;          // 度数变化图表视图
    QPushButton *m_webQueryBtn;          // 网页查询按钮
    ElectricityQuery *m_electricityQuery; // 电费查询窗口
    UserInfo m_currentUser;              // 当前用户信息
};

#endif // STUDENTPANEL_H
