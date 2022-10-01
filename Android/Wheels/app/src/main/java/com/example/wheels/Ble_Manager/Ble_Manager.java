package com.example.wheels.Ble_Manager;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import java.util.UUID;

import no.nordicsemi.android.ble.callback.DataReceivedCallback;
import no.nordicsemi.android.ble.data.Data;
import no.nordicsemi.android.ble.livedata.ObservableBleManager;

public class Ble_Manager extends ObservableBleManager {
    /** Nordic Blinky Service UUID. */
    public final static UUID LBS_UUID_SERVICE = UUID.fromString("da343711-d712-3230-8842-f9dea033233d");
    /** BUTTON characteristic UUID. */
    //private final static UUID LBS_UUID_BUTTON_CHAR = UUID.fromString("00001524-1212-efde-1523-785feabcd128");
    /** LED characteristic UUID. */
    //private final static UUID LBS_UUID_LED_CHAR = UUID.fromString("00001525-1212-efde-1523-785feabcd128");
    /** TELEMETRIA characteristic UUID. */
    private final static UUID LBS_UUID_TELEMETRIA_CHAR = UUID.fromString("da343711-d712-3230-8842-f6dea033233d");
    /** SLEEP characteristic UUID. */
    //private final static UUID LBS_UUID_SLEEP_CHAR = UUID.fromString("00001527-1212-efde-1523-785feabcd128");

    // Client characteristics
    //private BluetoothGattCharacteristic buttonCharacteristic, ledCharacteristic, batteryCharacteristic, buttonsleepCharacteristic;
    private BluetoothGattCharacteristic telemetriaCharacteristic;


    //Callback
    //private ButtonCallback buttonCallback;
    //private LedCallback ledCallback;
    private TelemetriaCallback telemetriaCallback;
    //private ButtonSleepCallback ButtonSleepCallback;

    //Live Data per OBSERVER
/*
    private final MutableLiveData<Boolean> buttonState = new MutableLiveData<>();
    public final LiveData<Boolean> getButtonState() {
        return buttonState;
    }
    private final MutableLiveData<Boolean> ledState = new MutableLiveData<>();
    public final LiveData<Boolean> getLedState() {
        return ledState;
    }
 */
    private final MutableLiveData<byte[]> TelemetriaState = new MutableLiveData<>();
    public final LiveData<byte[]> getTelemetriaState() {
        return TelemetriaState;
    }
    /*
        private final MutableLiveData<Boolean> buttonSleeptate = new MutableLiveData<>();
        public final LiveData<Boolean> getbuttonSleepState() {
            return buttonSleeptate;
        }
    */
    private final MutableLiveData<Boolean> ConnectionStateGATT = new MutableLiveData<>();
    public final LiveData<Boolean> getConnectionStateGATT() {return ConnectionStateGATT;}

    //General
    private boolean supported;
    public boolean connessioneGATT = false;


    public Ble_Manager(Context context) {
        super(context);
        //buttonCallback = new ButtonCallback();
        //ledCallback = new LedCallback();
        telemetriaCallback = new TelemetriaCallback();
        //ButtonSleepCallback = new ButtonSleepCallback();
    }


    @NonNull
    @Override
    protected BleManagerGattCallback getGattCallback() {
        return new ManagerGattCallback();
    }

    @Override
    public void log(int priority, @NonNull String message) {
        super.log(priority, message);
        Log.d("MyBleManager", message);
    }









    /**
     * BluetoothGatt callbacks object.
     */
    private class ManagerGattCallback extends BleManagerGattCallback{

        @Override
        protected boolean isRequiredServiceSupported(@NonNull BluetoothGatt gatt) {
            final BluetoothGattService service = gatt.getService(LBS_UUID_SERVICE);
            if (service != null) {
                //buttonCharacteristic = service.getCharacteristic(LBS_UUID_BUTTON_CHAR);
                //ledCharacteristic = service.getCharacteristic(LBS_UUID_LED_CHAR);
                telemetriaCharacteristic = service.getCharacteristic(LBS_UUID_TELEMETRIA_CHAR);
                //buttonsleepCharacteristic =service.getCharacteristic(LBS_UUID_SLEEP_CHAR);
            }

            // Validate properties
            /*
            boolean notify = false;
            if (buttonCharacteristic != null) {
                final int properties = buttonCharacteristic.getProperties();
                notify = (properties & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0;
            }

            boolean writeRequest = false;
            if (ledCharacteristic != null) {
                final int rxProperties = ledCharacteristic.getProperties();
                writeRequest = (rxProperties & BluetoothGattCharacteristic.PROPERTY_WRITE) != 0;
                ledCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            }
            */
            boolean notifyTelemetria = false;
            if (telemetriaCharacteristic != null) {
                final int properties = telemetriaCharacteristic.getProperties();
                notifyTelemetria = (properties & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0;
            }
            /*
            boolean writeRequestsleep = false;
            if (buttonsleepCharacteristic != null) {
                final int rxProperties = buttonsleepCharacteristic.getProperties();
                writeRequestsleep = (rxProperties & BluetoothGattCharacteristic.PROPERTY_WRITE) != 0;
                buttonsleepCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            }
            */
            // Return true if all required services have been found
            //supported = buttonCharacteristic != null && ledCharacteristic != null && buttonsleepCharacteristic != null && writeRequestsleep && writeRequest && notify && notifyBattery;
            supported =  telemetriaCharacteristic != null  && notifyTelemetria;
            return supported;
        }

        @Override
        protected boolean isOptionalServiceSupported(@NonNull BluetoothGatt gatt) {
            return super.isOptionalServiceSupported(gatt);
        }

        @Override
        protected void initialize() {
            //setNotificationCallback(buttonCharacteristic).with(buttonCallback);
            //readCharacteristic(ledCharacteristic).with(ledCallback).enqueue();
            //readCharacteristic(buttonCharacteristic).with(buttonCallback).enqueue();

            setNotificationCallback(telemetriaCharacteristic).with(telemetriaCallback);
            readCharacteristic(telemetriaCharacteristic).with(telemetriaCallback).enqueue();

            //enableNotifications(buttonCharacteristic).enqueue();
            enableNotifications(telemetriaCharacteristic).enqueue();

            //readCharacteristic(buttonsleepCharacteristic).with(ButtonSleepCallback).enqueue();

        }

        @Override
        protected void onDeviceDisconnected() {
            //buttonCharacteristic = null;
            //ledCharacteristic = null;
            telemetriaCharacteristic = null;
            //buttonsleepCharacteristic = null;
            connessioneGATT = false;
            ConnectionStateGATT.postValue(false);
        }

        @Override
        protected void onDeviceReady() {
            super.onDeviceReady();
            connessioneGATT = true;
            ConnectionStateGATT.postValue(true);

        }
    }

