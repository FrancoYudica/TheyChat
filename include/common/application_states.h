#ifndef __APP_STATES_H__
#define __APP_STATES_H__

/// @brief Possible states of the application
typedef enum {

    /// @brief Not set and invalid state
    APP_STATE_NULL,

    /// @brief Server accepts client connection after queue
    APP_STATE_CONNECT,

    /// @brief Asks user for valid credentials
    APP_STATE_LOGIN,

    /// @brief Allows client communication
    APP_STATE_CHAT,

    /// @brief Clean disconnect of the client
    APP_STATE_DISCONNECT
} AppState;

#endif