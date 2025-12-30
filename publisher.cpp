#include <iostream>
#include <string>
#include <solclient/solClient.h>
#include <solclient/solClientMsg.h>

static solClient_rxMsgCallback_returnCode_t
dummyRxCallback(solClient_opaqueSession_pt,
                solClient_opaqueMsg_pt,
                void *) {
    return SOLCLIENT_CALLBACK_OK;
}

static void
dummyEventCallback(solClient_opaqueSession_pt,
                   solClient_session_eventCallbackInfo_pt,
                   void *) {
    // No-op
}


int main() {
    solClient_returnCode_t rc;

    // Initialize API
    rc = solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, nullptr);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "solClient_initialize failed: " << rc << std::endl;
        return 1;
    }

    // Create context
    solClient_context_createFuncInfo_t ctxInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;
    solClient_opaqueContext_pt context = nullptr;

    rc = solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                  &context,
                                  &ctxInfo,
                                  sizeof(ctxInfo));
    if (rc != SOLCLIENT_OK) {
        std::cerr << "solClient_context_create failed: " << rc << std::endl;
        return 1;
    }

    // Session properties (NULL-terminated)
    const char *sessionProps[] = {
        SOLCLIENT_SESSION_PROP_HOST,      "tcp://localhost:55556",
        SOLCLIENT_SESSION_PROP_VPN_NAME,  "default",
        SOLCLIENT_SESSION_PROP_USERNAME,  "admin",
        SOLCLIENT_SESSION_PROP_PASSWORD,  "admin",
        NULL
    };

    // Session create info
    solClient_session_createFuncInfo_t sessInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;
    sessInfo.rxMsgInfo.callback_p = dummyRxCallback; 
    sessInfo.eventInfo.callback_p = dummyEventCallback;

    solClient_opaqueSession_pt session = nullptr;

    rc = solClient_session_create((solClient_propertyArray_pt)sessionProps,
                                  context,
                                  &session,
                                  &sessInfo,
                                  sizeof(sessInfo));
    if (rc != SOLCLIENT_OK) {
        std::cerr << "solClient_session_create failed: " << rc << std::endl;
        return 1;
    }

    rc = solClient_session_connect(session);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "solClient_session_connect failed: " << rc << std::endl;
        return 1;
    }

    std::cout << "Connected. Publishing messages..." << std::endl;

    // Create a message
    solClient_opaqueMsg_pt msg = nullptr;
    solClient_msg_alloc(&msg);

    // Set destination topic
    solClient_destination_t dest;
    dest.destType = SOLCLIENT_TOPIC_DESTINATION;
    dest.dest = (char *)"fx/rates/normalized";

    solClient_msg_setDestination(msg, &dest, sizeof(dest));

    // Payload
    std::string payload = "EURUSD=1.1034";

    solClient_msg_setBinaryAttachment(msg,
                                      payload.data(),
                                      payload.size());

    // Send
    rc = solClient_session_sendMsg(session, msg);
    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to send message: " << rc << std::endl;
    } else {
        std::cout << "Message sent: " << payload << std::endl;
    }

    // Cleanup
    solClient_msg_free(&msg);
    solClient_cleanup();

    return 0;
}
