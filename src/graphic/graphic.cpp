/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 */

#include "graphic.h"
#include "../gui/gui.h"
#include "../gui/textfield.h"
#include "../gui/status.h"
#include "../main.h"

BITMAP *buffer;

char itemCurrenyQ[10] = "0";
int map_x, map_y, camera_x, camera_y;
char npc_text[1000] = "";
char statsString2[255] = "n/a";
char skill_points[10] = "";
bool show_skill_dialog = false;
bool show_skill_list_dialog = false;
char npc_button[10] = "Close";

gcn::TextField *chatInput;
gcn::Label *debugInfo;

ChatBox *chatBox;
StatusWindow *statusWindow;
BuyDialog *buyDialog;
SellDialog *sellDialog;
BuySellDialog *buySellDialog;
InventoryWindow *inventoryWindow;
NpcListDialog *npcListDialog;
NpcTextDialog *npcTextDialog;
SkillDialog *skillDialog;
StatsWindow *statsWindow;

void ChatListener::action(const std::string& eventId)
{
    if (eventId == "chatinput") {
        std::string message = chatInput->getText();

        if (message.length() > 0) {
            chatBox->chat_send(char_info[0].name, message.c_str());
            chatInput->setText("");
        }
    }
}

void BuySellListener::action(const std::string& eventId)
{
    int actionId = -1;

    if (eventId == "buy") {
        actionId = 0;
    }
    else if (eventId == "sell") {
        actionId = 1;
    }

    if (actionId > -1) {
        WFIFOW(0) = net_w_value(0x00c5);
        WFIFOL(2) = net_l_value(current_npc);
        WFIFOB(6) = net_b_value(actionId);
        WFIFOSET(7);
    }

    buySellDialog->setVisible(false);
}


char hairtable[16][4][2] = {
    // S(x,y)  W(x,y)   N(x,y)   E(x,y)
    { { 0, 0}, {-1, 2}, {-1, 2}, {0, 2} }, // STAND
    { { 0, 2}, {-2, 3}, {-1, 2}, {1, 3} }, // WALK 1st frame
    { { 0, 3}, {-2, 4}, {-1, 3}, {1, 4} }, // WALK 2nd frame
    { { 0, 1}, {-2, 2}, {-1, 2}, {1, 2} }, // WALK 3rd frame
    { { 0, 2}, {-2, 3}, {-1, 2}, {1, 3} }, // WALK 4th frame
    { { 0, 1}, {1, 2}, {-1, 3}, {-2, 2} }, // ATTACK 1st frame
    { { 0, 1}, {-1, 2}, {-1, 3}, {0, 2} }, // ATTACK 2nd frame
    { { 0, 2}, {-4, 3}, {0, 4}, {3, 3}  }, // ATTACK 3rd frame
    { { 0, 2}, {-4, 3}, {0, 4}, {3, 3}  }, // ATTACK 4th frame
    { { 0, 0}, {-1, 2}, {-1, 2}, {-1, 2} }, // BOW_ATTACK 1st frame
    { { 0, 0}, {-1, 2}, {-1, 2}, {-1, 2} }, // BOW_ATTACK 2nd frame
    { { 0, 0}, {-1, 2}, {-1, 2}, {-1, 2}  }, // BOW_ATTACK 3rd frame
    { { 0, 0}, {-1, 2}, {-1, 2}, {-1, 2}  }, // BOW_ATTACK 4th frame
    { { 0, 4}, {-1, 6}, {-1, 6}, {0, 6} }, // SIT
    { { 0, 0}, {0, 0}, {0, 0}, {0, 0} }, // ?? HIT
    { { 0, 16}, {-1, 6}, {-1, 6}, {0, 6} } // DEAD
};

int get_x_offset(Being *being) {
    int offset = 0;
    char direction = being->direction;
    if (being->action == WALK) {
        if (direction != NORTH && direction != SOUTH) {
            offset = being->frame + 1;
            if (offset == 5)offset = 0;
            offset *= 8;
            if (direction == WEST || direction == NW || direction == SW) {
                offset = -offset;
                offset += 32;
            } else offset -= 32;
        }
    }
    return offset;
}

