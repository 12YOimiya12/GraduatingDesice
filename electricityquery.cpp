#include "electricityquery.h"
#include <QMessageBox>
#include <QDesktopServices>

ElectricityQuery::ElectricityQuery(QWidget *parent)
    : QMainWindow(parent)
    , m_urlEdit(nullptr)
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
    , m_resultDormLabel(nullptr)
    , m_helpText(nullptr)
    , m_rawHtmlText(nullptr)
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

    QGroupBox *urlGroup = new QGroupBox("目标网址");
    QFormLayout *urlLayout = new QFormLayout(urlGroup);
    
    m_urlEdit = new QLineEdit("https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1");
    urlLayout->addRow("网址:", m_urlEdit);
    
    mainLayout->addWidget(urlGroup);

    QGroupBox *infoGroup = new QGroupBox("查询信息记录（参考用）");
    QFormLayout *infoLayout = new QFormLayout(infoGroup);

    m_campusEdit = new QLineEdit();
    m_campusEdit->setPlaceholderText("如: 湘湖校区");
    infoLayout->addRow("缴费校区:", m_campusEdit);

    m_areaEdit = new QLineEdit();
    m_areaEdit->setPlaceholderText("如: 宿舍区");
    infoLayout->addRow("缴费区域:", m_areaEdit);

    m_buildingEdit = new QLineEdit();
    m_buildingEdit->setPlaceholderText("如: 38栋");
    infoLayout->addRow("缴费楼栋:", m_buildingEdit);

    m_floorEdit = new QLineEdit();
    m_floorEdit->setPlaceholderText("如: 1楼");
    infoLayout->addRow("缴费楼层:", m_floorEdit);

    m_roomEdit = new QLineEdit();
    m_roomEdit->setPlaceholderText("如: 38-101");
    infoLayout->addRow("缴费房间:", m_roomEdit);

    mainLayout->addWidget(infoGroup);

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

    QGroupBox *rawGroup = new QGroupBox("原始HTML（调试用）");
    QVBoxLayout *rawLayout = new QVBoxLayout(rawGroup);
    
    m_rawHtmlText = new QTextEdit();
    m_rawHtmlText->setReadOnly(true);
    m_rawHtmlText->setMaximumHeight(150);
    m_rawHtmlText->setPlaceholderText("获取数据后这里会显示原始HTML内容...");
    rawLayout->addWidget(m_rawHtmlText);
    
    mainLayout->addWidget(rawGroup);

    QGroupBox *helpGroup = new QGroupBox("说明");
    QVBoxLayout *helpLayout = new QVBoxLayout(helpGroup);

    m_helpText = new QTextEdit();
    m_helpText->setReadOnly(true);
    m_helpText->setMaximumHeight(120);
    m_helpText->setHtml(R"(
        <p><b>使用说明：</b></p>
        <ol style="margin: 5px 0; padding-left: 20px;">
            <li>点击"获取电费数据"直接尝试解析网页</li>
            <li>如果需要登录，请先点击"在浏览器中打开"</li>
            <li>在浏览器中完成登录后再回到这里获取数据</li>
            <li>上方查询信息用于记录您的宿舍信息</li>
        </ol>
    )");
    helpLayout->addWidget(m_helpText);

    mainLayout->addWidget(helpGroup);

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
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty()) {
        url = "https://ykt.jcu.edu.cn/epay/electric/load4electricbill?elcsysid=1";
    }
    
    m_statusLabel->setText("正在获取数据，请稍候...");
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #fff3cd; color: #856404; border-radius: 5px; text-align: center;");
    m_progressBar->setVisible(true);
    m_fetchBtn->setEnabled(false);
    
    m_resultKwhLabel->setText("剩余电费度数: --");
    m_resultAmountLabel->setText("剩余金额: --");
    m_resultDormLabel->setText("宿舍信息: --");
    m_rawHtmlText->clear();
    
    m_parser->fetchElectricityData(url);
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
    
    QString rawHtml = m_parser->getRawHtml();
    if (!rawHtml.isEmpty()) {
        m_rawHtmlText->setPlainText(rawHtml.left(5000));
    }
    
    QMessageBox::warning(this, "错误", QString("获取数据失败:\n%1\n\n注意：该网页可能需要先登录才能访问。").arg(error));
}

void ElectricityQuery::displayResults()
{
    QString kwh = m_parser->getRemainingKwh();
    QString amount = m_parser->getRemainingAmount();
    QString dorm = m_parser->getDormitory();
    QString rawHtml = m_parser->getRawHtml();
    
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
    
    if (!rawHtml.isEmpty()) {
        m_rawHtmlText->setPlainText(rawHtml.left(5000));
    }
    
    if (kwh.isEmpty() && rawHtml.isEmpty()) {
        QMessageBox::information(this, "提示", "未能获取到数据。\n\n可能原因：\n1. 网页需要先登录\n2. 网络连接问题\n3. 网址不正确\n\n建议：先点击'在浏览器中打开'完成登录，再尝试获取数据。");
    }
}

void ElectricityQuery::onBackClicked()
{
    close();
}
