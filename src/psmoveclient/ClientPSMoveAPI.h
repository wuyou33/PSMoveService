#ifndef CLIENT_PSMOVE_API_H
#define CLIENT_PSMOVE_API_H

//-- includes -----
#include "ClientConfig.h"
#include "ClientLog.h"

#ifdef HAS_PROTOCOL_ACCESS
#include "PSMoveProtocol.pb.h"
#endif // HAS_PROTOCOL_ACCESS


//-- pre-declarations -----
class ClientControllerView;
class ClientHMDView;

//-- macros -----
#ifdef HAS_PROTOCOL_ACCESS
#define GET_PSMOVEPROTOCOL_RESPONSE(handle) \
    reinterpret_cast<const PSMoveProtocol::Response *>(handle)
#define GET_PSMOVEPROTOCOL_EVENT(handle) \
    reinterpret_cast<const PSMoveProtocol::Response *>(handle) // events are a special case of responses
#endif // HAS_PROTOCOL_ACCESS

//-- interface -----
class CLIENTPSMOVEAPI ClientPSMoveAPI
{
public:
    enum eClientAPIConstants
    {
        INVALID_REQUEST_ID= -1
    };

    enum eClientPSMoveAPIEvent
    {
        // Client Events
        connectedToService,
        failedToConnectToService,
        disconnectedFromService,

        // Service Events
        opaqueServiceEvent, // Need to have protocol access to see what kind of event this is
        controllerListUpdated,
        trackerListUpdated,
    };

    enum eClientPSMoveResultCode
    {
        _clientPSMoveResultCode_ok,
        _clientPSMoveResultCode_error,
        _clientPSMoveResultCode_canceled
    };

    enum eMessagePayloadType
    {
        _messagePayloadType_Event,
        _messagePayloadType_Response,

        _messagePayloadType_Count
    };

    typedef const void *t_event_data_handle;
    struct EventMessage
    {
        eClientPSMoveAPIEvent event_type;
        t_event_data_handle event_data_handle;
    };

    typedef int t_request_id;
    typedef const void*t_response_handle;
    typedef void *t_request_handle;
    struct ResponseMessage
    {
        t_request_id request_id;
        t_response_handle response_handle;
        eClientPSMoveResultCode result_code;
    };

    struct Message
    {
        union{
            EventMessage event_data;
            ResponseMessage response_data;
        };
        eMessagePayloadType payload_type;
    };

    static bool startup(
        const std::string &host,
        const std::string &port,
        e_log_severity_level log_level = _log_severity_level_info);
    
    /**< Process incoming/outgoing networking requests via the network manager. */
    static void update();  

    /**< Poll the next message from the service in the queue */
    static bool poll_next_message(Message *message, size_t message_size);

    static void shutdown();

    static bool has_started();

    static ClientControllerView *allocate_controller_view(int ControllerID);
    static void free_controller_view(ClientControllerView *view);

    enum eControllerDataStreamFlags
    {
        defaultStreamOptions= 0x00,
        includeRawSensorData= 0x01
    };
    static t_request_id start_controller_data_stream(ClientControllerView *view, unsigned int flags);
    static t_request_id stop_controller_data_stream(ClientControllerView *view);
    static t_request_id set_controller_rumble(ClientControllerView *view, float rumble_amount);
    static t_request_id set_led_color(ClientControllerView *view, unsigned char r, unsigned char g, unsigned b);
    static t_request_id reset_pose(ClientControllerView *view);

    /// Used to send requests to the server by clients that have protocol access
    static t_request_id send_opaque_request(t_request_handle request_handle);

private:
    // Not allowed to instantiate
    ClientPSMoveAPI();

    // Singleton private implementation - same lifetime as the ClientPSMoveAPI
    static class ClientPSMoveAPIImpl *m_implementation_ptr;
};

#endif // CLIENT_PSMOVE_API_H