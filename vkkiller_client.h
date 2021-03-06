#ifndef VKKILLER_CLIENT_H
#define VKKILLER_CLIENT_H

#include <QTcpSocket>
#include <QDataStream>
#include <memory>


class VkKillerClient: public QTcpSocket {
    Q_OBJECT

public:
    VkKillerClient(QObject* parent = nullptr);

    VkKillerClient(const VkKillerClient&)               = delete;
    VkKillerClient& operator=(const VkKillerClient&)    = delete;
    VkKillerClient(VkKillerClient&&)                    = delete;
    VkKillerClient& operator=(VkKillerClient&&)         = delete;

    void getTopicsListRequest  (                                       ) noexcept;
    void setNameRequest        (const QString& name                    ) noexcept;
    void createTopicRequest    (const QString& name, const QString& msg) noexcept;
    void sendTextMessageRequest(quint16 topicId,     const QString& msg) noexcept;
    void getTopicHistoryRequest(quint16 topicId                        ) noexcept;
    void getLastMessagesRequest(quint16 topicId                        ) noexcept;

private:
    void sendingBegin(quint8 request_type) noexcept;
    void sendingEnd  (                   ) noexcept;


    std::unique_ptr<QByteArray>  m_buffer;
    std::unique_ptr<QDataStream> m_outstream;
};

#endif // VKKILLER_CLIENT_H