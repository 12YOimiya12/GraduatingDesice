#include "electricityquery.h"
#include <QMessageBox>
#include <QDesktopServices>

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
    resize(800, 900);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("一卡通电费查询");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    m_statusLabel = new QLabel("准备就绪，点击下方按钮获取电费数据");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #ecf0f1; border-radius: 5px; text-align: center;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    // 初始化URL编辑框（用于内部使用，不再显示在界面上）
    m_urlEdit = new QLineEdit("https://ykt.jcu.edu.cn/epay/electric/queryelectricbill");
    m_urlEdit->setVisible(false);
    mainLayout->addWidget(m_urlEdit);

    // 房间号输入框
    QGroupBox *roomGroup = new QGroupBox("查询房间");
    QFormLayout *roomLayout = new QFormLayout(roomGroup);
    
    m_roomNoEdit = new QLineEdit();
    m_roomNoEdit->setPlaceholderText("例如: 3669");
    m_roomNoEdit->setStyleSheet("padding: 8px;");
    roomLayout->addRow("房间号:", m_roomNoEdit);
    
    mainLayout->addWidget(roomGroup);

    // Cookie输入框
    QGroupBox *cookieGroup = new QGroupBox("登录Cookie（必填）");
    QVBoxLayout *cookieLayout = new QVBoxLayout(cookieGroup);
    
    QLabel *cookieHint = new QLabel("请先在浏览器中登录一卡通平台，然后复制Cookie：");
    cookieHint->setStyleSheet("color: #666; font-size: 12px;");
    cookieLayout->addWidget(cookieHint);
    
    m_cookieEdit = new QLineEdit();
    m_cookieEdit->setPlaceholderText("例如: JSESSIONID=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    m_cookieEdit->setStyleSheet("padding: 8px;");
    cookieLayout->addWidget(m_cookieEdit);
    
    QLabel *cookieHowTo = new QLabel("获取方法：按F12打开开发者工具 → Network → 找到任意请求 → Headers → Cookie");
    cookieHowTo->setStyleSheet("color: #999; font-size: 11px;");
    cookieLayout->addWidget(cookieHowTo);
    
    mainLayout->addWidget(cookieGroup);

    // 初始化查询信息编辑框（用于内部使用，不再显示在界面上）
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
    
    m_openBrowserBtn = new QPushButton("在浏览器中打开");
    m_openBrowserBtn->setStyleSheet("padding: 12px; background-color: #3498db; color: white; border: none; border-radius: 5px;");
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
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty()) {
        url = "https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1";
    }
    
    bool success = QDesktopServices::openUrl(QUrl(url));
    
    if (success) {
        m_statusLabel->setText("已在浏览器中打开，请在浏览器中完成登录等操作");
        m_statusLabel->setStyleSheet("padding: 10px; background-color: #d4edda; color: #155724; border-radius: 5px; text-align: center;");
    } else {
        QMessageBox::warning(this, "提示", "无法打开浏览器，请手动访问网址");
    }
}

void ElectricityQuery::onFetchDataClicked()
{
    // 获取房间号
    QString roomNo = m_roomNoEdit->text().trimmed();
    if (roomNo.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入房间号！");
        return;
    }
    
    // 获取Cookie
    QString cookie = m_cookieEdit->text().trimmed();
    if (cookie.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入登录Cookie！\n\n获取方法：\n1. 在浏览器中登录一卡通平台\n2. 按F12打开开发者工具\n3. 切换到Network标签\n4. 刷新页面，找到任意请求\n5. 点击请求，在Headers中找到Cookie\n6. 复制Cookie值粘贴到上方输入框");
        return;
    }
    
    // 获取当前登录用户信息
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
        QMessageBox::information(this, "提示", "未能获取到数据。\n\n可能原因：\n1. 网页需要先登录\n2. 网络连接问题\n3. 网址不正确\n\n建议：先点击'在浏览器中打开'完成登录，再尝试获取数据。");
    }
}

void ElectricityQuery::onBackClicked()
{
    close();
}
