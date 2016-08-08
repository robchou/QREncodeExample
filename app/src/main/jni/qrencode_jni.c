#define LOG_TAG "QREncode"

#include <string.h>
#include <jni.h>
#include <android/bitmap.h>

#include "log.h"
#include "libqrencode/qrencode.h"

//异常返回代码
const int kError = -1;
//无异常返回代码
const int kOk = 0;

QRcode *g_pQRCode = NULL;
//二维码周围的空白像素数
const int kMargin = 2;
//存储每个每个像素所需的字节数
const int kBytesPerPixel = 4;
//二维码背景色为白色
const unsigned char kBackgroundPixelColor[4] = { 0xff, 0xff, 0xff, 0xff };
//二维码前景色为黑色
const unsigned char kForegroundPixelColor[4] = { 0, 0, 0, 0xff };

JNIEXPORT jint JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeInit(JNIEnv *env, jobject instance,
                                                      jstring content_) {
    const char *content = (*env)->GetStringUTFChars(env, content_, 0);
    g_pQRCode = QRcode_encodeString(content, 0, QR_ECLEVEL_H, QR_MODE_8, 1);
    (*env)->ReleaseStringUTFChars(env, content_, content);

    if (g_pQRCode != NULL) {
        return g_pQRCode->width + kMargin * 2;
    } else {
        LOGE("QRCode_encodeString failed!");
        return kError;
    }
}

JNIEXPORT jint JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeCreateQRCodeBitmap(JNIEnv *env, jobject instance,
                                                                    jobject bitmap) {
    LOGE("gQRCode size: %d x %d", g_pQRCode->width, g_pQRCode->width);
    LOGE("gQRCode version is: %d", g_pQRCode->version);
    int ret;
    void* pixels;
    AndroidBitmapInfo info;
    unsigned char* p = g_pQRCode->data;

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("error: AndroidBitmap_lockPixels failed! error = %d", ret);
        return kError;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("error: AndroidBitmap_getInfo failed! error = %d", ret);
        return kError;
    }

    if (p == NULL) {
        LOGE("error: Get QRCode data failed!");
    }

    unsigned char *line = (unsigned char*) pixels;
    unsigned char* row = (unsigned char*)malloc(info.width * kBytesPerPixel);
    //初始化Bitmap的pixels为白色
    memset(line, 0xff, info.width * info.height * kBytesPerPixel);

    int x, y;
    //填充二维码数据
    for (y = 0; y < g_pQRCode->width; ++y) {
        //初始化一行的每个像素点为白色
        memset(row, 0xff, info.width * kBytesPerPixel);
        for (x = 0; x < g_pQRCode->width; ++x) {
            if (*p & 1) {
                memcpy(&row[(x + kMargin) * kBytesPerPixel], kForegroundPixelColor, kBytesPerPixel);
            }
            p++;
        }

        memcpy(&line[(y + kMargin) * info.width * kBytesPerPixel], row, info.width * kBytesPerPixel);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    free(row);

    return kOk;
}

JNIEXPORT void JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeFinalize(JNIEnv *env, jobject instance) {
    QRcode_free(g_pQRCode);
}
