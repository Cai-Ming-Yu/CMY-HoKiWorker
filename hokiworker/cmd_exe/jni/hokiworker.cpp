#include <libhokiworker/do.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Invalid usage" << endl;
        return -1;
    }

    string keyString = argv[1];
    vector<uint8_t> key(keyString.begin(), keyString.end());
    string command = argv[2];
    string mode;
    int compressionLevel;
    if (command.find("ce") == 0)
    {
        compressionLevel = stoi(argv[3]);
        mode = argv[4];
    }
    else if (command.find("dd") == 0)
    {
        mode = argv[3];
    }
    else
    {
        cerr << "Invalid usage" << endl;
        return -1;
    }

    if (command == "cefile")
    {
        if (mode == "stdout")
        {
            string filePath = argv[5];
            for (uint8_t value : compressEncryptFile(compressionLevel, key, filePath))
            {
                cout << static_cast<char>(value);
            }
        }
        else if (mode == "outfile")
        {
            string output = argv[5];
            string filePath = argv[6];
            compressEncryptFileToFile(output, compressionLevel, key, filePath);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else if (command == "cefiles")
    {
        if (mode == "stdout")
        {
            vector<string> filePaths;
            for (int i = 5; i < argc; ++i)
            {
                filePaths.push_back(argv[i]);
            }
            for (uint8_t value : compressEncryptFiles(compressionLevel, key, filePaths))
            {
                cout << static_cast<char>(value);
            }
        }
        else if (mode == "outfile")
        {
            string output = argv[5];
            vector<string> filePaths;
            for (int i = 6; i < argc; ++i)
            {
                filePaths.push_back(argv[i]);
            }
            compressEncryptFilesToFile(output, compressionLevel, key, filePaths);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else if (command == "cestr")
    {
        if (mode == "stdout")
        {
            string str = argv[5];
            for (uint8_t value : compressEncryptStr(compressionLevel, key, str))
            {
                cout << static_cast<char>(value);
            }
        }
        else if (mode == "outfile")
        {
            string output = argv[5];
            string str = argv[6];
            compressEncryptStrToFile(output, compressionLevel, key, str);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else if (command == "ddfile")
    {
        if (mode == "stdout")
        {
            string filePath = argv[4];
            for (uint8_t value : decryptDecompressFile(key, filePath))
            {
                cout << static_cast<char>(value);
            }
        }
        else if (mode == "outfile")
        {
            string output = argv[4];
            string filePath = argv[5];
            decryptDecompressFileToFile(output, key, filePath);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else if (command == "ddfiles")
    {
        if (mode == "stdout")
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
        else if (mode == "outfile")
        {
            string output = argv[4];
            string filePath = argv[5];
            decryptDecompressFilesToFile(output, key, filePath);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else if (command == "ddstr")
    {
        if (mode == "stdout")
        {
            string str = argv[4];
            for (uint8_t value : decryptDecompressStr(key, str))
            {
                cout << static_cast<char>(value);
            }
        }
        else if (mode == "outfile")
        {
            string output = argv[4];
            string str = argv[5];
            decryptDecompressStrToFile(output, key, str);
        }
        else
        {
            cerr << "Invalid usage" << endl;
            return -1;
        }
    }
    else
    {
        cerr << "Invalid usage" << endl;
        return -1;
    }

    return 0;
}