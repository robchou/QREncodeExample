package com.arcsoft.closeli.qrencode;

import android.graphics.Bitmap;

/**
 * 通过libqrencode生成二维码
 */
public class QREncode {

    /**
     * 生成二维码
     * @param content 需要生成的内容
     * @param width 二维码宽高
     * @return 包含二维码的Bitmap
     * @throws Exception
     */
    public Bitmap createQRCodeBitmap(String content, int width) throws Exception {
        int origWidth = nativeInit(content);
        if (origWidth < 0) {
            throw new Exception("Can't get original QRCode width");
        }

        Bitmap origBitmap = Bitmap.createBitmap(origWidth, origWidth, Bitmap.Config.ARGB_8888);
        int ret = nativeCreateQRCodeBitmap(origBitmap);
        if (ret < 0) {
            throw new Exception("Can't get original QRCode bitmap");
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
