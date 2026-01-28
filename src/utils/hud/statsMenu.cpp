#include "statsMenu.h"
#include "constants.h"

StatsMenu::StatsMenu(TextRenderer* textRenderer,Player* player)
{
    this->textRenderer = textRenderer;
    this->player = player;
}

void StatsMenu::renderMenu()
{
    if (!isVisible) {
        return;
    }
    float x = Config::StatsMenu::START_X;
    float y = Config::StatsMenu::START_Y;
    float scale = Config::StatsMenu::SCALE;

    y -= 30.0f; 
    std::string speedText = "Speed: " + std::to_string(static_cast<int>(player->getSpeed() * 10.0f));
    textRenderer->RenderText(speedText, x, y, scale, color);

    y -= 30.0f; 
    std::string projectileSpeedText = "Projectile Speed: " + std::to_string(static_cast<int>(player->getProjectileSpeed()));
    textRenderer->RenderText(projectileSpeedText, x, y, scale, color);

    y -= 30.0f; 
    std::string attackDamageText = "Attack Damage: " + std::to_string(static_cast<int>(player->getAttackDamage()));
    textRenderer->RenderText(attackDamageText, x, y, scale, color);

    y -= 30.0f;
    std::string attackSpeedText = "Attack Speed: " + std::to_string(static_cast<int>(player->getAttackSpeed() * 10.0f));
    textRenderer->RenderText(attackSpeedText, x, y, scale, color);

    y-= 30.0f;
    std::string jumpStrengthText = "Jump Strength: " + std::to_string(static_cast<int>(player->getJumpStrength() * 5.0f));
    textRenderer->RenderText(jumpStrengthText, x, y, scale, color);
}