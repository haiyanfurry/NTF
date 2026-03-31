#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginDialog();
    void onLoginSuccess(const QString& username);

private:
    QTabWidget *m_tabWidget;
    bool m_loggedIn;
};

#endif // MAINWINDOW_H