#define LOG_TAG "QRCodeWriter"

#include <string.h>
#include <jni.h>
#include <android/bitmap.h>

#include "libqrencode/qrencode.h"
#include "log.h"

QRcode *qrcode;
static const int MARGIN = 2;
static const int BYTES_PER_PIXEL = 4;

const unsigned char bg_color[4] = { 0xff, 0xff, 0xff, 0xff };
const unsigned char fg_color[4] = { 0, 0, 0, 0xff };

JNIEXPORT jint JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeInit(JNIEnv *env, jobject instance,
                                                      jstring content_) {
    const char *content = (*env)->GetStringUTFChars(env, content_, 0);
    qrcode = QRcode_encodeString(content, 0, QR_ECLEVEL_H, QR_MODE_8, 1);
    (*env)->ReleaseStringUTFChars(env, content_, content);

    if (qrcode != NULL) {
        return qrcode->width + MARGIN * 2;
    } else {
        LOGE("QRCode_encodeString failed!");

        return -1;
    }
}

JNIEXPORT jint JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeCreateQRCodeBitmap(JNIEnv *env, jobject instance,
                                                                    jobject bitmap) {
    LOGE("qrcode size: %d x %d", qrcode->width, qrcode->width);
    LOGE("qrcode version is: %d", qrcode->version);
    unsigned char* p = qrcode->data;
    int ret;
    void* pixels;
    AndroidBitmapInfo info;

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels failed ! error = %d", ret);
        return -1;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo failed ! error = %d", ret);
        return -1;
    }

    unsigned char *line = (unsigned char*) pixels;
    memset(line, 0xff, info.width * info.height * BYTES_PER_PIXEL);
    unsigned char* row = (unsigned char*) malloc(info.width * BYTES_PER_PIXEL);
    memset(row, 0xff, info.width * BYTES_PER_PIXEL);

    /* top margin */
    int x, y;
    for (y = 0; y < MARGIN; ++y) {
        memcpy(&line[y * info.width * BYTES_PER_PIXEL], row, info.width * BYTES_PER_PIXEL);
    }

    for (y = 0; y < qrcode->width; ++y) {
        memset(row, 0xff, info.width * BYTES_PER_PIXEL);
        for (x = 0; x < qrcode->width; ++x) {
            if (*p & 1) {
                memcpy(&row[(x + MARGIN) * BYTES_PER_PIXEL], fg_color, BYTES_PER_PIXEL);
            }
            p++;
        }

        memcpy(&line[(y + MARGIN) * info.width * BYTES_PER_PIXEL], row, info.width * BYTES_PER_PIXEL);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    free(row);

    return 0;
}

JNIEXPORT void JNICALL
Java_com_arcsoft_closeli_qrencode_QREncode_nativeFinalize(JNIEnv *env, jobject instance) {
    QRcode_free(qrcode);
}
