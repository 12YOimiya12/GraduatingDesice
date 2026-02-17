#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDateTime>

// 用户信息结构体
struct UserInfo {
    int id;                     // 用户ID
    QString username;           // 用户名
    QString password;           // 密码（SHA256加密）
    QString name;               // 真实姓名
    QString studentId;          // 学号
    QString dormitory;          // 宿舍号
    int role;                   // 角色（0:学生, 1:管理员）
    double balance;             // 账户余额
};

// 宿舍信息结构体
struct DormitoryInfo {
    int id;                     // 宿舍ID
    QString dormNumber;         // 宿舍号
    QString building;           // 楼栋
    int floor;                  // 楼层
    double currentBalance;      // 当前电费余额
    double lastReading;         // 上次读数
    double remainingKwh;        // 剩余电费度数
    QDateTime lastUpdate;       // 最后更新时间
    QDateTime lastKwhUpdate;    // 最后度数更新时间
};

// 用电记录结构体
struct ElectricityRecord {
    int id;                     // 记录ID
    int userId;                 // 用户ID
    QString dormitory;          // 宿舍号
    double usage;               // 用电量（度）
    double cost;                // 费用（元）
    QDateTime recordTime;       // 记录时间
    QString remark;             // 备注
};

// 充值记录结构体
struct RechargeRecord {
    int id;                     // 记录ID
    int userId;                 // 用户ID
    QString studentId;          // 学号
    QString dormitory;          // 宿舍号
    double amount;              // 充值金额
    double balanceAfter;        // 充值后余额
    QDateTime rechargeTime;     // 充值时间
    QString operatorName;       // 操作员姓名
};

// 电费变动记录结构体
struct ElectricityChangeRecord {
    int id;                     // 记录ID
    int userId;                 // 用户ID
    QString studentId;          // 学号
    QString dormitory;          // 宿舍号
    double changeAmount;        // 变动金额（正数为充值，负数为扣费）
    double balanceBefore;       // 变动前余额
    double balanceAfter;        // 变动后余额
    QString changeType;         // 变动类型（"充值"、"扣费"、"调整"）
    QString operatorName;       // 操作员姓名
    QString remark;             // 备注
    QDateTime changeTime;       // 变动时间
};

// 电费度数变动记录结构体
struct ElectricityKwhChangeRecord {
    int id;                     // 记录ID
    QString dormitory;          // 宿舍号
    double kwhBefore;           // 变动前度数
    double kwhAfter;            // 变动后度数
    double kwhChange;           // 度数变动量
    QString changeType;         // 变动类型（"查询"、"调整"）
    QString operatorName;       // 操作员姓名
    QString remark;             // 备注
    QDateTime changeTime;       // 变动时间
    QString queryUrl;           // 查询网址
};

/**
 * @brief 数据库管理类 - 单例模式
 * 
 * 负责管理SQLite数据库的连接、表创建、数据操作等
 * 使用单例模式确保全局只有一个数据库实例
 */
class DatabaseManager
{
public:
    /**
     * @brief 获取数据库管理器单例实例
     * @return DatabaseManager& 单例引用
     */
    static DatabaseManager& instance();
    
    /**
     * @brief 初始化数据库（创建表和索引）
     * @return bool 初始化成功返回true，失败返回false
     */
    bool initDatabase();
    
    /**
     * @brief 打开数据库连接
     * @return bool 连接成功返回true，失败返回false
     */
    bool openDatabase();
    
    /**
     * @brief 关闭数据库连接
     */
    void closeDatabase();
    
    // 用户管理相关方法
    bool addUser(const UserInfo& user);
    bool updateUser(const UserInfo& user);
    bool deleteUser(int userId);
    UserInfo getUserByUsername(const QString& username);
    UserInfo getUserById(int userId);
    QList<UserInfo> getAllUsers();
    QList<UserInfo> getStudents();
    
    // 宿舍管理相关方法
    bool addDormitory(const DormitoryInfo& dorm);
    bool updateDormitory(const DormitoryInfo& dorm);
    bool deleteDormitory(int dormId);
    DormitoryInfo getDormitoryById(int dormId);
    DormitoryInfo getDormitoryByNumber(const QString& dormNumber);
    QList<DormitoryInfo> getAllDormitories();
    
    // 用电记录相关方法
    bool addElectricityRecord(const ElectricityRecord& record);
    QList<ElectricityRecord> getElectricityRecordsByUser(int userId);
    QList<ElectricityRecord> getElectricityRecordsByDormitory(const QString& dormitory);
    QList<ElectricityRecord> getAllElectricityRecords();
    
    // 充值记录相关方法
    bool addRechargeRecord(const RechargeRecord& record);
    QList<RechargeRecord> getRechargeRecordsByUser(int userId);
    QList<RechargeRecord> getRechargeRecordsByDormitory(const QString& dormitory);
    QList<RechargeRecord> getAllRechargeRecords();
    
    // 电费变动记录相关方法
    bool addElectricityChangeRecord(const ElectricityChangeRecord& record);
    QList<ElectricityChangeRecord> getElectricityChangeRecordsByUser(int userId);
    QList<ElectricityChangeRecord> getElectricityChangeRecordsByDormitory(const QString& dormitory);
    QList<ElectricityChangeRecord> getAllElectricityChangeRecords();
    
    // 电费度数变动记录相关方法
    bool addElectricityKwhChangeRecord(const ElectricityKwhChangeRecord& record);
    QList<ElectricityKwhChangeRecord> getElectricityKwhChangeRecordsByDormitory(const QString& dormitory);
    QList<ElectricityKwhChangeRecord> getAllElectricityKwhChangeRecords();
    
    // 宿舍度数管理方法
    bool updateDormitoryKwh(const QString& dormNumber, double newKwh, const QString& operatorName = "系统", const QString& queryUrl = "");
    
    /**
     * @brief 用户充值操作
     * @param userId 用户ID
     * @param amount 充值金额
     * @param operatorName 操作员姓名
     * @return bool 充值成功返回true，失败返回false
     */
    bool recharge(int userId, double amount, const QString& operatorName);
    
    /**
     * @brief 更新用户余额
     * @param userId 用户ID
     * @param newBalance 新的余额
     * @return bool 更新成功返回true，失败返回false
     */
    bool updateBalance(int userId, double newBalance);
    
    /**
     * @brief 扣除宿舍电费
     * @param dormitory 宿舍号
     * @param cost 扣除金额
     * @return bool 扣除成功返回true，失败返回false
     */
    bool deductElectricityCost(const QString& dormitory, double cost);
    
    /**
     * @brief 用户身份验证
     * @param username 用户名
     * @param password 密码
     * @param user 验证成功后返回用户信息
     * @return bool 验证成功返回true，失败返回false
     */
    bool authenticateUser(const QString& username, const QString& password, UserInfo& user);
    
    /**
     * @brief 初始化示例数据
     * @return bool 初始化成功返回true，失败返回false
     */
    bool initSampleData();
    
    /**
     * @brief 生成示例度数变化记录
     * @return bool 生成成功返回true，失败返回false
     */
    bool generateSampleKwhChangeRecords();
    
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    /**
     * @brief 创建数据库表
     * @return bool 创建成功返回true，失败返回false
     */
    bool createTables();
    
    /**
     * @brief 创建数据库索引
     * @return bool 创建成功返回true，失败返回false
     */
    bool createIndexes();
    
    QSqlDatabase m_db; // 数据库连接对象
};

#endif // DATABASEMANAGER_H
