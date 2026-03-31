#ifndef CUSTOMERMANAGEMENT_H
#define CUSTOMERMANAGEMENT_H

#include <QWidget>

class QVBoxLayout;
class QPushButton;
class QTableWidget;
class QLineEdit;

class CustomerManagement : public QWidget {
    Q_OBJECT

public:
    explicit CustomerManagement(QWidget *parent = nullptr);
    ~CustomerManagement();

private slots:
    void onAddCustomerClicked();
    void onEditCustomerClicked();
    void onDeleteCustomerClicked();
    void onSearchClicked();

private:
    void setupCustomerManagement();
    void setupSearchBar();
    void setupCustomerTable();
    void setupActionButtons();

    QVBoxLayout *m_mainLayout;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;
    QTableWidget *m_customerTable;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // CUSTOMERMANAGEMENT_H