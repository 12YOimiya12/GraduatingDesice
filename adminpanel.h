#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QTabWidget>
#include "databasemanager.h"

/**
 * @brief 管理员端面板类
 * 
 * 提供管理员用户的主要功能界面，包括：
 * - 学生管理（增删改查）
 * - 宿舍管理（增删改查）
 * - 充值记录查询
 * - 用电记录查询
 * - 系统统计信息
 * - 为学生充值电费
 * - 扣除宿舍电费
 */
class AdminPanel : public QMainWindow
{
    Q_OBJECT

public:
    
    explicit AdminPanel(QWidget *parent = nullptr);
    
   
    ~AdminPanel();
    
    
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
     * @brief 添加学生按钮点击槽函数
     */
    void onAddStudentClicked();
    
    /**
     * @brief 编辑学生按钮点击槽函数
     */
    void onEditStudentClicked();
    
    /**
     * @brief 删除学生按钮点击槽函数
     */
    void onDeleteStudentClicked();
    
    /**
     * @brief 添加宿舍按钮点击槽函数
     */
    void onAddDormitoryClicked();
    
    /**
     * @brief 编辑宿舍按钮点击槽函数
     */
    void onEditDormitoryClicked();
    
    /**
     * @brief 删除宿舍按钮点击槽函数
     */
    void onDeleteDormitoryClicked();
    
    /**
     * @brief 为学生充值按钮点击槽函数
     */
    void onRechargeForStudentClicked();
    
    /**
     * @brief 扣除电费按钮点击槽函数
     */
    void onDeductElectricityClicked();
    
    /**
     * @brief 刷新按钮点击槽函数
     */
    void onRefreshClicked();
    
    /**
     * @brief 标签页切换槽函数
     * @param index 标签页索引
     */
    void onTabChanged(int index);
    
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
     * @brief 加载学生列表
     */
    void loadStudents();
    
    /**
     * @brief 加载宿舍列表
     */
    void loadDormitories();
    
    /**
     * @brief 加载充值记录
     */
    void loadRechargeRecords();
    
    /**
     * @brief 加载用电记录
     */
    void loadElectricityRecords();
    
    /**
     * @brief 加载统计信息
     */
    void loadStatistics();
    
    QLabel *m_userInfoLabel;                // 用户信息标签
    QTableWidget *m_studentsTable;          // 学生列表表格
    QTableWidget *m_dormitoriesTable;       // 宿舍列表表格
    QTableWidget *m_rechargeRecordsTable;   // 充值记录表格
    QTableWidget *m_electricityRecordsTable; // 用电记录表格
    QLabel *m_totalStudentsLabel;           // 总学生数标签
    QLabel *m_totalBalanceLabel;            // 总余额标签
    QLabel *m_totalRechargeLabel;           // 总充值金额标签
    UserInfo m_currentUser;                 // 当前用户信息
};

#endif // ADMINPANEL_H
