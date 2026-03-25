#ifndef ELECTRICITYQUERY_H
#define ELECTRICITYQUERY_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QDialog>
#include "electricityparser.h"

#ifdef Q_OS_WIN
#include <QAxWidget>
#endif

class LoginBrowserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginBrowserDialog(QWidget *parent = nullptr);
    ~LoginBrowserDialog();
    QString getCookies() const { return m_cookies; }

private slots:
    void onLoginComplete();

private:
#ifdef Q_OS_WIN
    QAxWidget *m_webBrowser;
#endif
    QString m_cookies;
    QPushButton *m_doneBtn;
    QLabel *m_statusLabel;
};

class ElectricityQuery : public QMainWindow
{
    Q_OBJECT

public:
    explicit ElectricityQuery(QWidget *parent = nullptr);
    ~ElectricityQuery();

private slots:
    void onOpenBrowserClicked();
    void onFetchDataClicked();
    void onDataReady();
    void onErrorOccurred(const QString &error);
    void onBackClicked();

private:
    void initUI();
    void displayResults();
    
    QLineEdit *m_urlEdit;
    QLineEdit *m_roomNoEdit;
    QLineEdit *m_cookieEdit;
    QLineEdit *m_campusEdit;
    QLineEdit *m_areaEdit;
    QLineEdit *m_buildingEdit;
    QLineEdit *m_floorEdit;
    QLineEdit *m_roomEdit;
    QPushButton *m_openBrowserBtn;
    QPushButton *m_fetchBtn;
    QPushButton *m_backBtn;
    QLabel *m_resultKwhLabel;
    QLabel *m_resultAmountLabel;
    QLabel *m_resultAccountLabel;
    QLabel *m_resultDormLabel;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    ElectricityParser *m_parser;
};

#endif // ELECTRICITYQUERY_H
