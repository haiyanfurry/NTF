#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QListWidget;

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

private slots:
    void onSetIPClicked();
    void onAddServerClicked();
    void onStartDiscoveryClicked();
    void onStopDiscoveryClicked();

private:
    void setupIPSettings();
    void setupServerList();
    void setupLocalDiscovery();

    QVBoxLayout *m_mainLayout;
    
    // IP设置
    QGroupBox *m_ipGroupBox;
    QLineEdit *m_ipEntry;
    QPushButton *m_setIPBtn;
    QLabel *m_currentIPLabel;
    
    // 服务器列表
    QGroupBox *m_serverListGroupBox;
    QLineEdit *m_serverNameEntry;
    QLineEdit *m_serverIPEntry;
    QPushButton *m_addServerBtn;
    QListWidget *m_serverListWidget;
    
    // 同城用户发现
    QGroupBox *m_discoveryGroupBox;
    QPushButton *m_startDiscoveryBtn;
    QPushButton *m_stopDiscoveryBtn;
    QListWidget *m_deviceListWidget;
};

#endif // SETTINGSWIDGET_H