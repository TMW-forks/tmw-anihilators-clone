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
 *  $Id $
 */

#include "trade.h"

#include <sstream>

#include <guichan/widgets/label.hpp>

#include "button.h"
#include "chat.h"
#include "inventorywindow.h"
#include "item_amount.h"
#include "itemcontainer.h"
#include "scrollarea.h"
#include "textfield.h"

#include "../inventory.h"
#include "../item.h"

#include "../net/network.h"

#include "../resources/iteminfo.h"

TradeWindow::TradeWindow():
    Window("Trade: You")
{
    setContentSize(322, 150);

    addButton = new Button("Add");
    okButton = new Button("Ok");
    cancelButton = new Button("Cancel");
    tradeButton = new Button("Trade");

    myInventory = new Inventory();
    partnerInventory = new Inventory();

    myItemContainer = new ItemContainer(myInventory);
    myItemContainer->setPosition(2, 2);

    myScroll = new ScrollArea(myItemContainer);
    myScroll->setPosition(8, 8);

    partnerItemContainer = new ItemContainer(partnerInventory);
    partnerItemContainer->setPosition(2, 58);

    partnerScroll = new ScrollArea(partnerItemContainer);
    partnerScroll->setPosition(8, 64);

    moneyLabel = new gcn::Label("You get: 0z");
    moneyField = new TextField();

    addButton->setEventId("add");
    okButton->setEventId("ok");
    cancelButton->setEventId("cancel");
    tradeButton->setEventId("trade");

    addButton->adjustSize();
    okButton->adjustSize();
    cancelButton->adjustSize();
    tradeButton->adjustSize();

    addButton->addActionListener(this);
    okButton->addActionListener(this);
    cancelButton->addActionListener(this);
    tradeButton->addActionListener(this);

    tradeButton->setEnabled(false);

    itemNameLabel = new gcn::Label("Name:");
    itemDescriptionLabel = new gcn::Label("Description:");

    add(myScroll);
    add(partnerScroll);
    add(addButton);
    add(okButton);
    add(cancelButton);
    add(tradeButton);
    add(itemNameLabel);
    add(itemDescriptionLabel);
    add(moneyField);
    add(moneyLabel);

    moneyField->setPosition(8, getHeight() - 20);
    moneyField->setWidth(50);
    
    moneyLabel->setPosition(8+ 50 + 6, getHeight() - 20);
    
    cancelButton->setPosition(getWidth() - 48, getHeight() - 24);
    tradeButton->setPosition(cancelButton->getX() - 40
        , getHeight() - 24);
    okButton->setPosition(tradeButton->getX() - 24,
        getHeight() - 24);
    addButton->setPosition(okButton->getX() - 32,
        getHeight() - 24);

    myItemContainer->setSize(getWidth() - 24 - 12 - 1,
        (INVENTORY_SIZE * 24) / (getWidth() / 24) - 1);
    myScroll->setSize(getWidth() - 16, (getHeight() - 76) / 2);

    partnerItemContainer->setSize(getWidth() - 24 - 12 - 1,
        (INVENTORY_SIZE * 24) / (getWidth() / 24) - 1);
    partnerScroll->setSize(getWidth() - 16, (getHeight() - 76) / 2);

    itemNameLabel->setPosition(8,
        partnerScroll->getY() + partnerScroll->getHeight() + 4);
    itemDescriptionLabel->setPosition(8,
        itemNameLabel->getY() + itemNameLabel->getHeight() + 4);

    setContentSize(getWidth(), getHeight());
}

TradeWindow::~TradeWindow()
{
    delete addButton;
    delete okButton;
    delete cancelButton;
    delete tradeButton;
    delete myItemContainer;
    delete myScroll;
    delete partnerItemContainer;
    delete partnerScroll;
    delete itemNameLabel;
    delete itemDescriptionLabel;
    delete moneyField;
    delete moneyLabel;

    delete myInventory;
    delete partnerInventory;
}

void TradeWindow::addMoney(int amount)
{
    std::stringstream tempMoney;
    tempMoney << "You get: " << amount << "z";
    moneyLabel->setCaption(tempMoney.str());
    moneyLabel->adjustSize();
}

void TradeWindow::addItem(int id, bool own, int quantity,
        bool equipment)
{
    if (own) {
        myInventory->addItem(id, quantity, equipment);
    } else {
        partnerInventory->addItem(id, quantity, equipment);
    }
}

void TradeWindow::removeItem(int id, bool own)
{
    if (own) {
        myInventory->removeItem(id);
    } else {
        partnerInventory->removeItem(id);
    }
}

void TradeWindow::changeQuantity(int index, bool own, int quantity)
{
    if (own) {
        myInventory->getItem(index)->setQuantity(quantity);
    } else {
        partnerInventory->getItem(index)->setQuantity(quantity);
    }
}

