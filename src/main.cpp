#include "Engine/Engine.hpp"

#include <iostream>


int main(int argc, char **argv) {
    /* Singular engine */
    EngineApp engine;
 
    /* Process args and initialize engine */
    if (engine.processArgs(argc, argv) || engine.init()) {
        return 1;
    }

    /* Main loop */
    engine.run();

    return 0;
}


