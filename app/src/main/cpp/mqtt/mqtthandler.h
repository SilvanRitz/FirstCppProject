#pragma once

#include "mqttpersistance.h"
#include <atomic>
#include <boost/optional.hpp>
#include <mqtt/async_client.h>

enum class ConnectResponse{
    ConnectionEstablished,
    AlreadyConnected,
    Failed,
};

const std::string DEFAULT_SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string DEFAULT_CLIENT_ID("async_subcribe_cpp");
const int DEFAULT_N_RETRY_ATTEMPTS = 5;


struct ClientInfo{
    std::string adress = DEFAULT_SERVER_ADDRESS;
    int nOfConnectionRetry = DEFAULT_N_RETRY_ATTEMPTS;
    std::string clientId = DEFAULT_CLIENT_ID;
        boost::optional <std::string> pw;
        boost::optional<std::string> thrustStore;
};


PTR_TYPEDEFS(MQTTHandler);
class MQTTHandler : public virtual mqtt::callback,
                    public virtual mqtt::iaction_listener {
public:
  MQTTHandler(const ClientInfo clientInfo);
  bool publish(std::string topic, std::string message,
               boost::optional<int> optQos);
  bool subscribe(std::string topic, boost::optional<int> optQos);
  ConnectResponse connect();
  void disconnect();

  void reconnect();

  // Re-connection failure
  void on_failure(const mqtt::token &tok) override;

  // (Re)connection success
  // Either this or connected() can be used for callbacks.
  void on_success(const mqtt::token &tok) override;

  // (Re)connection success
  void connected(const std::string &cause) override;

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string &cause) override;

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override;

  void delivery_complete(mqtt::delivery_token_ptr token) override;

private:
  MQTTPersistancePtr m_ptrPersist;
  mqtt::async_client m_client;
  mqtt::connect_options m_connOpts;
  callback m_cb;
  boost::optional<mqtt::token_ptr> m_ptrConnectionTok;

  int m_nretry;
  const int m_maxRetryAttempts;
};
