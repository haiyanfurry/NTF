#ifndef FINDFRIENDAPP_USER_H
#define FINDFRIENDAPP_USER_H

#include <string>

class User {
private:
    std::string username;
    std::string password;
    std::string userId;
    bool hasBadge;
    bool isOfficial;

public:
    User();
    User(const std::string& username, const std::string& password);
    ~User();

    // Getters
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getUserId() const;
    bool getHasBadge() const;
    bool getIsOfficial() const;

    // Setters
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void setUserId(const std::string& userId);
    void setHasBadge(bool hasBadge);
    void setIsOfficial(bool isOfficial);

    // 验证用户信息
    bool validate() const;

    // 生成用户ID
    void generateUserId();
};

#endif // FINDFRIENDAPP_USER_H