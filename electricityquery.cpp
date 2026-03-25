#include "electricityquery.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#ifdef Q_OS_WIN
#include <QAxObject>
#include <ActiveQt/QAxBase>
#endif

LoginBrowserDialog::LoginBrowserDialog(QWidget *parent)
    : QDialog(parent)
#ifdef Q_OS_WIN
    , m_webBrowser(nullptr)
#endif
    , m_doneBtn(nullptr)
    , m_statusLabel(nullptr)
{
    setWindowTitle("登录一卡通平台");
    resize(900, 700);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    m_statusLabel = new QLabel("请在下方浏览器中完成登录，登录成功后点击\"登录完成\"按钮");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #b8c2c9ff; color: #1565c0; border-radius: 5px;");
    layout->addWidget(m_statusLabel);
    
#ifdef Q_OS_WIN
    m_webBrowser = new QAxWidget(this);
    m_webBrowser->setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
    m_webBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_webBrowser->setProperty("Silent", true);
    
    layout->addWidget(m_webBrowser);
    
    QString loginUrl = "https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1";
    m_webBrowser->dynamicCall("Navigate(const QString&)", loginUrl);
#else
    QLabel *noSupportLabel = new QLabel("此功能仅支持Windows系统\n\n请手动在浏览器中登录并复制Cookie");
    noSupportLabel->setAlignment(Qt::AlignCenter);
    noSupportLabel->setStyleSheet("font-size: 16px; color: #666; padding: 50px;");
    layout->addWidget(noSupportLabel);
#endif
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_doneBtn = new QPushButton("登录完成");
    m_doneBtn->setStyleSheet("padding: 12px 30px; background-color: #4caf50; color: white; border: none; border-radius: 5px; font-weight: bold;");
    connect(m_doneBtn, &QPushButton::clicked, this, &LoginBrowserDialog::onLoginComplete);
    btnLayout->addStretch();
    btnLayout->addWidget(m_doneBtn);
    
    QPushButton *cancelBtn = new QPushButton("取消");
    cancelBtn->setStyleSheet("padding: 12px 30px; background-color: #9e9e9e; color: white; border: none; border-radius: 5px;");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    
    layout->addLayout(btnLayout);
}

LoginBrowserDialog::~LoginBrowserDialog()
{
}

void LoginBrowserDialog::onLoginComplete()
{
#ifdef Q_OS_WIN
    if (m_webBrowser) {
        QVariant result = m_webBrowser->dynamicCall("Document()");
        if (result.isValid()) {
            QAxObject *doc = m_webBrowser->querySubObject("Document()");
            if (doc) {
                m_cookies = doc->property("cookie").toString();
                delete doc;
            }
        }
        
        if (m_cookies.isEmpty()) {
            m_cookies = m_webBrowser->dynamicCall("GetCookie()").toString();
        }
    }
#endif
    
    if (m_cookies.isEmpty()) {
        QMessageBox::information(this, "提示", 
            "未能自动获取Cookie，请手动复制。\n\n"
            "获取方法：\n"
            "1. 在此浏览器中按F12打开开发者工具\n"
            "2. 切换到Network（网络）标签\n"
            "3. 刷新页面或点击任意链接\n"
            "4. 选择一个请求，在Headers中找到Cookie\n"
            "5. 复制Cookie值到主界面");
    }
    
    accept();
}