int get_y_offset(Being *being) {
    int offset = 0;
    char direction = being->direction;
    if (being->action == WALK) {
        if (direction != EAST && direction != WEST) {
            offset = being->frame + 1;
            if (offset == 5) offset = 0;
            offset *= 8;
            if (direction == NORTH || direction == NW || direction == NE) {
                offset = -offset;
                offset += 32;
            }
            else {
                offset -= 32;
            }
        }
    }
    return offset;
}


Graphics::Graphics()
{
    // Create drawing buffer
    // SDL probably doesn't need this buffer, it'll buffer for us.
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        error("Not enough memory to create buffer");
    }

    setTarget(buffer);
}

Graphics::~Graphics() {
    destroy_bitmap(buffer);
}

void Graphics::updateScreen()
{
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}


Engine::Engine()
{
    // Initializes GUI
    chatInput = new TextField();
    chatInput->setPosition(chatInput->getBorderSize(),
        SCREEN_H - chatInput->getHeight() - chatInput->getBorderSize() -1);
    chatInput->setWidth(592 - 2 * chatInput->getBorderSize());

    ChatListener *chatListener = new ChatListener();
    chatInput->setEventId("chatinput");
    chatInput->addActionListener(chatListener);

    debugInfo = new gcn::Label();

    chatBox = new ChatBox("./docs/chatlog.txt", 20);
    chatBox->setSize(592, 100);
    chatBox->setPosition(0, chatInput->getY() - 1 - chatBox->getHeight());

    guiTop->add(chatBox);
    guiTop->add(debugInfo);
    guiTop->add(chatInput);


    // Create dialogs

    statusWindow = new StatusWindow();
    statusWindow->setPosition(SCREEN_W - statusWindow->getWidth() - 10, 10);

    buyDialog = new BuyDialog();
    buyDialog->setVisible(false);

    sellDialog = new SellDialog();
    sellDialog->setVisible(false);

    buySellDialog = new BuySellDialog(new BuySellListener());
    buySellDialog->setVisible(false);

    inventoryWindow = new InventoryWindow();
    inventoryWindow->setVisible(false);
    inventoryWindow->setPosition(100, 100);

    npcTextDialog = new NpcTextDialog();
    npcTextDialog->setVisible(false);

    npcListDialog = new NpcListDialog();
    npcListDialog->setVisible(false);

    skillDialog = new SkillDialog();
    skillDialog->setVisible(false);

    statsWindow = new StatsWindow();
    statsWindow->setVisible(false);


    // Give focus to the chat input
    chatInput->requestFocus();

    // Load the sprite sets
    ResourceManager *resman = ResourceManager::getInstance();
    Image *npcbmp = resman->getImage("graphic/npcset.bmp");
    Image *emotionbmp = resman->getImage("graphic/emotionset.bmp");
    Image *tilesetbmp = resman->getImage("graphic/tileset.bmp");
    Image *monsterbitmap = resman->getImage("graphic/monsterset.bmp");

    if (!npcbmp) error("Unable to load npcset.bmp");
    if (!emotionbmp) error("Unable to load emotionset.bmp");
    if (!tilesetbmp) error("Unable to load tileset.bmp");
    if (!monsterbitmap) error("Unable to load monsterset.bmp");

    npcset = new Spriteset(npcbmp, 50, 80);
    emotionset = new Spriteset(emotionbmp, 19, 19);
    tileset = new Spriteset(tilesetbmp, 32, 32);
    monsterset = new Spriteset(monsterbitmap, 60, 60);
}

Engine::~Engine()
{
    delete chatBox;
    delete statusWindow;
    delete buyDialog;
    delete sellDialog;
    delete buySellDialog;
    delete npcListDialog;
    delete npcTextDialog;
    delete skillDialog;
    delete statsWindow;
    
    delete tileset;
    delete monsterset;
    delete npcset;
    delete emotionset;
}

