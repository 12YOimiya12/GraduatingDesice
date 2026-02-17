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

AdminPanel::AdminPanel(QWidget *parent)
    : QMainWindow(parent)
    , m_userInfoLabel(nullptr)
    , m_studentsTable(nullptr)
    , m_dormitoriesTable(nullptr)
    , m_rechargeRecordsTable(nullptr)
    , m_electricityRecordsTable(nullptr)
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
    
    QLabel *titleLabel = new QLabel("惠电宿舍电费充值管理系统 - 管理员端");
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
    m_dormitoriesTable->setHorizontalHeaderLabels({"ID", "宿舍号", "楼栋", "楼层", "当前余额(元)", "上次读数", "更新时间"});
    m_dormitoriesTable->horizontalHeader()->setStretchLastSection(true);
    m_dormitoriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dormitoriesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dormitoriesLayout->addWidget(m_dormitoriesTable);
    
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
    rechargeLayout->addWidget(m_rechargeRecordsTable);
    
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
    electricityLayout->addWidget(m_electricityRecordsTable);
    
    tabWidget->addTab(electricityTab, "用电记录");
    
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
        m_dormitoriesTable->setItem(i, 4, new QTableWidgetItem(QString::number(dorm.currentBalance, 'f', 2)));
        m_dormitoriesTable->setItem(i, 5, new QTableWidgetItem(QString::number(dorm.lastReading, 'f', 2)));
        m_dormitoriesTable->setItem(i, 6, new QTableWidgetItem(dorm.lastUpdate.toString("yyyy-MM-dd hh:mm:ss")));
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
    loadRechargeRecords();
    loadElectricityRecords();
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
    } else if (index == 4) {
        loadElectricityRecords();
    }
}