ElectricityQuery::ElectricityQuery(QWidget *parent)
    : QMainWindow(parent)
    , m_urlEdit(nullptr)
    , m_roomNoEdit(nullptr)
    , m_cookieEdit(nullptr)
    , m_campusEdit(nullptr)
    , m_areaEdit(nullptr)
    , m_buildingEdit(nullptr)
    , m_floorEdit(nullptr)
    , m_roomEdit(nullptr)
    , m_openBrowserBtn(nullptr)
    , m_fetchBtn(nullptr)
    , m_backBtn(nullptr)
    , m_resultKwhLabel(nullptr)
    , m_resultAmountLabel(nullptr)
    , m_resultAccountLabel(nullptr)
    , m_resultDormLabel(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_parser(new ElectricityParser(this))
{
    initUI();
    
    connect(m_parser, &ElectricityParser::dataReady, this, &ElectricityQuery::onDataReady);
    connect(m_parser, &ElectricityParser::errorOccurred, this, &ElectricityQuery::onErrorOccurred);
}

ElectricityQuery::~ElectricityQuery()
{
}

void ElectricityQuery::initUI()
{
    setWindowTitle("惠电 - 网页电费查询");
    resize(800, 700);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("一卡通电费查询");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    m_statusLabel = new QLabel("准备就绪，点击\"打开登录页面\"按钮开始查询");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #ecf0f1; border-radius: 5px; text-align: center;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    m_urlEdit = new QLineEdit("https://ykt.jcu.edu.cn/epay/electric/queryelectricbill");
    m_urlEdit->setVisible(false);
    mainLayout->addWidget(m_urlEdit);

    QGroupBox *roomGroup = new QGroupBox("查询房间");
    QFormLayout *roomLayout = new QFormLayout(roomGroup);
    
    m_roomNoEdit = new QLineEdit();
    m_roomNoEdit->setPlaceholderText("例如: 3669");
    m_roomNoEdit->setStyleSheet("padding: 8px;");
    roomLayout->addRow("房间号:", m_roomNoEdit);
    
    mainLayout->addWidget(roomGroup);

    QGroupBox *cookieGroup = new QGroupBox("登录Cookie");
    QVBoxLayout *cookieLayout = new QVBoxLayout(cookieGroup);
    
    QLabel *cookieHint = new QLabel("点击\"打开登录页面\"按钮，登录后Cookie将自动获取");
    cookieHint->setStyleSheet("color: #666; font-size: 12px;");
    cookieLayout->addWidget(cookieHint);
    
    m_cookieEdit = new QLineEdit();
    m_cookieEdit->setPlaceholderText("自动获取或手动输入Cookie");
    m_cookieEdit->setStyleSheet("padding: 8px;");
    cookieLayout->addWidget(m_cookieEdit);
    
    mainLayout->addWidget(cookieGroup);

    m_campusEdit = new QLineEdit();
    m_campusEdit->setVisible(false);
    mainLayout->addWidget(m_campusEdit);

    m_areaEdit = new QLineEdit();
    m_areaEdit->setVisible(false);
    mainLayout->addWidget(m_areaEdit);

    m_buildingEdit = new QLineEdit();
    m_buildingEdit->setVisible(false);
    mainLayout->addWidget(m_buildingEdit);

    m_floorEdit = new QLineEdit();
    m_floorEdit->setVisible(false);
    mainLayout->addWidget(m_floorEdit);

    m_roomEdit = new QLineEdit();
    m_roomEdit->setVisible(false);
    mainLayout->addWidget(m_roomEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_openBrowserBtn = new QPushButton("打开登录页面");
    m_openBrowserBtn->setStyleSheet("padding: 12px; background-color: #3498db; color: white; border: none; border-radius: 5px; font-weight: bold;");
    connect(m_openBrowserBtn, &QPushButton::clicked, this, &ElectricityQuery::onOpenBrowserClicked);
    btnLayout->addWidget(m_openBrowserBtn);

    m_fetchBtn = new QPushButton("获取电费数据");
    m_fetchBtn->setStyleSheet("padding: 12px; background-color: #27ae60; color: white; border: none; border-radius: 5px; font-weight: bold;");
    connect(m_fetchBtn, &QPushButton::clicked, this, &ElectricityQuery::onFetchDataClicked);
    btnLayout->addWidget(m_fetchBtn);

    mainLayout->addLayout(btnLayout);

    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressBar->setRange(0, 0);
    mainLayout->addWidget(m_progressBar);

    QGroupBox *resultGroup = new QGroupBox("查询结果");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    m_resultAccountLabel = new QLabel("学生账号: --");
    m_resultAccountLabel->setStyleSheet("font-size: 16px; color: #8e44ad; padding: 5px;");
    resultLayout->addWidget(m_resultAccountLabel);

    m_resultKwhLabel = new QLabel("剩余电费度数: --");
    m_resultKwhLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2980b9; padding: 10px;");
    resultLayout->addWidget(m_resultKwhLabel);

    m_resultAmountLabel = new QLabel("剩余金额: --");
    m_resultAmountLabel->setStyleSheet("font-size: 16px; color: #27ae60; padding: 5px;");
    resultLayout->addWidget(m_resultAmountLabel);

    m_resultDormLabel = new QLabel("宿舍信息: --");
    m_resultDormLabel->setStyleSheet("font-size: 14px; color: #7f8c8d; padding: 5px;");
    resultLayout->addWidget(m_resultDormLabel);

    mainLayout->addWidget(resultGroup);

    m_backBtn = new QPushButton("返回主界面");
    m_backBtn->setStyleSheet("padding: 12px; background-color: #95a5a6; color: white; border: none; border-radius: 5px;");
    connect(m_backBtn, &QPushButton::clicked, this, &ElectricityQuery::onBackClicked);
    mainLayout->addWidget(m_backBtn);

    mainLayout->addStretch();
}

void ElectricityQuery::onOpenBrowserClicked()
{
    LoginBrowserDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString cookies = dialog.getCookies();
        
        if (!cookies.isEmpty()) {
            m_cookieEdit->setText(cookies);
            m_statusLabel->setText("已自动获取Cookie，请输入房间号后点击\"获取电费数据\"");
            m_statusLabel->setStyleSheet("padding: 10px; background-color: #d4edda; color: #155724; border-radius: 5px; text-align: center;");
        } else {
            m_statusLabel->setText("未能自动获取Cookie，请手动输入或重试");
            m_statusLabel->setStyleSheet("padding: 10px; background-color: #fff3cd; color: #856404; border-radius: 5px; text-align: center;");
        }
    }
}

void ElectricityQuery::onFetchDataClicked()
{
    QString roomNo = m_roomNoEdit->text().trimmed();
    if (roomNo.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入房间号！");
        return;
    }
    
    QString cookie = m_cookieEdit->text().trimmed();
    if (cookie.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先点击\"打开登录页面\"获取Cookie！");
        return;
    }
    
    QString operatorName = "匿名用户";
    
    m_statusLabel->setText("正在获取数据，请稍候...");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #fff3cd; color: #856404; border-radius: 5px; text-align: center;");
    m_progressBar->setVisible(true);
    m_fetchBtn->setEnabled(false);
    
    m_resultAccountLabel->setText("学生账号: --");
    m_resultKwhLabel->setText("剩余电费度数: --");
    m_resultAmountLabel->setText("剩余金额: --");
    m_resultDormLabel->setText("宿舍信息: --");
    
    m_parser->fetchElectricityData(roomNo, operatorName, cookie);
}

