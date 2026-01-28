#pragma once

#include "textRenderer.h"
#include "player.h"


class StatsMenu {
    public:
        StatsMenu(TextRenderer* textRenderer,Player* player);
        void renderMenu();
        void setVisible(bool visible) { isVisible = visible; }
        bool getIsVisible() const { return isVisible; }
    private:
        TextRenderer* textRenderer;
        Player* player;
        bool isVisible = false;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

};