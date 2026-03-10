#ifndef ELECTRICITYQUERY_H
#define ELECTRICITYQUERY_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QProgressBar>
#include "electricityparser.h"

/**
 * @brief 电费查询窗口类
 * 
 * 提供网页电费查询的用户界面，主要功能包括：
 * - 显示电费查询URL和查询参数
 * - 提供浏览器打开和直接获取两种方式
 * - 显示解析结果（剩余度数、金额、宿舍信息）
 * - 显示原始HTML用于调试
 * - 提供进度条和状态显示
 */
class ElectricityQuery : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit ElectricityQuery(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ElectricityQuery();

private slots:
    /**
     * @brief 打开浏览器按钮点击槽函数
     */
    void onOpenBrowserClicked();
    
    /**
     * @brief 获取数据按钮点击槽函数
     */
    void onFetchDataClicked();
    
    /**
     * @brief 数据准备就绪槽函数
     */
    void onDataReady();
    
    /**
     * @brief 错误发生槽函数
     * @param error 错误信息
     */
    void onErrorOccurred(const QString &error);
    
    /**
     * @brief 返回按钮点击槽函数
     */
    void onBackClicked();

private:
    /**
     * @brief 初始化用户界面
     */
    void initUI();
    
    /**
     * @brief 显示解析结果
     */
    void displayResults();
    
    QLineEdit *m_urlEdit;                // URL输入框
    QLineEdit *m_roomNoEdit;             // 房间号输入框
    QLineEdit *m_cookieEdit;             // Cookie输入框
    QLineEdit *m_campusEdit;             // 校区输入框
    QLineEdit *m_areaEdit;               // 区域输入框
    QLineEdit *m_buildingEdit;           // 楼栋输入框
    QLineEdit *m_floorEdit;              // 楼层输入框
    QLineEdit *m_roomEdit;               // 房间输入框
    QPushButton *m_openBrowserBtn;       // 打开浏览器按钮
    QPushButton *m_fetchBtn;             // 获取数据按钮
    QPushButton *m_backBtn;              // 返回按钮
    QLabel *m_resultKwhLabel;            // 剩余度数显示标签
    QLabel *m_resultAmountLabel;         // 剩余金额显示标签
    QLabel *m_resultDormLabel;           // 宿舍信息显示标签
    QTextEdit *m_helpText;               // 帮助文本
    QTextEdit *m_rawHtmlText;            // 原始HTML显示框
    QProgressBar *m_progressBar;         // 进度条
    QLabel *m_statusLabel;               // 状态标签
    
    ElectricityParser *m_parser;         // 电费解析器
};

#endif // ELECTRICITYQUERY_H
