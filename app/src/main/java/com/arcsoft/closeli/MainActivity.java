package com.arcsoft.closeli;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;

import com.arcsoft.closeli.qrencode.QREncode;

public class MainActivity extends AppCompatActivity {
    private ImageView mImageView;
    private QREncode mQREncode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mQREncode = new QREncode();
        mImageView = (ImageView) findViewById(R.id.iv_qrcode);
        try {
            mImageView.setImageBitmap(mQREncode.createQRCodeBitmap("Hello World!", 500));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
