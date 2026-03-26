#include "adminpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineSeries>
#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QSpinBox>
#include <QTextEdit>
#include <QPixmap>

AdminPanel::AdminPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_userInfoLabel(nullptr)
    , m_studentsTable(nullptr)
    , m_dormitoriesTable(nullptr)
    , m_rechargeRecordsTable(nullptr)
    , m_electricityRecordsTable(nullptr)
    , m_kwhChangeRecordsTable(nullptr)
    , m_rechargeChartView(nullptr)
    , m_electricityChartView(nullptr)
    , m_totalStudentsLabel(nullptr)
    , m_totalBalanceLabel(nullptr)
    , m_totalRechargeLabel(nullptr)
{
    initUI();
}

AdminPanel::~AdminPanel()
{
}

void AdminPanel::initUI()
{
    setWindowTitle("惠电 - 管理员端");
    resize(1000, 700);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QLabel *titleLabel = new QLabel("惠电宿舍智能管理系统 - 管理员端");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    QHBoxLayout *userInfoLayout = new QHBoxLayout();
    
    m_userInfoLabel = new QLabel();
    m_userInfoLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    userInfoLayout->addWidget(m_userInfoLabel);
    
    userInfoLayout->addStretch();
    
    QPushButton *refreshBtn = new QPushButton("刷新数据");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::onRefreshClicked);
    userInfoLayout->addWidget(refreshBtn);
    
    QPushButton *logoutBtn = new QPushButton("退出登录");
    logoutBtn->setStyleSheet("padding: 8px 16px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
    connect(logoutBtn, &QPushButton::clicked, this, &AdminPanel::onLogoutClicked);
    userInfoLayout->addWidget(logoutBtn);
    
    mainLayout->addLayout(userInfoLayout);
    
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #ddd; } QTabBar::tab { padding: 10px 20px; }");
    
    QWidget *statsTab = new QWidget();
    QVBoxLayout *statsLayout = new QVBoxLayout(statsTab);
    
    QLabel *statsTitle = new QLabel("系统统计");
    statsTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    statsLayout->addWidget(statsTitle);
    
    QGridLayout *statsGrid = new QGridLayout();
    
    m_totalStudentsLabel = new QLabel();
    m_totalStudentsLabel->setStyleSheet("font-size: 18px; padding: 20px; background-color: #3498db; color: white; border-radius: 8px;");
    m_totalStudentsLabel->setAlignment(Qt::AlignCenter);
    statsGrid->addWidget(m_totalStudentsLabel, 0, 0);
    
    m_totalBalanceLabel = new QLabel();
    m_totalBalanceLabel->setStyleSheet("font-size: 18px; padding: 20px; background-color: #27ae60; color: white; border-radius: 8px;");
    m_totalBalanceLabel->setAlignment(Qt::AlignCenter);
    statsGrid->addWidget(m_totalBalanceLabel, 0, 1);
    
    m_totalRechargeLabel = new QLabel();
    m_totalRechargeLabel->setStyleSheet("font-size: 18px; padding: 20px; background-color: #f39c12; color: white; border-radius: 8px;");
    m_totalRechargeLabel->setAlignment(Qt::AlignCenter);
    statsGrid->addWidget(m_totalRechargeLabel, 1, 0, 1, 2);
    
    statsLayout->addLayout(statsGrid);
    statsLayout->addStretch();
    
    tabWidget->addTab(statsTab, "系统统计");
    
    QWidget *studentsTab = new QWidget();
    QVBoxLayout *studentsLayout = new QVBoxLayout(studentsTab);
    
    QLabel *studentsTitle = new QLabel("学生信息管理");
    studentsTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    studentsLayout->addWidget(studentsTitle);
    
    QHBoxLayout *studentsBtnLayout = new QHBoxLayout();
    
    QPushButton *addStudentBtn = new QPushButton("添加学生");
    addStudentBtn->setStyleSheet("padding: 8px 16px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(addStudentBtn, &QPushButton::clicked, this, &AdminPanel::onAddStudentClicked);
    studentsBtnLayout->addWidget(addStudentBtn);
    
    QPushButton *editStudentBtn = new QPushButton("编辑学生");
    editStudentBtn->setStyleSheet("padding: 8px 16px; background-color: #f39c12; color: white; border: none; border-radius: 4px;");
    connect(editStudentBtn, &QPushButton::clicked, this, &AdminPanel::onEditStudentClicked);
    studentsBtnLayout->addWidget(editStudentBtn);
    
    QPushButton *deleteStudentBtn = new QPushButton("删除学生");
    deleteStudentBtn->setStyleSheet("padding: 8px 16px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
    connect(deleteStudentBtn, &QPushButton::clicked, this, &AdminPanel::onDeleteStudentClicked);
    studentsBtnLayout->addWidget(deleteStudentBtn);
    
    QPushButton *rechargeForStudentBtn = new QPushButton("为学生充值");
    rechargeForStudentBtn->setStyleSheet("padding: 8px 16px; background-color: #9b59b6; color: white; border: none; border-radius: 4px;");
    connect(rechargeForStudentBtn, &QPushButton::clicked, this, &AdminPanel::onRechargeForStudentClicked);
    studentsBtnLayout->addWidget(rechargeForStudentBtn);
    
    QPushButton *batchRechargeBtn = new QPushButton("批量充值");
    batchRechargeBtn->setStyleSheet("padding: 8px 16px; background-color: #1abc9c; color: white; border: none; border-radius: 4px;");
    connect(batchRechargeBtn, &QPushButton::clicked, this, &AdminPanel::onBatchRechargeClicked);
    studentsBtnLayout->addWidget(batchRechargeBtn);
    
    QPushButton *batchDeductBtn = new QPushButton("批量扣费");
    batchDeductBtn->setStyleSheet("padding: 8px 16px; background-color: #e67e22; color: white; border: none; border-radius: 4px;");
    connect(batchDeductBtn, &QPushButton::clicked, this, &AdminPanel::onBatchDeductClicked);
    studentsBtnLayout->addWidget(batchDeductBtn);
    
    QPushButton *exportBtn = new QPushButton("导出Excel");
    exportBtn->setStyleSheet("padding: 8px 16px; background-color: #34495e; color: white; border: none; border-radius: 4px;");
    connect(exportBtn, &QPushButton::clicked, this, &AdminPanel::onExportToExcelClicked);
    studentsBtnLayout->addWidget(exportBtn);
    
    QPushButton *importBtn = new QPushButton("导入Excel");
    importBtn->setStyleSheet("padding: 8px 16px; background-color: #16a085; color: white; border: none; border-radius: 4px;");
    connect(importBtn, &QPushButton::clicked, this, &AdminPanel::onImportFromExcelClicked);
    studentsBtnLayout->addWidget(importBtn);
    
    studentsBtnLayout->addStretch();
    
    studentsLayout->addLayout(studentsBtnLayout);
    
    m_studentsTable = new QTableWidget();
    m_studentsTable->setColumnCount(7);
    m_studentsTable->setHorizontalHeaderLabels({"ID", "用户名", "姓名", "学号", "宿舍", "余额(元)", "角色"});
    m_studentsTable->horizontalHeader()->setStretchLastSection(true);
    m_studentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    studentsLayout->addWidget(m_studentsTable);
    
    tabWidget->addTab(studentsTab, "学生管理");
    
    QWidget *dormitoriesTab = new QWidget();
    QVBoxLayout *dormitoriesLayout = new QVBoxLayout(dormitoriesTab);
    
    QLabel *dormitoriesTitle = new QLabel("宿舍信息管理");
    dormitoriesTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    dormitoriesLayout->addWidget(dormitoriesTitle);
    
    QHBoxLayout *dormitoriesBtnLayout = new QHBoxLayout();
    
    QPushButton *addDormBtn = new QPushButton("添加宿舍");
    addDormBtn->setStyleSheet("padding: 8px 16px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(addDormBtn, &QPushButton::clicked, this, &AdminPanel::onAddDormitoryClicked);
    dormitoriesBtnLayout->addWidget(addDormBtn);
    
    QPushButton *editDormBtn = new QPushButton("编辑宿舍");
    editDormBtn->setStyleSheet("padding: 8px 16px; background-color: #f39c12; color: white; border: none; border-radius: 4px;");
    connect(editDormBtn, &QPushButton::clicked, this, &AdminPanel::onEditDormitoryClicked);
    dormitoriesBtnLayout->addWidget(editDormBtn);
    
    QPushButton *deleteDormBtn = new QPushButton("删除宿舍");
    deleteDormBtn->setStyleSheet("padding: 8px 16px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
    connect(deleteDormBtn, &QPushButton::clicked, this, &AdminPanel::onDeleteDormitoryClicked);
    dormitoriesBtnLayout->addWidget(deleteDormBtn);
    
    QPushButton *deductBtn = new QPushButton("电费扣费");
    deductBtn->setStyleSheet("padding: 8px 16px; background-color: #e67e22; color: white; border: none; border-radius: 4px;");
    connect(deductBtn, &QPushButton::clicked, this, &AdminPanel::onDeductElectricityClicked);
    dormitoriesBtnLayout->addWidget(deductBtn);
    
    dormitoriesBtnLayout->addStretch();
    
    dormitoriesLayout->addLayout(dormitoriesBtnLayout);
    
    m_dormitoriesTable = new QTableWidget();
    m_dormitoriesTable->setColumnCount(7);
    m_dormitoriesTable->setHorizontalHeaderLabels({"ID", "宿舍号", "楼栋", "楼层", "剩余度数(度)", "当前余额(元)", "最后更新时间"});
    m_dormitoriesTable->horizontalHeader()->setStretchLastSection(true);
    m_dormitoriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dormitoriesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dormitoriesTable->setMaximumHeight(200);
    dormitoriesLayout->addWidget(m_dormitoriesTable);
    
    QLabel *kwhChangeTitle = new QLabel("度数变化记录");
    kwhChangeTitle->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px; margin-top: 10px;");
    dormitoriesLayout->addWidget(kwhChangeTitle);
    
    m_kwhChangeRecordsTable = new QTableWidget();
    m_kwhChangeRecordsTable->setColumnCount(8);
    m_kwhChangeRecordsTable->setHorizontalHeaderLabels({"ID", "时间", "宿舍", "变动前(度)", "变动后(度)", "变动量(度)", "变动类型", "操作人"});
    m_kwhChangeRecordsTable->horizontalHeader()->setStretchLastSection(true);
    m_kwhChangeRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_kwhChangeRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dormitoriesLayout->addWidget(m_kwhChangeRecordsTable);
    
    tabWidget->addTab(dormitoriesTab, "宿舍管理");
    
    QWidget *rechargeTab = new QWidget();
    QVBoxLayout *rechargeLayout = new QVBoxLayout(rechargeTab);
    
    QLabel *rechargeTitle = new QLabel("充值记录");
    rechargeTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    rechargeLayout->addWidget(rechargeTitle);
    
    m_rechargeRecordsTable = new QTableWidget();
    m_rechargeRecordsTable->setColumnCount(7);
    m_rechargeRecordsTable->setHorizontalHeaderLabels({"ID", "时间", "学号", "宿舍", "充值金额(元)", "充值后余额(元)", "操作人"});
    m_rechargeRecordsTable->horizontalHeader()->setStretchLastSection(true);
    m_rechargeRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rechargeRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_rechargeRecordsTable->setMaximumHeight(250);
    rechargeLayout->addWidget(m_rechargeRecordsTable);
    
    QLabel *rechargeChartTitle = new QLabel("充值趋势图");
    rechargeChartTitle->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px; margin-top: 10px;");
    rechargeLayout->addWidget(rechargeChartTitle);
    
    m_rechargeChartView = new QChartView();
    m_rechargeChartView->setRenderHint(QPainter::Antialiasing);
    m_rechargeChartView->setMinimumHeight(250);
    rechargeLayout->addWidget(m_rechargeChartView);
    
    tabWidget->addTab(rechargeTab, "充值记录");
    
    QWidget *electricityTab = new QWidget();
    QVBoxLayout *electricityLayout = new QVBoxLayout(electricityTab);
    
    QLabel *electricityTitle = new QLabel("电费使用记录");
    electricityTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    electricityLayout->addWidget(electricityTitle);
    
    m_electricityRecordsTable = new QTableWidget();
    m_electricityRecordsTable->setColumnCount(7);
    m_electricityRecordsTable->setHorizontalHeaderLabels({"ID", "时间", "用户ID", "宿舍", "用电量(kWh)", "费用(元)", "备注"});
    m_electricityRecordsTable->horizontalHeader()->setStretchLastSection(true);
    m_electricityRecordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_electricityRecordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_electricityRecordsTable->setMaximumHeight(250);
    electricityLayout->addWidget(m_electricityRecordsTable);
    
    QLabel *electricityChartTitle = new QLabel("用电趋势图");
    electricityChartTitle->setStyleSheet("font-size: 14px; font-weight: bold; padding: 5px; margin-top: 10px;");
    electricityLayout->addWidget(electricityChartTitle);
    
    m_electricityChartView = new QChartView();
    m_electricityChartView->setRenderHint(QPainter::Antialiasing);
    m_electricityChartView->setMinimumHeight(250);
    electricityLayout->addWidget(m_electricityChartView);
    
    tabWidget->addTab(electricityTab, "用电记录");
    
    QWidget *faceAuditTab = initFaceAuditTab();
    tabWidget->addTab(faceAuditTab, "人脸审核");
    
    QWidget *repairHandleTab = initRepairHandleTab();
    tabWidget->addTab(repairHandleTab, "维修处理");
    
    QWidget *roomChangeAuditTab = initRoomChangeAuditTab();
    tabWidget->addTab(roomChangeAuditTab, "换寝审核");
    
    QWidget *applianceManageTab = initApplianceManageTab();
    tabWidget->addTab(applianceManageTab, "电器管理");
    
    connect(tabWidget, &QTabWidget::currentChanged, this, &AdminPanel::onTabChanged);
    
    mainLayout->addWidget(tabWidget);
}

void AdminPanel::setCurrentUser(const UserInfo& user)
{
    m_currentUser = user;
    loadUserInfo();
    loadStatistics();
    loadStudents();
    loadDormitories();
    loadRechargeRecords();
    loadElectricityRecords();
    updateRechargeChart();
    updateElectricityChart();
}

void AdminPanel::loadUserInfo()
{
    QString info = QString("管理员: %1").arg(m_currentUser.name);
    m_userInfoLabel->setText(info);
}

void AdminPanel::loadStatistics()
{
    QList<UserInfo> students = DatabaseManager::instance().getStudents();
    double totalBalance = 0;
    for (const UserInfo& s : students) {
        totalBalance += s.balance;
    }
    
    QList<RechargeRecord> records = DatabaseManager::instance().getAllRechargeRecords();
    double totalRecharge = 0;
    for (const RechargeRecord& r : records) {
        totalRecharge += r.amount;
    }
    
    m_totalStudentsLabel->setText(QString("学生总数\n%1 人").arg(students.size()));
    m_totalBalanceLabel->setText(QString("总余额\n¥ %1").arg(totalBalance, 0, 'f', 2));
    m_totalRechargeLabel->setText(QString("总充值金额\n¥ %1").arg(totalRecharge, 0, 'f', 2));
}

void AdminPanel::loadStudents()
{
    QList<UserInfo> students = DatabaseManager::instance().getAllUsers();
    
    m_studentsTable->setRowCount(students.size());
    
    for (int i = 0; i < students.size(); ++i) {
        const UserInfo& student = students[i];
        m_studentsTable->setItem(i, 0, new QTableWidgetItem(QString::number(student.id)));
        m_studentsTable->setItem(i, 1, new QTableWidgetItem(student.username));
        m_studentsTable->setItem(i, 2, new QTableWidgetItem(student.name));
        m_studentsTable->setItem(i, 3, new QTableWidgetItem(student.studentId));
        m_studentsTable->setItem(i, 4, new QTableWidgetItem(student.dormitory));
        m_studentsTable->setItem(i, 5, new QTableWidgetItem(QString::number(student.balance, 'f', 2)));
        m_studentsTable->setItem(i, 6, new QTableWidgetItem(student.role == 0 ? "管理员" : "学生"));
    }
}

void AdminPanel::loadDormitories()
{
    QList<DormitoryInfo> dorms = DatabaseManager::instance().getAllDormitories();
    
    m_dormitoriesTable->setRowCount(dorms.size());
    
    for (int i = 0; i < dorms.size(); ++i) {
        const DormitoryInfo& dorm = dorms[i];
        m_dormitoriesTable->setItem(i, 0, new QTableWidgetItem(QString::number(dorm.id)));
        m_dormitoriesTable->setItem(i, 1, new QTableWidgetItem(dorm.dormNumber));
        m_dormitoriesTable->setItem(i, 2, new QTableWidgetItem(dorm.building));
        m_dormitoriesTable->setItem(i, 3, new QTableWidgetItem(QString::number(dorm.floor)));
        m_dormitoriesTable->setItem(i, 4, new QTableWidgetItem(QString::number(dorm.remainingKwh, 'f', 2)));
        m_dormitoriesTable->setItem(i, 5, new QTableWidgetItem(QString::number(dorm.currentBalance, 'f', 2)));
        m_dormitoriesTable->setItem(i, 6, new QTableWidgetItem(dorm.lastKwhUpdate.toString("yyyy-MM-dd hh:mm:ss")));
    }
    
    loadKwhChangeRecords();
}

void AdminPanel::loadKwhChangeRecords()
{
    QList<ElectricityKwhChangeRecord> records = DatabaseManager::instance().getAllElectricityKwhChangeRecords();
    
    m_kwhChangeRecordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const ElectricityKwhChangeRecord& record = records[i];
        m_kwhChangeRecordsTable->setItem(i, 0, new QTableWidgetItem(QString::number(record.id)));
        m_kwhChangeRecordsTable->setItem(i, 1, new QTableWidgetItem(record.changeTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_kwhChangeRecordsTable->setItem(i, 2, new QTableWidgetItem(record.dormitory));
        m_kwhChangeRecordsTable->setItem(i, 3, new QTableWidgetItem(QString::number(record.kwhBefore, 'f', 2)));
        m_kwhChangeRecordsTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.kwhAfter, 'f', 2)));
        m_kwhChangeRecordsTable->setItem(i, 5, new QTableWidgetItem(QString::number(record.kwhChange, 'f', 2)));
        m_kwhChangeRecordsTable->setItem(i, 6, new QTableWidgetItem(record.changeType));
        m_kwhChangeRecordsTable->setItem(i, 7, new QTableWidgetItem(record.operatorName));
    }
}

void AdminPanel::loadRechargeRecords()
{
    QList<RechargeRecord> records = DatabaseManager::instance().getAllRechargeRecords();
    
    m_rechargeRecordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const RechargeRecord& record = records[i];
        m_rechargeRecordsTable->setItem(i, 0, new QTableWidgetItem(QString::number(record.id)));
        m_rechargeRecordsTable->setItem(i, 1, new QTableWidgetItem(record.rechargeTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_rechargeRecordsTable->setItem(i, 2, new QTableWidgetItem(record.studentId));
        m_rechargeRecordsTable->setItem(i, 3, new QTableWidgetItem(record.dormitory));
        m_rechargeRecordsTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.amount, 'f', 2)));
        m_rechargeRecordsTable->setItem(i, 5, new QTableWidgetItem(QString::number(record.balanceAfter, 'f', 2)));
        m_rechargeRecordsTable->setItem(i, 6, new QTableWidgetItem(record.operatorName));
    }
}

