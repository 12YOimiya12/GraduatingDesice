#include "databasemanager.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QFile>
#include <random>

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initDatabase()
{
    if (!openDatabase()) {
        return false;
    }
    
    if (!createTables()) {
        return false;
    }
    
    if (!createIndexes()) {
        return false;
    }
    
    // 执行数据库迁移，确保表结构是最新的
    if (!migrateDatabase()) {
        return false;
    }
    
    return true;
}

bool DatabaseManager::openDatabase()
{
    if (m_db.isOpen()) {
        return true;
    }
    
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("huidian.db");
    
    if (!m_db.open()) {
        qDebug() << "Database open error:" << m_db.lastError().text();
        return false;
    }
    
    return true;
}

void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;
    
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL,
            name TEXT NOT NULL,
            student_id TEXT,
            dormitory TEXT,
            role INTEGER NOT NULL DEFAULT 1,
            balance REAL NOT NULL DEFAULT 0.0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createUsersTable)) {
        qDebug() << "Create users table error:" << query.lastError().text();
        return false;
    }
    
    QString createDormitoriesTable = R"(
        CREATE TABLE IF NOT EXISTS dormitories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            dorm_number TEXT NOT NULL UNIQUE,
            building TEXT NOT NULL,
            floor INTEGER NOT NULL,
            current_balance REAL NOT NULL DEFAULT 0.0,
            last_reading REAL NOT NULL DEFAULT 0.0,
            remaining_kwh REAL NOT NULL DEFAULT 0.0,
            last_update DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_kwh_update DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createDormitoriesTable)) {
        qDebug() << "Create dormitories table error:" << query.lastError().text();
        return false;
    }
    
    QString createElectricityRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS electricity_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            dormitory TEXT NOT NULL,
            usage REAL NOT NULL,
            cost REAL NOT NULL,
            record_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            remark TEXT,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createElectricityRecordsTable)) {
        qDebug() << "Create electricity_records table error:" << query.lastError().text();
        return false;
    }
    
    QString createRechargeRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS recharge_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            student_id TEXT,
            dormitory TEXT,
            amount REAL NOT NULL,
            balance_after REAL NOT NULL,
            recharge_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            operator_name TEXT,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createRechargeRecordsTable)) {
        qDebug() << "Create recharge_records table error:" << query.lastError().text();
        return false;
    }
    
    // 创建电费变动记录表
    QString createElectricityChangeRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS electricity_change_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            student_id TEXT,
            dormitory TEXT,
            change_amount REAL NOT NULL,
            balance_before REAL NOT NULL,
            balance_after REAL NOT NULL,
            change_type TEXT NOT NULL,
            operator_name TEXT,
            remark TEXT,
            change_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createElectricityChangeRecordsTable)) {
        qDebug() << "Create electricity_change_records table error:" << query.lastError().text();
        return false;
    }
    
    // 创建电费度数变动记录表
    QString createElectricityKwhChangeRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS electricity_kwh_change_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            dormitory TEXT NOT NULL,
            kwh_before REAL NOT NULL,
            kwh_after REAL NOT NULL,
            kwh_change REAL NOT NULL,
            change_type TEXT NOT NULL,
            operator_name TEXT,
            remark TEXT,
            change_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            query_url TEXT
        )
    )";
    
    if (!query.exec(createElectricityKwhChangeRecordsTable)) {
        qDebug() << "Create electricity_kwh_change_records table error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::createIndexes()
{
    QSqlQuery query;
    
    QStringList indexQueries = {
        "CREATE INDEX IF NOT EXISTS idx_users_username ON users(username)",
        "CREATE INDEX IF NOT EXISTS idx_users_dormitory ON users(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_dormitories_number ON dormitories(dorm_number)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_user ON electricity_records(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_dorm ON electricity_records(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_recharge_user ON recharge_records(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_recharge_dorm ON recharge_records(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_change_user ON electricity_change_records(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_change_dorm ON electricity_change_records(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_change_time ON electricity_change_records(change_time)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_kwh_change_dorm ON electricity_kwh_change_records(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_kwh_change_time ON electricity_kwh_change_records(change_time)"
    };
    
    for (const QString& sql : indexQueries) {
        if (!query.exec(sql)) {
            qDebug() << "Create index error:" << query.lastError().text();
        }
    }
    
    return true;
}

bool DatabaseManager::migrateDatabase()
{
    QSqlQuery query;
    
    // 检查dormitories表是否包含remaining_kwh字段
    query.exec("PRAGMA table_info(dormitories)");
    bool hasRemainingKwh = false;
    bool hasLastKwhUpdate = false;
    
    while (query.next()) {
        QString columnName = query.value(1).toString();
        if (columnName == "remaining_kwh") {
            hasRemainingKwh = true;
        }
        if (columnName == "last_kwh_update") {
            hasLastKwhUpdate = true;
        }
    }
    
    // 如果缺少字段，则添加
    if (!hasRemainingKwh) {
        qDebug() << "Adding remaining_kwh column to dormitories table";
        if (!query.exec("ALTER TABLE dormitories ADD COLUMN remaining_kwh REAL DEFAULT 0.0")) {
            qDebug() << "Failed to add remaining_kwh column:" << query.lastError().text();
            return false;
        }
    }
    
    if (!hasLastKwhUpdate) {
        qDebug() << "Adding last_kwh_update column to dormitories table";
        // SQLite不支持DEFAULT CURRENT_TIMESTAMP，使用NULL作为默认值
        if (!query.exec("ALTER TABLE dormitories ADD COLUMN last_kwh_update DATETIME")) {
            qDebug() << "Failed to add last_kwh_update column:" << query.lastError().text();
            return false;
        }
        // 更新现有记录的时间戳
        if (!query.exec("UPDATE dormitories SET last_kwh_update = datetime('now') WHERE last_kwh_update IS NULL")) {
            qDebug() << "Failed to update last_kwh_update values:" << query.lastError().text();
        }
    }
    
    qDebug() << "Database migration completed successfully";
    return true;
}

QString hashPassword(const QString& password)
{
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool DatabaseManager::addUser(const UserInfo& user)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO users (username, password, name, student_id, dormitory, role, balance)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(user.username);
    query.addBindValue(hashPassword(user.password));
    query.addBindValue(user.name);
    query.addBindValue(user.studentId);
    query.addBindValue(user.dormitory);
    query.addBindValue(user.role);
    query.addBindValue(user.balance);
    
    if (!query.exec()) {
        qDebug() << "Add user error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateUser(const UserInfo& user)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE users 
        SET username=?, name=?, student_id=?, dormitory=?, role=?, balance=?
        WHERE id=?
    )");
    
    query.addBindValue(user.username);
    query.addBindValue(user.name);
    query.addBindValue(user.studentId);
    query.addBindValue(user.dormitory);
    query.addBindValue(user.role);
    query.addBindValue(user.balance);
    query.addBindValue(user.id);
    
    if (!query.exec()) {
        qDebug() << "Update user error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteUser(int userId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id=?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Delete user error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

UserInfo DatabaseManager::getUserByUsername(const QString& username)
{
    UserInfo user;
    user.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username=?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        user.name = query.value("name").toString();
        user.studentId = query.value("student_id").toString();
        user.dormitory = query.value("dormitory").toString();
        user.role = query.value("role").toInt();
        user.balance = query.value("balance").toDouble();
    }
    
    return user;
}

UserInfo DatabaseManager::getUserById(int userId)
{
    UserInfo user;
    user.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE id=?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        user.name = query.value("name").toString();
        user.studentId = query.value("student_id").toString();
        user.dormitory = query.value("dormitory").toString();
        user.role = query.value("role").toInt();
        user.balance = query.value("balance").toDouble();
    }
    
    return user;
}

QList<UserInfo> DatabaseManager::getAllUsers()
{
    QList<UserInfo> users;
    
    QSqlQuery query("SELECT * FROM users ORDER BY id");
    
    while (query.next()) {
        UserInfo user;
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        user.name = query.value("name").toString();
        user.studentId = query.value("student_id").toString();
        user.dormitory = query.value("dormitory").toString();
        user.role = query.value("role").toInt();
        user.balance = query.value("balance").toDouble();
        users.append(user);
    }
    
    return users;
}

QList<UserInfo> DatabaseManager::getStudents()
{
    QList<UserInfo> users;
    
    QSqlQuery query("SELECT * FROM users WHERE role=1 ORDER BY id");
    
    while (query.next()) {
        UserInfo user;
        user.id = query.value("id").toInt();
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        user.name = query.value("name").toString();
        user.studentId = query.value("student_id").toString();
        user.dormitory = query.value("dormitory").toString();
        user.role = query.value("role").toInt();
        user.balance = query.value("balance").toDouble();
        users.append(user);
    }
    
    return users;
}

bool DatabaseManager::addDormitory(const DormitoryInfo& dorm)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO dormitories (dorm_number, building, floor, current_balance, last_reading)
        VALUES (?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(dorm.dormNumber);
    query.addBindValue(dorm.building);
    query.addBindValue(dorm.floor);
    query.addBindValue(dorm.currentBalance);
    query.addBindValue(dorm.lastReading);
    
    if (!query.exec()) {
        qDebug() << "Add dormitory error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateDormitory(const DormitoryInfo& dorm)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE dormitories 
        SET dorm_number=?, building=?, floor=?, current_balance=?, last_reading=?, last_update=CURRENT_TIMESTAMP
        WHERE id=?
    )");
    
    query.addBindValue(dorm.dormNumber);
    query.addBindValue(dorm.building);
    query.addBindValue(dorm.floor);
    query.addBindValue(dorm.currentBalance);
    query.addBindValue(dorm.lastReading);
    query.addBindValue(dorm.id);
    
    if (!query.exec()) {
        qDebug() << "Update dormitory error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteDormitory(int dormId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM dormitories WHERE id=?");
    query.addBindValue(dormId);
    
    if (!query.exec()) {
        qDebug() << "Delete dormitory error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

DormitoryInfo DatabaseManager::getDormitoryById(int dormId)
{
    DormitoryInfo dorm;
    dorm.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM dormitories WHERE id=?");
    query.addBindValue(dormId);
    
    if (query.exec() && query.next()) {
        dorm.id = query.value("id").toInt();
        dorm.dormNumber = query.value("dorm_number").toString();
        dorm.building = query.value("building").toString();
        dorm.floor = query.value("floor").toInt();
        dorm.currentBalance = query.value("current_balance").toDouble();
        dorm.lastReading = query.value("last_reading").toDouble();
        dorm.remainingKwh = query.value("remaining_kwh").toDouble();
        dorm.lastUpdate = query.value("last_update").toDateTime();
        dorm.lastKwhUpdate = query.value("last_kwh_update").toDateTime();
    }
    
    return dorm;
}

DormitoryInfo DatabaseManager::getDormitoryByNumber(const QString& dormNumber)
{
    DormitoryInfo dorm;
    dorm.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM dormitories WHERE dorm_number=?");
    query.addBindValue(dormNumber);
    
    if (query.exec() && query.next()) {
        dorm.id = query.value("id").toInt();
        dorm.dormNumber = query.value("dorm_number").toString();
        dorm.building = query.value("building").toString();
        dorm.floor = query.value("floor").toInt();
        dorm.currentBalance = query.value("current_balance").toDouble();
        dorm.lastReading = query.value("last_reading").toDouble();
        dorm.remainingKwh = query.value("remaining_kwh").toDouble();
        dorm.lastUpdate = query.value("last_update").toDateTime();
        dorm.lastKwhUpdate = query.value("last_kwh_update").toDateTime();
    }
    
    return dorm;
}

QList<DormitoryInfo> DatabaseManager::getAllDormitories()
{
    QList<DormitoryInfo> dorms;
    
    QSqlQuery query("SELECT * FROM dormitories ORDER BY building, floor, dorm_number");
    
    while (query.next()) {
        DormitoryInfo dorm;
        dorm.id = query.value("id").toInt();
        dorm.dormNumber = query.value("dorm_number").toString();
        dorm.building = query.value("building").toString();
        dorm.floor = query.value("floor").toInt();
        dorm.currentBalance = query.value("current_balance").toDouble();
        dorm.lastReading = query.value("last_reading").toDouble();
        dorm.remainingKwh = query.value("remaining_kwh").toDouble();
        dorm.lastUpdate = query.value("last_update").toDateTime();
        dorm.lastKwhUpdate = query.value("last_kwh_update").toDateTime();
        dorms.append(dorm);
    }
    
    return dorms;
}

bool DatabaseManager::addElectricityRecord(const ElectricityRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO electricity_records (user_id, dormitory, usage, cost, remark)
        VALUES (?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(record.userId);
    query.addBindValue(record.dormitory);
    query.addBindValue(record.usage);
    query.addBindValue(record.cost);
    query.addBindValue(record.remark);
    
    if (!query.exec()) {
        qDebug() << "Add electricity record error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<ElectricityRecord> DatabaseManager::getElectricityRecordsByUser(int userId)
{
    QList<ElectricityRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_records WHERE user_id=? ORDER BY record_time DESC");
    query.addBindValue(userId);
    
    while (query.next()) {
        ElectricityRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.dormitory = query.value("dormitory").toString();
        record.usage = query.value("usage").toDouble();
        record.cost = query.value("cost").toDouble();
        record.recordTime = query.value("record_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseManager::generateSampleKwhChangeRecords()
{
    // 获取所有寝室列表
    QList<DormitoryInfo> dorms = getAllDormitories();
    if (dorms.isEmpty()) {
        qDebug() << "No dormitories found for generating sample records";
        return false;
    }
    
    // 设置随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-100, 100);
    
    // 为每个寝室生成10次度数变化记录
    for (const DormitoryInfo& dorm : dorms) {
        double currentKwh = 0.0; // 起始度数为0
        QDateTime currentTime = QDateTime::currentDateTime().addDays(-10); // 从10天前开始
        
        for (int i = 0; i < 10; ++i) {
            // 生成随机变化值（-100到100之间）
            double changeKwh = dis(gen); // -100到100
            
            // 确保度数不会变为负数
            double newKwh = currentKwh + changeKwh;
            if (newKwh < 0) {
                newKwh = 0;
                changeKwh = newKwh - currentKwh;
            }
            
            // 创建度数变化记录
            ElectricityKwhChangeRecord record;
            record.dormitory = dorm.dormNumber;
            record.kwhBefore = currentKwh;
            record.kwhAfter = newKwh;
            record.kwhChange = changeKwh;
            
            // 根据变化值设置变动类型
            if (changeKwh > 0) {
                record.changeType = "度数增加";
            } else if (changeKwh < 0) {
                record.changeType = "度数减少";
            } else {
                record.changeType = "度数不变";
            }
            
            record.operatorName = "系统生成";
            record.remark = "示例数据";
            record.queryUrl = "https://example.com/electricity/query";
            record.changeTime = currentTime;
            
            // 添加记录到数据库
            if (!addElectricityKwhChangeRecord(record)) {
                qDebug() << "Failed to add kwh change record for dormitory" << dorm.dormNumber;
                return false;
            }
            
            // 更新当前度数和时间
            currentKwh = newKwh;
            currentTime = currentTime.addDays(1); // 时间间隔为一天
        }
        
        // 直接更新寝室的剩余度数，不记录度数变化（避免重复记录）
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE dormitories SET remaining_kwh=?, last_kwh_update=CURRENT_TIMESTAMP WHERE dorm_number=?");
        updateQuery.addBindValue(currentKwh);
        updateQuery.addBindValue(dorm.dormNumber);
        
        if (!updateQuery.exec()) {
            qDebug() << "Failed to update dormitory kwh for" << dorm.dormNumber;
            return false;
        }
    }
    
    qDebug() << "Generated sample kwh change records for" << dorms.size() << "dormitories";
    return true;
}

bool DatabaseManager::addElectricityKwhChangeRecord(const ElectricityKwhChangeRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO electricity_kwh_change_records 
        (dormitory, kwh_before, kwh_after, kwh_change, change_type, operator_name, remark, query_url)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(record.dormitory);
    query.addBindValue(record.kwhBefore);
    query.addBindValue(record.kwhAfter);
    query.addBindValue(record.kwhChange);
    query.addBindValue(record.changeType);
    query.addBindValue(record.operatorName);
    query.addBindValue(record.remark);
    query.addBindValue(record.queryUrl);
    
    if (!query.exec()) {
        qDebug() << "Add electricity kwh change record error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<ElectricityKwhChangeRecord> DatabaseManager::getElectricityKwhChangeRecordsByDormitory(const QString& dormitory)
{
    QList<ElectricityKwhChangeRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_kwh_change_records WHERE dormitory=? ORDER BY change_time DESC");
    query.addBindValue(dormitory);
    
    if (!query.exec()) {
        qDebug() << "Get electricity kwh change records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityKwhChangeRecord record;
        record.id = query.value("id").toInt();
        record.dormitory = query.value("dormitory").toString();
        record.kwhBefore = query.value("kwh_before").toDouble();
        record.kwhAfter = query.value("kwh_after").toDouble();
        record.kwhChange = query.value("kwh_change").toDouble();
        record.changeType = query.value("change_type").toString();
        record.operatorName = query.value("operator_name").toString();
        record.remark = query.value("remark").toString();
        record.changeTime = query.value("change_time").toDateTime();
        record.queryUrl = query.value("query_url").toString();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityKwhChangeRecord> DatabaseManager::getAllElectricityKwhChangeRecords()
{
    QList<ElectricityKwhChangeRecord> records;
    
    QSqlQuery query("SELECT * FROM electricity_kwh_change_records ORDER BY change_time DESC");
    
    if (!query.exec()) {
        qDebug() << "Get all electricity kwh change records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityKwhChangeRecord record;
        record.id = query.value("id").toInt();
        record.dormitory = query.value("dormitory").toString();
        record.kwhBefore = query.value("kwh_before").toDouble();
        record.kwhAfter = query.value("kwh_after").toDouble();
        record.kwhChange = query.value("kwh_change").toDouble();
        record.changeType = query.value("change_type").toString();
        record.operatorName = query.value("operator_name").toString();
        record.remark = query.value("remark").toString();
        record.changeTime = query.value("change_time").toDateTime();
        record.queryUrl = query.value("query_url").toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseManager::updateDormitoryKwh(const QString& dormNumber, double newKwh, const QString& operatorName, const QString& queryUrl)
{
    m_db.transaction();
    
    // 获取当前宿舍信息
    DormitoryInfo dorm = getDormitoryByNumber(dormNumber);
    if (dorm.id == -1) {
        m_db.rollback();
        return false;
    }
    
    double oldKwh = dorm.remainingKwh;
    double kwhChange = newKwh - oldKwh;
    
    // 更新宿舍度数
    QSqlQuery query;
    query.prepare("UPDATE dormitories SET remaining_kwh=?, last_kwh_update=CURRENT_TIMESTAMP WHERE dorm_number=?");
    query.addBindValue(newKwh);
    query.addBindValue(dormNumber);
    
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "Update dormitory kwh error:" << query.lastError().text();
        return false;
    }
    
    // 记录度数变动
    ElectricityKwhChangeRecord record;
    record.dormitory = dormNumber;
    record.kwhBefore = oldKwh;
    record.kwhAfter = newKwh;
    record.kwhChange = kwhChange;
    record.changeType = "查询";
    record.operatorName = operatorName;
    record.remark = QString("网页查询更新度数，变动量: %1 度").arg(kwhChange, 0, 'f', 2);
    record.queryUrl = queryUrl;
    
    if (!addElectricityKwhChangeRecord(record)) {
        m_db.rollback();
        return false;
    }
    
    m_db.commit();
    return true;
}

QList<ElectricityRecord> DatabaseManager::getElectricityRecordsByDormitory(const QString& dormitory)
{
    QList<ElectricityRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_records WHERE dormitory=? ORDER BY record_time DESC");
    query.addBindValue(dormitory);
    
    while (query.next()) {
        ElectricityRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.dormitory = query.value("dormitory").toString();
        record.usage = query.value("usage").toDouble();
        record.cost = query.value("cost").toDouble();
        record.recordTime = query.value("record_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityRecord> DatabaseManager::getAllElectricityRecords()
{
    QList<ElectricityRecord> records;
    
    QSqlQuery query("SELECT * FROM electricity_records ORDER BY record_time DESC");
    
    while (query.next()) {
        ElectricityRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.dormitory = query.value("dormitory").toString();
        record.usage = query.value("usage").toDouble();
        record.cost = query.value("cost").toDouble();
        record.recordTime = query.value("record_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseManager::addRechargeRecord(const RechargeRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO recharge_records (user_id, student_id, dormitory, amount, balance_after, operator_name)
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(record.userId);
    query.addBindValue(record.studentId);
    query.addBindValue(record.dormitory);
    query.addBindValue(record.amount);
    query.addBindValue(record.balanceAfter);
    query.addBindValue(record.operatorName);
    
    if (!query.exec()) {
        qDebug() << "Add recharge record error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<RechargeRecord> DatabaseManager::getRechargeRecordsByUser(int userId)
{
    QList<RechargeRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM recharge_records WHERE user_id=? ORDER BY recharge_time DESC");
    query.addBindValue(userId);
    
    while (query.next()) {
        RechargeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.amount = query.value("amount").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.rechargeTime = query.value("recharge_time").toDateTime();
        record.operatorName = query.value("operator_name").toString();
        records.append(record);
    }
    
    return records;
}

QList<RechargeRecord> DatabaseManager::getRechargeRecordsByDormitory(const QString& dormitory)
{
    QList<RechargeRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM recharge_records WHERE dormitory=? ORDER BY recharge_time DESC");
    query.addBindValue(dormitory);
    
    while (query.next()) {
        RechargeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.amount = query.value("amount").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.rechargeTime = query.value("recharge_time").toDateTime();
        record.operatorName = query.value("operator_name").toString();
        records.append(record);
    }
    
    return records;
}

QList<RechargeRecord> DatabaseManager::getAllRechargeRecords()
{
    QList<RechargeRecord> records;
    
    QSqlQuery query("SELECT * FROM recharge_records ORDER BY recharge_time DESC");
    
    while (query.next()) {
        RechargeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.amount = query.value("amount").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.rechargeTime = query.value("recharge_time").toDateTime();
        record.operatorName = query.value("operator_name").toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseManager::recharge(int userId, double amount, const QString& operatorName)
{
    m_db.transaction();
    
    UserInfo user = getUserById(userId);
    if (user.id == -1) {
        m_db.rollback();
        return false;
    }
    
    double newBalance = user.balance + amount;
    
    if (!updateBalance(userId, newBalance)) {
        m_db.rollback();
        return false;
    }
    
    // 添加充值记录
    RechargeRecord rechargeRecord;
    rechargeRecord.userId = userId;
    rechargeRecord.studentId = user.studentId;
    rechargeRecord.dormitory = user.dormitory;
    rechargeRecord.amount = amount;
    rechargeRecord.balanceAfter = newBalance;
    rechargeRecord.operatorName = operatorName;
    
    if (!addRechargeRecord(rechargeRecord)) {
        m_db.rollback();
        return false;
    }
    
    // 添加电费变动记录
    ElectricityChangeRecord changeRecord;
    changeRecord.userId = userId;
    changeRecord.studentId = user.studentId;
    changeRecord.dormitory = user.dormitory;
    changeRecord.changeAmount = amount;
    changeRecord.balanceBefore = user.balance;
    changeRecord.balanceAfter = newBalance;
    changeRecord.changeType = "充值";
    changeRecord.operatorName = operatorName;
    changeRecord.remark = QString("电费充值 %1 元").arg(amount);
    
    if (!addElectricityChangeRecord(changeRecord)) {
        m_db.rollback();
        return false;
    }
    
    m_db.commit();
    return true;
}

bool DatabaseManager::updateBalance(int userId, double newBalance)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET balance=? WHERE id=?");
    query.addBindValue(newBalance);
    query.addBindValue(userId);
    
    return query.exec();
}

bool DatabaseManager::deductElectricityCost(const QString& dormitory, double cost)
{
    m_db.transaction();
    
    QList<UserInfo> users = getStudents();
    for (const UserInfo& user : users) {
        if (user.dormitory == dormitory) {
            double newBalance = user.balance - cost;
            if (newBalance < 0) {
                newBalance = 0;
            }
            
            if (!updateBalance(user.id, newBalance)) {
                m_db.rollback();
                return false;
            }
            
            // 添加用电记录
            ElectricityRecord electricityRecord;
            electricityRecord.userId = user.id;
            electricityRecord.dormitory = dormitory;
            electricityRecord.usage = cost / 0.6;
            electricityRecord.cost = cost;
            electricityRecord.remark = "电费扣费";
            
            if (!addElectricityRecord(electricityRecord)) {
                m_db.rollback();
                return false;
            }
            
            // 添加电费变动记录
            ElectricityChangeRecord changeRecord;
            changeRecord.userId = user.id;
            changeRecord.studentId = user.studentId;
            changeRecord.dormitory = dormitory;
            changeRecord.changeAmount = -cost;
            changeRecord.balanceBefore = user.balance;
            changeRecord.balanceAfter = newBalance;
            changeRecord.changeType = "扣费";
            changeRecord.operatorName = "系统自动扣费";
            changeRecord.remark = QString("电费扣费 %1 元").arg(cost);
            
            if (!addElectricityChangeRecord(changeRecord)) {
                m_db.rollback();
                return false;
            }
        }
    }
    
    m_db.commit();
    return true;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password, UserInfo& user)
{
    UserInfo foundUser = getUserByUsername(username);
    if (foundUser.id == -1) {
        return false;
    }
    
    QString hashedPassword = hashPassword(password);
    if (foundUser.password != hashedPassword) {
        return false;
    }
    
    user = foundUser;
    return true;
}

bool DatabaseManager::initSampleData()
{
    UserInfo admin;
    admin.username = "admin";
    admin.password = "admin123";
    admin.name = "系统管理员";
    admin.role = 0;
    admin.balance = 0.0;
    
    if (!addUser(admin)) {
        qDebug() << "Add admin error, maybe already exists";
    }
    
    QStringList dormNumbers = {"101", "102", "103", "201", "202", "203"};
    for (const QString& num : dormNumbers) {
        DormitoryInfo dorm;
        dorm.dormNumber = num;
        dorm.building = "A栋";
        dorm.floor = num.toInt() / 100;
        dorm.currentBalance = 100.0;
        dorm.lastReading = 0.0;
        
        if (!addDormitory(dorm)) {
            qDebug() << "Add dormitory" << num << "error, maybe already exists";
        }
    }
    
    QList<UserInfo> students;
    
    UserInfo s1;
    s1.username = "2021001";
    s1.password = "123456";
    s1.name = "张三";
    s1.studentId = "2021001";
    s1.dormitory = "101";
    s1.role = 1;
    s1.balance = 200.0;
    students.append(s1);
    
    UserInfo s2;
    s2.username = "2021002";
    s2.password = "123456";
    s2.name = "李四";
    s2.studentId = "2021002";
    s2.dormitory = "101";
    s2.role = 1;
    s2.balance = 180.5;
    students.append(s2);
    
    UserInfo s3;
    s3.username = "2021003";
    s3.password = "123456";
    s3.name = "王五";
    s3.studentId = "2021003";
    s3.dormitory = "102";
    s3.role = 1;
    s3.balance = 150.0;
    students.append(s3);
    
    UserInfo s4;
    s4.username = "2021004";
    s4.password = "123456";
    s4.name = "赵六";
    s4.studentId = "2021004";
    s4.dormitory = "201";
    s4.role = 1;
    s4.balance = 250.0;
    students.append(s4);
    
    for (const UserInfo& student : students) {
        if (!addUser(student)) {
            qDebug() << "Add student" << student.username << "error, maybe already exists";
        }
    }
    
    return true;
}

bool DatabaseManager::addElectricityChangeRecord(const ElectricityChangeRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO electricity_change_records 
        (user_id, student_id, dormitory, change_amount, balance_before, balance_after, 
         change_type, operator_name, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(record.userId);
    query.addBindValue(record.studentId);
    query.addBindValue(record.dormitory);
    query.addBindValue(record.changeAmount);
    query.addBindValue(record.balanceBefore);
    query.addBindValue(record.balanceAfter);
    query.addBindValue(record.changeType);
    query.addBindValue(record.operatorName);
    query.addBindValue(record.remark);
    
    if (!query.exec()) {
        qDebug() << "Add electricity change record error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<ElectricityChangeRecord> DatabaseManager::getElectricityChangeRecordsByUser(int userId)
{
    QList<ElectricityChangeRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_change_records WHERE user_id=? ORDER BY change_time DESC");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Get electricity change records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityChangeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.changeAmount = query.value("change_amount").toDouble();
        record.balanceBefore = query.value("balance_before").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.changeType = query.value("change_type").toString();
        record.operatorName = query.value("operator_name").toString();
        record.remark = query.value("remark").toString();
        record.changeTime = query.value("change_time").toDateTime();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityChangeRecord> DatabaseManager::getElectricityChangeRecordsByDormitory(const QString& dormitory)
{
    QList<ElectricityChangeRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_change_records WHERE dormitory=? ORDER BY change_time DESC");
    query.addBindValue(dormitory);
    
    if (!query.exec()) {
        qDebug() << "Get electricity change records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityChangeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.changeAmount = query.value("change_amount").toDouble();
        record.balanceBefore = query.value("balance_before").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.changeType = query.value("change_type").toString();
        record.operatorName = query.value("operator_name").toString();
        record.remark = query.value("remark").toString();
        record.changeTime = query.value("change_time").toDateTime();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityChangeRecord> DatabaseManager::getAllElectricityChangeRecords()
{
    QList<ElectricityChangeRecord> records;
    
    QSqlQuery query("SELECT * FROM electricity_change_records ORDER BY change_time DESC");
    
    if (!query.exec()) {
        qDebug() << "Get all electricity change records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityChangeRecord record;
        record.id = query.value("id").toInt();
        record.userId = query.value("user_id").toInt();
        record.studentId = query.value("student_id").toString();
        record.dormitory = query.value("dormitory").toString();
        record.changeAmount = query.value("change_amount").toDouble();
        record.balanceBefore = query.value("balance_before").toDouble();
        record.balanceAfter = query.value("balance_after").toDouble();
        record.changeType = query.value("change_type").toString();
        record.operatorName = query.value("operator_name").toString();
        record.remark = query.value("remark").toString();
        record.changeTime = query.value("change_time").toDateTime();
        records.append(record);
    }
    
    return records;
}