void ElectricityQuery::onDataReady()
{
    m_progressBar->setVisible(false);
    m_fetchBtn->setEnabled(true);
    
    displayResults();
    
    m_statusLabel->setText("数据获取完成！");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #d4edda; color: #155724; border-radius: 5px; text-align: center;");
}

void ElectricityQuery::onErrorOccurred(const QString &error)
{
    m_progressBar->setVisible(false);
    m_fetchBtn->setEnabled(true);
    
    m_statusLabel->setText(QString("错误: %1").arg(error));
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #f8d7da; color: #721c24; border-radius: 5px; text-align: center;");
    
    QMessageBox::warning(this, "错误", QString("获取数据失败:\n%1\n\n注意：该网页可能需要先登录才能访问。").arg(error));
}

void ElectricityQuery::displayResults()
{
    QString kwh = m_parser->getRemainingKwh();
    QString amount = m_parser->getRemainingAmount();
    QString account = m_parser->getStudentAccount();
    QString dorm = m_parser->getDormitory();
    
    if (!account.isEmpty()) {
        m_resultAccountLabel->setText(QString("学生账号: %1").arg(account));
    } else {
        m_resultAccountLabel->setText("学生账号: 未找到");
    }
    
    if (!kwh.isEmpty()) {
        m_resultKwhLabel->setText(QString("剩余电费度数: <b>%1</b> 度").arg(kwh));
        m_resultKwhLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #27ae60; padding: 10px;");
    } else {
        m_resultKwhLabel->setText("剩余电费度数: <span style='color: #e74c3c;'>未找到</span>");
        m_resultKwhLabel->setStyleSheet("font-size: 18px; padding: 10px;");
    }
    
    if (!amount.isEmpty()) {
        m_resultAmountLabel->setText(QString("剩余金额: %1 元").arg(amount));
    } else {
        m_resultAmountLabel->setText("剩余金额: 未找到");
    }
    
    if (!dorm.isEmpty()) {
        m_resultDormLabel->setText(QString("宿舍信息: %1").arg(dorm));
    } else {
        m_resultDormLabel->setText("宿舍信息: 未找到");
    }
    
    if (kwh.isEmpty()) {
        QMessageBox::information(this, "提示", "未能获取到数据。\n\n可能原因：\n1. 网页需要先登录\n2. 网络连接问题\n3. 网址不正确\n\n建议：先点击'打开登录页面'完成登录，再尝试获取数据。");
    }
}

void ElectricityQuery::onBackClicked()
{
    close();
}