void AdminPanel::loadElectricityRecords()
{
    QList<ElectricityRecord> records = DatabaseManager::instance().getAllElectricityRecords();
    
    m_electricityRecordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        const ElectricityRecord& record = records[i];
        m_electricityRecordsTable->setItem(i, 0, new QTableWidgetItem(QString::number(record.id)));
        m_electricityRecordsTable->setItem(i, 1, new QTableWidgetItem(record.recordTime.toString("yyyy-MM-dd hh:mm:ss")));
        m_electricityRecordsTable->setItem(i, 2, new QTableWidgetItem(QString::number(record.userId)));
        m_electricityRecordsTable->setItem(i, 3, new QTableWidgetItem(record.dormitory));
        m_electricityRecordsTable->setItem(i, 4, new QTableWidgetItem(QString::number(record.usage, 'f', 2)));
        m_electricityRecordsTable->setItem(i, 5, new QTableWidgetItem(QString::number(record.cost, 'f', 2)));
        m_electricityRecordsTable->setItem(i, 6, new QTableWidgetItem(record.remark));
    }
}

void AdminPanel::onLogoutClicked()
{
    emit logout();
    close();
}

void AdminPanel::onAddStudentClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加学生");
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *usernameEdit = new QLineEdit();
    QLineEdit *passwordEdit = new QLineEdit();
    QLineEdit *nameEdit = new QLineEdit();
    QLineEdit *studentIdEdit = new QLineEdit();
    QLineEdit *dormEdit = new QLineEdit();
    
    formLayout->addRow("用户名:", usernameEdit);
    formLayout->addRow("密码:", passwordEdit);
    formLayout->addRow("姓名:", nameEdit);
    formLayout->addRow("学号:", studentIdEdit);
    formLayout->addRow("宿舍:", dormEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        UserInfo student;
        student.username = usernameEdit->text();
        student.password = passwordEdit->text();
        student.name = nameEdit->text();
        student.studentId = studentIdEdit->text();
        student.dormitory = dormEdit->text();
        student.role = 1;
        student.balance = 0.0;
        
        if (DatabaseManager::instance().addUser(student)) {
            QMessageBox::information(this, "成功", "学生添加成功！");
            loadStudents();
            loadStatistics();
        } else {
            QMessageBox::warning(this, "失败", "学生添加失败，用户名可能已存在！");
        }
    }
}

