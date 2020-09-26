add_library(ownMqtt mqtthelper.cpp
                    mqtthelper.h
                    mqtthandler.cpp
                    mqtthandler.h
                    mqttpersistance.cpp
                    mqttpersistance.h)

add_library(src::mqtt ALIAS ownMqtt)
target_link_libraries(ownMqtt -lpaho-mqttpp3 -lpaho-mqtt3as)
target_include_directories(ownMqtt INTERFACE ../cppUtils)
target_link_libraries(ownMqtt PRIVATE spdlog::spdlog)
target_include_directories(ownMqtt INTERFACE logger::nolog)
