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
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

ElectricityParser::ElectricityParser(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_remainingKwh("")
    , m_remainingAmount("")
    , m_studentAccount("")
    , m_dormitory("")
    , m_rawHtml("")
    , m_currentOperatorName("系统")
    , m_roomNo("")
    , m_cookie("")
    , m_isFinished(false)
    , m_hasError(false)
    , m_errorString("")
    , m_currentStep(FetchStep::None)
{
    connect(m_manager, &QNetworkAccessManager::finished, this, &ElectricityParser::onReplyFinished);
}

ElectricityParser::~ElectricityParser()
{
}

void ElectricityParser::fetchElectricityData(const QString &roomNo, const QString &operatorName, const QString &cookie)
{
    m_isFinished = false;
    m_hasError = false;
    m_errorString = "";
    m_remainingKwh = "";
    m_remainingAmount = "";
    m_studentAccount = "";
    m_dormitory = roomNo;
    m_rawHtml = "";
    m_currentOperatorName = operatorName;
    m_roomNo = roomNo;
    m_cookie = cookie;
    
    // 先获取账户余额
    fetchAccountBalance();
}

void ElectricityParser::fetchAccountBalance()
{
    m_currentStep = FetchStep::FetchingBalance;
    
    QString url = "https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1";
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);
    
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Safari/537.36");
    
    if (!m_cookie.isEmpty()) {
        request.setRawHeader("Cookie", m_cookie.toUtf8());
        qDebug() << "Using cookie for balance request";
    }
    
    qDebug() << "Fetching account balance from:" << url;
    m_manager->get(request);
}

void ElectricityParser::fetchRoomElecDegree()
{
    m_currentStep = FetchStep::FetchingElecDegree;
    
    QString url = "https://ykt.jcu.edu.cn/epay/electric/queryelectricbill";
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);
    
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Referer", "https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    
    if (!m_cookie.isEmpty()) {
        request.setRawHeader("Cookie", m_cookie.toUtf8());
        qDebug() << "Using cookie for elec degree request";
    }
    
    QUrlQuery postData;
    postData.addQueryItem("sysid", "1");
    postData.addQueryItem("roomNo", m_roomNo);
    postData.addQueryItem("elcarea", "1");
    postData.addQueryItem("elcbuis", "284");
    
    qDebug() << "Fetching room electricity degree for room:" << m_roomNo;
    m_manager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
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
    QString response = QString::fromUtf8(data);
    
    qDebug() << "Response received for step:" << static_cast<int>(m_currentStep) << "length:" << response.length();
    qDebug() << "Response preview:" << response.left(500);
    
    if (m_currentStep == FetchStep::FetchingBalance) {
        // 解析账户余额（HTML）
        parseHtml(response);
        m_rawHtml = response;
        
        // 继续获取房间剩余电量
        reply->deleteLater();
        fetchRoomElecDegree();
        return;
        
    } else if (m_currentStep == FetchStep::FetchingElecDegree) {
        // 解析房间剩余电量（JSON）
        parseJson(response);
        
        // 如果解析到剩余度数，记录度数变动
        if (!m_remainingKwh.isEmpty() && !m_dormitory.isEmpty()) {
            bool ok;
            double kwh = m_remainingKwh.toDouble(&ok);
            if (ok) {
                DatabaseManager::instance().updateDormitoryKwh(m_dormitory, kwh, m_currentOperatorName, "queryelectricbill");
            }
        }
        
        m_isFinished = true;
        reply->deleteLater();
        
        qDebug() << "All data fetched - Kwh:" << m_remainingKwh << "Amount:" << m_remainingAmount << "Dorm:" << m_dormitory;
        emit dataReady();
        return;
    }
    
    reply->deleteLater();
}

void ElectricityParser::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply);
    Q_UNUSED(errors);
    qDebug() << "SSL errors occurred, but ignoring";
}

void ElectricityParser::parseJson(const QString &jsonStr)
{
    QString trimmed = jsonStr.trimmed();
    if (!trimmed.startsWith("{") && !trimmed.startsWith("[")) {
        qDebug() << "Response is not JSON format";
        return;
    }
    
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return;
    }
    
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON format: not an object";
        return;
    }
    
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "JSON object keys:" << jsonObj.keys();
    
    if (jsonObj.contains("retcode")) {
        int retcode = jsonObj["retcode"].toInt();
        qDebug() << "retcode:" << retcode;
        
        if (retcode == 0) {
            if (jsonObj.contains("restElecDegree")) {
                double degree = jsonObj["restElecDegree"].toDouble();
                m_remainingKwh = QString::number(degree);
                qDebug() << "Found remaining kwh from JSON:" << m_remainingKwh;
            } else {
                qDebug() << "JSON does not contain 'restElecDegree' field";
            }
        } else {
            qDebug() << "API returned error, retcode:" << retcode << "retmsg:" << jsonObj.value("retmsg").toString();
        }
    } else {
        qDebug() << "JSON does not contain 'retcode' field";
    }
}

void ElectricityParser::parseHtml(const QString &html)
{
    qDebug() << "Starting HTML parsing for account balance, length:" << html.length();
    
    QList<QPair<QString, QString>> patterns;
    
    // 匹配学生账号（根据一卡通页面结构：转账账户：<span class="c_3497ea">122060200133</span>）
    patterns << qMakePair(QString("转账账户[：:]\\s*<[^>]*c_3497ea[^>]*>\\s*(\\d+)"), QString("account"));
    patterns << qMakePair(QString("账户[：:]\\s*<[^>]*c_3497ea[^>]*>\\s*(\\d+)"), QString("account"));
    
    // 匹配账户余额（根据一卡通页面结构：账户余额： <span class="c_3497ea">35.22 </span> 元）
    patterns << qMakePair(QString("账户余额[：:]\\s*<[^>]*c_3497ea[^>]*>\\s*(\\d+\\.?\\d*)"), QString("amount"));
    patterns << qMakePair(QString("余额[：:]\\s*<[^>]*c_3497ea[^>]*>\\s*(\\d+\\.?\\d*)"), QString("amount"));
    patterns << qMakePair(QString("账户余额[：:].{0,50}?(\\d+\\.?\\d*)"), QString("amount"));
    patterns << qMakePair(QString("余额[：:].{0,50}?(\\d+\\.?\\d*)"), QString("amount"));
    
    for (const auto &patternPair : patterns) {
        QString result = extractByPattern(html, patternPair.first);
        if (!result.isEmpty()) {
            if (patternPair.second == "amount" && m_remainingAmount.isEmpty()) {
                m_remainingAmount = result;
                qDebug() << "Found remaining amount with pattern:" << patternPair.first << "value:" << m_remainingAmount;
            } else if (patternPair.second == "account" && m_studentAccount.isEmpty()) {
                m_studentAccount = result;
                qDebug() << "Found student account with pattern:" << patternPair.first << "value:" << m_studentAccount;
            }
        }
    }
    
    qDebug() << "HTML parsing completed - Account:" << m_studentAccount << "Amount:" << m_remainingAmount;
}

QString ElectricityParser::extractByPattern(const QString &html, const QString &pattern)
{
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
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