void AdminPanel::onEditStudentClicked()
{
    int currentRow = m_studentsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的学生！");
        return;
    }
    
    int userId = m_studentsTable->item(currentRow, 0)->text().toInt();
    UserInfo student = DatabaseManager::instance().getUserById(userId);
    
    QDialog dialog(this);
    dialog.setWindowTitle("编辑学生");
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *usernameEdit = new QLineEdit(student.username);
    QLineEdit *nameEdit = new QLineEdit(student.name);
    QLineEdit *studentIdEdit = new QLineEdit(student.studentId);
    QLineEdit *dormEdit = new QLineEdit(student.dormitory);
    
    formLayout->addRow("用户名:", usernameEdit);
    formLayout->addRow("姓名:", nameEdit);
    formLayout->addRow("学号:", studentIdEdit);
    formLayout->addRow("宿舍:", dormEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        student.username = usernameEdit->text();
        student.name = nameEdit->text();
        student.studentId = studentIdEdit->text();
        student.dormitory = dormEdit->text();
        
        if (DatabaseManager::instance().updateUser(student)) {
            QMessageBox::information(this, "成功", "学生信息更新成功！");
            loadStudents();
        } else {
            QMessageBox::warning(this, "失败", "学生信息更新失败！");
        }
    }
}

void AdminPanel::onDeleteStudentClicked()
{
    int currentRow = m_studentsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的学生！");
        return;
    }
    
    int userId = m_studentsTable->item(currentRow, 0)->text().toInt();
    
    auto reply = QMessageBox::question(this, "确认", "确定要删除该学生吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteUser(userId)) {
            QMessageBox::information(this, "成功", "学生删除成功！");
            loadStudents();
            loadStatistics();
        } else {
            QMessageBox::warning(this, "失败", "学生删除失败！");
        }
    }
}

