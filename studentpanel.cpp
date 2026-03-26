#include "studentpanel.h"
#include "databasemanager.h"
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
#include <QLineSeries>
#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QFileDialog>
#include <QComboBox>
#include <QTextEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QPixmap>
#include <QMediaDevices>
#include <QApplication>
#include <QDir>

CameraDialog::CameraDialog(QWidget *parent)
    : QDialog(parent)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_videoWidget(nullptr)
    , m_imageCapture(nullptr)
    , m_imageReady(false)
{
    setWindowTitle("摄像头采集");
    setMinimumSize(640, 480);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    m_videoWidget = new QVideoWidget();
    m_videoWidget->setMinimumSize(320, 240);
    m_videoWidget->setStyleSheet("background-color: black;");
    contentLayout->addWidget(m_videoWidget);
    
    m_previewLabel = new QLabel();
    m_previewLabel->setMinimumSize(320, 240);
    m_previewLabel->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc;");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setText("预览区域");
    m_previewLabel->setScaledContents(true);
    contentLayout->addWidget(m_previewLabel);
    
    mainLayout->addLayout(contentLayout);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_captureBtn = new QPushButton("拍照");
    m_captureBtn->setStyleSheet("padding: 10px 30px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(m_captureBtn, &QPushButton::clicked, this, &CameraDialog::onCaptureClicked);
    btnLayout->addWidget(m_captureBtn);
    
    m_confirmBtn = new QPushButton("确认使用");
    m_confirmBtn->setEnabled(false);
    m_confirmBtn->setStyleSheet("padding: 10px 30px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(m_confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(m_confirmBtn);
    
    QPushButton *cancelBtn = new QPushButton("取消");
    cancelBtn->setStyleSheet("padding: 10px 30px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(btnLayout);
    
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "错误", "未检测到摄像头设备！");
        return;
    }
    
    m_camera = new QCamera(cameras.first(), this);
    m_captureSession = new QMediaCaptureSession(this);
    m_imageCapture = new QImageCapture(this);
    
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);
    m_captureSession->setImageCapture(m_imageCapture);
    
    connect(m_imageCapture, &QImageCapture::imageCaptured, this, &CameraDialog::onImageCaptured);
    
    m_camera->start();
}

CameraDialog::~CameraDialog()
{
    if (m_camera) {
        m_camera->stop();
    }
}

void CameraDialog::onCaptureClicked()
{
    if (m_imageCapture && m_camera) {
        m_imageCapture->capture();
    }
}

