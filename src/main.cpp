#include "viewer.h"
#include "game.h"

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif


int main()
{
    // Create viewer instance
    Viewer viewer(SCR_WIDTH, SCR_HEIGHT);

    // Create game instance
    Game game(&viewer);
    game.Init();


    viewer.update_callback = [&]() {
        game.Update(); // update game state
    };

    viewer.draw_ui_callback = [&]() {
        game.RenderUI(); // Draw game UI elements
    };

    viewer.run();

}



