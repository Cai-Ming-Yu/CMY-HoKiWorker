package tokyo.caimingyu.hokiworker.library;

import androidx.annotation.NonNull;

public class HKLib {
    static {
        System.loadLibrary("hokiworker");
    }

    public static native void compressEncryptFileToFile(@NonNull String outputPath, @NonNull int compressionLevel, @NonNull String key, @NonNull String inputPath);

    public static native void decryptDecompressFileToFile(@NonNull String outputPath, @NonNull String key, @NonNull String inputPath);

    public static native void compressEncryptFilesToFile(@NonNull String outputPath, @NonNull int compressionLevel, @NonNull String key, @NonNull String[] inputPaths);

    public static native void decryptDecompressFilesToFile(@NonNull String outputPath, @NonNull String key, @NonNull String inputPath);
}