void CameraDialog::onImageCaptured(int id, const QImage &preview)
{
    Q_UNUSED(id);
    m_capturedImage = QPixmap::fromImage(preview);
    m_previewLabel->setPixmap(m_capturedImage.scaled(m_previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_imageReady = true;
    m_confirmBtn->setEnabled(true);
}

StudentPanel::StudentPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_userInfoLabel(nullptr)
    , m_balanceLabel(nullptr)
    , m_remainingKwhLabel(nullptr)
    , m_electricityTable(nullptr)
    , m_rechargeTable(nullptr)
    , m_changeRecordsTable(nullptr)
    , m_kwhChangeRecordsTable(nullptr)
    , m_balanceChartView(nullptr)
    , m_kwhChartView(nullptr)
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
    
    QLabel *titleLabel = new QLabel("惠电宿舍智能管理系统 - 学生端");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout *userInfoLayout = new QHBoxLayout();
    
    m_userInfoLabel = new QLabel();
    m_userInfoLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    userInfoLayout->addWidget(m_userInfoLabel);
    
    m_balanceLabel = new QLabel("当前余额: 加载中...");
    m_balanceLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #e74c3c;");
    userInfoLayout->addWidget(m_balanceLabel);
    
    m_remainingKwhLabel = new QLabel("剩余度数: 加载中...");
    m_remainingKwhLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #27ae60;");
    userInfoLayout->addWidget(m_remainingKwhLabel);
    
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
    
    // 电费变动记录标签页
    QWidget *changeRecordsTab = new QWidget();
    QVBoxLayout *changeRecordsLayout = new QVBoxLayout(changeRecordsTab);
    
    QLabel *changeRecordsTitle = new QLabel("电费变动记录");
    changeRecordsTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    changeRecordsLayout->addWidget(changeRecordsTitle);
    
    m_changeRecordsTable = new QTableWidget();
    m_changeRecordsTable->setColumnCount(8);
    m_changeRecordsTable->setHorizontalHeaderLabels({"时间", "变动类型", "变动金额(元)", "变动前余额(元)", "变动后余额(元)", "宿舍", "操作人", "备注"});
    m_changeRecordsTable->horizontalHeader()->setStretchLastSection(true);
    m_changeRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_changeRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_changeRecordsTable->setMaximumHeight(250);
    changeRecordsLayout->addWidget(m_changeRecordsTable);
    
    QLabel *balanceChartTitle = new QLabel("电费变化趋势图");
    balanceChartTitle->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px; margin-top: 10px;");
    changeRecordsLayout->addWidget(balanceChartTitle);
    
    m_balanceChartView = new QChartView();
    m_balanceChartView->setRenderHint(QPainter::Antialiasing);
    m_balanceChartView->setMinimumHeight(250);
    changeRecordsLayout->addWidget(m_balanceChartView);
    
    tabWidget->addTab(changeRecordsTab, "金额变动记录");
    
    // 电费度数变动记录标签页
    QWidget *kwhChangeRecordsTab = new QWidget();
    QVBoxLayout *kwhChangeRecordsLayout = new QVBoxLayout(kwhChangeRecordsTab);
    
    QLabel *kwhChangeRecordsTitle = new QLabel("电费度数变动记录");
    kwhChangeRecordsTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    kwhChangeRecordsLayout->addWidget(kwhChangeRecordsTitle);
    
    m_kwhChangeRecordsTable = new QTableWidget();
    m_kwhChangeRecordsTable->setColumnCount(8);
    m_kwhChangeRecordsTable->setHorizontalHeaderLabels({"时间", "变动类型", "变动度数(度)", "变动前度数(度)", "变动后度数(度)", "宿舍", "操作人", "查询网址"});
    m_kwhChangeRecordsTable->horizontalHeader()->setStretchLastSection(true);
    m_kwhChangeRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_kwhChangeRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_kwhChangeRecordsTable->setMaximumHeight(250);
    kwhChangeRecordsLayout->addWidget(m_kwhChangeRecordsTable);
    
    QLabel *kwhChartTitle = new QLabel("度数变化趋势图");
    kwhChartTitle->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px; margin-top: 10px;");
    kwhChangeRecordsLayout->addWidget(kwhChartTitle);
    
    m_kwhChartView = new QChartView();
    m_kwhChartView->setRenderHint(QPainter::Antialiasing);
    m_kwhChartView->setMinimumHeight(250);
    kwhChangeRecordsLayout->addWidget(m_kwhChartView);
    
    tabWidget->addTab(kwhChangeRecordsTab, "度数变动记录");
    
    QWidget *faceRegisterTab = initFaceRegisterTab();
    tabWidget->addTab(faceRegisterTab, "人脸录入");
    
    QWidget *repairRequestTab = initRepairRequestTab();
    tabWidget->addTab(repairRequestTab, "维修申请");
    
    QWidget *roomChangeTab = initRoomChangeTab();
    tabWidget->addTab(roomChangeTab, "换寝申请");
    
    QWidget *applianceControlTab = initApplianceControlTab();
    tabWidget->addTab(applianceControlTab, "电器控制");
    
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
    loadElectricityChangeRecords();
    loadElectricityKwhChangeRecords();
    updateBalanceChart();
    updateKwhChart();
    
    checkLowKwhWarning();
}

void StudentPanel::checkLowKwhWarning()
{
    if (m_currentUser.dormitory.isEmpty()) {
        return;
    }
    
    DormitoryInfo dorm = DatabaseManager::instance().getDormitoryByNumber(m_currentUser.dormitory);
    
    if (dorm.id != -1 && dorm.remainingKwh < 3.0) {
        QMessageBox::warning(this, "电费预警", 
            QString("警告：您的宿舍(%1)剩余度数不足3度！\n当前剩余: %2 度\n\n请及时充值，避免断电！")
            .arg(m_currentUser.dormitory)
            .arg(dorm.remainingKwh, 0, 'f', 2));
        
        if (m_remainingKwhLabel) {
            m_remainingKwhLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #e74c3c; background-color: #ffebee; border: 2px solid #e74c3c; border-radius: 4px;");
        }
    }
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
    
    DormitoryInfo dorm = DatabaseManager::instance().getDormitoryByNumber(m_currentUser.dormitory);
    if (dorm.id != -1) {
        m_remainingKwhLabel->setText(QString("剩余度数: %1 度").arg(dorm.remainingKwh, 0, 'f', 2));
    } else {
        m_remainingKwhLabel->setText("剩余度数: 获取失败");
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
    loadElectricityChangeRecords();
    loadElectricityKwhChangeRecords();
    updateBalanceChart();
    updateKwhChart();
    QMessageBox::information(this, "刷新", "数据已刷新!");
}

void StudentPanel::onTabChanged(int index)
{
    if (index == 1) {
        loadElectricityRecords();
    } else if (index == 2) {
        loadRechargeRecords();
    } else if (index == 3) {
        loadElectricityChangeRecords();
        updateBalanceChart();
    } else if (index == 4) {
        loadElectricityKwhChangeRecords();
        updateKwhChart();
    }
}

void StudentPanel::onWebQueryClicked()
{
    if (!m_electricityQuery) {
        m_electricityQuery = new ElectricityQuery();
    }
    m_electricityQuery->show();
}

void StudentPanel::loadElectricityChangeRecords()
{
    QList<ElectricityChangeRecord> records = DatabaseManager::instance().getElectricityChangeRecordsByUser(m_currentUser.id);
    
    m_changeRecordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const ElectricityChangeRecord& record = records[i];
        
        // 设置颜色样式
        QString amountStyle;
        if (record.changeAmount > 0) {
            amountStyle = "color: #27ae60;"; // 绿色表示充值
        } else {
            amountStyle = "color: #e74c3c;"; // 红色表示扣费
        }
        
        m_changeRecordsTable->setItem(i, 0, new QTableWidgetItem(record.changeTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_changeRecordsTable->setItem(i, 1, new QTableWidgetItem(record.changeType));
        
        QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(record.changeAmount, 'f', 2));
        amountItem->setData(Qt::TextAlignmentRole, Qt::AlignRight);
        if (!amountStyle.isEmpty()) {
            amountItem->setData(Qt::UserRole, amountStyle);
        }
        m_changeRecordsTable->setItem(i, 2, amountItem);
        
        m_changeRecordsTable->setItem(i, 3, new QTableWidgetItem(QString::number(record.balanceBefore, 'f', 2)));
        m_changeRecordsTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.balanceAfter, 'f', 2)));
        m_changeRecordsTable->setItem(i, 5, new QTableWidgetItem(record.dormitory));
        m_changeRecordsTable->setItem(i, 6, new QTableWidgetItem(record.operatorName));
        m_changeRecordsTable->setItem(i, 7, new QTableWidgetItem(record.remark));
    }
}

