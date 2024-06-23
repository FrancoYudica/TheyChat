#ifndef __APP_STATES_H__
#define __APP_STATES_H__

/// @brief Possible states of the application
typedef enum {

    /// @brief Not set and invalid state
    APP_STATE_NULL,

    /// @brief Handles and prints the last ocurred error
    APP_STATE_ERROR,

    /// @brief Client haven't established any connection
    APP_STATE_OFFLINE,

    /// @brief Establishes connection with server
    APP_STATE_CONNECT,

    /// @brief Server max capacity reached, client in queue
    APP_STATE_QUEUE,

    /// @brief Asks user for valid credentials
    APP_STATE_LOGIN,

    /// @brief Allows client communication
    APP_STATE_CHAT,

    /// @brief Clean disconnect of the client
    APP_STATE_DISCONNECT
} AppState;

#endif