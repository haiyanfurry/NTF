#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString username() const { return m_userEdit->text(); }
    QString password() const { return m_passEdit->text(); }

private slots:
    void onLogin();

private:
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
};
#endif