void TradeWindow::increaseQuantity(int index, bool own, int quantity)
{
    if (own) {
        myInventory->getItem(index)->increaseQuantity(quantity);
    } else {
        partnerInventory->getItem(index)->increaseQuantity(quantity);
    }
}

void TradeWindow::reset()
{
    myInventory->resetItems();
    partnerInventory->resetItems();
    tradeButton->setEnabled(false);
    okButton->setEnabled(true);
    ok_other = false;
    ok_me = false;
    moneyLabel->setCaption("You get: 0z");
    moneyField->setEnabled(true);
    moneyField->setText("");
}

void TradeWindow::setTradeButton(bool enabled)
{
    tradeButton->setEnabled(enabled);
}

void TradeWindow::receivedOk(bool own)
{
    if (own) {
        ok_me = true;
        if (ok_other) {
            tradeButton->setEnabled(true);
            okButton->setEnabled(false);
        } else {
            tradeButton->setEnabled(false);
            okButton->setEnabled(false);
        }
    } else {
        ok_other = true;
        if (ok_me) {
            tradeButton->setEnabled(true);
            okButton->setEnabled(false);
        } else {
            tradeButton->setEnabled(false);
            okButton->setEnabled(true);
        }
    }
}

void TradeWindow::tradeItem(Item *item, int quantity)
{
    WFIFOW(0) = net_w_value(0x00e8);
    WFIFOW(2) = net_w_value(item->getInvIndex());
    WFIFOL(4) = net_l_value(quantity);
    WFIFOSET(8);
    while ((out_size > 0)) flush();
}

void TradeWindow::mouseClick(int x, int y, int button, int count)
{
    Window::mouseClick(x, y, button, count);

    Item *item;

    // myItems selected
    if (x >= myScroll->getX() + 3
        && x <= myScroll->getX() + myScroll->getWidth() - 10
        && y >= myScroll->getY() + 16
        && y <= myScroll->getY() + myScroll->getHeight() + 15
        && (item = myItemContainer->getItem()))
    {
            partnerItemContainer->selectNone();
    // partnerItems selected
    }
    else if (x >= partnerScroll->getX() + 3
        && x <= partnerScroll->getX() + partnerScroll->getWidth() - 20
        && y >= partnerScroll->getY() + 16
        && y <= partnerScroll->getY() + partnerScroll->getHeight() + 15
        && (item = partnerItemContainer->getItem()))
    {
            myItemContainer->selectNone();
    } else {
        return;
    }

    // Show Name and Description
    std::string SomeText;
    SomeText = "Name: " + item->getInfo()->getName();
    itemNameLabel->setCaption(SomeText);
    itemNameLabel->adjustSize();
    SomeText = "Description: " + item->getInfo()->getDescription();
    itemDescriptionLabel->setCaption(SomeText);
    itemDescriptionLabel->adjustSize();
}

void TradeWindow::action(const std::string &eventId)
{
    Item *item = inventoryWindow->getItem();

    if (eventId == "add") {
        if (!item) {
            return;
        }

        if (myInventory->getFreeSlot() < 1) {
            return;
        }

        if (myInventory->contains(item)) {
            chatWindow->chat_log("Failed adding item. You can not "
                    "overlap one kind of item on the window.", BY_SERVER);
            return;
        }

        if (item->getQuantity() == 1) {
            tradeItem(item, 1);
        }
        else {
            // Choose amount of items to trade
            new ItemAmountWindow(AMOUNT_TRADE_ADD, this, item);
        }
    }
    else if (eventId == "cancel")
    {
        WFIFOW(0) = net_w_value(0x00ed);
        WFIFOSET(2);
        while ((out_size > 0)) flush();
    }
    else if (eventId == "ok")
    {
        std::stringstream tempMoney[2];
        tempMoney[0] << moneyField->getText();
        int tempInt;
        if (tempMoney[0] >> tempInt)
        {
            tempMoney[1] << tempInt;
            moneyField->setText(tempMoney[1].str());

            WFIFOW(0) = net_w_value(0x00e8);
            WFIFOW(2) = net_w_value(0);
            WFIFOL(4) = net_l_value(tempInt);
            WFIFOSET(8);
            while ((out_size > 0)) flush();
        } else {
            moneyField->setText("");
        }
        moneyField->setEnabled(false);
        WFIFOW(0) = net_w_value(0x00eb);
        WFIFOSET(2);
        while ((out_size > 0)) flush();
    }
    else if (eventId == "trade")
    {
        WFIFOW(0) = net_w_value(0x00ef);
        WFIFOSET(2);
        while ((out_size > 0)) flush();
    }
}
