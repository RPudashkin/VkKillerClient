#ifndef VKKILLER_SERVER_CONSTANTS_H
#define VKKILLER_SERVER_CONSTANTS_H

namespace Server_constant {
    constexpr quint16 MAX_TOPICS_AMOUNT       = 150;
    constexpr quint16 MAX_MESSAGE_LENGTH      = 300;
    constexpr quint8  MAX_CLIENT_NAME_LENGTH  = 32;
    constexpr quint8  MAX_TOPIC_NAME_LENGTH   = 150;
    constexpr int     MESSAGING_COOLDOWN      = 15;  // sec.
    constexpr int     TOPIC_CREATING_COOLDOWN = 180; // sec.
    constexpr char	  SEPARATING_CH           = '\1';
}

#endif // VKKILLER_SERVER_CONSTANTS_H
