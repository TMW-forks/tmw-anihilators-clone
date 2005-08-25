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

#include "login.h"

#include <string>
#include <sstream>

#include <guichan/sdl/sdlinput.hpp>

#include <guichan/widgets/label.hpp>

#include "../main.h"

#include "button.h"
#include "checkbox.h"
#include "gui.h"
#include "passwordfield.h"
#include "textfield.h"
#include "ok_dialog.h"

#include "../configuration.h"
#include "../graphics.h"
#include "../log.h"
#include "../serverinfo.h"

#include "../net/network.h"

extern Graphics *graphics;

OkDialog *wrongLoginNotice = NULL;
LoginDialog *loginDialog = NULL;

WrongUsernameNoticeListener wrongUsernameNoticeListener;
WrongPasswordNoticeListener wrongPasswordNoticeListener;

void WrongPasswordNoticeListener::action(const std::string &eventId)
{
        // Reset the password and put the caret ready to retype it.
        loginDialog->passField->setText("");
        loginDialog->passField->setCaretPosition(0);
        loginDialog->passField->requestFocus();
        wrongLoginNotice = NULL;
}

void WrongUsernameNoticeListener::action(const std::string &eventId)
{
    // Set the focus on the username Field
    loginDialog->userField->setCaretPosition(LEN_MAX_USERNAME - 1);
    loginDialog->userField->requestFocus();
    wrongLoginNotice = NULL;
}

LoginDialog::LoginDialog():
    Window("Login")
{
    userLabel = new gcn::Label("Name:");
    passLabel = new gcn::Label("Password:");
    serverLabel = new gcn::Label("Server:");
    userField = new TextField("player");
    passField = new PasswordField();
    serverField = new TextField();
    keepCheck = new CheckBox("Keep", false);
    okButton = new Button("OK");
    cancelButton = new Button("Cancel");
    registerButton = new Button("Register");

    setContentSize(200, 100);

    userLabel->setPosition(5, 5);
    passLabel->setPosition(5, 14 + userLabel->getHeight());
    serverLabel->setPosition(
            5, 23 + userLabel->getHeight() + passLabel->getHeight());
    userField->setPosition(65, 5);
    passField->setPosition(65, 14 + userLabel->getHeight());
    serverField->setPosition(
            65, 23 + userLabel->getHeight() + passLabel->getHeight());
    userField->setWidth(130);
    passField->setWidth(130);
    serverField->setWidth(130);
    keepCheck->setPosition(4, 77);
    keepCheck->setMarked(config.getValue("remember", 0));
    cancelButton->setPosition(
            200 - cancelButton->getWidth() - 5,
            100 - cancelButton->getHeight() - 5);
    okButton->setPosition(
            cancelButton->getX() - okButton->getWidth() - 5,
            100 - okButton->getHeight() - 5);
    registerButton->setPosition(keepCheck->getX() + keepCheck->getWidth() + 10,
            100 - registerButton->getHeight() - 5);

    userField->setEventId("ok");
    passField->setEventId("ok");
    serverField->setEventId("ok");
    okButton->setEventId("ok");
    cancelButton->setEventId("cancel");
    registerButton->setEventId("register");

    userField->addActionListener(this);
    passField->addActionListener(this);
    serverField->addActionListener(this);
    keepCheck->addActionListener(this);
    okButton->addActionListener(this);
    cancelButton->addActionListener(this);
    registerButton->addActionListener(this);

    add(userLabel);
    add(passLabel);
    add(serverLabel);
    add(userField);
    add(passField);
    add(serverField);
    add(keepCheck);
    add(okButton);
    add(cancelButton);
    add(registerButton);

    setLocationRelativeTo(getParent());
    userField->requestFocus();
    userField->setCaretPosition(userField->getText().length());

    if (config.getValue("remember", 0) != 0) {
        if (config.getValue("username", "") != "") {
            userField->setText(config.getValue("username", ""));
            passField->requestFocus();
        }
    }

    serverField->setText(config.getValue("host", ""));

}

LoginDialog::~LoginDialog()
{
    delete userLabel;
    delete passLabel;
    delete serverLabel;
    delete userField;
    delete passField;
    delete serverField;
    delete keepCheck;
    delete okButton;
    delete cancelButton;
    delete registerButton;
}

