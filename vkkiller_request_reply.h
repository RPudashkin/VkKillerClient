#ifndef VKKILLER_REQUEST_REPLY_H
#define VKKILLER_REQUEST_REPLY_H

namespace Request_type {
    constexpr quint8 SET_NAME                       = 1;
    constexpr quint8 GET_TOPICS_LIST                = 2;
    constexpr quint8 CREATE_TOPIC                   = 3;
    constexpr quint8 TEXT_MESSAGE                   = 4;
    constexpr quint8 GET_TOPIC_HISTORY              = 5;
    constexpr quint8 GET_LAST_MESSAGES_FROM_TOPIC   = 6;
}

namespace Reply_type {
    constexpr quint8 CONNECTED                      = 1;
    constexpr quint8 TOPICS_LIST                    = 2;
    constexpr quint8 TOPIC_HISTORY                  = 3;
    constexpr quint8 LAST_MESSAGES                  = 4;

    constexpr quint8 WRONG_NAME                     = 5;
    constexpr quint8 FAILED_TOPIC_CREATE            = 6;
    constexpr quint8 WRONG_TOPIC_NAME               = 7;
    constexpr quint8 UNKNOWN_TOPIC                  = 8;
    constexpr quint8 WRONG_MESSAGE                  = 9;
    constexpr quint8 UNKNOWN_REQUEST                = 10;
    constexpr quint8 TOO_FAST_MESSAGING             = 11;
    constexpr quint8 TOO_FAST_TOPIC_CREATING        = 12;
}

#endif // VKKILLER_REQUEST_REPLY_H