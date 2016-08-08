#define LOG_TAG "QRCodeWriter"

#include <string.h>
#include <jni.h>
#include <android/bitmap.h>
#include "log.h"
#include "libqrencode/qrencode.h"

QRcode *qrcode;
static const int MARGIN = 2;
static const int BYTES_PER_PIXEL = 4;

const unsigned char bg_color[BYTES_PER_PIXEL] = { 0xff, 0xff, 0xff, 0xff };
const unsigned char fg_color[BYTES_PER_PIXEL] = { 0, 0, 0, 0xff };

JNIEXPORT jstring JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeStringFromJni(JNIEnv *env, jobject instance) {

    return (*env)->NewStringUTF(env, "Hello from JNI");
}

JNIEXPORT void JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeGenColorStripes(JNIEnv *env, jobject instance,
                                                            jobject bitmap) {
    LOGE("generateRGBStripes");
    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    LOGE("Bitmap size is: %d x %d", info.width, info.height);
    LOGE("Bitmap stride is %d", info.stride);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Format is RGBA_8888");
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels failed ! error = %d", ret);
    }

    LOGD("%p", pixels);

    uint32_t* line = (uint32_t*) pixels;

    for (int i = 0; i < info.height / 3; ++i) {
        for (int j = 0; j < info.width; ++j) {
            line[i * info.width + j] = 0xffff0000;
        }
    }

    for (int i = info.height / 3; i < info.height * 2 / 3; ++i) {
        for (int j = 0; j < info.width; ++j) {
            line[i * info.width + j] = 0xff00ff00;
        }
    }

    for (int i = info.height * 2 / 3; i < info.height; ++i) {
        for (int j = 0; j < info.width; ++j) {
            line[i * info.width + j] = 0xff0000ff;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}

JNIEXPORT void JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeCreateQRCodeBitmap(JNIEnv *env, jobject instance,
                                                               jobject bitmap) {
    LOGE("qrcode size: %d x %d", qrcode->width, qrcode->width);
    LOGE("qrcode version is: %d", qrcode->version);
    unsigned char* p = qrcode->data;
    int ret;
    void* pixels;
    AndroidBitmapInfo info;

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels failed ! error = %d", ret);
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo failed ! error = %d", ret);
    }

    unsigned char *line = (unsigned char*) pixels;
    memset(line, 0xff, info.width * info.height * BYTES_PER_PIXEL);
    unsigned char* row = (unsigned char*) malloc(info.width * BYTES_PER_PIXEL);
    memset(row, 0xff, info.width * BYTES_PER_PIXEL);

    /* top margin */
    for (int y = 0; y < MARGIN; ++y) {
        memcpy(&line[y * info.width * BYTES_PER_PIXEL], row, info.width * BYTES_PER_PIXEL);
    }

    for (int y = 0; y < qrcode->width; ++y) {
        memset(row, 0xff, info.width * BYTES_PER_PIXEL);
        for (int x = 0; x < qrcode->width; ++x) {
            if (*p & 1) {
                memcpy(&row[(x + MARGIN) * BYTES_PER_PIXEL], fg_color, BYTES_PER_PIXEL);
            }
            p++;
        }

        memcpy(&line[(y + MARGIN) * info.width * BYTES_PER_PIXEL], row, info.width * BYTES_PER_PIXEL);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    free(row);
}

JNIEXPORT void JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeCreateQRCode(JNIEnv *env, jobject instance,
                                                               jobject bitmap) {
    LOGE("qrcode size: %d x %d", qrcode->width, qrcode->width);
    LOGE("qrcode version is: %d", qrcode->version);
    unsigned char *p = qrcode->data;
    int ret;
    int width = qrcode->width;
    int realwidth;
    void* pixels;
    AndroidBitmapInfo info;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo failed ! error = %d", ret);
        return;
    }

    realwidth = info.width;
    LOGE("realwidth is: %d", realwidth);
    int size = realwidth / width;

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels failed ! error = %d", ret);
    }

    unsigned char *line = (unsigned char*) pixels;
    unsigned char *row = (unsigned char*) malloc(realwidth * BYTES_PER_PIXEL);
    for (int i = 0; i < realwidth; ++i) {
        memcpy(&row[i * BYTES_PER_PIXEL], fg_color, sizeof(fg_color));
    }

    memset(line, 0xff, realwidth * realwidth * BYTES_PER_PIXEL);

    LOGE("width is %d", width);
    LOGE("size is %d", size);
    for (int y = 0; y < width; ++y) {
        memset(row, 0xff, realwidth * BYTES_PER_PIXEL);
        for (int x = 0; x < width; ++x) {
            for (int xx = 0; xx < size; ++xx) {
                if (*p & 1) {
                    memcpy(&row[(x * size + xx) * BYTES_PER_PIXEL], fg_color, BYTES_PER_PIXEL);
                }
            }
            ++p;
        }

        for (int yy = 0; yy < size; ++yy) {
            memcpy(&line[(y * size + yy) * realwidth * BYTES_PER_PIXEL], row, realwidth * BYTES_PER_PIXEL);
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}

JNIEXPORT jint JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeInit(JNIEnv *env, jobject instance, jstring content_) {
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

JNIEXPORT void JNICALL
Java_cn_closeli_qrencode_QRCodeWriter_nativeFinalize(JNIEnv *env, jobject instance) {
    QRcode_free(qrcode);
}

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
    for (int y = 0; y < MARGIN; ++y) {
        memcpy(&line[y * info.width * BYTES_PER_PIXEL], row, info.width * BYTES_PER_PIXEL);
    }

    for (int y = 0; y < qrcode->width; ++y) {
        memset(row, 0xff, info.width * BYTES_PER_PIXEL);
        for (int x = 0; x < qrcode->width; ++x) {
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