void AdminPanel::onAddDormitoryClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加宿舍");
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *dormNumberEdit = new QLineEdit();
    QLineEdit *buildingEdit = new QLineEdit();
    QLineEdit *floorEdit = new QLineEdit();
    
    formLayout->addRow("宿舍号:", dormNumberEdit);
    formLayout->addRow("楼栋:", buildingEdit);
    formLayout->addRow("楼层:", floorEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        DormitoryInfo dorm;
        dorm.dormNumber = dormNumberEdit->text();
        dorm.building = buildingEdit->text();
        dorm.floor = floorEdit->text().toInt();
        dorm.currentBalance = 0.0;
        dorm.lastReading = 0.0;
        
        if (DatabaseManager::instance().addDormitory(dorm)) {
            QMessageBox::information(this, "成功", "宿舍添加成功！");
            loadDormitories();
        } else {
            QMessageBox::warning(this, "失败", "宿舍添加失败，宿舍号可能已存在！");
        }
    }
}

void AdminPanel::onEditDormitoryClicked()
{
    int currentRow = m_dormitoriesTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的宿舍！");
        return;
    }
    
    int dormId = m_dormitoriesTable->item(currentRow, 0)->text().toInt();
    DormitoryInfo dorm = DatabaseManager::instance().getDormitoryById(dormId);
    
    QDialog dialog(this);
    dialog.setWindowTitle("编辑宿舍");
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *dormNumberEdit = new QLineEdit(dorm.dormNumber);
    QLineEdit *buildingEdit = new QLineEdit(dorm.building);
    QLineEdit *floorEdit = new QLineEdit(QString::number(dorm.floor));
    
    formLayout->addRow("宿舍号:", dormNumberEdit);
    formLayout->addRow("楼栋:", buildingEdit);
    formLayout->addRow("楼层:", floorEdit);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        dorm.dormNumber = dormNumberEdit->text();
        dorm.building = buildingEdit->text();
        dorm.floor = floorEdit->text().toInt();
        
        if (DatabaseManager::instance().updateDormitory(dorm)) {
            QMessageBox::information(this, "成功", "宿舍信息更新成功！");
            loadDormitories();
        } else {
            QMessageBox::warning(this, "失败", "宿舍信息更新失败！");
        }
    }
}

void AdminPanel::onDeleteDormitoryClicked()
{
    int currentRow = m_dormitoriesTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的宿舍！");
        return;
    }
    
    int dormId = m_dormitoriesTable->item(currentRow, 0)->text().toInt();
    
    auto reply = QMessageBox::question(this, "确认", "确定要删除该宿舍吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteDormitory(dormId)) {
            QMessageBox::information(this, "成功", "宿舍删除成功！");
            loadDormitories();
        } else {
            QMessageBox::warning(this, "失败", "宿舍删除失败！");
        }
    }
}

void AdminPanel::onRechargeForStudentClicked()
{
    int currentRow = m_studentsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要充值的学生！");
        return;
    }
    
    int userId = m_studentsTable->item(currentRow, 0)->text().toInt();
    QString studentName = m_studentsTable->item(currentRow, 2)->text();
    
    bool ok;
    double amount = QInputDialog::getDouble(this, "充值", QString("为学生 %1 充值金额(元):").arg(studentName), 50, 10, 10000, 2, &ok);
    
    if (ok && amount > 0) {
        if (DatabaseManager::instance().recharge(userId, amount, m_currentUser.name)) {
            QMessageBox::information(this, "成功", QString("充值成功! 充值金额: ¥%1").arg(amount, 0, 'f', 2));
            loadStudents();
            loadDormitories();
            loadStatistics();
            loadRechargeRecords();
        } else {
            QMessageBox::warning(this, "失败", "充值失败,请稍后重试!");
        }
    }
}

void AdminPanel::onDeductElectricityClicked()
{
    int currentRow = m_dormitoriesTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要扣费的宿舍！");
        return;
    }
    
    QString dormNumber = m_dormitoriesTable->item(currentRow, 1)->text();
    
    bool ok;
    double cost = QInputDialog::getDouble(this, "电费扣费", QString("宿舍 %1 扣费金额(元):").arg(dormNumber), 10, 1, 1000, 2, &ok);
    
    if (ok && cost > 0) {
        if (DatabaseManager::instance().deductElectricityCost(dormNumber, cost)) {
            QMessageBox::information(this, "成功", QString("扣费成功! 扣费金额: ¥%1").arg(cost, 0, 'f', 2));
            loadStudents();
            loadDormitories();
            loadElectricityRecords();
            loadStatistics();
        } else {
            QMessageBox::warning(this, "失败", "扣费失败,请稍后重试!");
        }
    }
}

void AdminPanel::onRefreshClicked()
{
    loadStatistics();
    loadStudents();
    loadDormitories();
    loadKwhChangeRecords();
    loadRechargeRecords();
    loadElectricityRecords();
    updateRechargeChart();
    updateElectricityChart();
    QMessageBox::information(this, "刷新", "数据已刷新!");
}

void AdminPanel::onTabChanged(int index)
{
    if (index == 1) {
        loadStudents();
    } else if (index == 2) {
        loadDormitories();
    } else if (index == 3) {
        loadRechargeRecords();
        updateRechargeChart();
    } else if (index == 4) {
        loadElectricityRecords();
        updateElectricityChart();
    }
}

void AdminPanel::updateRechargeChart()
{
    QList<RechargeRecord> records = DatabaseManager::instance().getAllRechargeRecords();
    
    QChart *chart = new QChart();
    chart->setTitle("充值金额趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QLineSeries *series = new QLineSeries();
    series->setName("充值金额(元)");
    
    // 只显示最近20条记录
    int startIndex = qMax(0, records.size() - 20);
    double cumulativeAmount = 0;
    for (int i = startIndex; i < records.size(); ++i) {
        const RechargeRecord& record = records[i];
        QDateTime dateTime = record.rechargeTime;
        qint64 timestamp = dateTime.toMSecsSinceEpoch();
        cumulativeAmount += record.amount;
        series->append(timestamp, cumulativeAmount);
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
    axisY->setTitleText("累计充值金额(元)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_rechargeChartView->setChart(chart);
}

void AdminPanel::updateElectricityChart()
{
    QList<ElectricityRecord> records = DatabaseManager::instance().getAllElectricityRecords();
    
    QChart *chart = new QChart();
    chart->setTitle("用电量趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QLineSeries *series = new QLineSeries();
    series->setName("用电量(kWh)");
    
    // 只显示最近20条记录
    int startIndex = qMax(0, records.size() - 20);
    for (int i = startIndex; i < records.size(); ++i) {
        const ElectricityRecord& record = records[i];
        QDateTime dateTime = record.recordTime;
        qint64 timestamp = dateTime.toMSecsSinceEpoch();
        series->append(timestamp, record.usage);
    }
    
    chart->addSeries(series);
    
    // 设置X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("MM-dd hh:mm");
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    // 设置Y轴（用电量轴）
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("用电量(kWh)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_electricityChartView->setChart(chart);
}

void AdminPanel::onBatchRechargeClicked()
{
    QList<QTableWidgetItem*> selectedItems = m_studentsTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要充值的学生！");
        return;
    }
    
    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }
    
    bool ok;
    double amount = QInputDialog::getDouble(this, "批量充值", "请输入充值金额（元）:", 0, 0, 10000, 2, &ok);
    if (!ok || amount <= 0) {
        return;
    }
    
    int successCount = 0;
    for (int row : selectedRows) {
        int userId = m_studentsTable->item(row, 0)->text().toInt();
        if (DatabaseManager::instance().recharge(userId, amount, m_currentUser.name)) {
            successCount++;
        }
    }
    
    loadStudents();
    loadDormitories();
    loadStatistics();
    loadRechargeRecords();
    updateRechargeChart();
    
    QMessageBox::information(this, "批量充值完成", 
        QString("成功为 %1 名学生充值 %2 元").arg(successCount).arg(amount));
}

void AdminPanel::onBatchDeductClicked()
{
    QList<QTableWidgetItem*> selectedItems = m_studentsTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要扣费的学生！");
        return;
    }
    
    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }
    
    bool ok;
    double amount = QInputDialog::getDouble(this, "批量扣费", "请输入扣费金额（元）:", 0, 0, 10000, 2, &ok);
    if (!ok || amount <= 0) {
        return;
    }
    
    int successCount = 0;
    for (int row : selectedRows) {
        int userId = m_studentsTable->item(row, 0)->text().toInt();
        UserInfo user = DatabaseManager::instance().getUserById(userId);
        double newBalance = user.balance - amount;
        if (newBalance < 0) newBalance = 0;
        
        if (DatabaseManager::instance().updateBalance(userId, newBalance)) {
            ElectricityChangeRecord changeRecord;
            changeRecord.userId = userId;
            changeRecord.studentId = user.studentId;
            changeRecord.dormitory = user.dormitory;
            changeRecord.changeAmount = -amount;
            changeRecord.balanceBefore = user.balance;
            changeRecord.balanceAfter = newBalance;
            changeRecord.changeType = "批量扣费";
            changeRecord.operatorName = m_currentUser.name;
            changeRecord.remark = QString("批量扣费 %1 元").arg(amount);
            DatabaseManager::instance().addElectricityChangeRecord(changeRecord);
            successCount++;
        }
    }
    
    loadStudents();
    loadDormitories();
    loadStatistics();
    
    QMessageBox::information(this, "批量扣费完成", 
        QString("成功为 %1 名学生扣费 %2 元").arg(successCount).arg(amount));
}

void AdminPanel::onExportToExcelClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出Excel", "", "CSV文件 (*.csv);;所有文件 (*.*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件！");
        return;
    }

    QTextStream out(&file);
    out.setGenerateByteOrderMark(true);

    out << "ID,用户名,姓名,学号,宿舍,余额(元),角色\n";

    QList<UserInfo> students = DatabaseManager::instance().getAllUsers();
    for (const UserInfo& student : students) {
        out << student.id << ","
            << student.username << ","
            << student.name << ","
            << student.studentId << ","
            << student.dormitory << ","
            << QString::number(student.balance, 'f', 2) << ","
            << (student.role == 0 ? "管理员" : "学生") << "\n";
    }

    file.close();
    QMessageBox::information(this, "导出成功", QString("数据已导出到:\n%1").arg(fileName));
}

