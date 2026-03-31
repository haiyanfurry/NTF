#ifndef MERCHANTDASHBOARD_H
#define MERCHANTDASHBOARD_H

#include <QWidget>

class QVBoxLayout;
class QLabel;
class QPushButton;
class QTableWidget;

class MerchantDashboard : public QWidget {
    Q_OBJECT

public:
    explicit MerchantDashboard(QWidget *parent = nullptr);
    ~MerchantDashboard();

private slots:
    void onRefreshClicked();

private:
    void setupDashboard();
    void setupStats();
    void setupRecentOrders();
    void setupQuickActions();

    QVBoxLayout *m_mainLayout;
    QLabel *m_salesLabel;
    QLabel *m_ordersLabel;
    QLabel *m_customersLabel;
    QTableWidget *m_recentOrdersTable;
    QPushButton *m_refreshButton;
};

#endif // MERCHANTDASHBOARD_H