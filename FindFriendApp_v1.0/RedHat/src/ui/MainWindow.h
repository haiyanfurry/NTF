#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginDialog();
    void onLoginSuccess(const QString& username);

private:
    QTabWidget *m_tabWidget;
    bool m_loggedIn;
};
#endif
