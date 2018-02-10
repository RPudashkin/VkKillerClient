#include "vkkiller_client.h"
#include "vkkiller_request_reply.h"


VkKillerClient::VkKillerClient(QObject* parent):
    QTcpSocket      (parent),
    m_buffer        (std::make_unique<QByteArray> ()),
    m_outstream     (std::make_unique<QDataStream>(m_buffer.get(), QIODevice::WriteOnly))
{
    m_outstream->setVersion(QDataStream::Qt_DefaultCompiledVersion);
}


inline void VkKillerClient::sendingBegin(quint8 request_type) noexcept {
    m_buffer->clear();
    m_outstream->device()->seek(0);
    *m_outstream << quint16(0) << request_type;
}


inline void VkKillerClient::sendingEnd() noexcept {
    m_outstream->device()->seek(0);
    *m_outstream << quint16(m_buffer->size() - sizeof(quint16));
    write(*m_buffer);
}


void VkKillerClient::getTopicsListRequest() noexcept {
    sendingBegin(Request_type::GET_TOPICS_LIST);
    sendingEnd  ();
}


void VkKillerClient::sendTextMessageRequest(quint16 topicId, const QString& msg) noexcept {
    sendingBegin(Request_type::TEXT_MESSAGE);
    *m_outstream << topicId << msg;
    sendingEnd  ();
}

void VkKillerClient::getTopicHistoryRequest(quint16 topicId) noexcept {
    sendingBegin(Request_type::GET_TOPIC_HISTORY);
    *m_outstream << topicId;
    sendingEnd  ();
}


void VkKillerClient::getLastMessagesRequest(quint16 topicId) noexcept {
    sendingBegin(Request_type::GET_LAST_MESSAGES_FROM_TOPIC);
    *m_outstream << topicId;
    sendingEnd  ();
}


void VkKillerClient::createTopicRequest(const QString& name, const QString& msg) noexcept {
    sendingBegin(Request_type::CREATE_TOPIC);
    *m_outstream << name << msg;
    sendingEnd  ();
}


void VkKillerClient::setNameRequest(const QString& name) noexcept {
    sendingBegin(Request_type::SET_NAME);
    *m_outstream << name;
    sendingEnd  ();
}