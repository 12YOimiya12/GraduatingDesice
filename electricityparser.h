#ifndef ELECTRICITYPARSER_H
#define ELECTRICITYPARSER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QPair>

/**
 * @brief 电费数据解析器类
 * 
 * 负责从网页获取电费数据并解析，主要功能包括：
 * - 通过HTTP请求获取电费网页HTML
 * - 使用正则表达式解析剩余电费度数、余额和宿舍信息
 * - 支持多种正则表达式模式匹配
 * - 处理SSL错误和网络错误
 */
class ElectricityParser : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit ElectricityParser(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ElectricityParser();
    
    /**
     * @brief 获取电费数据（使用默认URL）
     */
    void fetchElectricityData();
    
    /**
     * @brief 获取电费数据（指定URL）
     * @param url 电费查询网页URL
     */
    void fetchElectricityData(const QString &url);
    
    // 获取解析结果的方法
    QString getRemainingKwh() const { return m_remainingKwh; }      // 剩余电费度数
    QString getRemainingAmount() const { return m_remainingAmount; } // 剩余金额
    QString getDormitory() const { return m_dormitory; }            // 宿舍信息
    QString getRawHtml() const { return m_rawHtml; }                // 原始HTML
    bool isFinished() const { return m_isFinished; }                // 是否完成
    bool hasError() const { return m_hasError; }                    // 是否有错误
    QString getError() const { return m_errorString; }              // 错误信息
    
    /**
     * @brief 获取所有解析数据
     * @return QMap<QString, QString> 包含所有数据的映射
     */
    QMap<QString, QString> getAllData() const;

signals:
    /**
     * @brief 数据准备就绪信号
     */
    void dataReady();
    
    /**
     * @brief 错误发生信号
     * @param error 错误信息
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief 网络请求完成槽函数
     * @param reply 网络回复对象
     */
    void onReplyFinished(QNetworkReply *reply);
    
    /**
     * @brief SSL错误处理槽函数
     * @param reply 网络回复对象
     * @param errors SSL错误列表
     */
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    /**
     * @brief 解析HTML内容
     * @param html HTML内容
     */
    void parseHtml(const QString &html);
    
    /**
     * @brief 使用正则表达式提取数据
     * @param html HTML内容
     * @param pattern 正则表达式模式
     * @return QString 提取的数据
     */
    QString extractByPattern(const QString &html, const QString &pattern);
    
    QNetworkAccessManager *m_manager;    // 网络访问管理器
    QString m_remainingKwh;              // 剩余电费度数
    QString m_remainingAmount;           // 剩余金额
    QString m_dormitory;                 // 宿舍信息
    QString m_rawHtml;                   // 原始HTML内容
    bool m_isFinished;                   // 是否完成标志
    bool m_hasError;                     // 错误标志
    QString m_errorString;               // 错误信息
};

#endif // ELECTRICITYPARSER_H
