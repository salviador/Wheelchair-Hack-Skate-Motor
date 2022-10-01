package com.example.wheel;

import android.view.View;

import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.wheel.R;
import com.example.wheel.Device;



public class ScanBLEAdapter extends RecyclerView.Adapter<ScanBLEAdapter.ViewHoler> {

    private OnItemClickListener onItemClickListener;

    private List<Device> devices;

    public ScanBLEAdapter (List<Device> d){
        devices = d;
    }

    public void clearDevice() {
        devices.clear();
    }


    @Override
    public int getItemCount() {
        return devices.size();
    }

    @Override
    public long getItemId(int position) {
        return devices != null ? devices.size() : 0;
    }

    @NonNull
    @Override
    public ViewHoler onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.recycler_device,parent,false);
        return new ViewHoler(view, onItemClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHoler holder, int position) {
        holder.device = devices.get(position);
        holder.deviceNAME.setText(holder.device.getName());
        holder.deviceADDRESS.setText(holder.device.getAddress());
        //final int rssiPercent = (int) (100.0f * (127.0f + holder.device.getRssi()) / (127.0f + 20.0f));
        holder.deviceRSSI.setText(String.valueOf(holder.device.getRssi()) + " dBm");
    }





    public class ViewHoler extends RecyclerView.ViewHolder{
        public View parentView;
        public TextView deviceNAME;
        public TextView deviceADDRESS;
        public TextView deviceRSSI;
        public Device device;

        public ViewHoler(@NonNull View itemView, OnItemClickListener listener) {
            super(itemView);
            parentView = itemView;
            deviceNAME = (TextView) itemView.findViewById(R.id.device_name);
            deviceADDRESS = (TextView) itemView.findViewById(R.id.device_address);
            deviceRSSI = (TextView) itemView.findViewById(R.id.textView_rssi);

            itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(listener != null) {
                        Device _d = devices.get(getAdapterPosition());
                        listener.onItemClick(v, getAdapterPosition(), _d); //OnItemClickListener mItemClickListener;
                    }
                }
            });
        }
    }


    //CallBack

    public void SetOnItemClickListener(OnItemClickListener mItemClickListener) {
        this.onItemClickListener = mItemClickListener;
    }

    public interface OnItemClickListener {
        public void onItemClick(View view, int position, Device device);
    }

}
