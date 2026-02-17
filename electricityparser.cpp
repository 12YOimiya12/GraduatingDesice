#include "electricityparser.h"
#include "databasemanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

ElectricityParser::ElectricityParser(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_remainingKwh("")
    , m_remainingAmount("")
    , m_dormitory("")
    , m_rawHtml("")
    , m_currentOperatorName("系统")
    , m_currentUrl("")
    , m_isFinished(false)
    , m_hasError(false)
    , m_errorString("")
{
    connect(m_manager, &QNetworkAccessManager::finished, this, &ElectricityParser::onReplyFinished);
}

ElectricityParser::~ElectricityParser()
{
}

void ElectricityParser::fetchElectricityData()
{
    fetchElectricityData("https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1");
}

void ElectricityParser::fetchElectricityData(const QString &url)
{
    fetchElectricityData(url, "", "系统");
}

void ElectricityParser::fetchElectricityData(const QString &url, const QString &dormitory, const QString &operatorName)
{
    m_isFinished = false;
    m_hasError = false;
    m_errorString = "";
    m_remainingKwh = "";
    m_remainingAmount = "";
    m_dormitory = dormitory;
    m_rawHtml = "";
    m_currentOperatorName = operatorName;
    m_currentUrl = url;
    
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);
    
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    
    m_manager->get(request);
}

void ElectricityParser::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_hasError = true;
        m_errorString = QString("网络错误: %1").arg(reply->errorString());
        qDebug() << "Network error:" << m_errorString;
        emit errorOccurred(m_errorString);
        reply->deleteLater();
        return;
    }
    
    QByteArray data = reply->readAll();
    m_rawHtml = QString::fromUtf8(data);
    
    qDebug() << "HTML received, length:" << m_rawHtml.length();
    
    parseHtml(m_rawHtml);
    
    // 如果解析到剩余度数并且有宿舍信息，记录度数变动
    if (!m_remainingKwh.isEmpty() && !m_dormitory.isEmpty()) {
        bool ok;
        double kwh = m_remainingKwh.toDouble(&ok);
        if (ok) {
            DatabaseManager::instance().updateDormitoryKwh(m_dormitory, kwh, m_currentOperatorName, m_currentUrl);
        }
    }
    
    m_isFinished = true;
    reply->deleteLater();
    
    emit dataReady();
}

void ElectricityParser::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply);
    Q_UNUSED(errors);
    qDebug() << "SSL errors occurred, but ignoring";
}

void ElectricityParser::parseHtml(const QString &html)
{
    QList<QPair<QString, QString>> patterns;
    
    patterns << qMakePair(QString("剩余.*?(\\d+\\.?\\d*).*?度"), QString("kwh"));
    patterns << qMakePair(QString("剩余.*?(\\d+\\.?\\d*).*?kWh"), QString("kwh"));
    patterns << qMakePair(QString("剩余.*?(\\d+\\.?\\d*).*?千瓦时"), QString("kwh"));
    patterns << qMakePair(QString("剩余电量[：:].*?(\\d+\\.?\\d*)"), QString("kwh"));
    patterns << qMakePair(QString("剩余度数[：:].*?(\\d+\\.?\\d*)"), QString("kwh"));
    patterns << qMakePair(QString("电量.*?剩余[：:].*?(\\d+\\.?\\d*)"), QString("kwh"));
    
    patterns << qMakePair(QString("剩余.*?(\\d+\\.?\\d*).*?元"), QString("amount"));
    patterns << qMakePair(QString("余额[：:].*?(\\d+\\.?\\d*)"), QString("amount"));
    patterns << qMakePair(QString("剩余金额[：:].*?(\\d+\\.?\\d*)"), QString("amount"));
    
    patterns << qMakePair(QString("宿舍[：:].*?(\\S+)"), QString("dorm"));
    patterns << qMakePair(QString("房间[：:].*?(\\S+)"), QString("dorm"));
    patterns << qMakePair(QString("寝室[：:].*?(\\S+)"), QString("dorm"));
    
    for (const auto &patternPair : patterns) {
        QString result = extractByPattern(html, patternPair.first);
        if (!result.isEmpty()) {
            if (patternPair.second == "kwh" && m_remainingKwh.isEmpty()) {
                m_remainingKwh = result;
                qDebug() << "Found remaining kwh:" << m_remainingKwh;
            } else if (patternPair.second == "amount" && m_remainingAmount.isEmpty()) {
                m_remainingAmount = result;
                qDebug() << "Found remaining amount:" << m_remainingAmount;
            } else if (patternPair.second == "dorm" && m_dormitory.isEmpty()) {
                m_dormitory = result;
                qDebug() << "Found dormitory:" << m_dormitory;
            }
        }
    }
    
    if (m_remainingKwh.isEmpty()) {
        QRegularExpression numRegex("(\\d+\\.?\\d*)");
        QRegularExpressionMatchIterator it = numRegex.globalMatch(html);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString num = match.captured(1);
            bool ok;
            double val = num.toDouble(&ok);
            if (ok && val > 0 && val < 10000) {
                if (m_remainingKwh.isEmpty()) {
                    m_remainingKwh = num;
                }
            }
        }
    }
    
    qDebug() << "Parsing complete - Kwh:" << m_remainingKwh << "Amount:" << m_remainingAmount << "Dorm:" << m_dormitory;
}

QString ElectricityParser::extractByPattern(const QString &html, const QString &pattern)
{
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(html);
    
    if (match.hasMatch()) {
        if (match.lastCapturedIndex() >= 1) {
            return match.captured(1).trimmed();
        }
    }
    
    return QString();
}

QMap<QString, QString> ElectricityParser::getAllData() const
{
    QMap<QString, QString> data;
    data["remaining_kwh"] = m_remainingKwh;
    data["remaining_amount"] = m_remainingAmount;
    data["dormitory"] = m_dormitory;
    return data;
}
