# CMY-HoKiWorker
コマンドライン/JNIでの圧縮/解凍、暗号化/復号化。

Through C++, cpp-base64, miniz, tiny-AES-c, and zstd are encapsulated into a single encryption and decryption tool, which supports encryption and decryption of multiple files or a single file, and has been used by the command line and JNI.

The file can be saved through base64, but due to the complexity of the code, there is only relevant code in the file ```do.hpp```, and it is not implemented in the command line and JNI.

At present, it can be used through NDK to build the command line version, and to build the Android software version through Gradle, due to the many features, no more description, please check the source code by yourself, if you are interested in enriching the functions or documentation of this project, please use ```PR```.

## [License](https://github.com/Cai-Ming-Yu/CMY-HoKiWorker/blob/C-M-Y/LICENSE)