void AdminPanel::onImportFromExcelClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "导入Excel", "", "CSV文件 (*.csv);;所有文件 (*.*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件！");
        return;
    }

    QTextStream in(&file);

    in.readLine();

    int importCount = 0;
    int updateCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() >= 5) {
            QString username = fields[1].trimmed();
            QString name = fields[2].trimmed();
            QString studentId = fields[3].trimmed();
            QString dormitory = fields[4].trimmed();

            UserInfo existingUser = DatabaseManager::instance().getUserByUsername(username);

            if (existingUser.id == -1) {
                UserInfo newUser;
                newUser.username = username;
                newUser.password = "123456";
                newUser.name = name;
                newUser.studentId = studentId;
                newUser.dormitory = dormitory;
                newUser.role = 1;
                newUser.balance = 0.0;

                if (DatabaseManager::instance().addUser(newUser)) {
                    importCount++;
                }
            } else {
                existingUser.name = name;
                existingUser.studentId = studentId;
                existingUser.dormitory = dormitory;

                if (DatabaseManager::instance().updateUser(existingUser)) {
                    updateCount++;
                }
            }
        }
    }

    file.close();
    loadStudents();
    loadStatistics();

    QMessageBox::information(this, "导入成功", QString("导入完成！\n新增: %1 条\n更新: %2 条").arg(importCount).arg(updateCount));
}

QWidget* AdminPanel::initFaceAuditTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("人脸信息审核");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QLabel *tipLabel = new QLabel("提示：双击表格行可查看详情并进行审核处理");
    tipLabel->setStyleSheet("font-size: 12px; color: #666; padding: 5px;");
    layout->addWidget(tipLabel);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::loadPendingFaceInfos);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_faceAuditTable = new QTableWidget();
    m_faceAuditTable->setColumnCount(7);
    m_faceAuditTable->setHorizontalHeaderLabels({"ID", "学号", "姓名", "宿舍", "提交时间", "状态", "拒绝原因"});
    m_faceAuditTable->horizontalHeader()->setStretchLastSection(true);
    m_faceAuditTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_faceAuditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_faceAuditTable->setAlternatingRowColors(true);
    connect(m_faceAuditTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        Q_UNUSED(column);
        QTableWidgetItem *idItem = m_faceAuditTable->item(row, 0);
        if (idItem) {
            int faceId = idItem->text().toInt();
            showFaceAuditDialog(faceId);
        }
    });
    layout->addWidget(m_faceAuditTable);
    
    loadPendingFaceInfos();
    
    return tab;
}

void AdminPanel::showFaceAuditDialog(int faceId)
{
    FaceInfo info = DatabaseManager::instance().getFaceInfoById(faceId);
    if (info.id == -1) return;
    
    QDialog dialog(this);
    dialog.setWindowTitle("人脸信息审核");
    dialog.setMinimumSize(500, 400);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QHBoxLayout *infoLayout = new QHBoxLayout();
    
    QLabel *imageLabel = new QLabel();
    imageLabel->setFixedSize(200, 200);
    imageLabel->setStyleSheet("border: 1px solid #ccc; background-color: #f5f5f5;");
    imageLabel->setAlignment(Qt::AlignCenter);
    if (!info.faceImagePath.isEmpty()) {
        QPixmap pixmap(info.faceImagePath);
        if (!pixmap.isNull()) {
            imageLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setText("无法加载图片");
        }
    } else {
        imageLabel->setText("暂无照片");
    }
    infoLayout->addWidget(imageLabel);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("学号:", new QLabel(info.studentId));
    formLayout->addRow("姓名:", new QLabel(info.name));
    formLayout->addRow("宿舍:", new QLabel(info.dormitory));
    formLayout->addRow("提交时间:", new QLabel(info.submitTime.toString("yyyy-MM-dd hh:mm:ss")));
    
    QString statusText;
    switch (info.status) {
        case 0: statusText = "待审核"; break;
        case 1: statusText = "已通过"; break;
        case 2: statusText = "已拒绝"; break;
    }
    QLabel *statusLabel = new QLabel(statusText);
    if (info.status == 0) statusLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    else if (info.status == 1) statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    else statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    formLayout->addRow("状态:", statusLabel);
    
    if (!info.rejectReason.isEmpty()) {
        formLayout->addRow("拒绝原因:", new QLabel(info.rejectReason));
    }
    if (!info.auditorName.isEmpty()) {
        formLayout->addRow("审核人:", new QLabel(info.auditorName));
    }
    if (info.auditTime.isValid()) {
        formLayout->addRow("审核时间:", new QLabel(info.auditTime.toString("yyyy-MM-dd hh:mm:ss")));
    }
    
    infoLayout->addLayout(formLayout);
    layout->addLayout(infoLayout);
    
    if (info.status == 0) {
        QLineEdit *reasonEdit = new QLineEdit();
        reasonEdit->setPlaceholderText("如拒绝请输入原因");
        layout->addWidget(reasonEdit);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox();
        QPushButton *approveBtn = buttonBox->addButton("通过审核", QDialogButtonBox::AcceptRole);
        approveBtn->setStyleSheet("padding: 10px 30px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
        
        QPushButton *rejectBtn = buttonBox->addButton("拒绝", QDialogButtonBox::RejectRole);
        rejectBtn->setStyleSheet("padding: 10px 30px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
        
        QPushButton *cancelBtn = buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
        cancelBtn->setStyleSheet("padding: 10px 30px; background-color: #95a5a6; color: white; border: none; border-radius: 4px;");
        
        layout->addWidget(buttonBox);
        
        connect(approveBtn, &QPushButton::clicked, this, [&]() {
            if (DatabaseManager::instance().auditFaceInfo(faceId, 1, m_currentUser.name)) {
                QMessageBox::information(&dialog, "成功", "人脸信息已通过审核！");
                dialog.accept();
                loadPendingFaceInfos();
            }
        });
        
        connect(rejectBtn, &QPushButton::clicked, this, [&]() {
            QString reason = reasonEdit->text().trimmed();
            if (reason.isEmpty()) {
                QMessageBox::warning(&dialog, "提示", "请输入拒绝原因！");
                return;
            }
            if (DatabaseManager::instance().auditFaceInfo(faceId, 2, m_currentUser.name, reason)) {
                QMessageBox::information(&dialog, "成功", "人脸信息已拒绝！");
                dialog.accept();
                loadPendingFaceInfos();
            }
        });
        
        connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    } else {
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        layout->addWidget(buttonBox);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    }
    
    dialog.exec();
}

void AdminPanel::loadPendingFaceInfos()
{
    QList<FaceInfo> faceInfos = DatabaseManager::instance().getAllFaceInfos();
    
    m_faceAuditTable->setRowCount(faceInfos.size());
    
    for (int i = 0; i < faceInfos.size(); ++i) {
        const FaceInfo& info = faceInfos[i];
        m_faceAuditTable->setItem(i, 0, new QTableWidgetItem(QString::number(info.id)));
        m_faceAuditTable->setItem(i, 1, new QTableWidgetItem(info.studentId));
        m_faceAuditTable->setItem(i, 2, new QTableWidgetItem(info.name));
        m_faceAuditTable->setItem(i, 3, new QTableWidgetItem(info.dormitory));
        m_faceAuditTable->setItem(i, 4, new QTableWidgetItem(info.submitTime.toString("yyyy-MM-dd hh:mm")));
        
        QString statusText;
        switch (info.status) {
            case 0: statusText = "待审核"; break;
            case 1: statusText = "已通过"; break;
            case 2: statusText = "已拒绝"; break;
        }
        m_faceAuditTable->setItem(i, 5, new QTableWidgetItem(statusText));
        m_faceAuditTable->setItem(i, 6, new QTableWidgetItem(info.rejectReason));
    }
}

void AdminPanel::onAuditFaceInfoClicked()
{
    loadPendingFaceInfos();
}

QWidget* AdminPanel::initRepairHandleTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("维修申请处理");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QLabel *tipLabel = new QLabel("提示：双击表格行可查看详情并进行处理");
    tipLabel->setStyleSheet("font-size: 12px; color: #666; padding: 5px;");
    layout->addWidget(tipLabel);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::loadPendingRepairRequests);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_repairHandleTable = new QTableWidget();
    m_repairHandleTable->setColumnCount(8);
    m_repairHandleTable->setHorizontalHeaderLabels({"ID", "宿舍", "姓名", "维修类型", "问题描述", "优先级", "状态", "处理结果"});
    m_repairHandleTable->horizontalHeader()->setStretchLastSection(true);
    m_repairHandleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_repairHandleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_repairHandleTable->setAlternatingRowColors(true);
    connect(m_repairHandleTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        Q_UNUSED(column);
        QTableWidgetItem *idItem = m_repairHandleTable->item(row, 0);
        if (idItem) {
            int requestId = idItem->text().toInt();
            showRepairHandleDialog(requestId);
        }
    });
    layout->addWidget(m_repairHandleTable);
    
    loadPendingRepairRequests();
    
    return tab;
}