void StudentPanel::loadElectricityKwhChangeRecords()
{
    QList<ElectricityKwhChangeRecord> records = DatabaseManager::instance().getElectricityKwhChangeRecordsByDormitory(m_currentUser.dormitory);
    
    m_kwhChangeRecordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const ElectricityKwhChangeRecord& record = records[i];
        
        // 设置颜色样式
        QString kwhStyle;
        if (record.kwhChange > 0) {
            kwhStyle = "color: #27ae60;"; // 绿色表示度数增加
        } else if (record.kwhChange < 0) {
            kwhStyle = "color: #e74c3c;"; // 红色表示度数减少
        }
        
        m_kwhChangeRecordsTable->setItem(i, 0, new QTableWidgetItem(record.changeTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_kwhChangeRecordsTable->setItem(i, 1, new QTableWidgetItem(record.changeType));
        
        QTableWidgetItem *kwhChangeItem = new QTableWidgetItem(QString::number(record.kwhChange, 'f', 2));
        kwhChangeItem->setData(Qt::TextAlignmentRole, Qt::AlignRight);
        if (!kwhStyle.isEmpty()) {
            kwhChangeItem->setData(Qt::UserRole, kwhStyle);
        }
        m_kwhChangeRecordsTable->setItem(i, 2, kwhChangeItem);
        
        m_kwhChangeRecordsTable->setItem(i, 3, new QTableWidgetItem(QString::number(record.kwhBefore, 'f', 2)));
        m_kwhChangeRecordsTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.kwhAfter, 'f', 2)));
        m_kwhChangeRecordsTable->setItem(i, 5, new QTableWidgetItem(record.dormitory));
        m_kwhChangeRecordsTable->setItem(i, 6, new QTableWidgetItem(record.operatorName));
        
        // 显示查询网址，如果过长则截断显示
        QString queryUrl = record.queryUrl;
        if (queryUrl.length() > 50) {
            queryUrl = queryUrl.left(50) + "...";
        }
        m_kwhChangeRecordsTable->setItem(i, 7, new QTableWidgetItem(queryUrl));
    }
}

