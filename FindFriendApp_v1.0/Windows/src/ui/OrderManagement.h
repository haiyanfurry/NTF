#ifndef ORDERMANAGEMENT_H
#define ORDERMANAGEMENT_H

#include <QWidget>

class QVBoxLayout;
class QPushButton;
class QTableWidget;
class QLineEdit;

class OrderManagement : public QWidget {
    Q_OBJECT

public:
    explicit OrderManagement(QWidget *parent = nullptr);
    ~OrderManagement();

private slots:
    void onProcessOrderClicked();
    void onCancelOrderClicked();
    void onRefundOrderClicked();
    void onSearchClicked();

private:
    void setupOrderManagement();
    void setupSearchBar();
    void setupOrderTable();
    void setupActionButtons();

    QVBoxLayout *m_mainLayout;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;
    QTableWidget *m_orderTable;
    QPushButton *m_processButton;
    QPushButton *m_cancelButton;
    QPushButton *m_refundButton;
};

#endif // ORDERMANAGEMENT_H