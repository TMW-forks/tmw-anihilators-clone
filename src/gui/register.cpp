/*
 *  The Mana World
 *  Copyright (C) 2004  The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "register.h"

#include <string>
#include <sstream>

#include <guichan/widgets/label.hpp>

#include "../main.h"
#include "../configuration.h"
#include "../log.h"
#include "../logindata.h"

#include "button.h"
#include "checkbox.h"
#include "login.h"
#include "passwordfield.h"
#include "radiobutton.h"
#include "textfield.h"
#include "ok_dialog.h"

#include "widgets/layout.h"

#include "../utils/gettext.h"
#include "../utils/strprintf.h"

/**
 * Listener used while dealing with wrong data. It is used to direct the focus
 * to the field which contained wrong data when the Ok button was pressed on
 * the error notice.
 */
class WrongDataNoticeListener : public gcn::ActionListener {
    public:
        void setTarget(gcn::TextField *textField);
        void action(const gcn::ActionEvent &event);
    private:
        gcn::TextField *mTarget;
};

void WrongDataNoticeListener::setTarget(gcn::TextField *textField)
{
    mTarget = textField;
}

void WrongDataNoticeListener::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "ok")
    {
        mTarget->requestFocus();
    }
}


RegisterDialog::RegisterDialog(LoginData *loginData):
    Window(_("Register")),
    mWrongDataNoticeListener(new WrongDataNoticeListener),
    mLoginData(loginData)
{
    gcn::Label *userLabel = new gcn::Label(_("Name:"));
    gcn::Label *passwordLabel = new gcn::Label(_("Password:"));
    gcn::Label *confirmLabel = new gcn::Label(_("Confirm:"));
    gcn::Label *serverLabel = new gcn::Label(_("Server:"));
    mUserField = new TextField(loginData->username);
    mPasswordField = new PasswordField(loginData->password);
    mConfirmField = new PasswordField;
    mServerField = new TextField(loginData->hostname);
    mMaleButton = new RadioButton(_("Male"), "sex", true);
    mFemaleButton = new RadioButton(_("Female"), "sex", false);
    mRegisterButton = new Button(_("Register"), "register", this);
    mCancelButton = new Button(_("Cancel"), "cancel", this);

    ContainerPlacer place;
    place = getPlacer(0, 0);
    place(0, 0, userLabel);
    place(0, 1, passwordLabel);
    place(0, 2, confirmLabel);
    place(1, 3, mMaleButton);
    place(2, 3, mFemaleButton);
    place(0, 4, serverLabel);
    place(1, 0, mUserField, 3).setPadding(2);
    place(1, 1, mPasswordField, 3).setPadding(2);
    place(1, 2, mConfirmField, 3).setPadding(2);
    place(1, 4, mServerField, 3).setPadding(2);
    place = getPlacer(0, 2);
    place(1, 0, mRegisterButton);
    place(2, 0, mCancelButton);
    reflowLayout(250, 0);

    mUserField->addKeyListener(this);
    mPasswordField->addKeyListener(this);
    mConfirmField->addKeyListener(this);
    mServerField->addKeyListener(this);

    /* TODO:
     * This is a quick and dirty way to respond to the ENTER key, regardless of
     * which text field is selected. There may be a better way now with the new
     * input system of Guichan 0.6.0. See also the login dialog.
     */
    mUserField->setActionEventId("register");
    mPasswordField->setActionEventId("register");
    mConfirmField->setActionEventId("register");
    mServerField->setActionEventId("register");
    mUserField->addActionListener(this);
    mPasswordField->addActionListener(this);
    mConfirmField->addActionListener(this);
    mServerField->addActionListener(this);

    setLocationRelativeTo(getParent());
    setVisible(true);
    mUserField->requestFocus();
    mUserField->setCaretPosition(mUserField->getText().length());

    mRegisterButton->setEnabled(canSubmit());
}

RegisterDialog::~RegisterDialog()
{
    delete mWrongDataNoticeListener;
}

void RegisterDialog::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "cancel")
    {
        state = LOGIN_STATE;
    }
    else if (event.getId() == "register" && canSubmit())
    {
        const std::string user = mUserField->getText();
        logger->log("RegisterDialog::register Username is %s", user.c_str());

        std::string errorMsg;
        int error = 0;

        if (user.length() < LEN_MIN_USERNAME)
        {
            // Name too short
            errorMsg = strprintf
                (_("The username needs to be at least %d characters long."),
                 LEN_MIN_USERNAME);
            error = 1;
        }
        else if (user.length() > LEN_MAX_USERNAME - 1 )
        {
            // Name too long
            errorMsg = strprintf
                (_("The username needs to be less than %d characters long."),
                 LEN_MAX_USERNAME);
            error = 1;
        }
        else if (mPasswordField->getText().length() < LEN_MIN_PASSWORD)
        {
            // Pass too short
            errorMsg = strprintf
                (_("The password needs to be at least %d characters long."),
                 LEN_MIN_PASSWORD);
            error = 2;
        }
        else if (mPasswordField->getText().length() > LEN_MAX_PASSWORD - 1 )
        {
            // Pass too long
            errorMsg = strprintf
                (_("The password needs to be less than %d characters long."),
                 LEN_MAX_PASSWORD);
            error = 2;
        }
        else if (mPasswordField->getText() != mConfirmField->getText())
        {
            // Password does not match with the confirmation one
            errorMsg = _("Passwords do not match.");
            error = 2;
        }

        if (error > 0)
        {
            if (error == 1)
            {
                mWrongDataNoticeListener->setTarget(this->mUserField);
            }
            else if (error == 2)
            {
                // Reset password confirmation
                mPasswordField->setText("");
                mConfirmField->setText("");

                mWrongDataNoticeListener->setTarget(this->mPasswordField);
            }

            OkDialog *mWrongRegisterNotice =
                new OkDialog(_("Error"), errorMsg);
            mWrongRegisterNotice->addActionListener(mWrongDataNoticeListener);
        }
        else
        {
            // No errors detected, register the new user.
            mRegisterButton->setEnabled(false);

            mLoginData->hostname = mServerField->getText();
            mLoginData->port = (short) config.getValue("port", 0);
            mLoginData->username = mUserField->getText();
            mLoginData->password = mPasswordField->getText();
            mLoginData->username += mFemaleButton->isSelected() ? "_F" : "_M";
            mLoginData->registerLogin = true;

            state = ACCOUNT_STATE;
        }
    }
}

void RegisterDialog::keyPressed(gcn::KeyEvent &keyEvent)
{
    mRegisterButton->setEnabled(canSubmit());
}

bool RegisterDialog::canSubmit() const
{
    return !mUserField->getText().empty() &&
           !mPasswordField->getText().empty() &&
           !mConfirmField->getText().empty() &&
           !mServerField->getText().empty() &&
           state == REGISTER_STATE;
}
