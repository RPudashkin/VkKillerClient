#ifndef VKKILLER_REQUEST_REPLY_H
#define VKKILLER_REQUEST_REPLY_H

namespace Request_type {
    constexpr quint8 SET_NAME                       = 0x01;
    constexpr quint8 GET_TOPICS_LIST                = 0x02;
    constexpr quint8 CREATE_TOPIC                   = 0x03;
    constexpr quint8 TEXT_MESSAGE                   = 0x04;
    constexpr quint8 GET_TOPIC_HISTORY              = 0x05;
    constexpr quint8 GET_LAST_MESSAGES_FROM_TOPIC   = 0x06;
}

namespace Reply_type {
    constexpr quint8 OK                             = 0x01;
    constexpr quint8 WRONG_NAME                     = 0x02;
    constexpr quint8 FAILED_TOPIC_CREATE            = 0x03;
    constexpr quint8 WRONG_TOPIC_NAME               = 0x04;
    constexpr quint8 UNKNOWN_TOPIC                  = 0x05;
    constexpr quint8 WRONG_MESSAGE                  = 0x06;
    constexpr quint8 UNKNOWN_REQUEST                = 0x07;
    constexpr quint8 TOO_FAST_MESSAGING             = 0x08;
}

#endif // VKKILLER_REQUEST_REPLY_H
