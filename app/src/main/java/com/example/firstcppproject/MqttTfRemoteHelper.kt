package com.example.firstcppproject

import android.widget.Toast

class MqttTfRemoteHelper(_mqttHelper : MqttClientHelper) {

    val mqttHelper = _mqttHelper

    enum class LightPlace(val id: String) {WorkPlace("15978"), AboveCouch("15977"), LivingRoom("15976")}

    public fun switchLightState(lightPlace : LightPlace, switchOn : Boolean){
        val topic = "tinkerforge/request/remote_switch_bricklet/v1g/switch_socket_b"
        val switchTo = if(switchOn)  "\"on\"" else "\"off\""
        val msg = "{\"address\": " + lightPlace.id +", \"switch_to\": "+switchTo+", \"unit\":0}"
        mqttHelper.publish(topic,msg, qos=1)
    }

}