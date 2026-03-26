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

// 电费查询记录结构体
struct ElectricityQueryRecord {
    int id;                     // 记录ID
    QString studentAccount;     // 学生账号（一卡通号）
    QString dormitory;          // 宿舍号
    double remainingKwh;        // 剩余度数
    double remainingAmount;     // 剩余金额
    QString operatorName;       // 操作员
    QString queryUrl;           // 查询网址
    QDateTime queryTime;        // 查询时间
    QString remark;             // 备注
};

// 人脸信息结构体
struct FaceInfo {
    int id;                     // 记录ID
    int userId;                 // 用户ID
    QString studentId;          // 学号
    QString name;               // 姓名
    QString dormitory;          // 宿舍号
    QString faceImagePath;      // 人脸图片路径
    QString faceFeatureData;    // 人脸特征数据（Base64编码）
    int status;                 // 状态（0:待审核, 1:已通过, 2:已拒绝）
    QString rejectReason;       // 拒绝原因
    QDateTime submitTime;       // 提交时间
    QDateTime auditTime;        // 审核时间
    QString auditorName;        // 审核人
    QString remark;             // 备注
};

// 维修申请结构体
struct RepairRequest {
    int id;                     // 申请ID
    int userId;                 // 用户ID
    QString studentId;          // 学号
    QString name;               // 姓名
    QString dormitory;          // 宿舍号
    QString contactPhone;       // 联系电话
    int repairType;             // 维修类型（0:电路, 1:水管, 2:门窗, 3:家具, 4:其他）
    QString repairTypeText;     // 维修类型文本
    QString description;        // 问题描述
    QString imagePath;          // 图片路径
    int status;                 // 状态（0:待处理, 1:处理中, 2:已完成, 3:已关闭）
    int priority;               // 优先级（0:普通, 1:紧急, 2:非常紧急）
    QString handlerName;        // 处理人
    QString handleResult;       // 处理结果
    QDateTime submitTime;       // 提交时间
    QDateTime handleTime;       // 处理时间
    QDateTime completeTime;     // 完成时间
    QString remark;             // 备注
};

// 换寝申请结构体
struct RoomChangeRequest {
    int id;                     // 申请ID
    int userId;                 // 用户ID
    QString studentId;          // 学号
    QString name;               // 姓名
    QString currentDormitory;   // 当前宿舍
    QString targetDormitory;    // 目标宿舍
    int changeReason;           // 换寝原因（0:室友矛盾, 1:身体原因, 2:学业需要, 3:其他）
    QString changeReasonText;   // 换寝原因文本
    QString description;        // 详细说明
    int status;                 // 状态（0:待审核, 1:已通过, 2:已拒绝, 3:已完成）
    QString rejectReason;       // 拒绝原因
    QString auditorName;        // 审核人
    QDateTime submitTime;       // 提交时间
    QDateTime auditTime;        // 审核时间
    QDateTime completeTime;     // 完成时间
    QString remark;             // 备注
};

// 电器控制结构体
struct ApplianceControl {
    int id;                     // 控制器ID
    QString dormitory;          // 宿舍号
    QString applianceName;      // 电器名称
    QString applianceType;      // 电器类型（空调、热水器、照明等）
    int status;                 // 状态（0:关闭, 1:开启）
    int powerLevel;             // 功率档位（0-100）
    double currentPower;        // 当前功率（瓦）
    double dailyUsage;          // 日用电量（度）
    QString schedule;           // 定时任务（JSON格式）
    bool isOnline;              // 是否在线
    QDateTime lastUpdate;       // 最后更新时间
    QString remark;             // 备注
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
    
    // 电费查询记录相关方法
    bool addElectricityQueryRecord(const ElectricityQueryRecord& record);
    QList<ElectricityQueryRecord> getElectricityQueryRecordsByDormitory(const QString& dormitory);
    QList<ElectricityQueryRecord> getElectricityQueryRecordsByStudentAccount(const QString& studentAccount);
    QList<ElectricityQueryRecord> getAllElectricityQueryRecords();
    
    // 保存完整电费查询结果（包含学生账号、剩余电费、剩余度数、宿舍号）
    bool saveElectricityQueryResult(const QString& studentAccount, const QString& dormitory, 
                                    double remainingKwh, double remainingAmount, 
                                    const QString& operatorName = "系统", const QString& queryUrl = "");
    
    // 人脸信息管理相关方法
    bool addFaceInfo(const FaceInfo& faceInfo);
    bool updateFaceInfo(const FaceInfo& faceInfo);
    bool deleteFaceInfo(int faceId);
    FaceInfo getFaceInfoById(int faceId);
    FaceInfo getFaceInfoByUserId(int userId);
    QList<FaceInfo> getAllFaceInfos();
    QList<FaceInfo> getPendingFaceInfos();
    bool auditFaceInfo(int faceId, int status, const QString& auditorName, const QString& rejectReason = "");
    
    // 维修申请管理相关方法
    bool addRepairRequest(const RepairRequest& request);
    bool updateRepairRequest(const RepairRequest& request);
    bool deleteRepairRequest(int requestId);
    RepairRequest getRepairRequestById(int requestId);
    QList<RepairRequest> getAllRepairRequests();
    QList<RepairRequest> getRepairRequestsByUser(int userId);
    QList<RepairRequest> getRepairRequestsByDormitory(const QString& dormitory);
    QList<RepairRequest> getPendingRepairRequests();
    bool handleRepairRequest(int requestId, int status, const QString& handlerName, const QString& handleResult = "");
    
    // 换寝申请管理相关方法
    bool addRoomChangeRequest(const RoomChangeRequest& request);
    bool updateRoomChangeRequest(const RoomChangeRequest& request);
    bool deleteRoomChangeRequest(int requestId);
    RoomChangeRequest getRoomChangeRequestById(int requestId);
    QList<RoomChangeRequest> getAllRoomChangeRequests();
    QList<RoomChangeRequest> getRoomChangeRequestsByUser(int userId);
    QList<RoomChangeRequest> getPendingRoomChangeRequests();
    bool auditRoomChangeRequest(int requestId, int status, const QString& auditorName, const QString& rejectReason = "");
    bool completeRoomChange(int requestId);
    
    // 电器控制管理相关方法
    bool addApplianceControl(const ApplianceControl& appliance);
    bool updateApplianceControl(const ApplianceControl& appliance);
    bool deleteApplianceControl(int applianceId);
    ApplianceControl getApplianceControlById(int applianceId);
    QList<ApplianceControl> getApplianceControlsByDormitory(const QString& dormitory);
    QList<ApplianceControl> getAllApplianceControls();
    bool updateApplianceStatus(int applianceId, int status, int powerLevel = -1);
    bool setApplianceSchedule(int applianceId, const QString& schedule);
    
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
    
    /**
     * @brief 数据库迁移，确保表结构是最新的
     * @return bool 迁移成功返回true，失败返回false
     */
    bool migrateDatabase();
    
    QSqlDatabase m_db; // 数据库连接对象
};

#endif // DATABASEMANAGER_H
