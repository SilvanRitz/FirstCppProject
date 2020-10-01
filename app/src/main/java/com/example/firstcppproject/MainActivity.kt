package com.example.firstcppproject

import android.graphics.Color
import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    val ColorConnected = Color.parseColor("#1D9922")
    val ColorDisconnected = Color.parseColor("#F44336")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val mqttHelper = MqttClientHelper(applicationContext, {
            val status_connected = findViewById(R.id.status_connected) as Button
            status_connected.setBackgroundColor(ColorConnected)
            status_connected.setText("Connected")
        }, {
            val status_connected = findViewById(R.id.status_connected) as Button
            status_connected.setBackgroundColor(ColorDisconnected)
            status_connected.setText("Disconnected")
        })
        val lightControl = MqttTfRemoteHelper(mqttHelper)


        val button_workplace_on = findViewById(R.id.button_workplace_on) as Button
        button_workplace_on.setOnClickListener {
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.WorkPlace, true)
        }
        val button_workplace_off = findViewById(R.id.button_workplace_off) as Button
        button_workplace_off.setOnClickListener {
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.WorkPlace, false)
        }

        val button_couch_on = findViewById(R.id.button_couch_on) as Button
        button_couch_on.setOnClickListener {
//                Toast.makeText(this@MainActivity, "You clicked me.", Toast.LENGTH_SHORT).show()
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.AboveCouch, true)
        }
        val button_couch_off = findViewById(R.id.button_couch_off) as Button
        button_couch_off.setOnClickListener {
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.AboveCouch, false)
        }

        val button_living_room_on = findViewById(R.id.button_living_room_on) as Button
        button_living_room_on.setOnClickListener {
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.LivingRoom, true)

        }
        val button_living_room_off = findViewById(R.id.button_living_room_off) as Button
        button_living_room_off.setOnClickListener {
            lightControl.switchLightState(MqttTfRemoteHelper.LightPlace.LivingRoom, false)
        }

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }

        const val TAG = "AndroidMqttClient"
    }

}
