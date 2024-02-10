#pragma once

#include <iostream>
#include <vector>
#include <fstream>

#include <tiny-AES-c/aes.hpp>
#include <cpp-base64/base64.h>
#include <zstd.h>
#include <miniz/miniz.h>

using namespace std;

namespace fs = filesystem;

static string encodeBase64StrToStr(const string &input)
{
    return base64_encode(input);
}

static vector<uint8_t> encodeBase64StrToData(const string &input)
{
    string encodedString = encodeBase64StrToStr(input);
    return vector<uint8_t>(encodedString.begin(), encodedString.end());
}

static string encodeBase64DataToStr(const vector<uint8_t> &data)
{
    return encodeBase64StrToStr(string(data.begin(), data.end()));
}

static vector<uint8_t> encodeBase64DataToData(const vector<uint8_t> &data)
{
    string encodedString = encodeBase64DataToStr(data);
    return vector<uint8_t>(encodedString.begin(), encodedString.end());
}

static string decodeBase64StrToStr(const string &input)
{
    return base64_decode(input);
}

static vector<uint8_t> decodeBase64StrToData(const string &input)
{
    string decodedString = decodeBase64StrToStr(input);
    return vector<uint8_t>(decodedString.begin(), decodedString.end());
}

static string decodeBase64DataToStr(const vector<uint8_t> &data)
{
    return decodeBase64StrToStr(string(data.begin(), data.end()));
}

static vector<uint8_t> decodeBase64DataToData(const vector<uint8_t> &data)
{
    string decodedString = decodeBase64DataToStr(data);
    return vector<uint8_t>(decodedString.begin(), decodedString.end());
}

static vector<uint8_t> compressData(const int &compressionLevel, const vector<uint8_t> &data)
{
    vector<uint8_t> compressedData;
    size_t const maxCompressedSize = ZSTD_compressBound(data.size());
    compressedData.resize(maxCompressedSize);
    size_t const compressedSize = ZSTD_compress(compressedData.data(), maxCompressedSize, data.data(), data.size(), compressionLevel);
    if (ZSTD_isError(compressedSize))
    {
        cerr << "Failed to compress the file" << endl;
        return vector<uint8_t>();
    }
    compressedData.resize(compressedSize);
    return compressedData;
}

static vector<uint8_t> decompressData(const vector<uint8_t> &compressedData)
{
    vector<uint8_t> decompressedData;
    size_t const decompressedSize = ZSTD_getFrameContentSize(compressedData.data(), compressedData.size());
    if (decompressedSize == ZSTD_CONTENTSIZE_ERROR)
    {
        cerr << "Failed to get decompressed size" << endl;
        return vector<uint8_t>();
    }
    decompressedData.resize(decompressedSize);
    size_t const actualDecompressedSize = ZSTD_decompress(decompressedData.data(), decompressedSize, compressedData.data(), compressedData.size());
    if (ZSTD_isError(actualDecompressedSize))
    {
        cerr << "Failed to decompress data: " << ZSTD_getErrorName(actualDecompressedSize) << endl;
        return vector<uint8_t>();
    }
    else
    {
        decompressedData.resize(actualDecompressedSize);
    }
    return decompressedData;
}

static vector<uint8_t> encryptData(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    if (key.size() != 16 && key.size() != 24 && key.size() != 32)
    {
        cerr << "Wrong key" << endl;
        return vector<uint8_t>();
    }
    AES_ctx ctx;
    AES_init_ctx(&ctx, key.data());
    size_t dataSize = data.size();
    size_t paddedSize = ((dataSize + AES_BLOCKLEN - 1) / AES_BLOCKLEN) * AES_BLOCKLEN;
    vector<uint8_t> paddedData(paddedSize, 0);
    copy(data.begin(), data.end(), paddedData.begin());
    for (size_t i = 0; i < paddedSize; i += AES_BLOCKLEN)
    {
        AES_ECB_encrypt(&ctx, &paddedData[i]);
    }
    return paddedData;
}

static vector<uint8_t> decryptData(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    if (key.size() != 16 && key.size() != 24 && key.size() != 32)
    {
        cerr << "Wrong key" << endl;
        return vector<uint8_t>();
    }
    AES_ctx ctx;
    AES_init_ctx(&ctx, key.data());
    vector<uint8_t> decryptedData = data;
    for (size_t i = 0; i < decryptedData.size(); i += AES_BLOCKLEN)
    {
        AES_ECB_decrypt(&ctx, &decryptedData[i]);
    }
    size_t padSize = 0;
    for (size_t i = decryptedData.size() - 1; i >= decryptedData.size() - AES_BLOCKLEN; --i)
    {
        if (decryptedData[i] == 0)
        {
            padSize++;
        }
    }
    decryptedData.resize(decryptedData.size() - padSize);
    return decryptedData;
}