void StudentPanel::updateBalanceChart()
{
    QList<ElectricityChangeRecord> records = DatabaseManager::instance().getElectricityChangeRecordsByDormitory(m_currentUser.dormitory);
    
    QChart *chart = new QChart();
    chart->setTitle("电费余额变化趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QLineSeries *series = new QLineSeries();
    series->setName("余额(元)");
    
    // 只显示最近20条记录
    int startIndex = qMax(0, records.size() - 20);
    for (int i = startIndex; i < records.size(); ++i) {
        const ElectricityChangeRecord& record = records[i];
        QDateTime dateTime = record.changeTime;
        qint64 timestamp = dateTime.toMSecsSinceEpoch();
        series->append(timestamp, record.balanceAfter);
    }
    
    chart->addSeries(series);
    
    // 设置X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("MM-dd hh:mm");
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    // 设置Y轴（金额轴）
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("余额(元)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_balanceChartView->setChart(chart);
}

void StudentPanel::updateKwhChart()
{
    QList<ElectricityKwhChangeRecord> records = DatabaseManager::instance().getElectricityKwhChangeRecordsByDormitory(m_currentUser.dormitory);
    
    QChart *chart = new QChart();
    chart->setTitle("电费度数变化趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QLineSeries *series = new QLineSeries();
    series->setName("度数(度)");
    
    // 只显示最近20条记录
    int startIndex = qMax(0, records.size() - 20);
    for (int i = startIndex; i < records.size(); ++i) {
        const ElectricityKwhChangeRecord& record = records[i];
        QDateTime dateTime = record.changeTime;
        qint64 timestamp = dateTime.toMSecsSinceEpoch();
        series->append(timestamp, record.kwhAfter);
    }
    
    chart->addSeries(series);
    
    // 设置X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("MM-dd hh:mm");
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    // 设置Y轴（度数轴）
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("度数(度)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_kwhChartView->setChart(chart);
}

QWidget* StudentPanel::initFaceRegisterTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("人脸信息录入");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QHBoxLayout *infoLayout = new QHBoxLayout();
    
    m_faceImageLabel = new QLabel();
    m_faceImageLabel->setFixedSize(200, 200);
    m_faceImageLabel->setStyleSheet("border: 2px dashed #ccc; background-color: #f5f5f5;");
    m_faceImageLabel->setAlignment(Qt::AlignCenter);
    m_faceImageLabel->setText("暂无人脸照片");
    m_faceImageLabel->setScaledContents(true);
    infoLayout->addWidget(m_faceImageLabel);
    
    QVBoxLayout *statusLayout = new QVBoxLayout();
    
    m_faceStatusLabel = new QLabel("状态: 未录入");
    m_faceStatusLabel->setStyleSheet("font-size: 14px; padding: 10px;");
    statusLayout->addWidget(m_faceStatusLabel);
    
    QLabel *tipLabel = new QLabel("提示: 请上传清晰的正面人脸照片，\n确保光线充足、五官清晰可见。");
    tipLabel->setStyleSheet("font-size: 12px; color: #666; padding: 10px;");
    tipLabel->setWordWrap(true);
    statusLayout->addWidget(tipLabel);
    
    statusLayout->addStretch();
    infoLayout->addLayout(statusLayout);
    
    layout->addLayout(infoLayout);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *cameraBtn = new QPushButton("摄像头拍照");
    cameraBtn->setStyleSheet("padding: 10px 20px; background-color: #9b59b6; color: white; border: none; border-radius: 4px;");
    connect(cameraBtn, &QPushButton::clicked, this, [this]() {
        CameraDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QPixmap capturedImage = dialog.getCapturedImage();
            if (!capturedImage.isNull()) {
                QString savePath = QApplication::applicationDirPath() + "/face_images";
                QDir dir;
                if (!dir.exists(savePath)) {
                    dir.mkpath(savePath);
                }
                QString fileName = savePath + "/" + m_currentUser.studentId + "_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".jpg";
                if (capturedImage.save(fileName)) {
                    m_faceImageLabel->setPixmap(capturedImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    
                    FaceInfo faceInfo;
                    faceInfo.userId = m_currentUser.id;
                    faceInfo.studentId = m_currentUser.studentId;
                    faceInfo.name = m_currentUser.name;
                    faceInfo.dormitory = m_currentUser.dormitory;
                    faceInfo.faceImagePath = fileName;
                    faceInfo.status = 0;
                    faceInfo.submitTime = QDateTime::currentDateTime();
                    
                    if (DatabaseManager::instance().addFaceInfo(faceInfo)) {
                        m_faceStatusLabel->setText("状态: 待审核");
                        m_faceStatusLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #f39c12;");
                        QMessageBox::information(this, "成功", "人脸照片已提交，等待管理员审核！");
                    } else {
                        QMessageBox::warning(this, "失败", "人脸照片提交失败，请重试！");
                    }
                }
            }
        }
    });
    btnLayout->addWidget(cameraBtn);
    
    QPushButton *uploadBtn = new QPushButton("上传人脸照片");
    uploadBtn->setStyleSheet("padding: 10px 20px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(uploadBtn, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择人脸照片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                m_faceImageLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                
                FaceInfo faceInfo;
                faceInfo.userId = m_currentUser.id;
                faceInfo.studentId = m_currentUser.studentId;
                faceInfo.name = m_currentUser.name;
                faceInfo.dormitory = m_currentUser.dormitory;
                faceInfo.faceImagePath = fileName;
                faceInfo.status = 0;
                faceInfo.submitTime = QDateTime::currentDateTime();
                
                if (DatabaseManager::instance().addFaceInfo(faceInfo)) {
                    m_faceStatusLabel->setText("状态: 待审核");
                    m_faceStatusLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #f39c12;");
                    QMessageBox::information(this, "成功", "人脸照片已提交，等待管理员审核！");
                } else {
                    QMessageBox::warning(this, "失败", "人脸照片提交失败，请重试！");
                }
            }
        }
    });
    btnLayout->addWidget(uploadBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新状态");
    refreshBtn->setStyleSheet("padding: 10px 20px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPanel::loadFaceInfo);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    loadFaceInfo();
    
    return tab;
}

void StudentPanel::loadFaceInfo()
{
    FaceInfo faceInfo = DatabaseManager::instance().getFaceInfoByUserId(m_currentUser.id);
    
    if (faceInfo.id == -1) {
        m_faceStatusLabel->setText("状态: 未录入");
        m_faceStatusLabel->setStyleSheet("font-size: 14px; padding: 10px; color: #666;");
        m_faceImageLabel->setText("暂无人脸照片");
        m_faceImageLabel->setPixmap(QPixmap());
        return;
    }
    
    if (!faceInfo.faceImagePath.isEmpty()) {
        QPixmap pixmap(faceInfo.faceImagePath);
        if (!pixmap.isNull()) {
            m_faceImageLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    
    QString statusText;
    QString statusStyle;
    switch (faceInfo.status) {
        case 0:
            statusText = "状态: 待审核";
            statusStyle = "font-size: 14px; padding: 10px; color: #f39c12;";
            break;
        case 1:
            statusText = "状态: 已通过";
            statusStyle = "font-size: 14px; padding: 10px; color: #27ae60;";
            break;
        case 2:
            statusText = QString("状态: 已拒绝 (%1)").arg(faceInfo.rejectReason);
            statusStyle = "font-size: 14px; padding: 10px; color: #e74c3c;";
            break;
    }
    m_faceStatusLabel->setText(statusText);
    m_faceStatusLabel->setStyleSheet(statusStyle);
}

QWidget* StudentPanel::initRepairRequestTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("宿舍维修申请");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *addBtn = new QPushButton("新建维修申请");
    addBtn->setStyleSheet("padding: 8px 16px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(addBtn, &QPushButton::clicked, this, &StudentPanel::onRepairRequestClicked);
    btnLayout->addWidget(addBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPanel::loadRepairRequests);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_repairRequestTable = new QTableWidget();
    m_repairRequestTable->setColumnCount(7);
    m_repairRequestTable->setHorizontalHeaderLabels({"提交时间", "维修类型", "问题描述", "状态", "优先级", "处理人", "处理结果"});
    m_repairRequestTable->horizontalHeader()->setStretchLastSection(true);
    m_repairRequestTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_repairRequestTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_repairRequestTable);
    
    loadRepairRequests();
    
    return tab;
}

void StudentPanel::loadRepairRequests()
{
    QList<RepairRequest> requests = DatabaseManager::instance().getRepairRequestsByUser(m_currentUser.id);
    
    m_repairRequestTable->setRowCount(requests.size());
    
    for (int i = 0; i < requests.size(); ++i) {
        const RepairRequest& req = requests[i];
        m_repairRequestTable->setItem(i, 0, new QTableWidgetItem(req.submitTime.toString("yyyy-MM-dd hh:mm")));
        m_repairRequestTable->setItem(i, 1, new QTableWidgetItem(req.repairTypeText));
        m_repairRequestTable->setItem(i, 2, new QTableWidgetItem(req.description.left(30) + (req.description.length() > 30 ? "..." : "")));
        
        QString statusText;
        switch (req.status) {
            case 0: statusText = "待处理"; break;
            case 1: statusText = "处理中"; break;
            case 2: statusText = "已完成"; break;
            case 3: statusText = "已关闭"; break;
        }
        m_repairRequestTable->setItem(i, 3, new QTableWidgetItem(statusText));
        
        QString priorityText;
        switch (req.priority) {
            case 0: priorityText = "普通"; break;
            case 1: priorityText = "紧急"; break;
            case 2: priorityText = "非常紧急"; break;
        }
        m_repairRequestTable->setItem(i, 4, new QTableWidgetItem(priorityText));
        m_repairRequestTable->setItem(i, 5, new QTableWidgetItem(req.handlerName));
        m_repairRequestTable->setItem(i, 6, new QTableWidgetItem(req.handleResult));
    }
}

void StudentPanel::onRepairRequestClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建维修申请");
    dialog.setMinimumWidth(400);
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *phoneEdit = new QLineEdit();
    formLayout->addRow("联系电话:", phoneEdit);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("电路问题", 0);
    typeCombo->addItem("水管问题", 1);
    typeCombo->addItem("门窗问题", 2);
    typeCombo->addItem("家具问题", 3);
    typeCombo->addItem("其他", 4);
    formLayout->addRow("维修类型:", typeCombo);
    
    QComboBox *priorityCombo = new QComboBox();
    priorityCombo->addItem("普通", 0);
    priorityCombo->addItem("紧急", 1);
    priorityCombo->addItem("非常紧急", 2);
    formLayout->addRow("优先级:", priorityCombo);
    
    QTextEdit *descEdit = new QTextEdit();
    descEdit->setMaximumHeight(100);
    formLayout->addRow("问题描述:", descEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        RepairRequest request;
        request.userId = m_currentUser.id;
        request.studentId = m_currentUser.studentId;
        request.name = m_currentUser.name;
        request.dormitory = m_currentUser.dormitory;
        request.contactPhone = phoneEdit->text();
        request.repairType = typeCombo->currentData().toInt();
        request.repairTypeText = typeCombo->currentText();
        request.priority = priorityCombo->currentData().toInt();
        request.description = descEdit->toPlainText();
        request.status = 0;
        request.submitTime = QDateTime::currentDateTime();
        
        if (DatabaseManager::instance().addRepairRequest(request)) {
            QMessageBox::information(this, "成功", "维修申请已提交！");
            loadRepairRequests();
        } else {
            QMessageBox::warning(this, "失败", "维修申请提交失败！");
        }
    }
}

QWidget* StudentPanel::initRoomChangeTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("换寝申请");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *addBtn = new QPushButton("新建换寝申请");
    addBtn->setStyleSheet("padding: 8px 16px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(addBtn, &QPushButton::clicked, this, &StudentPanel::onRoomChangeClicked);
    btnLayout->addWidget(addBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPanel::loadRoomChangeRequests);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_roomChangeTable = new QTableWidget();
    m_roomChangeTable->setColumnCount(6);
    m_roomChangeTable->setHorizontalHeaderLabels({"提交时间", "当前宿舍", "目标宿舍", "换寝原因", "状态", "审核意见"});
    m_roomChangeTable->horizontalHeader()->setStretchLastSection(true);
    m_roomChangeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_roomChangeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_roomChangeTable);
    
    loadRoomChangeRequests();
    
    return tab;
}

void StudentPanel::loadRoomChangeRequests()
{
    QList<RoomChangeRequest> requests = DatabaseManager::instance().getRoomChangeRequestsByUser(m_currentUser.id);
    
    m_roomChangeTable->setRowCount(requests.size());
    
    for (int i = 0; i < requests.size(); ++i) {
        const RoomChangeRequest& req = requests[i];
        m_roomChangeTable->setItem(i, 0, new QTableWidgetItem(req.submitTime.toString("yyyy-MM-dd hh:mm")));
        m_roomChangeTable->setItem(i, 1, new QTableWidgetItem(req.currentDormitory));
        m_roomChangeTable->setItem(i, 2, new QTableWidgetItem(req.targetDormitory));
        m_roomChangeTable->setItem(i, 3, new QTableWidgetItem(req.changeReasonText));
        
        QString statusText;
        switch (req.status) {
            case 0: statusText = "待审核"; break;
            case 1: statusText = "已通过"; break;
            case 2: statusText = "已拒绝"; break;
            case 3: statusText = "已完成"; break;
        }
        m_roomChangeTable->setItem(i, 4, new QTableWidgetItem(statusText));
        m_roomChangeTable->setItem(i, 5, new QTableWidgetItem(req.rejectReason));
    }
}

void StudentPanel::onRoomChangeClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建换寝申请");
    dialog.setMinimumWidth(400);
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLabel *currentDormLabel = new QLabel(m_currentUser.dormitory);
    formLayout->addRow("当前宿舍:", currentDormLabel);
    
    QLineEdit *targetDormEdit = new QLineEdit();
    formLayout->addRow("目标宿舍:", targetDormEdit);
    
    QComboBox *reasonCombo = new QComboBox();
    reasonCombo->addItem("室友矛盾", 0);
    reasonCombo->addItem("身体原因", 1);
    reasonCombo->addItem("学业需要", 2);
    reasonCombo->addItem("其他", 3);
    formLayout->addRow("换寝原因:", reasonCombo);
    
    QTextEdit *descEdit = new QTextEdit();
    descEdit->setMaximumHeight(100);
    formLayout->addRow("详细说明:", descEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        RoomChangeRequest request;
        request.userId = m_currentUser.id;
        request.studentId = m_currentUser.studentId;
        request.name = m_currentUser.name;
        request.currentDormitory = m_currentUser.dormitory;
        request.targetDormitory = targetDormEdit->text();
        request.changeReason = reasonCombo->currentData().toInt();
        request.changeReasonText = reasonCombo->currentText();
        request.description = descEdit->toPlainText();
        request.status = 0;
        request.submitTime = QDateTime::currentDateTime();
        
        if (DatabaseManager::instance().addRoomChangeRequest(request)) {
            QMessageBox::information(this, "成功", "换寝申请已提交！");
            loadRoomChangeRequests();
        } else {
            QMessageBox::warning(this, "失败", "换寝申请提交失败！");
        }
    }
}