void AdminPanel::showRepairHandleDialog(int requestId)
{
    RepairRequest req = DatabaseManager::instance().getRepairRequestById(requestId);
    if (req.id == -1) return;
    
    QDialog dialog(this);
    dialog.setWindowTitle("维修申请处理");
    dialog.setMinimumSize(500, 450);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("申请ID:", new QLabel(QString::number(req.id)));
    formLayout->addRow("宿舍:", new QLabel(req.dormitory));
    formLayout->addRow("申请人:", new QLabel(req.name));
    formLayout->addRow("学号:", new QLabel(req.studentId));
    formLayout->addRow("联系电话:", new QLabel(req.contactPhone));
    formLayout->addRow("维修类型:", new QLabel(req.repairTypeText));
    
    QString priorityText;
    switch (req.priority) {
        case 0: priorityText = "普通"; break;
        case 1: priorityText = "紧急"; break;
        case 2: priorityText = "非常紧急"; break;
    }
    QLabel *priorityLabel = new QLabel(priorityText);
    if (req.priority == 2) priorityLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    else if (req.priority == 1) priorityLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    formLayout->addRow("优先级:", priorityLabel);
    
    QTextEdit *descEdit = new QTextEdit();
    descEdit->setPlainText(req.description);
    descEdit->setReadOnly(true);
    descEdit->setMaximumHeight(80);
    formLayout->addRow("问题描述:", descEdit);
    
    formLayout->addRow("提交时间:", new QLabel(req.submitTime.toString("yyyy-MM-dd hh:mm:ss")));
    
    QString statusText;
    switch (req.status) {
        case 0: statusText = "待处理"; break;
        case 1: statusText = "处理中"; break;
        case 2: statusText = "已完成"; break;
        case 3: statusText = "已关闭"; break;
    }
    QLabel *statusLabel = new QLabel(statusText);
    if (req.status == 0) statusLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    else if (req.status == 1) statusLabel->setStyleSheet("color: #3498db; font-weight: bold;");
    else if (req.status == 2) statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    else statusLabel->setStyleSheet("color: #95a5a6; font-weight: bold;");
    formLayout->addRow("状态:", statusLabel);
    
    if (!req.handlerName.isEmpty()) {
        formLayout->addRow("处理人:", new QLabel(req.handlerName));
    }
    if (!req.handleResult.isEmpty()) {
        formLayout->addRow("处理结果:", new QLabel(req.handleResult));
    }
    
    layout->addLayout(formLayout);
    
    if (req.status < 2) {
        QTextEdit *resultEdit = new QTextEdit();
        resultEdit->setPlaceholderText("请输入处理结果...");
        resultEdit->setMaximumHeight(60);
        layout->addWidget(resultEdit);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox();
        
        QPushButton *startBtn = nullptr;
        QPushButton *completeBtn = nullptr;
        
        if (req.status == 0) {
            startBtn = buttonBox->addButton("开始处理", QDialogButtonBox::AcceptRole);
            startBtn->setStyleSheet("padding: 10px 30px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
        } else {
            completeBtn = buttonBox->addButton("完成处理", QDialogButtonBox::AcceptRole);
            completeBtn->setStyleSheet("padding: 10px 30px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
        }
        
        QPushButton *closeBtn = buttonBox->addButton("关闭申请", QDialogButtonBox::RejectRole);
        closeBtn->setStyleSheet("padding: 10px 30px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
        
        QPushButton *cancelBtn = buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
        cancelBtn->setStyleSheet("padding: 10px 30px; background-color: #95a5a6; color: white; border: none; border-radius: 4px;");
        
        layout->addWidget(buttonBox);
        
        if (startBtn) {
            connect(startBtn, &QPushButton::clicked, this, [&]() {
                if (DatabaseManager::instance().handleRepairRequest(requestId, 1, m_currentUser.name)) {
                    QMessageBox::information(&dialog, "成功", "维修申请已开始处理！");
                    dialog.accept();
                    loadPendingRepairRequests();
                }
            });
        }
        
        if (completeBtn) {
            connect(completeBtn, &QPushButton::clicked, this, [&]() {
                QString result = resultEdit->toPlainText().trimmed();
                if (result.isEmpty()) {
                    QMessageBox::warning(&dialog, "提示", "请输入处理结果！");
                    return;
                }
                if (DatabaseManager::instance().handleRepairRequest(requestId, 2, m_currentUser.name, result)) {
                    QMessageBox::information(&dialog, "成功", "维修已完成！");
                    dialog.accept();
                    loadPendingRepairRequests();
                }
            });
        }
        
        connect(closeBtn, &QPushButton::clicked, this, [&]() {
            if (QMessageBox::question(&dialog, "确认", "确定要关闭此维修申请吗？") == QMessageBox::Yes) {
                if (DatabaseManager::instance().handleRepairRequest(requestId, 3, m_currentUser.name, "已关闭")) {
                    QMessageBox::information(&dialog, "成功", "维修申请已关闭！");
                    dialog.accept();
                    loadPendingRepairRequests();
                }
            }
        });
        
        connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    } else {
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        layout->addWidget(buttonBox);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    }
    
    dialog.exec();
}

void AdminPanel::loadPendingRepairRequests()
{
    QList<RepairRequest> requests = DatabaseManager::instance().getAllRepairRequests();
    
    m_repairHandleTable->setRowCount(requests.size());
    
    for (int i = 0; i < requests.size(); ++i) {
        const RepairRequest& req = requests[i];
        m_repairHandleTable->setItem(i, 0, new QTableWidgetItem(QString::number(req.id)));
        m_repairHandleTable->setItem(i, 1, new QTableWidgetItem(req.dormitory));
        m_repairHandleTable->setItem(i, 2, new QTableWidgetItem(req.name));
        m_repairHandleTable->setItem(i, 3, new QTableWidgetItem(req.repairTypeText));
        m_repairHandleTable->setItem(i, 4, new QTableWidgetItem(req.description.left(20) + (req.description.length() > 20 ? "..." : "")));
        
        QString priorityText;
        switch (req.priority) {
            case 0: priorityText = "普通"; break;
            case 1: priorityText = "紧急"; break;
            case 2: priorityText = "非常紧急"; break;
        }
        m_repairHandleTable->setItem(i, 5, new QTableWidgetItem(priorityText));
        
        QString statusText;
        switch (req.status) {
            case 0: statusText = "待处理"; break;
            case 1: statusText = "处理中"; break;
            case 2: statusText = "已完成"; break;
            case 3: statusText = "已关闭"; break;
        }
        m_repairHandleTable->setItem(i, 6, new QTableWidgetItem(statusText));
        m_repairHandleTable->setItem(i, 7, new QTableWidgetItem(req.handleResult));
    }
}

void AdminPanel::onHandleRepairClicked()
{
    loadPendingRepairRequests();
}

QWidget* AdminPanel::initRoomChangeAuditTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("换寝申请审核");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QLabel *tipLabel = new QLabel("提示：双击表格行可查看详情并进行审核处理");
    tipLabel->setStyleSheet("font-size: 12px; color: #666; padding: 5px;");
    layout->addWidget(tipLabel);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::loadPendingRoomChangeRequests);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_roomChangeAuditTable = new QTableWidget();
    m_roomChangeAuditTable->setColumnCount(7);
    m_roomChangeAuditTable->setHorizontalHeaderLabels({"ID", "学号", "姓名", "当前宿舍", "目标宿舍", "换寝原因", "状态"});
    m_roomChangeAuditTable->horizontalHeader()->setStretchLastSection(true);
    m_roomChangeAuditTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_roomChangeAuditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_roomChangeAuditTable->setAlternatingRowColors(true);
    connect(m_roomChangeAuditTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        Q_UNUSED(column);
        QTableWidgetItem *idItem = m_roomChangeAuditTable->item(row, 0);
        if (idItem) {
            int requestId = idItem->text().toInt();
            showRoomChangeDialog(requestId);
        }
    });
    layout->addWidget(m_roomChangeAuditTable);
    
    loadPendingRoomChangeRequests();
    
    return tab;
}

