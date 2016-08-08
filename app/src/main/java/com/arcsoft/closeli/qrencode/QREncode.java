package com.arcsoft.closeli.qrencode;

import android.graphics.Bitmap;

/**
 * Created by robin on 8/8/16.
 */
public class QREncode {
    public QREncode() {
    }

    public Bitmap createQRCodeBitmap(String content, int width) throws Exception {
        int origWidth = nativeInit(content);
        if (origWidth < 0) {
            throw new Exception("Can't get QRCode original width");
        }

        Bitmap origBitmap = Bitmap.createBitmap(origWidth, origWidth, Bitmap.Config.ARGB_8888);
        int ret = nativeCreateQRCodeBitmap(origBitmap);
        if (ret < 0) {
            throw new Exception("Can't get QRCode original bitmap");
        }
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(origBitmap, width, width, false);

        nativeFinalize();

        return scaledBitmap;
    }

    private native int nativeInit(String content);
    private native int nativeCreateQRCodeBitmap(Bitmap bitmap);
    private native void nativeFinalize();

    static {
        System.loadLibrary("qrencode");
    }
}
