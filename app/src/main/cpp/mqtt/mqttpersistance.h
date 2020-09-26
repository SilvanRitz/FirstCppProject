#pragma once

#include <mqtt/client.h>
#include <map>
#include "../cppUtils/SharedPtrUtils.h"


PTR_TYPEDEFS(MQTTPersistance);
class MQTTPersistance : virtual public mqtt::iclient_persistence
{
public:
    MQTTPersistance();

    // "Open" the store
    void open(const std::string &clientId,
                               const std::string &serverURI) override;

    // Close the persistent store that was previously opened.
    void close() override;

    // Clears persistence, so that it no longer contains any persisted data.
    void clear() override;

    // Returns whether or not data is persisted using the specified key.
    bool contains_key(const std::string &key) override;

    // Returns the keys in this persistent data store.
    const mqtt::string_collection& keys() const override;

    // Puts the specified data into the persistent store.
    void put(const std::string &key,
                              const std::vector<mqtt::string_view> &bufs) override;

    // Gets the specified data out of the persistent store.
    mqtt::string_view get(const std::string &key) const override;

    // Remove the data for the specified key.
    void remove(const std::string &key) override;
private:

    // Whether the store is open
    bool m_open;

    // Use an STL map to store shared persistence pointers
    // against string keys.
    std::map<std::string, std::string> m_store;


};
