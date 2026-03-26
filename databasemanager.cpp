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
    
    // 创建电费查询记录表
    QString createElectricityQueryRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS electricity_query_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_account TEXT,
            dormitory TEXT NOT NULL,
            remaining_kwh REAL NOT NULL DEFAULT 0.0,
            remaining_amount REAL NOT NULL DEFAULT 0.0,
            operator_name TEXT,
            query_url TEXT,
            query_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            remark TEXT
        )
    )";
    
    if (!query.exec(createElectricityQueryRecordsTable)) {
        qDebug() << "Create electricity_query_records table error:" << query.lastError().text();
        return false;
    }
    
    QString createFaceInfoTable = R"(
        CREATE TABLE IF NOT EXISTS face_info (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            student_id TEXT NOT NULL,
            name TEXT NOT NULL,
            dormitory TEXT NOT NULL,
            face_image_path TEXT,
            face_feature_data TEXT,
            status INTEGER NOT NULL DEFAULT 0,
            reject_reason TEXT,
            submit_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            audit_time DATETIME,
            auditor_name TEXT,
            remark TEXT,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createFaceInfoTable)) {
        qDebug() << "Create face_info table error:" << query.lastError().text();
        return false;
    }
    
    QString createRepairRequestsTable = R"(
        CREATE TABLE IF NOT EXISTS repair_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            student_id TEXT NOT NULL,
            name TEXT NOT NULL,
            dormitory TEXT NOT NULL,
            contact_phone TEXT,
            repair_type INTEGER NOT NULL DEFAULT 0,
            description TEXT,
            image_path TEXT,
            status INTEGER NOT NULL DEFAULT 0,
            priority INTEGER NOT NULL DEFAULT 0,
            handler_name TEXT,
            handle_result TEXT,
            submit_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            handle_time DATETIME,
            complete_time DATETIME,
            remark TEXT,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createRepairRequestsTable)) {
        qDebug() << "Create repair_requests table error:" << query.lastError().text();
        return false;
    }
    
    QString createRoomChangeRequestsTable = R"(
        CREATE TABLE IF NOT EXISTS room_change_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            student_id TEXT NOT NULL,
            name TEXT NOT NULL,
            current_dormitory TEXT NOT NULL,
            target_dormitory TEXT,
            change_reason INTEGER NOT NULL DEFAULT 0,
            description TEXT,
            status INTEGER NOT NULL DEFAULT 0,
            reject_reason TEXT,
            auditor_name TEXT,
            submit_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            audit_time DATETIME,
            complete_time DATETIME,
            remark TEXT,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createRoomChangeRequestsTable)) {
        qDebug() << "Create room_change_requests table error:" << query.lastError().text();
        return false;
    }
    
    QString createApplianceControlsTable = R"(
        CREATE TABLE IF NOT EXISTS appliance_controls (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            dormitory TEXT NOT NULL,
            appliance_name TEXT NOT NULL,
            appliance_type TEXT NOT NULL,
            status INTEGER NOT NULL DEFAULT 0,
            power_level INTEGER NOT NULL DEFAULT 0,
            current_power REAL NOT NULL DEFAULT 0.0,
            daily_usage REAL NOT NULL DEFAULT 0.0,
            schedule TEXT,
            is_online INTEGER NOT NULL DEFAULT 0,
            last_update DATETIME DEFAULT CURRENT_TIMESTAMP,
            remark TEXT
        )
    )";
    
    if (!query.exec(createApplianceControlsTable)) {
        qDebug() << "Create appliance_controls table error:" << query.lastError().text();
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
        "CREATE INDEX IF NOT EXISTS idx_electricity_kwh_change_time ON electricity_kwh_change_records(change_time)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_query_dorm ON electricity_query_records(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_query_student ON electricity_query_records(student_account)",
        "CREATE INDEX IF NOT EXISTS idx_electricity_query_time ON electricity_query_records(query_time)",
        "CREATE INDEX IF NOT EXISTS idx_face_info_user ON face_info(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_face_info_status ON face_info(status)",
        "CREATE INDEX IF NOT EXISTS idx_repair_user ON repair_requests(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_repair_dorm ON repair_requests(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_repair_status ON repair_requests(status)",
        "CREATE INDEX IF NOT EXISTS idx_room_change_user ON room_change_requests(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_room_change_status ON room_change_requests(status)",
        "CREATE INDEX IF NOT EXISTS idx_appliance_dorm ON appliance_controls(dormitory)",
        "CREATE INDEX IF NOT EXISTS idx_appliance_status ON appliance_controls(status)"
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

bool DatabaseManager::addElectricityQueryRecord(const ElectricityQueryRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO electricity_query_records 
        (student_account, dormitory, remaining_kwh, remaining_amount, operator_name, query_url, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(record.studentAccount);
    query.addBindValue(record.dormitory);
    query.addBindValue(record.remainingKwh);
    query.addBindValue(record.remainingAmount);
    query.addBindValue(record.operatorName);
    query.addBindValue(record.queryUrl);
    query.addBindValue(record.remark);
    
    if (!query.exec()) {
        qDebug() << "Add electricity query record error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<ElectricityQueryRecord> DatabaseManager::getElectricityQueryRecordsByDormitory(const QString& dormitory)
{
    QList<ElectricityQueryRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_query_records WHERE dormitory=? ORDER BY query_time DESC");
    query.addBindValue(dormitory);
    
    if (!query.exec()) {
        qDebug() << "Get electricity query records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityQueryRecord record;
        record.id = query.value("id").toInt();
        record.studentAccount = query.value("student_account").toString();
        record.dormitory = query.value("dormitory").toString();
        record.remainingKwh = query.value("remaining_kwh").toDouble();
        record.remainingAmount = query.value("remaining_amount").toDouble();
        record.operatorName = query.value("operator_name").toString();
        record.queryUrl = query.value("query_url").toString();
        record.queryTime = query.value("query_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityQueryRecord> DatabaseManager::getElectricityQueryRecordsByStudentAccount(const QString& studentAccount)
{
    QList<ElectricityQueryRecord> records;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM electricity_query_records WHERE student_account=? ORDER BY query_time DESC");
    query.addBindValue(studentAccount);
    
    if (!query.exec()) {
        qDebug() << "Get electricity query records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityQueryRecord record;
        record.id = query.value("id").toInt();
        record.studentAccount = query.value("student_account").toString();
        record.dormitory = query.value("dormitory").toString();
        record.remainingKwh = query.value("remaining_kwh").toDouble();
        record.remainingAmount = query.value("remaining_amount").toDouble();
        record.operatorName = query.value("operator_name").toString();
        record.queryUrl = query.value("query_url").toString();
        record.queryTime = query.value("query_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

QList<ElectricityQueryRecord> DatabaseManager::getAllElectricityQueryRecords()
{
    QList<ElectricityQueryRecord> records;
    
    QSqlQuery query("SELECT * FROM electricity_query_records ORDER BY query_time DESC");
    
    if (!query.exec()) {
        qDebug() << "Get all electricity query records error:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        ElectricityQueryRecord record;
        record.id = query.value("id").toInt();
        record.studentAccount = query.value("student_account").toString();
        record.dormitory = query.value("dormitory").toString();
        record.remainingKwh = query.value("remaining_kwh").toDouble();
        record.remainingAmount = query.value("remaining_amount").toDouble();
        record.operatorName = query.value("operator_name").toString();
        record.queryUrl = query.value("query_url").toString();
        record.queryTime = query.value("query_time").toDateTime();
        record.remark = query.value("remark").toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseManager::saveElectricityQueryResult(const QString& studentAccount, const QString& dormitory, 
                                                  double remainingKwh, double remainingAmount, 
                                                  const QString& operatorName, const QString& queryUrl)
{
    m_db.transaction();
    
    // 1. 保存查询记录到 electricity_query_records 表
    ElectricityQueryRecord queryRecord;
    queryRecord.studentAccount = studentAccount;
    queryRecord.dormitory = dormitory;
    queryRecord.remainingKwh = remainingKwh;
    queryRecord.remainingAmount = remainingAmount;
    queryRecord.operatorName = operatorName;
    queryRecord.queryUrl = queryUrl;
    queryRecord.remark = QString("网页查询 - 学生账号:%1, 剩余度数:%2, 剩余金额:%3")
                            .arg(studentAccount)
                            .arg(remainingKwh, 0, 'f', 2)
                            .arg(remainingAmount, 0, 'f', 2);
    
    if (!addElectricityQueryRecord(queryRecord)) {
        m_db.rollback();
        qDebug() << "Failed to add electricity query record";
        return false;
    }
    
    // 2. 更新 dormitories 表（宿舍信息）
    DormitoryInfo dorm = getDormitoryByNumber(dormitory);
    double oldKwh = dorm.id != -1 ? dorm.remainingKwh : 0.0;
    double oldAmount = dorm.id != -1 ? dorm.currentBalance : 0.0;
    double kwhChange = remainingKwh - oldKwh;
    double amountChange = remainingAmount - oldAmount;
    
    if (dorm.id != -1) {
        // 更新宿舍度数和余额
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE dormitories SET remaining_kwh=?, current_balance=?, last_kwh_update=CURRENT_TIMESTAMP, last_update=CURRENT_TIMESTAMP WHERE dorm_number=?");
        updateQuery.addBindValue(remainingKwh);
        updateQuery.addBindValue(remainingAmount);
        updateQuery.addBindValue(dormitory);
        
        if (!updateQuery.exec()) {
            qDebug() << "Update dormitory error:" << updateQuery.lastError().text();
        }
    } else {
        // 如果宿舍不存在，创建新宿舍记录
        DormitoryInfo newDorm;
        newDorm.dormNumber = dormitory;
        newDorm.building = "未知";
        newDorm.floor = 0;
        newDorm.currentBalance = remainingAmount;
        newDorm.lastReading = 0.0;
        newDorm.remainingKwh = remainingKwh;
        
        if (!addDormitory(newDorm)) {
            qDebug() << "Failed to add new dormitory";
        }
    }
    
    // 3. 记录度数变动到 electricity_kwh_change_records 表
    ElectricityKwhChangeRecord kwhRecord;
    kwhRecord.dormitory = dormitory;
    kwhRecord.kwhBefore = oldKwh;
    kwhRecord.kwhAfter = remainingKwh;
    kwhRecord.kwhChange = kwhChange;
    kwhRecord.changeType = "网页查询";
    kwhRecord.operatorName = operatorName;
    kwhRecord.remark = QString("学生账号:%1, 度数变动: %2 度").arg(studentAccount).arg(kwhChange, 0, 'f', 2);
    kwhRecord.queryUrl = queryUrl;
    
    if (!addElectricityKwhChangeRecord(kwhRecord)) {
        qDebug() << "Failed to add kwh change record";
    }
    
    // 4. 记录电费金额变动到 electricity_change_records 表
    // 查找该宿舍关联的用户
    QList<UserInfo> users = getStudents();
    for (const UserInfo& user : users) {
        if (user.dormitory == dormitory) {
            ElectricityChangeRecord changeRecord;
            changeRecord.userId = user.id;
            changeRecord.studentId = studentAccount.isEmpty() ? user.studentId : studentAccount;
            changeRecord.dormitory = dormitory;
            changeRecord.changeAmount = amountChange;
            changeRecord.balanceBefore = oldAmount;
            changeRecord.balanceAfter = remainingAmount;
            changeRecord.changeType = "网页查询";
            changeRecord.operatorName = operatorName;
            changeRecord.remark = QString("网页查询更新余额，变动: %1 元").arg(amountChange, 0, 'f', 2);
            
            if (!addElectricityChangeRecord(changeRecord)) {
                qDebug() << "Failed to add electricity change record for user:" << user.username;
            }
            break; // 只记录第一个匹配的用户
        }
    }
    
    m_db.commit();
    qDebug() << "Electricity query result saved successfully - Student:" << studentAccount 
             << "Dorm:" << dormitory << "Kwh:" << remainingKwh << "Amount:" << remainingAmount;
    return true;
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

// ==================== 人脸信息管理 ====================

bool DatabaseManager::addFaceInfo(const FaceInfo& faceInfo)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO face_info (user_id, student_id, name, dormitory, face_image_path, face_feature_data, status, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(faceInfo.userId);
    query.addBindValue(faceInfo.studentId);
    query.addBindValue(faceInfo.name);
    query.addBindValue(faceInfo.dormitory);
    query.addBindValue(faceInfo.faceImagePath);
    query.addBindValue(faceInfo.faceFeatureData);
    query.addBindValue(faceInfo.status);
    query.addBindValue(faceInfo.remark);
    
    if (!query.exec()) {
        qDebug() << "Add face info error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateFaceInfo(const FaceInfo& faceInfo)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE face_info SET student_id=?, name=?, dormitory=?, face_image_path=?, 
        face_feature_data=?, status=?, reject_reason=?, auditor_name=?, remark=? 
        WHERE id=?
    )");
    
    query.addBindValue(faceInfo.studentId);
    query.addBindValue(faceInfo.name);
    query.addBindValue(faceInfo.dormitory);
    query.addBindValue(faceInfo.faceImagePath);
    query.addBindValue(faceInfo.faceFeatureData);
    query.addBindValue(faceInfo.status);
    query.addBindValue(faceInfo.rejectReason);
    query.addBindValue(faceInfo.auditorName);
    query.addBindValue(faceInfo.remark);
    query.addBindValue(faceInfo.id);
    
    if (!query.exec()) {
        qDebug() << "Update face info error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteFaceInfo(int faceId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM face_info WHERE id=?");
    query.addBindValue(faceId);
    
    if (!query.exec()) {
        qDebug() << "Delete face info error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

FaceInfo DatabaseManager::getFaceInfoById(int faceId)
{
    FaceInfo faceInfo;
    faceInfo.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM face_info WHERE id=?");
    query.addBindValue(faceId);
    
    if (query.exec() && query.next()) {
        faceInfo.id = query.value("id").toInt();
        faceInfo.userId = query.value("user_id").toInt();
        faceInfo.studentId = query.value("student_id").toString();
        faceInfo.name = query.value("name").toString();
        faceInfo.dormitory = query.value("dormitory").toString();
        faceInfo.faceImagePath = query.value("face_image_path").toString();
        faceInfo.faceFeatureData = query.value("face_feature_data").toString();
        faceInfo.status = query.value("status").toInt();
        faceInfo.rejectReason = query.value("reject_reason").toString();
        faceInfo.submitTime = query.value("submit_time").toDateTime();
        faceInfo.auditTime = query.value("audit_time").toDateTime();
        faceInfo.auditorName = query.value("auditor_name").toString();
        faceInfo.remark = query.value("remark").toString();
    }
    
    return faceInfo;
}

FaceInfo DatabaseManager::getFaceInfoByUserId(int userId)
{
    FaceInfo faceInfo;
    faceInfo.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM face_info WHERE user_id=? ORDER BY submit_time DESC LIMIT 1");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        faceInfo.id = query.value("id").toInt();
        faceInfo.userId = query.value("user_id").toInt();
        faceInfo.studentId = query.value("student_id").toString();
        faceInfo.name = query.value("name").toString();
        faceInfo.dormitory = query.value("dormitory").toString();
        faceInfo.faceImagePath = query.value("face_image_path").toString();
        faceInfo.faceFeatureData = query.value("face_feature_data").toString();
        faceInfo.status = query.value("status").toInt();
        faceInfo.rejectReason = query.value("reject_reason").toString();
        faceInfo.submitTime = query.value("submit_time").toDateTime();
        faceInfo.auditTime = query.value("audit_time").toDateTime();
        faceInfo.auditorName = query.value("auditor_name").toString();
        faceInfo.remark = query.value("remark").toString();
    }
    
    return faceInfo;
}

QList<FaceInfo> DatabaseManager::getAllFaceInfos()
{
    QList<FaceInfo> list;
    
    QSqlQuery query("SELECT * FROM face_info ORDER BY submit_time DESC");
    
    while (query.next()) {
        FaceInfo faceInfo;
        faceInfo.id = query.value("id").toInt();
        faceInfo.userId = query.value("user_id").toInt();
        faceInfo.studentId = query.value("student_id").toString();
        faceInfo.name = query.value("name").toString();
        faceInfo.dormitory = query.value("dormitory").toString();
        faceInfo.faceImagePath = query.value("face_image_path").toString();
        faceInfo.faceFeatureData = query.value("face_feature_data").toString();
        faceInfo.status = query.value("status").toInt();
        faceInfo.rejectReason = query.value("reject_reason").toString();
        faceInfo.submitTime = query.value("submit_time").toDateTime();
        faceInfo.auditTime = query.value("audit_time").toDateTime();
        faceInfo.auditorName = query.value("auditor_name").toString();
        faceInfo.remark = query.value("remark").toString();
        list.append(faceInfo);
    }
    
    return list;
}

QList<FaceInfo> DatabaseManager::getPendingFaceInfos()
{
    QList<FaceInfo> list;
    
    QSqlQuery query("SELECT * FROM face_info WHERE status=0 ORDER BY submit_time DESC");
    
    while (query.next()) {
        FaceInfo faceInfo;
        faceInfo.id = query.value("id").toInt();
        faceInfo.userId = query.value("user_id").toInt();
        faceInfo.studentId = query.value("student_id").toString();
        faceInfo.name = query.value("name").toString();
        faceInfo.dormitory = query.value("dormitory").toString();
        faceInfo.faceImagePath = query.value("face_image_path").toString();
        faceInfo.faceFeatureData = query.value("face_feature_data").toString();
        faceInfo.status = query.value("status").toInt();
        faceInfo.rejectReason = query.value("reject_reason").toString();
        faceInfo.submitTime = query.value("submit_time").toDateTime();
        faceInfo.auditTime = query.value("audit_time").toDateTime();
        faceInfo.auditorName = query.value("auditor_name").toString();
        faceInfo.remark = query.value("remark").toString();
        list.append(faceInfo);
    }
    
    return list;
}

bool DatabaseManager::auditFaceInfo(int faceId, int status, const QString& auditorName, const QString& rejectReason)
{
    QSqlQuery query;
    query.prepare("UPDATE face_info SET status=?, auditor_name=?, audit_time=CURRENT_TIMESTAMP, reject_reason=? WHERE id=?");
    query.addBindValue(status);
    query.addBindValue(auditorName);
    query.addBindValue(rejectReason);
    query.addBindValue(faceId);
    
    if (!query.exec()) {
        qDebug() << "Audit face info error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// ==================== 维修申请管理 ====================

bool DatabaseManager::addRepairRequest(const RepairRequest& request)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO repair_requests (user_id, student_id, name, dormitory, contact_phone, 
        repair_type, description, image_path, status, priority, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(request.userId);
    query.addBindValue(request.studentId);
    query.addBindValue(request.name);
    query.addBindValue(request.dormitory);
    query.addBindValue(request.contactPhone);
    query.addBindValue(request.repairType);
    query.addBindValue(request.description);
    query.addBindValue(request.imagePath);
    query.addBindValue(request.status);
    query.addBindValue(request.priority);
    query.addBindValue(request.remark);
    
    if (!query.exec()) {
        qDebug() << "Add repair request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateRepairRequest(const RepairRequest& request)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE repair_requests SET contact_phone=?, repair_type=?, description=?, 
        image_path=?, status=?, priority=?, handler_name=?, handle_result=?, remark=? 
        WHERE id=?
    )");
    
    query.addBindValue(request.contactPhone);
    query.addBindValue(request.repairType);
    query.addBindValue(request.description);
    query.addBindValue(request.imagePath);
    query.addBindValue(request.status);
    query.addBindValue(request.priority);
    query.addBindValue(request.handlerName);
    query.addBindValue(request.handleResult);
    query.addBindValue(request.remark);
    query.addBindValue(request.id);
    
    if (!query.exec()) {
        qDebug() << "Update repair request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteRepairRequest(int requestId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM repair_requests WHERE id=?");
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        qDebug() << "Delete repair request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

RepairRequest DatabaseManager::getRepairRequestById(int requestId)
{
    RepairRequest request;
    request.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM repair_requests WHERE id=?");
    query.addBindValue(requestId);
    
    if (query.exec() && query.next()) {
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.dormitory = query.value("dormitory").toString();
        request.contactPhone = query.value("contact_phone").toString();
        request.repairType = query.value("repair_type").toInt();
        request.description = query.value("description").toString();
        request.imagePath = query.value("image_path").toString();
        request.status = query.value("status").toInt();
        request.priority = query.value("priority").toInt();
        request.handlerName = query.value("handler_name").toString();
        request.handleResult = query.value("handle_result").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.handleTime = query.value("handle_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
    }
    
    return request;
}

QList<RepairRequest> DatabaseManager::getAllRepairRequests()
{
    QList<RepairRequest> list;
    
    QSqlQuery query("SELECT * FROM repair_requests ORDER BY submit_time DESC");
    
    while (query.next()) {
        RepairRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.dormitory = query.value("dormitory").toString();
        request.contactPhone = query.value("contact_phone").toString();
        request.repairType = query.value("repair_type").toInt();
        request.description = query.value("description").toString();
        request.imagePath = query.value("image_path").toString();
        request.status = query.value("status").toInt();
        request.priority = query.value("priority").toInt();
        request.handlerName = query.value("handler_name").toString();
        request.handleResult = query.value("handle_result").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.handleTime = query.value("handle_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

QList<RepairRequest> DatabaseManager::getRepairRequestsByUser(int userId)
{
    QList<RepairRequest> list;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM repair_requests WHERE user_id=? ORDER BY submit_time DESC");
    query.addBindValue(userId);
    
    while (query.next()) {
        RepairRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.dormitory = query.value("dormitory").toString();
        request.contactPhone = query.value("contact_phone").toString();
        request.repairType = query.value("repair_type").toInt();
        request.description = query.value("description").toString();
        request.imagePath = query.value("image_path").toString();
        request.status = query.value("status").toInt();
        request.priority = query.value("priority").toInt();
        request.handlerName = query.value("handler_name").toString();
        request.handleResult = query.value("handle_result").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.handleTime = query.value("handle_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

QList<RepairRequest> DatabaseManager::getRepairRequestsByDormitory(const QString& dormitory)
{
    QList<RepairRequest> list;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM repair_requests WHERE dormitory=? ORDER BY submit_time DESC");
    query.addBindValue(dormitory);
    
    while (query.next()) {
        RepairRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.dormitory = query.value("dormitory").toString();
        request.contactPhone = query.value("contact_phone").toString();
        request.repairType = query.value("repair_type").toInt();
        request.description = query.value("description").toString();
        request.imagePath = query.value("image_path").toString();
        request.status = query.value("status").toInt();
        request.priority = query.value("priority").toInt();
        request.handlerName = query.value("handler_name").toString();
        request.handleResult = query.value("handle_result").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.handleTime = query.value("handle_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

QList<RepairRequest> DatabaseManager::getPendingRepairRequests()
{
    QList<RepairRequest> list;
    
    QSqlQuery query("SELECT * FROM repair_requests WHERE status=0 OR status=1 ORDER BY priority DESC, submit_time ASC");
    
    while (query.next()) {
        RepairRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.dormitory = query.value("dormitory").toString();
        request.contactPhone = query.value("contact_phone").toString();
        request.repairType = query.value("repair_type").toInt();
        request.description = query.value("description").toString();
        request.imagePath = query.value("image_path").toString();
        request.status = query.value("status").toInt();
        request.priority = query.value("priority").toInt();
        request.handlerName = query.value("handler_name").toString();
        request.handleResult = query.value("handle_result").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.handleTime = query.value("handle_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

bool DatabaseManager::handleRepairRequest(int requestId, int status, const QString& handlerName, const QString& handleResult)
{
    QSqlQuery query;
    
    if (status == 1) {
        query.prepare("UPDATE repair_requests SET status=?, handler_name=?, handle_time=CURRENT_TIMESTAMP WHERE id=?");
        query.addBindValue(status);
        query.addBindValue(handlerName);
        query.addBindValue(requestId);
    } else if (status == 2) {
        query.prepare("UPDATE repair_requests SET status=?, handler_name=?, handle_result=?, complete_time=CURRENT_TIMESTAMP WHERE id=?");
        query.addBindValue(status);
        query.addBindValue(handlerName);
        query.addBindValue(handleResult);
        query.addBindValue(requestId);
    } else {
        query.prepare("UPDATE repair_requests SET status=?, handler_name=?, handle_result=? WHERE id=?");
        query.addBindValue(status);
        query.addBindValue(handlerName);
        query.addBindValue(handleResult);
        query.addBindValue(requestId);
    }
    
    if (!query.exec()) {
        qDebug() << "Handle repair request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// ==================== 换寝申请管理 ====================

bool DatabaseManager::addRoomChangeRequest(const RoomChangeRequest& request)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO room_change_requests (user_id, student_id, name, current_dormitory, 
        target_dormitory, change_reason, description, status, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(request.userId);
    query.addBindValue(request.studentId);
    query.addBindValue(request.name);
    query.addBindValue(request.currentDormitory);
    query.addBindValue(request.targetDormitory);
    query.addBindValue(request.changeReason);
    query.addBindValue(request.description);
    query.addBindValue(request.status);
    query.addBindValue(request.remark);
    
    if (!query.exec()) {
        qDebug() << "Add room change request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateRoomChangeRequest(const RoomChangeRequest& request)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE room_change_requests SET target_dormitory=?, change_reason=?, description=?, 
        status=?, reject_reason=?, auditor_name=?, remark=? WHERE id=?
    )");
    
    query.addBindValue(request.targetDormitory);
    query.addBindValue(request.changeReason);
    query.addBindValue(request.description);
    query.addBindValue(request.status);
    query.addBindValue(request.rejectReason);
    query.addBindValue(request.auditorName);
    query.addBindValue(request.remark);
    query.addBindValue(request.id);
    
    if (!query.exec()) {
        qDebug() << "Update room change request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteRoomChangeRequest(int requestId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM room_change_requests WHERE id=?");
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        qDebug() << "Delete room change request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

RoomChangeRequest DatabaseManager::getRoomChangeRequestById(int requestId)
{
    RoomChangeRequest request;
    request.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM room_change_requests WHERE id=?");
    query.addBindValue(requestId);
    
    if (query.exec() && query.next()) {
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.currentDormitory = query.value("current_dormitory").toString();
        request.targetDormitory = query.value("target_dormitory").toString();
        request.changeReason = query.value("change_reason").toInt();
        request.description = query.value("description").toString();
        request.status = query.value("status").toInt();
        request.rejectReason = query.value("reject_reason").toString();
        request.auditorName = query.value("auditor_name").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.auditTime = query.value("audit_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
    }
    
    return request;
}

QList<RoomChangeRequest> DatabaseManager::getAllRoomChangeRequests()
{
    QList<RoomChangeRequest> list;
    
    QSqlQuery query("SELECT * FROM room_change_requests ORDER BY submit_time DESC");
    
    while (query.next()) {
        RoomChangeRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.currentDormitory = query.value("current_dormitory").toString();
        request.targetDormitory = query.value("target_dormitory").toString();
        request.changeReason = query.value("change_reason").toInt();
        request.description = query.value("description").toString();
        request.status = query.value("status").toInt();
        request.rejectReason = query.value("reject_reason").toString();
        request.auditorName = query.value("auditor_name").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.auditTime = query.value("audit_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

QList<RoomChangeRequest> DatabaseManager::getRoomChangeRequestsByUser(int userId)
{
    QList<RoomChangeRequest> list;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM room_change_requests WHERE user_id=? ORDER BY submit_time DESC");
    query.addBindValue(userId);
    
    while (query.next()) {
        RoomChangeRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.currentDormitory = query.value("current_dormitory").toString();
        request.targetDormitory = query.value("target_dormitory").toString();
        request.changeReason = query.value("change_reason").toInt();
        request.description = query.value("description").toString();
        request.status = query.value("status").toInt();
        request.rejectReason = query.value("reject_reason").toString();
        request.auditorName = query.value("auditor_name").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.auditTime = query.value("audit_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

QList<RoomChangeRequest> DatabaseManager::getPendingRoomChangeRequests()
{
    QList<RoomChangeRequest> list;
    
    QSqlQuery query("SELECT * FROM room_change_requests WHERE status=0 ORDER BY submit_time ASC");
    
    while (query.next()) {
        RoomChangeRequest request;
        request.id = query.value("id").toInt();
        request.userId = query.value("user_id").toInt();
        request.studentId = query.value("student_id").toString();
        request.name = query.value("name").toString();
        request.currentDormitory = query.value("current_dormitory").toString();
        request.targetDormitory = query.value("target_dormitory").toString();
        request.changeReason = query.value("change_reason").toInt();
        request.description = query.value("description").toString();
        request.status = query.value("status").toInt();
        request.rejectReason = query.value("reject_reason").toString();
        request.auditorName = query.value("auditor_name").toString();
        request.submitTime = query.value("submit_time").toDateTime();
        request.auditTime = query.value("audit_time").toDateTime();
        request.completeTime = query.value("complete_time").toDateTime();
        request.remark = query.value("remark").toString();
        list.append(request);
    }
    
    return list;
}

bool DatabaseManager::auditRoomChangeRequest(int requestId, int status, const QString& auditorName, const QString& rejectReason)
{
    QSqlQuery query;
    query.prepare("UPDATE room_change_requests SET status=?, auditor_name=?, audit_time=CURRENT_TIMESTAMP, reject_reason=? WHERE id=?");
    query.addBindValue(status);
    query.addBindValue(auditorName);
    query.addBindValue(rejectReason);
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        qDebug() << "Audit room change request error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::completeRoomChange(int requestId)
{
    m_db.transaction();
    
    RoomChangeRequest request = getRoomChangeRequestById(requestId);
    if (request.id == -1) {
        m_db.rollback();
        return false;
    }
    
    UserInfo user = getUserById(request.userId);
    if (user.id == -1) {
        m_db.rollback();
        return false;
    }
    
    user.dormitory = request.targetDormitory;
    if (!updateUser(user)) {
        m_db.rollback();
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE room_change_requests SET status=3, complete_time=CURRENT_TIMESTAMP WHERE id=?");
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "Complete room change error:" << query.lastError().text();
        return false;
    }
    
    m_db.commit();
    return true;
}

// ==================== 电器控制管理 ====================

bool DatabaseManager::addApplianceControl(const ApplianceControl& appliance)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO appliance_controls (dormitory, appliance_name, appliance_type, status, 
        power_level, current_power, daily_usage, schedule, is_online, remark)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(appliance.dormitory);
    query.addBindValue(appliance.applianceName);
    query.addBindValue(appliance.applianceType);
    query.addBindValue(appliance.status);
    query.addBindValue(appliance.powerLevel);
    query.addBindValue(appliance.currentPower);
    query.addBindValue(appliance.dailyUsage);
    query.addBindValue(appliance.schedule);
    query.addBindValue(appliance.isOnline ? 1 : 0);
    query.addBindValue(appliance.remark);
    
    if (!query.exec()) {
        qDebug() << "Add appliance control error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateApplianceControl(const ApplianceControl& appliance)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE appliance_controls SET appliance_name=?, appliance_type=?, status=?, 
        power_level=?, current_power=?, daily_usage=?, schedule=?, is_online=?, remark=? 
        WHERE id=?
    )");
    
    query.addBindValue(appliance.applianceName);
    query.addBindValue(appliance.applianceType);
    query.addBindValue(appliance.status);
    query.addBindValue(appliance.powerLevel);
    query.addBindValue(appliance.currentPower);
    query.addBindValue(appliance.dailyUsage);
    query.addBindValue(appliance.schedule);
    query.addBindValue(appliance.isOnline ? 1 : 0);
    query.addBindValue(appliance.remark);
    query.addBindValue(appliance.id);
    
    if (!query.exec()) {
        qDebug() << "Update appliance control error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteApplianceControl(int applianceId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM appliance_controls WHERE id=?");
    query.addBindValue(applianceId);
    
    if (!query.exec()) {
        qDebug() << "Delete appliance control error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

ApplianceControl DatabaseManager::getApplianceControlById(int applianceId)
{
    ApplianceControl appliance;
    appliance.id = -1;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM appliance_controls WHERE id=?");
    query.addBindValue(applianceId);
    
    if (query.exec() && query.next()) {
        appliance.id = query.value("id").toInt();
        appliance.dormitory = query.value("dormitory").toString();
        appliance.applianceName = query.value("appliance_name").toString();
        appliance.applianceType = query.value("appliance_type").toString();
        appliance.status = query.value("status").toInt();
        appliance.powerLevel = query.value("power_level").toInt();
        appliance.currentPower = query.value("current_power").toDouble();
        appliance.dailyUsage = query.value("daily_usage").toDouble();
        appliance.schedule = query.value("schedule").toString();
        appliance.isOnline = query.value("is_online").toInt() == 1;
        appliance.lastUpdate = query.value("last_update").toDateTime();
        appliance.remark = query.value("remark").toString();
    }
    
    return appliance;
}

QList<ApplianceControl> DatabaseManager::getApplianceControlsByDormitory(const QString& dormitory)
{
    QList<ApplianceControl> list;
    
    QSqlQuery query;
    query.prepare("SELECT * FROM appliance_controls WHERE dormitory=? ORDER BY appliance_type");
    query.addBindValue(dormitory);
    
    while (query.next()) {
        ApplianceControl appliance;
        appliance.id = query.value("id").toInt();
        appliance.dormitory = query.value("dormitory").toString();
        appliance.applianceName = query.value("appliance_name").toString();
        appliance.applianceType = query.value("appliance_type").toString();
        appliance.status = query.value("status").toInt();
        appliance.powerLevel = query.value("power_level").toInt();
        appliance.currentPower = query.value("current_power").toDouble();
        appliance.dailyUsage = query.value("daily_usage").toDouble();
        appliance.schedule = query.value("schedule").toString();
        appliance.isOnline = query.value("is_online").toInt() == 1;
        appliance.lastUpdate = query.value("last_update").toDateTime();
        appliance.remark = query.value("remark").toString();
        list.append(appliance);
    }
    
    return list;
}

QList<ApplianceControl> DatabaseManager::getAllApplianceControls()
{
    QList<ApplianceControl> list;
    
    QSqlQuery query("SELECT * FROM appliance_controls ORDER BY dormitory, appliance_type");
    
    while (query.next()) {
        ApplianceControl appliance;
        appliance.id = query.value("id").toInt();
        appliance.dormitory = query.value("dormitory").toString();
        appliance.applianceName = query.value("appliance_name").toString();
        appliance.applianceType = query.value("appliance_type").toString();
        appliance.status = query.value("status").toInt();
        appliance.powerLevel = query.value("power_level").toInt();
        appliance.currentPower = query.value("current_power").toDouble();
        appliance.dailyUsage = query.value("daily_usage").toDouble();
        appliance.schedule = query.value("schedule").toString();
        appliance.isOnline = query.value("is_online").toInt() == 1;
        appliance.lastUpdate = query.value("last_update").toDateTime();
        appliance.remark = query.value("remark").toString();
        list.append(appliance);
    }
    
    return list;
}

bool DatabaseManager::updateApplianceStatus(int applianceId, int status, int powerLevel)
{
    QSqlQuery query;
    
    if (powerLevel >= 0) {
        query.prepare("UPDATE appliance_controls SET status=?, power_level=?, last_update=CURRENT_TIMESTAMP WHERE id=?");
        query.addBindValue(status);
        query.addBindValue(powerLevel);
    } else {
        query.prepare("UPDATE appliance_controls SET status=?, last_update=CURRENT_TIMESTAMP WHERE id=?");
        query.addBindValue(status);
    }
    query.addBindValue(applianceId);
    
    if (!query.exec()) {
        qDebug() << "Update appliance status error:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::setApplianceSchedule(int applianceId, const QString& schedule)
{
    QSqlQuery query;
    query.prepare("UPDATE appliance_controls SET schedule=?, last_update=CURRENT_TIMESTAMP WHERE id=?");
    query.addBindValue(schedule);
    query.addBindValue(applianceId);
    
    if (!query.exec()) {
        qDebug() << "Set appliance schedule error:" << query.lastError().text();
        return false;
    }
    
    return true;
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
