#include "mqtthandler.h"
#include "spdlog/spdlog.h"

const auto TIMEOUT = std::chrono::seconds(10);
const int QOS = 1;

MQTTHandler::MQTTHandler(const ClientInfo clientInfo)
    : m_ptrPersist(std::make_shared<MQTTPersistance>()),
      m_client(clientInfo.adress, clientInfo.clientId, m_ptrPersist.get()),
      m_maxRetryAttempts(clientInfo.nOfConnectionRetry) {

  spdlog::set_level(spdlog::level::debug); // Set global log level to debug
  spdlog::debug("Create MQTTHandler");
  m_client.set_callback(*this);

  m_connOpts.set_keep_alive_interval(20);
  m_connOpts.set_clean_session(true);
  m_connOpts.set_user_name(clientInfo.clientId);
 if (clientInfo.pw.is_initialized()) {
     m_connOpts.set_password(clientInfo.pw.get());
 }
  if (clientInfo.thrustStore.is_initialized()) {
    mqtt::ssl_options sslopts;
    sslopts.set_trust_store(clientInfo.thrustStore.get());
    m_connOpts.set_ssl(sslopts);
  }
}

ConnectResponse MQTTHandler::connect() {
  if (m_ptrConnectionTok.is_initialized()) {
    return ConnectResponse::AlreadyConnected;
  }
  m_ptrConnectionTok = m_client.connect(m_connOpts);
  auto timeout = m_ptrConnectionTok.get()->wait_for(TIMEOUT);
  return (timeout == false) ? (ConnectResponse::ConnectionEstablished)
                            : (ConnectResponse::Failed);
}

void MQTTHandler::disconnect() {
  if (not m_ptrConnectionTok.is_initialized()) {
    spdlog::warn("MQTTHandler: Nothing to disconnect. Already disconnected");
    return;
  }
  spdlog::debug("MQTTHandler: Disconnect client");
  auto disconnectToken = m_client.disconnect();
  disconnectToken->wait();
  m_ptrConnectionTok = boost::none;
}

bool MQTTHandler::publish(std::string topic, std::string message,
                          boost::optional<int> optQos) {
  auto connectState = connect();
  if (connectState == ConnectResponse::Failed) {
    spdlog::warn("MQTTHandler: Publish could not connect to Broker!");
    return false;
  }
  spdlog::debug("MQTTHandler: Publish topic [{0}] and message [{1}]!", topic,
                message);
  auto pubmsg = mqtt::make_message(topic, message);
  const auto qos = (optQos.is_initialized() ? optQos.get() : QOS);
  pubmsg->set_qos(qos);
  auto tok = m_client.publish(pubmsg, nullptr, *this);
  tok->wait();
  auto toks = m_client.get_pending_delivery_tokens();
  if (!toks.empty()) {
    spdlog::warn("MQTTHandler: There are pending delivery tokens!");
  }
  if (connectState != ConnectResponse::AlreadyConnected) {
    disconnect();
  }
  return true;
}

bool MQTTHandler::subscribe(std::string topic, boost::optional<int> optQos) {
  auto connectState = connect();
  if (connectState == ConnectResponse::Failed) {
    spdlog::warn("MQTTHandler: Publish could not connect to Broker!");
    return false;
  }
  const auto qos = (optQos.is_initialized() ? optQos.get() : QOS);
  spdlog::debug("MQTTHandler: Subscribe to topic [{0}] with QOS: [{1}]!", topic,
                qos);
  m_client.subscribe(topic, qos, nullptr, *this);
  return true;
}

void MQTTHandler::reconnect() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2500));
  try {
    m_client.connect(m_connOpts, nullptr, *this);
  } catch (const mqtt::exception &exc) {
    spdlog::error("MQTTHandler: {0}", exc.what());
    exit(1);
  }
}

// Re-connection failure
void MQTTHandler::on_failure(const mqtt::token & /*tok*/) {
  spdlog::warn("MQTTHandler: Connection attempt failed");
  m_ptrConnectionTok = boost::none;
  if (++m_nretry > m_maxRetryAttempts)
    exit(1);
  reconnect();
}

void MQTTHandler::on_success(const mqtt::token & /*tok*/) {}

void MQTTHandler::connected(const std::string & /*cause*/) {
  spdlog::debug("MQTTHandler: Connection success");
}

void MQTTHandler::connection_lost(const std::string &cause) {
  spdlog::warn("MQTTHandler: Connection lost");
  m_ptrConnectionTok = boost::none;
  if (!cause.empty()) {
    spdlog::debug("MQTTHandler: Connection lost cause: [{0}]", cause);
  }
  spdlog::warn("MQTTHandler: Reconnecting...");
  reconnect();
}

// Callback for when a message arrives.
void MQTTHandler::message_arrived(mqtt::const_message_ptr msg) {
  spdlog::debug("MQTTHandler: Message arrived. Topic: [{0}], Payload: [{1}]",
                msg->get_topic(), msg->to_string());
}

void MQTTHandler::delivery_complete(mqtt::delivery_token_ptr tok) {
  spdlog::debug("MQTTHandler: Delivery complete for token: [{0}]",
                (tok ? tok->get_message_id() : -1));
}