void LoginDialog::action(const std::string& eventId)
{
    if (eventId == "ok") {
        const std::string user = userField->getText();
        logger->log("Network: Username is %s", user.c_str());

        // Store config settings
        config.setValue("remember", keepCheck->isMarked());
        if (keepCheck->isMarked()) {
            config.setValue("username", user);
            config.setValue("host", serverField->getText());
        } else {
            config.setValue("username", "");
        }

        // Check login
        if (user.length() == 0) {
            wrongLoginNotice = new OkDialog("Error", "Enter your username first", &wrongUsernameNoticeListener);
        } else {
            switch (attemptLogin(user, passField->getText()) )
            {
                case LOGIN_UNKNOWN_ERROR:
                    wrongLoginNotice = new OkDialog("Error", "Unknown Error.");
                default:
                break;

                case LOGIN_WRONG_PASSWORD:
                    wrongLoginNotice = new OkDialog("Error", "Wrong Password", &wrongPasswordNoticeListener);
                break;

                case LOGIN_UNREGISTERED_ID:
                    wrongLoginNotice = new OkDialog("Error", "Unregistered ID.");
                break;

                case LOGIN_EXPIRED:
                    wrongLoginNotice = new OkDialog("Error", "This ID is expired");
                break;

                case LOGIN_REJECTED:
                    wrongLoginNotice = new OkDialog("Error", "Rejected from server");
                break;

                case LOGIN_BLOCKED:
                    wrongLoginNotice = new OkDialog("Error", "You have been blocked by the GM Team");
                break;

                case LOGIN_USERNAME_TWICE:
                    wrongLoginNotice = new OkDialog("Error", "The username does already exist.");
                break;


            }
            close_session();
        }
    } else if (eventId == "cancel") {
        state = EXIT;
    } else if (eventId == "register") {
        const std::string user = userField->getText();
        logger->log("LoginDialog::register Username is %s", user.c_str());

        // Store config settings
        config.setValue("remember", keepCheck->isMarked());
        if (keepCheck->isMarked()) {
            config.setValue("username", user);
        } else {
            config.setValue("username", "");
        }

        // Check login
        if (user.length() == 0) // No username
        {
            wrongLoginNotice = new OkDialog("Error", "Enter your username first.", &wrongUsernameNoticeListener);
        }
        else if (user.length() < LEN_MIN_USERNAME) // Name too short
        {
            std::stringstream errorMessage;
            errorMessage << "The username needs to be at least ";
            errorMessage << LEN_MIN_USERNAME;
            errorMessage << " characters long.";
            wrongLoginNotice = new OkDialog("Error", errorMessage.str(), &wrongUsernameNoticeListener);
        }
        else if (user.length() > LEN_MAX_USERNAME - 1 ) // Name too long
        {
            std::stringstream errorMessage;
            errorMessage << "The username needs to be less than ";
            errorMessage << LEN_MAX_USERNAME;
            errorMessage << " characters long.";
            wrongLoginNotice = new OkDialog("Error", errorMessage.str(), &wrongUsernameNoticeListener);
        }
        else if (passField->getText().length() < LEN_MIN_PASSWORD) // Pass too short
        {
            std::stringstream errorMessage;
            errorMessage << "The password needs to be at least ";
            errorMessage << LEN_MIN_PASSWORD;
            errorMessage << " characters long.";
            wrongLoginNotice = new OkDialog("Error", errorMessage.str(), &wrongPasswordNoticeListener);
        }
        else if (passField->getText().length() > LEN_MAX_PASSWORD - 1 ) // Pass too long
        {
            std::stringstream errorMessage;
            errorMessage << "The password needs to be less than ";
            errorMessage << LEN_MAX_PASSWORD;
            errorMessage << " characters long.";
            wrongLoginNotice = new OkDialog("Error", errorMessage.str(), &wrongPasswordNoticeListener);
        }
        else // If no errors, register the new user.
        {
            attemptLogin(user + "_M", passField->getText());
            close_session();
        }
    }
}


void login()
{
    loginDialog = new LoginDialog();

    while (state == LOGIN)
    {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    state = EXIT;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        state = EXIT;
                    }
                    break;
            }

            guiInput->pushInput(event);
        }

        gui->logic();

        graphics->drawImage(login_wallpaper, 0, 0);
        gui->draw();
        graphics->updateScreen();
    }

    delete loginDialog;
}


int attemptLogin(const std::string& user, const std::string& pass) {
    username = user;
    password = pass;
    int ret;

    // Connect to login server
    ret = open_session(
            config.getValue("host", "animesites.de").c_str(),
            (short)config.getValue("port", 0));

    if (ret == SOCKET_ERROR) {
        state = LOGIN;
        wrongLoginNotice = new OkDialog("Error", "Unable to connect to login server");
        return LOGIN_NO_CONNECTION;
    }

    // Send login infos

    WFIFOW(0) = net_w_value(0x0064);

    WFIFOL(2) = 0;

    memcpy(WFIFOP(6), username.c_str(), LEN_MAX_USERNAME - 1);
    memcpy(WFIFOP(30), password.c_str(), LEN_MAX_PASSWORD - 1);
    WFIFOB(54) = 0;
    WFIFOSET(55);

    while ((in_size < 23) || (out_size > 0)) {
        flush();
    }

    // Login ok
    if (RFIFOW(0) == 0x0069) {
        while (in_size < RFIFOW(2)) {
            flush();
        }
        n_server = (RFIFOW(2) - 47) / 32;
        server_info = (SERVER_INFO*)malloc(sizeof(SERVER_INFO) * n_server);
        account_ID = RFIFOL(8);
        session_ID1 = RFIFOL(4);
        session_ID2 = RFIFOL(12);
        sex = RFIFOB(46);
        for (int i = 0; i < n_server; i++) {
            server_info[i].address = RFIFOL(47 + 32 * i);
            memcpy(server_info[i].name, RFIFOP(47 + 32 * i + 6), 20);
            server_info[i].online_users = RFIFOW(47 + 32 * i + 26);
            server_info[i].port = RFIFOW(47 + 32 * i + 4);
            state = CHAR_SERVER;
        }
        logger->log("Network: Server: %s (%s:%d)", server_info[0].name,
                iptostring(server_info[0].address),
                server_info[0].port);
        RFIFOSKIP(RFIFOW(2));
        return LOGIN_OK;
    }
    else if (RFIFOW(0) == 0x006a) {
        logger->log("Login::error code: %i", RFIFOB(2));
        ret = 0;
        switch (RFIFOB(2)) {
            case 0:
                ret = LOGIN_UNREGISTERED_ID;
                break;
            case 1:
                ret = LOGIN_WRONG_PASSWORD;
                break;
            case 2:
                ret = LOGIN_EXPIRED;
                break;
            case 3:
                ret = LOGIN_REJECTED;
                break;
            case 4:
                ret = LOGIN_BLOCKED;
                break;
            case 9:
                ret = LOGIN_USERNAME_TWICE;
                break;
        }
        state = LOGIN;
        RFIFOSKIP(23);
        return ret;
    }
    else {
        state = LOGIN;
        return LOGIN_UNKNOWN_ERROR;
    }
    // Todo: add other packets, also encrypted
}
