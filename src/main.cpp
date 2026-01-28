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
        if(!game.getPlayer()->isAlive()) {
            game.RenderDeathUI(); // Draw death UI if player is dead
            return;
        }
        if(game.getHasWon()) {
            game.RenderWinUI(); // Draw win UI if player has won
            return;
        }
        game.RenderUI(); // Draw game UI elements
    };

    viewer.run();

    glfwTerminate();
    return 0;

}



