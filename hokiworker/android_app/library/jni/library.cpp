#include <libhokiworker/do.hpp>
#include <jni.h>

using namespace std;

extern "C" JNIEXPORT void JNICALL Java_tokyo_caimingyu_hokiworker_library_HKLib_compressEncryptFileToFile(JNIEnv *env, jobject obj, jstring outputPath, jint compressionLevel, jstring key, jstring inputPath)
{
    const char *inputPathStr = env->GetStringUTFChars(inputPath, nullptr);
    const char *outputPathStr = env->GetStringUTFChars(outputPath, nullptr);
    const char *keyStr = env->GetStringUTFChars(key, nullptr);
    int compressionLevelValue = static_cast<int>(compressionLevel);
    const jsize keyLength = env->GetStringUTFLength(key);
    vector<uint8_t> keyData(keyStr, keyStr + keyLength);

    compressEncryptFileToFile(outputPathStr, compressionLevelValue, keyData, inputPathStr);

    env->ReleaseStringUTFChars(key, keyStr);
    env->ReleaseStringUTFChars(inputPath, inputPathStr);
    env->ReleaseStringUTFChars(outputPath, outputPathStr);
}

extern "C" JNIEXPORT void JNICALL Java_tokyo_caimingyu_hokiworker_library_HKLib_decryptDecompressFileToFile(JNIEnv *env, jobject obj, jstring outputPath, jstring key, jstring inputPath)
{
    const char *inputPathStr = env->GetStringUTFChars(inputPath, nullptr);
    const char *outputPathStr = env->GetStringUTFChars(outputPath, nullptr);
    const char *keyStr = env->GetStringUTFChars(key, nullptr);
    const jsize keyLength = env->GetStringUTFLength(key);
    vector<uint8_t> keyData(keyStr, keyStr + keyLength);

    decryptDecompressFileToFile(outputPathStr, keyData, inputPathStr);

    env->ReleaseStringUTFChars(key, keyStr);
    env->ReleaseStringUTFChars(inputPath, inputPathStr);
    env->ReleaseStringUTFChars(outputPath, outputPathStr);
}

extern "C" JNIEXPORT void JNICALL Java_tokyo_caimingyu_hokiworker_library_HKLib_compressEncryptFilesToFile(JNIEnv *env, jobject obj, jstring outputPath, jint compressionLevel, jstring key, jobjectArray inputPaths)
{
    jsize length = env->GetArrayLength(inputPaths);
    std::vector<std::string> inputPathsStr;
    for (int i = 0; i < length; i++)
    {
        jstring stringItem = (jstring)env->GetObjectArrayElement(inputPaths, i);
        const char *rawString = env->GetStringUTFChars(stringItem, 0);
        std::string cppString(rawString);
        env->ReleaseStringUTFChars(stringItem, rawString);
        inputPathsStr.push_back(cppString);
        env->DeleteLocalRef(stringItem);
    }
    const char *outputPathStr = env->GetStringUTFChars(outputPath, nullptr);
    const char *keyStr = env->GetStringUTFChars(key, nullptr);
    int compressionLevelValue = static_cast<int>(compressionLevel);
    const jsize keyLength = env->GetStringUTFLength(key);
    vector<uint8_t> keyData(keyStr, keyStr + keyLength);

    compressEncryptFilesToFile(outputPathStr, compressionLevelValue, keyData, inputPathsStr);

    env->ReleaseStringUTFChars(key, keyStr);
    env->ReleaseStringUTFChars(outputPath, outputPathStr);
}

extern "C" JNIEXPORT void JNICALL Java_tokyo_caimingyu_hokiworker_library_HKLib_decryptDecompressFilesToFile(JNIEnv *env, jobject obj, jstring outputPath, jstring key, jstring inputPath)
{
    const char *inputPathStr = env->GetStringUTFChars(inputPath, nullptr);
    const char *outputPathStr = env->GetStringUTFChars(outputPath, nullptr);
    const char *keyStr = env->GetStringUTFChars(key, nullptr);
    const jsize keyLength = env->GetStringUTFLength(key);
    vector<uint8_t> keyData(keyStr, keyStr + keyLength);

    decryptDecompressFilesToFile(outputPathStr, keyData, inputPathStr);

    env->ReleaseStringUTFChars(key, keyStr);
    env->ReleaseStringUTFChars(inputPath, inputPathStr);
    env->ReleaseStringUTFChars(outputPath, outputPathStr);
}

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    return;
}