QWidget* StudentPanel::initApplianceControlTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("电器远程控制");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *refreshBtn = new QPushButton("刷新状态");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPanel::loadApplianceControls);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_applianceTable = new QTableWidget();
    m_applianceTable->setColumnCount(6);
    m_applianceTable->setHorizontalHeaderLabels({"电器名称", "类型", "状态", "功率档位", "日用电量(度)", "操作"});
    m_applianceTable->horizontalHeader()->setStretchLastSection(true);
    m_applianceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_applianceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_applianceTable);
    
    loadApplianceControls();
    
    return tab;
}

void StudentPanel::loadApplianceControls()
{
    QList<ApplianceControl> appliances = DatabaseManager::instance().getApplianceControlsByDormitory(m_currentUser.dormitory);
    
    m_applianceTable->setRowCount(appliances.size());
    
    for (int i = 0; i < appliances.size(); ++i) {
        const ApplianceControl& app = appliances[i];
        m_applianceTable->setItem(i, 0, new QTableWidgetItem(app.applianceName));
        m_applianceTable->setItem(i, 1, new QTableWidgetItem(app.applianceType));
        m_applianceTable->setItem(i, 2, new QTableWidgetItem(app.status == 1 ? "开启" : "关闭"));
        m_applianceTable->setItem(i, 3, new QTableWidgetItem(QString::number(app.powerLevel) + "%"));
        m_applianceTable->setItem(i, 4, new QTableWidgetItem(QString::number(app.dailyUsage, 'f', 2)));
        
        QWidget *btnWidget = new QWidget();
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(4, 4, 4, 4);
        
        QPushButton *toggleBtn = new QPushButton(app.status == 1 ? "关闭" : "开启");
        toggleBtn->setStyleSheet(QString("padding: 4px 8px; background-color: %1; color: white; border: none; border-radius: 3px;")
            .arg(app.status == 1 ? "#e74c3c" : "#27ae60"));
        toggleBtn->setProperty("applianceId", app.id);
        connect(toggleBtn, &QPushButton::clicked, this, [this, toggleBtn]() {
            int applianceId = toggleBtn->property("applianceId").toInt();
            ApplianceControl app = DatabaseManager::instance().getApplianceControlById(applianceId);
            int newStatus = (app.status == 1) ? 0 : 1;
            if (DatabaseManager::instance().updateApplianceStatus(applianceId, newStatus)) {
                loadApplianceControls();
                QMessageBox::information(this, "成功", QString("%1已%2").arg(app.applianceName).arg(newStatus == 1 ? "开启" : "关闭"));
            }
        });
        btnLayout->addWidget(toggleBtn);
        
        QPushButton *adjustBtn = new QPushButton("调节");
        adjustBtn->setStyleSheet("padding: 4px 8px; background-color: #f39c12; color: white; border: none; border-radius: 3px;");
        adjustBtn->setProperty("applianceId", app.id);
        connect(adjustBtn, &QPushButton::clicked, this, [this, adjustBtn]() {
            int applianceId = adjustBtn->property("applianceId").toInt();
            ApplianceControl app = DatabaseManager::instance().getApplianceControlById(applianceId);
            
            bool ok;
            int powerLevel = QInputDialog::getInt(this, "调节功率", "请输入功率档位(0-100):", app.powerLevel, 0, 100, 10, &ok);
            if (ok) {
                if (DatabaseManager::instance().updateApplianceStatus(applianceId, app.status, powerLevel)) {
                    loadApplianceControls();
                    QMessageBox::information(this, "成功", QString("%1功率已调整为%2%").arg(app.applianceName).arg(powerLevel));
                }
            }
        });
        btnLayout->addWidget(adjustBtn);
        
        m_applianceTable->setCellWidget(i, 5, btnWidget);
    }
}

void StudentPanel::onApplianceControlClicked()
{
    loadApplianceControls();
}

void StudentPanel::onFaceRegisterClicked()
{
    loadFaceInfo();
}