void Engine::draw()
{
    map_x = (player_node->x - 13) * 32 +
        get_x_offset(player_node);
    map_y = (player_node->y - 9) * 32 +
        get_y_offset(player_node);

    camera_x = map_x >> 5;
    camera_y = map_y >> 5;

    int offset_x = map_x & 31;
    int offset_y = map_y & 31;

    sort();

    frame++;

    // Draw tiles below nodes
    for (int j = 0; j < 20; j++) {
        for (int i = 0; i < 26; i++) {
            unsigned short tile0 = get_tile(i + camera_x, j + camera_y, 0);
            unsigned short tile1 = get_tile(i + camera_x, j + camera_y, 1);

            if (tile0 < tileset->spriteset.size()) {
                tileset->spriteset[tile0]->draw(buffer,
                        i * 32 - offset_x, j * 32 - offset_y);
            }
            if (tile1 > 0 && tile1 < tileset->spriteset.size()) {
                tileset->spriteset[tile1]->draw(buffer,
                        i * 32 - offset_x, j * 32 - offset_y);
            }
            
        }
    }
        
    // Draw nodes
    std::list<Being*>::iterator beingIterator = beings.begin();
    while (beingIterator != beings.end()) {
        Being *being = (*beingIterator);

        unsigned short x = being->x;
        unsigned short y = being->y;
        unsigned char dir = being->direction / 2;
        int sx = x - camera_x;
        int sy = y - camera_y;
    
#ifdef DEBUG
        textprintf_ex(buffer, font, sx*32, sy*32+40, makecol(255, 255, 255), -1, "%i,%i | %i", x, y, being->frame);
        rect(buffer, sx*32, sy*32, sx*32+32, sy*32+32, makecol(0,0,255));
#endif
        
        if ((being->job >= 100) && (being->job <= 110)) { // Draw a NPC
            npcset->spriteset[4 * (being->job - 100) + dir]->draw(buffer,
                    sx * 32 - 8 - offset_x,
                    sy * 32 - 52 - offset_y);
        }
        else if (being->job < 10) { // Draw a player
            being->text_x = sx * 32 + get_x_offset(being) - offset_x;
            being->text_y = sy * 32 + get_y_offset(being) - offset_y;
            int hf = being->hair_color - 1 + 10 * (dir + 4 *
                    (being->hair_style - 1));
            
            if (being->action == SIT || being->action == DEAD) being->frame = 0;
            if (being->action == ATTACK) {
                int pf = being->frame + being->action + 4 * being->weapon;

                playerset->spriteset[4 * pf + dir]->draw(buffer,
                        being->text_x - 64, being->text_y - 80);
                hairset->spriteset[hf]->draw(buffer,
                        being->text_x - 2 + 2 * hairtable[pf][dir][0],
                        being->text_y - 50 + 2 * hairtable[pf][dir][1]);
            }
            else {
                int pf = being->frame + being->action;

                playerset->spriteset[4 * pf + dir]->draw(buffer,
                        being->text_x - 64, being->text_y - 80);
                hairset->spriteset[hf]->draw(buffer,
                        being->text_x - 2 + 2 * hairtable[pf][dir][0],
                        being->text_y - 50 + 2 * hairtable[pf][dir][1]);
            }
            if (being->emotion != 0) {
                emotionset->spriteset[being->emotion - 1]->draw(buffer,
                        sx * 32 - 5 + get_x_offset(being) - offset_x,
                        sy * 32 - 45 + get_y_offset(being) - offset_y);
                being->emotion_time--;
                if (being->emotion_time == 0) {
                    being->emotion = 0;
                }
            }
            if (being->action != STAND && being->action != SIT
                    && being->action != DEAD) {
                being->frame =
                    (get_elapsed_time(being->tick_time) * 4) / (being->speed);

                if (being->frame >= 4) {
                    being->frame = 0;
                    being->action = STAND;
                    if (being->id == player_node->id) {
                        walk_status = 0;
                    }
                }
            }
        }
        else if (being->job == 45) { // Draw a warp
        } else { // Draw a monster
            if (being->frame >= 4)
                being->frame = 3;

            being->text_x = sx * 32 - 42 + get_x_offset(being) - offset_x;
            being->text_y = sy * 32 - 65 + get_y_offset(being) - offset_y;

            int sprnum = dir + 4 * (being->job - 1002);
            int mf = being->frame + being->action;

            if (being->action == MONSTER_DEAD) {
                monsterset->spriteset[sprnum + 8 * MONSTER_DEAD]->draw(buffer,
                        being->text_x + 30, being->text_y + 40);
            }
            else {
                monsterset->spriteset[sprnum + 8 * mf]->draw(buffer,
                        being->text_x + 30, being->text_y + 40);
            }

            if (being->action != STAND) {
                being->frame =
                    (get_elapsed_time(being->tick_time) * 4) / (being->speed);

                if (being->frame >= 4) {
                    if (being->action != MONSTER_DEAD && being->path) {
                        if (being->path->next) {
                            int old_x, old_y, new_x, new_y;
                            old_x = being->path->x;
                            old_y = being->path->y;
                            being->path = being->path->next;
                            new_x = being->path->x;
                            new_y = being->path->y;
                            direction = 0;

                            if (new_x > old_x) {
                                if (new_y > old_y)      direction = SE;
                                else if (new_y < old_y) direction = NE;
                                else                    direction = EAST;
                            }
                            else if (new_x < old_x) {
                                if (new_y > old_y)      direction = SW;
                                else if (new_y < old_y) direction = NW;
                                else                    direction = WEST;
                            }
                            else {
                                if (new_y > old_y)      direction = SOUTH;
                                else if (new_y < old_y) direction = NORTH;
                            }

                            being->x = being->path->x;
                            being->y = being->path->y;
                            being->direction = direction;
                        } else {
                            being->action = STAND;
                        }
                        if (being->action != MONSTER_DEAD) {
                            being->frame = 0;
                        }
                        being->tick_time = tick_time;
                        //being->frame = 0;
                    }
                }
            }
        }
        
        if (being->action == MONSTER_DEAD && being->frame >= 20) {
            delete being;
            beingIterator = beings.erase(beingIterator);
        }
        else {
            beingIterator++;
        }

        // nodes are ordered so if the next being y is > then the
        // last drawed for fringe layer, draw the missing lines
    }

    // Draw tiles above nodes
    for (int j = 0; j < 20; j++) {
        for (int i = 0; i < 26; i++) {
            unsigned short tile = get_tile(i + camera_x, j + camera_y, 2);

            if (tile > 0 && tile < tileset->spriteset.size()) {
                tileset->spriteset[tile]->draw(
                        buffer, i * 32 - offset_x, j * 32 - offset_y);
            }
#ifdef DEBUG            
            rect(buffer, i * 32 - offset_x, j * 32 - offset_y,
                    i * 32 - offset_x + 32, j * 32 - offset_y + 32,
                    makecol(0,0,0));
#endif
        }
    }
    
    // Draw player speech
    beingIterator = beings.begin();
    while (beingIterator != beings.end()) {
        Being *being = (*beingIterator);

        if (being->speech != NULL) {
            if (being->speech_color == makecol(255, 255, 255)) {
                textprintf_centre_ex(buffer, font,
                        being->text_x,
                        being->text_y - 60,
                        being->speech_color, -1,
                        "%s", being->speech);
            }
            else {
                textprintf_centre_ex(buffer, font,
                        being->text_x + 60,
                        being->text_y,
                        being->speech_color, -1,
                        "%s", being->speech);
            }

            being->speech_time--;
            if (being->speech_time == 0) {
                free(being->speech);
                being->speech = NULL;
            }
        }

        beingIterator++;
    }

    if (statsWindow->isVisible()) {
        statsWindow->update();
    }
    if (statusWindow->isVisible()) {
        statusWindow->update();
    }

    gui->draw();

    std::stringstream debugStream;
    debugStream << "[" << fps << " fps] " <<
        (mouse_x / 32 + camera_x) << ", " << (mouse_y / 32 + camera_y);
    debugInfo->setCaption(debugStream.str());
    debugInfo->adjustSize();
}
