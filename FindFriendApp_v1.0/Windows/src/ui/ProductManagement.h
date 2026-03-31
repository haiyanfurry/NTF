#ifndef PRODUCTMANAGEMENT_H
#define PRODUCTMANAGEMENT_H

#include <QWidget>

class QVBoxLayout;
class QPushButton;
class QTableWidget;
class QLineEdit;

class ProductManagement : public QWidget {
    Q_OBJECT

public:
    explicit ProductManagement(QWidget *parent = nullptr);
    ~ProductManagement();

private slots:
    void onAddProductClicked();
    void onEditProductClicked();
    void onDeleteProductClicked();
    void onSearchClicked();

private:
    void setupProductManagement();
    void setupSearchBar();
    void setupProductTable();
    void setupActionButtons();

    QVBoxLayout *m_mainLayout;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchButton;
    QTableWidget *m_productTable;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // PRODUCTMANAGEMENT_H