#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QTabWidget>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
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
    
    /**
     * @brief 批量充值按钮点击槽函数
     */
    void onBatchRechargeClicked();
    
    /**
     * @brief 批量扣费按钮点击槽函数
     */
    void onBatchDeductClicked();
    
    /**
     * @brief 导出数据到Excel按钮点击槽函数
     */
    void onExportToExcelClicked();
    
    /**
     * @brief 从Excel导入数据按钮点击槽函数
     */
    void onImportFromExcelClicked();
    
    void onAuditFaceInfoClicked();
    void onHandleRepairClicked();
    void onAuditRoomChangeClicked();
    void onManageApplianceClicked();
    
private:
    void initUI();
    void loadUserInfo();
    void loadStudents();
    void loadDormitories();
    void loadRechargeRecords();
    void loadElectricityRecords();
    void loadStatistics();
    void updateRechargeChart();
    void updateElectricityChart();
    void loadKwhChangeRecords();
    
    QWidget* initFaceAuditTab();
    QWidget* initRepairHandleTab();
    QWidget* initRoomChangeAuditTab();
    QWidget* initApplianceManageTab();
    void loadPendingFaceInfos();
    void loadPendingRepairRequests();
    void loadPendingRoomChangeRequests();
    void loadAllAppliances();
    void showFaceAuditDialog(int faceId);
    void showRepairHandleDialog(int requestId);
    void showRoomChangeDialog(int requestId);
    
    QLabel *m_userInfoLabel;
    QTableWidget *m_studentsTable;
    QTableWidget *m_dormitoriesTable;
    QTableWidget *m_rechargeRecordsTable;
    QTableWidget *m_electricityRecordsTable;
    QTableWidget *m_kwhChangeRecordsTable;
    QChartView *m_rechargeChartView;
    QChartView *m_electricityChartView;
    QLabel *m_totalStudentsLabel;
    QLabel *m_totalBalanceLabel;
    QLabel *m_totalRechargeLabel;
    UserInfo m_currentUser;
    
    QTableWidget *m_faceAuditTable;
    QTableWidget *m_repairHandleTable;
    QTableWidget *m_roomChangeAuditTable;
    QTableWidget *m_applianceManageTable;
};

#endif // ADMINPANEL_H
