#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        THEME_DEFAULT,
        THEME_DARK,
        THEME_LIGHT
    };

    static ThemeManager* instance();
    
    // 加载主题
    bool loadTheme(Theme theme);
    bool loadTheme(const QString& qssFilePath);
    
    // 获取当前主题
    Theme currentTheme() const { return m_currentTheme; }
    
    // 获取主题名称
    QString themeName(Theme theme) const;
    
    // 获取主题文件路径
    QString themePath(Theme theme) const;

signals:
    void themeChanged(Theme newTheme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    
    static ThemeManager* s_instance;
    Theme m_currentTheme;
    QMap<Theme, QString> m_themePaths;
};

#endif // THEME_MANAGER_H