    @Override
    protected boolean shouldClearCacheWhenDisconnected() {
        return !supported;
    }












//Implementazione CALLBACK caratteristich BLE recive



    private class TelemetriaCallback implements DataReceivedCallback {
        @Override
        public void onDataReceived(@NonNull BluetoothDevice device, @NonNull Data data) {
            if (data.size() == 0) {
                return;
            }
            //final int state = data.getIntValue(Data.FORMAT_UINT8, 0);
            //float v_battery = (float) ((float) state / 10.0);
            byte[] dataraw = data.getValue();

            //Log.d("BLE MANAGER", "Battery: " + v_battery);
            Log.d("BLE MANAGER", "Battery Charatteristic RECEIVE ***********");
            TelemetriaState.postValue(dataraw);
        }
    }

    /*


    private class ButtonCallback implements DataReceivedCallback{
        private static final int STATE_RELEASED = 0x00;
        private static final int STATE_PRESSED = 0x01;

        @Override
        public void onDataReceived(@NonNull BluetoothDevice device, @NonNull Data data) {
            if (data.size() != 1) {
                return;
            }
            final int state = data.getIntValue(Data.FORMAT_UINT8, 0);
            if (state == STATE_PRESSED) {
                Log.d("BLE MANAGER", "Button: " + state);
                buttonState.postValue(true);
            } else if (state == STATE_RELEASED) {
                Log.d("BLE MANAGER", "Button: " + state);
                buttonState.postValue(false);
            } else {
            }
        }
    }


    private class ButtonSleepCallback implements DataReceivedCallback, DataSentCallback {
        private static final byte STATE_OFF = 0x00;
        private static final byte STATE_ON = 0x01;

        @Override
        public void onDataReceived(@NonNull BluetoothDevice device, @NonNull Data data) {
            parse(device,data);
        }

        @Override
        public void onDataSent(@NonNull BluetoothDevice device, @NonNull Data data) {
            parse(device, data);
        }

        private void parse(@NonNull final BluetoothDevice device, @NonNull final Data data) {
            if (data.size() != 1) {
                return;
            }

            final int state = data.getIntValue(Data.FORMAT_UINT8, 0);
            if (state == STATE_ON) {
                ledState.postValue(true);
            } else if (state == STATE_OFF) {
                ledState.postValue(false);
            } else {
            }
        }
    }

    private class LedCallback implements DataReceivedCallback, DataSentCallback {
        private static final byte STATE_OFF = 0x00;
        private static final byte STATE_ON = 0x01;

        @Override
        public void onDataReceived(@NonNull BluetoothDevice device, @NonNull Data data) {
            parse(device,data);
        }

        @Override
        public void onDataSent(@NonNull BluetoothDevice device, @NonNull Data data) {
            parse(device, data);
        }

        private void parse(@NonNull final BluetoothDevice device, @NonNull final Data data) {
            if (data.size() != 1) {
                return;
            }

            final int state = data.getIntValue(Data.FORMAT_UINT8, 0);
            if (state == STATE_ON) {
                ledState.setValue(true);
            } else if (state == STATE_OFF) {
                ledState.setValue(false);
            } else {
            }
        }
    }


    //Implementazione BLE caratteristic trasmit

    public void turnLed(final boolean on) {
        final byte STATE_OFF = 0x00;
        final byte STATE_ON = 0x01;

        // Are we connected?
        if (ledCharacteristic == null)
            return;

        Log.d("BLE MANAGER", "LED: ");

        if(on){
            writeCharacteristic(ledCharacteristic,Data.opCode(STATE_ON)).with(ledCallback).enqueue();
        }else {
            writeCharacteristic(ledCharacteristic,Data.opCode(STATE_OFF)).with(ledCallback).enqueue();
        }
    }

    public void Button_Sleep_SEND(final boolean on) {
        final byte STATE_OFF = 0x00;
        final byte STATE_ON = 0x01;

        // Are we connected?
        if (buttonsleepCharacteristic == null)
            return;

        Log.d("BLE MANAGER", "buttonsleepCharacteristic: ");

        if(on){
            writeCharacteristic(buttonsleepCharacteristic,Data.opCode(STATE_ON)).with(ButtonSleepCallback).enqueue();
        }else {
            writeCharacteristic(buttonsleepCharacteristic,Data.opCode(STATE_OFF)).with(ButtonSleepCallback).enqueue();
        }
    }
*/
}
