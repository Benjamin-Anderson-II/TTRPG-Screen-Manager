// Making sure everything is set up for other files
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "log.h"
std::ofstream logFile;

#include <iostream>

#include "app.h"

int main(int argc, const char *argv[]) {
    std::cout << "Starting Application\n";
    auto o_app = App::create();
    if(o_app == std::nullopt){
        std::cerr << "Fatal Error: Failed to Create App\n";
        exit(1);
    }

    o_app.value().launch();
    return 0;
}
