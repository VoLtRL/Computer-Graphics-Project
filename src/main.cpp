#include "viewer.h"
#include "game.h"
#include "constants.h"

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif


int main()
{
    // Create viewer instance
    Viewer viewer(Config::SCR_WIDTH, Config::SCR_HEIGHT);
    Game game(&viewer);
    
    game.Init();

    viewer.update_callback = [&]() {
        game.Update(); // update game state
    };

    viewer.draw_ui_callback = [&]() {
        game.RenderUI(); // Draw game UI elements
    };

    viewer.run();

    glfwTerminate();
    return 0;

}



