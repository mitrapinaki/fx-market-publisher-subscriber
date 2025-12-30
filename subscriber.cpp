#include <iostream>
#include <cstring>
#include <solclient/solClient.h>
#include <solclient/solClientMsg.h>
#include <unistd.h>

// ------------------------------------------------------------
// Event callback
// ------------------------------------------------------------
static void
eventCallback(solClient_opaqueSession_pt,
              solClient_session_eventCallbackInfo_pt info,
              void *) {
    if (!info) {
        std::cout << "[Event] Null event info" << std::endl;
        return;
    }

    const char *evStr = solClient_session_eventToString(info->sessionEvent);
    std::cout << "[Event] " << (evStr ? evStr : "Unknown event") << std::endl;
}

// ------------------------------------------------------------
// Message callback
// ------------------------------------------------------------
static solClient_rxMsgCallback_returnCode_t
messageCallback(solClient_opaqueSession_pt,
                solClient_opaqueMsg_pt msg,
                void *) {

    std::cout << "[Callback] messageCallback fired" << std::endl;

    if (!msg) {
        std::cout << "[Message] Null message pointer" << std::endl;
        return SOLCLIENT_CALLBACK_OK;
    }

    void *payload = nullptr;
    solClient_uint32_t size = 0;

    solClient_returnCode_t rc =
        solClient_msg_getBinaryAttachmentPtr(msg, &payload, &size);

    if (rc != SOLCLIENT_OK) {
        std::cout << "[Message] getBinaryAttachmentPtr rc = " << rc << std::endl;
        return SOLCLIENT_CALLBACK_OK;
    }

    if (payload && size > 0) {
        std::string text(static_cast<char*>(payload), size);
        std::cout << "[Message] Received: " << text << std::endl;
    } else {
        std::cout << "[Message] Empty payload" << std::endl;
    }

    return SOLCLIENT_CALLBACK_OK;
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main() {
    solClient_returnCode_t rc;

    std::cout << "Initializing Solace API..." << std::endl;
    rc = solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, nullptr);
    std::cout << "solClient_initialize rc = " << rc << std::endl;

    // --------------------------------------------------------
    // Create context WITH internal thread
    // --------------------------------------------------------
    solClient_context_createFuncInfo_t ctxInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;
    solClient_opaqueContext_pt context = nullptr;

    rc = solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                  &context,
                                  &ctxInfo,
                                  sizeof(ctxInfo));

    std::cout << "context_create rc = " << rc << std::endl;

    if (rc != SOLCLIENT_OK || !context) {
        std::cerr << "Failed to create context" << std::endl;
        return 1;
    }

    // --------------------------------------------------------
    // Session properties
    // --------------------------------------------------------
    const char *sessionProps[] = {
        SOLCLIENT_SESSION_PROP_HOST,      "tcp://localhost:55556",
        SOLCLIENT_SESSION_PROP_VPN_NAME,  "default",
        SOLCLIENT_SESSION_PROP_USERNAME,  "admin",
        SOLCLIENT_SESSION_PROP_PASSWORD,  "admin",
        NULL
    };

    // --------------------------------------------------------
    // Session creation info
    // --------------------------------------------------------
    solClient_session_createFuncInfo_t sessInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;

    sessInfo.rxMsgInfo.callback_p = messageCallback;
    sessInfo.eventInfo.callback_p = eventCallback;

    solClient_opaqueSession_pt session = nullptr;

    rc = solClient_session_create(
        (solClient_propertyArray_pt)sessionProps,
        context,
        &session,
        &sessInfo,
        sizeof(sessInfo)
    );

    std::cout << "session_create rc = " << rc << std::endl;
    std::cout << "session pointer = " << session << std::endl;

    if (rc != SOLCLIENT_OK || session == nullptr) {
        std::cerr << "Session creation failed" << std::endl;
        return 1;
    }

    // --------------------------------------------------------
    // Connect session
    // --------------------------------------------------------
    rc = solClient_session_connect(session);
    std::cout << "session_connect rc = " << rc << std::endl;

    if (rc != SOLCLIENT_OK) {
        std::cerr << "Failed to connect session" << std::endl;
        return 1;
    }

    // --------------------------------------------------------
    // Subscribe
    // --------------------------------------------------------
    //Topic Subscription
    rc = solClient_session_topicSubscribe(session, "fx/rates/normalized");
    //Queue Subscription not supported in 7.3.x only after 8.0
    //solClient_session_queueSubscribe(session, "fx.normalized.q", SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM);


    std::cout << "topicSubscribe rc = " << rc << std::endl;

    if (rc != SOLCLIENT_OK) {
        std::cerr << "Subscription failed" << std::endl;
        return 1;
    }

    std::cout << "Listening on topic fx/rates/normalized..." << std::endl;

    // --------------------------------------------------------
    // No event loop needed — internal thread handles everything
    // --------------------------------------------------------
    while (true) {
        sleep(1);
    }

    return 0;
}
