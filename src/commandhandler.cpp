/*
 *  The Mana World
 *  Copyright 2008 The Mana World Development Team
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
 
 
#include "commandhandler.h"
#include "channelmanager.h"
#include "channel.h"
#include "game.h"
#include "gui/chat.h"
#include "net/chatserver/chatserver.h"
#include "net/gameserver/player.h"

void CommandHandler::handleCommand(const std::string &command)
{
    std::string::size_type pos = command.find(' ');
    std::string type(command, 0, pos);
    std::string args(command, pos == std::string::npos ? command.size() : pos + 1);

    if (type == "announce")
    {
        handleAnnounce(args);
    }
    else if (type == "help")
    {
        handleHelp();
    }
    else if (type == "where")
    {
        handleWhere();
    }
    else if (type == "who")
    {
        handleWho();
    }
    else if (type == "msg")
    {
        handleMsg(args);
    }
    else if (type == "channel")
    {
        handleChannel(args);
    }
    else if (type == "join")
    {
        handleJoin(args);
    }
    else if (type == "listchannels")
    {
        handleListChannels();
    }
    else if (type == "listusers")
    {
        handleListUsers();
    }
    else if (type == "quit")
    {
        handleQuit();
    }
    else if (type == "admin")
    {
        Net::GameServer::Player::say("/" + args);
    }
    else if (type == "clear")
    {
        handleClear();
    }
    else
    {
        chatWindow->chatLog("Unknown command");
    }
}

void CommandHandler::handleAnnounce(const std::string &args)
{
    Net::ChatServer::announce(args);
}

void CommandHandler::handleHelp()
{
    chatWindow->chatLog("-- Help --");
    chatWindow->chatLog("/help > Display this help.");
    chatWindow->chatLog("/announce > Global announcement (GM only)");
    chatWindow->chatLog("/where > Display map name");
    chatWindow->chatLog("/who > Display number of online users");
    chatWindow->chatLog("/msg > Send a private message to a user");
    chatWindow->chatLog("/listchannels > Display all public channels");
    chatWindow->chatLog("/listusers > Lists the users in the current channel");
    chatWindow->chatLog("/channel > Register a new channel");
    chatWindow->chatLog("/join > Join an already registered channel");
    chatWindow->chatLog("/quit > Leave a channel");
    chatWindow->chatLog("/admin > Send a command to the server (GM only)");
    chatWindow->chatLog("/clear > Clears this window");
}

void CommandHandler::handleWhere()
{
    chatWindow->chatLog(map_path, BY_SERVER);
}

void CommandHandler::handleWho()
{
    
}

void CommandHandler::handleMsg(const std::string &args)
{
    std::string::size_type pos = args.find(' ');
    std::string recipient(args, 0, pos);
    std::string text(args, pos+1);
    Net::ChatServer::privMsg(recipient, text);
}

void CommandHandler::handleChannel(const std::string &args)
{
    std::string::size_type pos = args.find(" ");
    std::string name(args, 0, pos);
    std::string password;
    std::string announcement;
    if(pos == std::string::npos)
    {
        password = std::string();
        announcement = std::string();
    }
    else
    {
        password = std::string(args, pos + 1, args.find(" ", pos + 1) - pos - 1);
        pos = args.find("\"");
        announcement = std::string(args, pos == std::string::npos ? args.size() :
                           pos + 1, args.size() - pos - 2);
    }
    chatWindow->chatLog("Requesting to register channel " + name);
    Net::ChatServer::registerChannel(name, announcement, password);
}

void CommandHandler::handleJoin(const std::string &args)
{
    std::string::size_type pos = args.find(' ');
    std::string name(args, 0, pos);
    std::string password(args, pos+1);
    chatWindow->chatLog("Requesting to join channel " + name);
    Net::ChatServer::enterChannel(name, password);
}

void CommandHandler::handleListChannels()
{
    Net::ChatServer::getChannelList();
}

void CommandHandler::handleListUsers()
{
    Net::ChatServer::getUserList(chatWindow->getFocused());
}

void CommandHandler::handleQuit()
{
    if (Channel *channel = channelManager->findByName(chatWindow->getFocused()))
    {
        Net::ChatServer::quitChannel(channel->getId());
    }
    else
    {
        chatWindow->chatLog("Unable to quit this channel", BY_CHANNEL);
    }
}

void CommandHandler::handleClear()
{
    chatWindow->clearTab(chatWindow->getFocused());
}
