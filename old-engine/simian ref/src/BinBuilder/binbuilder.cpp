#include <iostream>
#include <vector>
#include <algorithm>

#include "Simian/OS.h"
#include "Simian/Model.h"

int main(int argc, char* argv[])
{
    std::string dir = "Models";
    if (argc > 1)
        dir = argv[1];

    std::cout << "Finding all DAE files and building bins." << std::endl;

    std::vector<std::string> files;
    Simian::OS::Instance().GetFilesInDirectory(dir, files);

    for (unsigned i = 0; i < files.size(); ++i)
    {
        std::string& file = files[i];

        if (file.length() < 3)
            continue;

        std::string extension = files[i].substr(files[i].length() - 3);
        
        std::transform(extension.begin(), extension.end(), extension.begin(), toupper);

        if (extension != "DAE")
            continue;

        std::cout << "Converting " << file << "...";
        Simian::Model* model = new Simian::Model(NULL);
        model->LoadFile("Models/" + file);
        model->Unload();
        delete model;
        std::cout << "OK" << std::endl;
    }
}
