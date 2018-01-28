#include "Engine/EngineApp/EngineApp.hpp"

#include <iostream>


int main(int argc, char **argv) {
    /* Singular engine */
    EngineApp engine;
    engine.processArgs(argc, argv);
    engine.init();

    /* Main loop */
    engine.run();

    return 0;
}