static vector<uint8_t> compressEncryptData(const int &compressionLevel, const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return encryptData(key, compressData(compressionLevel, data));
}

static vector<uint8_t> decryptDecompressData(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return decompressData(decryptData(key, data));
}

static vector<uint8_t> compressEncryptFile(const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return compressEncryptData(compressionLevel, key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static vector<uint8_t> compressEncryptFiles(const int &compressionLevel, const vector<uint8_t> &key, const vector<string> &inputPaths)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_writer_init_heap(&zip, 0, 0))
    {
        cerr << "Failed to compress data" << endl;
        return vector<uint8_t>();
    }
    for (const auto &path : inputPaths)
    {
        if (fs::is_regular_file(path))
        {
            ifstream fileStream(path, ios::binary);
            vector<uint8_t> fileData((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());
            mz_zip_writer_add_mem(&zip, fs::path(path).filename().string().c_str(), fileData.data(), fileData.size(), 0);
        }
        else if (fs::is_directory(path))
        {
            for (const auto &entry : fs::recursive_directory_iterator(path))
            {
                if (fs::is_regular_file(entry.path()))
                {
                    ifstream fileStream(entry.path(), ios::binary);
                    vector<uint8_t> fileData((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());
                    mz_zip_writer_add_mem(&zip, fs::relative(entry.path(), path).string().c_str(), fileData.data(), fileData.size(), 0);
                }
            }
        }
    }
    void *pBuf = nullptr;
    size_t size;
    mz_zip_writer_finalize_heap_archive(&zip, &pBuf, &size);
    mz_zip_writer_end(&zip);
    vector<uint8_t> packedData(reinterpret_cast<uint8_t *>(pBuf), reinterpret_cast<uint8_t *>(pBuf) + size);
    mz_free(pBuf);
    return compressEncryptData(compressionLevel, key, packedData);
}

static void compressEncryptFileToFile(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    vector<uint8_t> compressedEncryptedData = compressEncryptFile(compressionLevel, key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedData.data()), compressedEncryptedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptFilesToFile(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const vector<string> &inputPaths)
{
    vector<uint8_t> compressedEncryptedData = compressEncryptFiles(compressionLevel, key, inputPaths);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedData.data()), compressedEncryptedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static vector<uint8_t> compressEncryptStr(const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    return compressEncryptData(compressionLevel, key, vector<uint8_t>(inputString.begin(), inputString.end()));
}

static void compressEncryptStrToFile(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> compressedEncryptedData = compressEncryptStr(compressionLevel, key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedData.data()), compressedEncryptedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static vector<uint8_t> decryptDecompressFile(const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return decryptDecompressData(key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static void decryptDecompressFileToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    vector<uint8_t> decryptedDecompressedData = decryptDecompressFile(key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decryptedDecompressedData.data()), decryptedDecompressedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressFilesToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    if (!fs::exists(outputPath))
    {
        fs::create_directories(outputPath);
    }
    ifstream inputFile(inputPath, ios::binary);
    if (!inputFile.is_open())
    {
        cerr << "Failed to open input file: " << inputPath << endl;
        return;
    }
    inputFile.seekg(0, ios::end);
    streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);
    vector<uint8_t> fileData(fileSize);
    if (!inputFile.read(reinterpret_cast<char *>(fileData.data()), fileSize))
    {
        cerr << "Failed to read input file: " << inputPath << endl;
        return;
    }
    inputFile.close();
    vector<uint8_t> decryptedDecompressedData = decryptDecompressData(key, fileData);
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (mz_zip_reader_init_mem(&zip, decryptedDecompressedData.data(), decryptedDecompressedData.size(), 0) == MZ_TRUE)
    {
        for (int i = 0; i < mz_zip_reader_get_num_files(&zip); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (mz_zip_reader_file_stat(&zip, i, &file_stat))
            {
                string file_path = outputPath + "/" + file_stat.m_filename;
                string parent_dir = file_path.substr(0, file_path.find_last_of('/'));
                if (!parent_dir.empty() && !fs::exists(parent_dir))
                {
                    fs::create_directories(parent_dir);
                }
                if (!file_stat.m_is_directory)
                {
                    vector<uint8_t> file_data(file_stat.m_uncomp_size);
                    mz_zip_reader_extract_to_mem(&zip, i, file_data.data(), file_data.size(), 0);
                    ofstream output_file(file_path, ios::out | ios::binary);
                    output_file.write(reinterpret_cast<const char *>(file_data.data()), file_data.size());
                    output_file.close();
                }
                else
                {
                    fs::create_directories(file_path);
                }
            }
        }
        mz_zip_reader_end(&zip);
    }
}

static vector<uint8_t> decryptDecompressStr(const vector<uint8_t> &key, const string &inputString)
{
    return decryptDecompressData(key, vector<uint8_t>(inputString.begin(), inputString.end()));
}

static void decryptDecompressStrToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decryptedDecompressedData = decryptDecompressStr(key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decryptedDecompressedData.data()), decryptedDecompressedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static vector<uint8_t> compressEncryptBase64Data(const int &compressionLevel, const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return compressEncryptData(compressionLevel, key, decodeBase64DataToData(data));
}

static vector<uint8_t> decryptDecompressBase64Data(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return decryptDecompressData(key, decodeBase64DataToData(data));
}

static vector<uint8_t> compressEncryptDataToBase64(const int &compressionLevel, const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return encodeBase64DataToData(compressEncryptData(compressionLevel, key, data));
}

static vector<uint8_t> decryptDecompressDataToBase64(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return encodeBase64DataToData(decryptDecompressData(key, data));
}

static vector<uint8_t> compressEncryptBase64DataToBase64(const int &compressionLevel, const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return encodeBase64DataToData(compressEncryptBase64Data(compressionLevel, key, data));
}

static vector<uint8_t> decryptDecompressBase64DataToBase64(const vector<uint8_t> &key, const vector<uint8_t> &data)
{
    return encodeBase64DataToData(decryptDecompressBase64Data(key, data));
}

static vector<uint8_t> compressEncryptBase64Str(const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    return compressEncryptStr(compressionLevel, key, decodeBase64StrToStr(inputString));
}

static vector<uint8_t> compressEncryptStrToBase64(const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    return encodeBase64DataToData(compressEncryptStr(compressionLevel, key, inputString));
}

static vector<uint8_t> compressEncryptBase64StrToBase64(const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    return encodeBase64DataToData(compressEncryptBase64Str(compressionLevel, key, inputString));
}

static vector<uint8_t> decryptDecompressStrToBase64(const vector<uint8_t> &key, const string &inputString)
{
    return encodeBase64DataToData(decryptDecompressStr(key, inputString));
}

static vector<uint8_t> decryptDecompressBase64Str(const vector<uint8_t> &key, const string &inputString)
{
    return decryptDecompressStr(key, decodeBase64StrToStr(inputString));
}

static vector<uint8_t> decryptDecompressBase64StrToBase64(const vector<uint8_t> &key, const string &inputString)
{
    return encodeBase64DataToData(decryptDecompressBase64Str(key, inputString));
}

static void compressEncryptBase64StrToFile(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decodedCompressedEncryptedData = compressEncryptBase64Str(compressionLevel, key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedCompressedEncryptedData.data()), decodedCompressedEncryptedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptStrToBase64File(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> compressedEncryptedEncodedData = compressEncryptStrToBase64(compressionLevel, key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedEncodedData.data()), compressedEncryptedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptBase64StrToBase64File(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decodedCompressedEncryptedEncodedData = compressEncryptBase64StrToBase64(compressionLevel, key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedCompressedEncryptedEncodedData.data()), decodedCompressedEncryptedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressStrToBase64File(const string &outputPath, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decryptedDecompressedEncodedData = decryptDecompressStrToBase64(key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decryptedDecompressedEncodedData.data()), decryptedDecompressedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressBase64StrToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decodedDecryptedDecompressedData = decryptDecompressBase64Str(key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedDecryptedDecompressedData.data()), decodedDecryptedDecompressedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressBase64StrToBase64File(const string &outputPath, const vector<uint8_t> &key, const string &inputString)
{
    vector<uint8_t> decodedDecryptedDecompressedEncodedData = decryptDecompressBase64StrToBase64(key, inputString);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedDecryptedDecompressedEncodedData.data()), decodedDecryptedDecompressedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static vector<uint8_t> compressEncryptFileToBase64(const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    return encodeBase64DataToData(compressEncryptFile(compressionLevel, key, inputPath));
}

static vector<uint8_t> compressEncryptFilesToBase64(const int &compressionLevel, const vector<uint8_t> &key, const vector<string> &inputPaths)
{
    return encodeBase64DataToData(compressEncryptFiles(compressionLevel, key, inputPaths));
}

static vector<uint8_t> compressEncryptBase64File(const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return compressEncryptBase64Data(compressionLevel, key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static vector<uint8_t> compressEncryptBase64FileToBase64(const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return compressEncryptBase64DataToBase64(compressionLevel, key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static vector<uint8_t> decryptDecompressFileToBase64(const vector<uint8_t> &key, const string &inputPath)
{
    return encodeBase64DataToData(decryptDecompressFile(key, inputPath));
}

static vector<uint8_t> decryptDecompressBase64File(const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return decryptDecompressBase64Data(key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static vector<uint8_t> decryptDecompressBase64FileToBase64(const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    return decryptDecompressBase64DataToBase64(key, vector<uint8_t>((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>()));
}

static void compressEncryptBase64FileToFile(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    vector<uint8_t> decodedCompressedEncryptedData = compressEncryptBase64File(compressionLevel, key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedCompressedEncryptedData.data()), decodedCompressedEncryptedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptFileToBase64File(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    vector<uint8_t> compressedEncryptedEncodedData = compressEncryptFileToBase64(compressionLevel, key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedEncodedData.data()), compressedEncryptedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptBase64FileToBase64File(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const string &inputPath)
{
    vector<uint8_t> decodedCompressedEncryptedEncodedData = compressEncryptBase64FileToBase64(compressionLevel, key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedCompressedEncryptedEncodedData.data()), decodedCompressedEncryptedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void compressEncryptFilesToBase64File(const string &outputPath, const int &compressionLevel, const vector<uint8_t> &key, const vector<string> &inputPaths)
{
    vector<uint8_t> compressedEncryptedEncodedDatas = compressEncryptFilesToBase64(compressionLevel, key, inputPaths);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(compressedEncryptedEncodedDatas.data()), compressedEncryptedEncodedDatas.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressBase64FileToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    vector<uint8_t> decodedDecryptedDecompressedData = decryptDecompressBase64File(key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedDecryptedDecompressedData.data()), decodedDecryptedDecompressedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressFileToBase64File(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    vector<uint8_t> decryptedDecompressedEncodedData = decryptDecompressFileToBase64(key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decryptedDecompressedEncodedData.data()), decryptedDecompressedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressBase64FileToBase64File(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    ifstream fileStream(inputPath, ios::binary);
    vector<uint8_t> decodedDecryptedDecompressedEncodedData = decryptDecompressBase64FileToBase64(key, inputPath);
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (outputFile.is_open())
    {
        outputFile.write(reinterpret_cast<const char *>(decodedDecryptedDecompressedEncodedData.data()), decodedDecryptedDecompressedEncodedData.size());
        outputFile.close();
    }
    else
    {
        cerr << "Failed to write to the output file" << endl;
        return;
    }
}

static void decryptDecompressBase64FilesToFile(const string &outputPath, const vector<uint8_t> &key, const string &inputPath)
{
    if (!fs::exists(outputPath))
    {
        fs::create_directories(outputPath);
    }
    ifstream inputFile(inputPath, ios::binary);
    if (!inputFile.is_open())
    {
        cerr << "Failed to open input file: " << inputPath << endl;
        return;
    }
    inputFile.seekg(0, ios::end);
    streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);
    vector<uint8_t> fileData(fileSize);
    if (!inputFile.read(reinterpret_cast<char *>(fileData.data()), fileSize))
    {
        cerr << "Failed to read input file: " << inputPath << endl;
        return;
    }
    inputFile.close();
    vector<uint8_t> decodedDecryptedDecompressedData = decryptDecompressBase64Data(key, fileData);
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (mz_zip_reader_init_mem(&zip, decodedDecryptedDecompressedData.data(), decodedDecryptedDecompressedData.size(), 0) == MZ_TRUE)
    {
        for (int i = 0; i < mz_zip_reader_get_num_files(&zip); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (mz_zip_reader_file_stat(&zip, i, &file_stat))
            {
                string file_path = outputPath + "/" + file_stat.m_filename;
                string parent_dir = file_path.substr(0, file_path.find_last_of('/'));
                if (!parent_dir.empty() && !fs::exists(parent_dir))
                {
                    fs::create_directories(parent_dir);
                }
                if (!file_stat.m_is_directory)
                {
                    vector<uint8_t> file_data(file_stat.m_uncomp_size);
                    mz_zip_reader_extract_to_mem(&zip, i, file_data.data(), file_data.size(), 0);
                    ofstream output_file(file_path, ios::out | ios::binary);
                    output_file.write(reinterpret_cast<const char *>(file_data.data()), file_data.size());
                    output_file.close();
                }
                else
                {
                    fs::create_directories(file_path);
                }
            }
        }
        mz_zip_reader_end(&zip);
    }
}