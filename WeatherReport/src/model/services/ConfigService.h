#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H
#include <QString>
class ConfigService {
private:
    QString m_apiKey;
public:
    void setApiKey(const QString& key) {
        m_apiKey = key;
    }
};
#endif // CONFIGSERVICE_H