void AdminPanel::showRoomChangeDialog(int requestId)
{
    RoomChangeRequest req = DatabaseManager::instance().getRoomChangeRequestById(requestId);
    if (req.id == -1) return;
    
    QDialog dialog(this);
    dialog.setWindowTitle("换寝申请审核");
    dialog.setMinimumSize(500, 400);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("申请ID:", new QLabel(QString::number(req.id)));
    formLayout->addRow("学号:", new QLabel(req.studentId));
    formLayout->addRow("姓名:", new QLabel(req.name));
    formLayout->addRow("当前宿舍:", new QLabel(req.currentDormitory));
    formLayout->addRow("目标宿舍:", new QLabel(req.targetDormitory));
    formLayout->addRow("换寝原因:", new QLabel(req.changeReasonText));
    
    QTextEdit *reasonEdit = new QTextEdit();
    reasonEdit->setPlainText(req.description);
    reasonEdit->setReadOnly(true);
    reasonEdit->setMaximumHeight(80);
    formLayout->addRow("详细说明:", reasonEdit);
    
    formLayout->addRow("提交时间:", new QLabel(req.submitTime.toString("yyyy-MM-dd hh:mm:ss")));
    
    QString statusText;
    switch (req.status) {
        case 0: statusText = "待审核"; break;
        case 1: statusText = "已通过"; break;
        case 2: statusText = "已拒绝"; break;
        case 3: statusText = "已完成"; break;
    }
    QLabel *statusLabel = new QLabel(statusText);
    if (req.status == 0) statusLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    else if (req.status == 1) statusLabel->setStyleSheet("color: #3498db; font-weight: bold;");
    else if (req.status == 2) statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    else statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    formLayout->addRow("状态:", statusLabel);
    
    if (!req.auditorName.isEmpty()) {
        formLayout->addRow("审核人:", new QLabel(req.auditorName));
    }
    if (!req.rejectReason.isEmpty()) {
        formLayout->addRow("拒绝原因:", new QLabel(req.rejectReason));
    }
    
    layout->addLayout(formLayout);
    
    if (req.status == 0) {
        QLineEdit *rejectReasonEdit = new QLineEdit();
        rejectReasonEdit->setPlaceholderText("如拒绝请输入原因");
        layout->addWidget(rejectReasonEdit);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox();
        
        QPushButton *approveBtn = buttonBox->addButton("通过审核", QDialogButtonBox::AcceptRole);
        approveBtn->setStyleSheet("padding: 10px 30px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
        
        QPushButton *rejectBtn = buttonBox->addButton("拒绝", QDialogButtonBox::RejectRole);
        rejectBtn->setStyleSheet("padding: 10px 30px; background-color: #e74c3c; color: white; border: none; border-radius: 4px;");
        
        QPushButton *cancelBtn = buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
        cancelBtn->setStyleSheet("padding: 10px 30px; background-color: #95a5a6; color: white; border: none; border-radius: 4px;");
        
        layout->addWidget(buttonBox);
        
        connect(approveBtn, &QPushButton::clicked, this, [&]() {
            if (DatabaseManager::instance().auditRoomChangeRequest(requestId, 1, m_currentUser.name)) {
                QMessageBox::information(&dialog, "成功", "换寝申请已通过！");
                dialog.accept();
                loadPendingRoomChangeRequests();
            }
        });
        
        connect(rejectBtn, &QPushButton::clicked, this, [&]() {
            QString reason = rejectReasonEdit->text().trimmed();
            if (reason.isEmpty()) {
                QMessageBox::warning(&dialog, "提示", "请输入拒绝原因！");
                return;
            }
            if (DatabaseManager::instance().auditRoomChangeRequest(requestId, 2, m_currentUser.name, reason)) {
                QMessageBox::information(&dialog, "成功", "换寝申请已拒绝！");
                dialog.accept();
                loadPendingRoomChangeRequests();
            }
        });
        
        connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    } else if (req.status == 1) {
        QDialogButtonBox *buttonBox = new QDialogButtonBox();
        
        QPushButton *completeBtn = buttonBox->addButton("完成换寝", QDialogButtonBox::AcceptRole);
        completeBtn->setStyleSheet("padding: 10px 30px; background-color: #f39c12; color: white; border: none; border-radius: 4px;");
        
        QPushButton *closeBtn = buttonBox->addButton("关闭", QDialogButtonBox::RejectRole);
        closeBtn->setStyleSheet("padding: 10px 30px; background-color: #95a5a6; color: white; border: none; border-radius: 4px;");
        
        layout->addWidget(buttonBox);
        
        connect(completeBtn, &QPushButton::clicked, this, [&]() {
            if (DatabaseManager::instance().completeRoomChange(requestId)) {
                QMessageBox::information(&dialog, "成功", "换寝已完成！学生宿舍信息已更新。");
                dialog.accept();
                loadPendingRoomChangeRequests();
                loadStudents();
                loadDormitories();
            }
        });
        
        connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    } else {
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        layout->addWidget(buttonBox);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    }
    
    dialog.exec();
}

void AdminPanel::loadPendingRoomChangeRequests()
{
    QList<RoomChangeRequest> requests = DatabaseManager::instance().getAllRoomChangeRequests();
    
    m_roomChangeAuditTable->setRowCount(requests.size());
    
    for (int i = 0; i < requests.size(); ++i) {
        const RoomChangeRequest& req = requests[i];
        m_roomChangeAuditTable->setItem(i, 0, new QTableWidgetItem(QString::number(req.id)));
        m_roomChangeAuditTable->setItem(i, 1, new QTableWidgetItem(req.studentId));
        m_roomChangeAuditTable->setItem(i, 2, new QTableWidgetItem(req.name));
        m_roomChangeAuditTable->setItem(i, 3, new QTableWidgetItem(req.currentDormitory));
        m_roomChangeAuditTable->setItem(i, 4, new QTableWidgetItem(req.targetDormitory));
        m_roomChangeAuditTable->setItem(i, 5, new QTableWidgetItem(req.changeReasonText));
        
        QString statusText;
        switch (req.status) {
            case 0: statusText = "待审核"; break;
            case 1: statusText = "已通过"; break;
            case 2: statusText = "已拒绝"; break;
            case 3: statusText = "已完成"; break;
        }
        m_roomChangeAuditTable->setItem(i, 6, new QTableWidgetItem(statusText));
    }
}

void AdminPanel::onAuditRoomChangeClicked()
{
    loadPendingRoomChangeRequests();
}

QWidget* AdminPanel::initApplianceManageTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    QLabel *title = new QLabel("电器控制管理");
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *addBtn = new QPushButton("添加电器");
    addBtn->setStyleSheet("padding: 8px 16px; background-color: #27ae60; color: white; border: none; border-radius: 4px;");
    connect(addBtn, &QPushButton::clicked, this, &AdminPanel::onManageApplianceClicked);
    btnLayout->addWidget(addBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminPanel::loadAllAppliances);
    btnLayout->addWidget(refreshBtn);
    
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    m_applianceManageTable = new QTableWidget();
    m_applianceManageTable->setColumnCount(7);
    m_applianceManageTable->setHorizontalHeaderLabels({"宿舍", "电器名称", "类型", "状态", "功率档位", "日用电量", "操作"});
    m_applianceManageTable->horizontalHeader()->setStretchLastSection(true);
    m_applianceManageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_applianceManageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_applianceManageTable);
    
    loadAllAppliances();
    
    return tab;
}

