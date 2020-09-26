
#include "mqttpersistance.h"
#include <mqtt/client.h>

#include "spdlog/sinks/null_sink.h"
#include "../logger/nolog.h"

//#ifdef LOG_MQTT_PERSISTANCE
//#define LOG spdlog
//#endif
//#ifndef LOG_MQTT_PERSISTANCE
//#define LOG nolog
//#endif


namespace LOG = /*spdlog*/nolog;

MQTTPersistance::MQTTPersistance() : m_open(false) {auto logger = spdlog::create<spdlog::sinks::null_sink_st>("null_logger");}

// "Open" the store
void MQTTPersistance::open(const std::string &clientId,
                           const std::string &serverURI) {
  std::cout << "[Opening persistence store for '" << clientId << "' at '"
            << serverURI << "']" << std::endl;
  m_open = true;
}

// Close the persistent store that was previously opened.
void MQTTPersistance::close() {
     LOG::debug("MQTTPersistance: Closing persistence store");
  m_open = false;
}

// Clears persistence, so that it no longer contains any persisted data.
void MQTTPersistance::clear() {
    LOG::debug("MQTTPersistance: Clearing persistence store");
  m_store.clear();
}

// Returns whether or not data is persisted using the specified key.
bool MQTTPersistance::contains_key(const std::string &key) {
  return m_store.find(key) != m_store.end();
}

// Returns the keys in this persistent data store.
const mqtt::string_collection &MQTTPersistance::keys() const {
  static mqtt::string_collection ks;
  ks.clear();
  for (const auto &k : m_store)
    ks.push_back(k.first);
  return ks;
}

// Puts the specified data into the persistent store.
void MQTTPersistance::put(const std::string &key,
                          const std::vector<mqtt::string_view> &bufs) {
  LOG::debug("MQTTPersistance: Persisting data with key {0}", key);
  std::string str;
  for (const auto &b : bufs)
    str += b.str();
  m_store[key] = std::move(str);
}

// Gets the specified data out of the persistent store.
mqtt::string_view MQTTPersistance::get(const std::string &key) const {
    LOG::debug("MQTTPersistance: Searching persistence for key {0}", key);
  auto p = m_store.find(key);
  if (p == m_store.end())
    throw mqtt::persistence_exception();
  LOG::debug("MQTTPersistance: Found persistence data for key {0}", key);

  return mqtt::string_view(p->second);
}

// Remove the data for the specified key.
void MQTTPersistance::remove(const std::string &key) {
    LOG::debug("MQTTPersistance: Persistence removing key {0}", key);
  auto p = m_store.find(key);
  if (p == m_store.end())
    throw mqtt::persistence_exception();
  m_store.erase(p);
  LOG::debug("MQTTPersistance: Persistence key removed {0}", key);
}