void AdminPanel::loadAllAppliances()
{
    QList<ApplianceControl> appliances = DatabaseManager::instance().getAllApplianceControls();
    
    m_applianceManageTable->setRowCount(appliances.size());
    
    for (int i = 0; i < appliances.size(); ++i) {
        const ApplianceControl& app = appliances[i];
        m_applianceManageTable->setItem(i, 0, new QTableWidgetItem(app.dormitory));
        m_applianceManageTable->setItem(i, 1, new QTableWidgetItem(app.applianceName));
        m_applianceManageTable->setItem(i, 2, new QTableWidgetItem(app.applianceType));
        m_applianceManageTable->setItem(i, 3, new QTableWidgetItem(app.status == 1 ? "开启" : "关闭"));
        m_applianceManageTable->setItem(i, 4, new QTableWidgetItem(QString::number(app.powerLevel) + "%"));
        m_applianceManageTable->setItem(i, 5, new QTableWidgetItem(QString::number(app.dailyUsage, 'f', 2) + "度"));
        
        QWidget *btnWidget = new QWidget();
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(4, 4, 4, 4);
        
        QPushButton *editBtn = new QPushButton("编辑");
        editBtn->setStyleSheet("padding: 4px 8px; background-color: #f39c12; color: white; border: none; border-radius: 3px;");
        editBtn->setProperty("applianceId", app.id);
        connect(editBtn, &QPushButton::clicked, this, [this, editBtn]() {
            int applianceId = editBtn->property("applianceId").toInt();
            ApplianceControl app = DatabaseManager::instance().getApplianceControlById(applianceId);
            
            QDialog dialog(this);
            dialog.setWindowTitle("编辑电器");
            QFormLayout *formLayout = new QFormLayout(&dialog);
            
            QLineEdit *nameEdit = new QLineEdit(app.applianceName);
            formLayout->addRow("电器名称:", nameEdit);
            
            QComboBox *typeCombo = new QComboBox();
            typeCombo->addItem("空调");
            typeCombo->addItem("热水器");
            typeCombo->addItem("照明");
            typeCombo->addItem("插座");
            typeCombo->addItem("其他");
            typeCombo->setCurrentText(app.applianceType);
            formLayout->addRow("电器类型:", typeCombo);
            
            QSpinBox *powerSpin = new QSpinBox();
            powerSpin->setRange(0, 100);
            powerSpin->setValue(app.powerLevel);
            formLayout->addRow("功率档位:", powerSpin);
            
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            formLayout->addRow(buttonBox);
            
            connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            
            if (dialog.exec() == QDialog::Accepted) {
                app.applianceName = nameEdit->text();
                app.applianceType = typeCombo->currentText();
                app.powerLevel = powerSpin->value();
                
                if (DatabaseManager::instance().updateApplianceControl(app)) {
                    loadAllAppliances();
                    QMessageBox::information(this, "成功", "电器信息已更新！");
                }
            }
        });
        btnLayout->addWidget(editBtn);
        
        QPushButton *deleteBtn = new QPushButton("删除");
        deleteBtn->setStyleSheet("padding: 4px 8px; background-color: #e74c3c; color: white; border: none; border-radius: 3px;");
        deleteBtn->setProperty("applianceId", app.id);
        connect(deleteBtn, &QPushButton::clicked, this, [this, deleteBtn]() {
            int applianceId = deleteBtn->property("applianceId").toInt();
            if (QMessageBox::question(this, "确认", "确定要删除这个电器吗？") == QMessageBox::Yes) {
                if (DatabaseManager::instance().deleteApplianceControl(applianceId)) {
                    loadAllAppliances();
                    QMessageBox::information(this, "成功", "电器已删除！");
                }
            }
        });
        btnLayout->addWidget(deleteBtn);
        
        m_applianceManageTable->setCellWidget(i, 6, btnWidget);
    }
}

void AdminPanel::onManageApplianceClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加电器");
    QFormLayout *formLayout = new QFormLayout(&dialog);
    
    QLineEdit *dormEdit = new QLineEdit();
    formLayout->addRow("宿舍号:", dormEdit);
    
    QLineEdit *nameEdit = new QLineEdit();
    formLayout->addRow("电器名称:", nameEdit);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("空调");
    typeCombo->addItem("热水器");
    typeCombo->addItem("照明");
    typeCombo->addItem("插座");
    typeCombo->addItem("其他");
    formLayout->addRow("电器类型:", typeCombo);
    
    QSpinBox *powerSpin = new QSpinBox();
    powerSpin->setRange(0, 100);
    powerSpin->setValue(50);
    formLayout->addRow("功率档位:", powerSpin);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formLayout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        ApplianceControl appliance;
        appliance.dormitory = dormEdit->text();
        appliance.applianceName = nameEdit->text();
        appliance.applianceType = typeCombo->currentText();
        appliance.status = 0;
        appliance.powerLevel = powerSpin->value();
        appliance.currentPower = 0;
        appliance.dailyUsage = 0;
        appliance.isOnline = true;
        appliance.lastUpdate = QDateTime::currentDateTime();
        
        if (DatabaseManager::instance().addApplianceControl(appliance)) {
            loadAllAppliances();
            QMessageBox::information(this, "成功", "电器已添加！");
        } else {
            QMessageBox::warning(this, "失败", "电器添加失败！");
        }
    }
}
