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

#ifndef TRADE_H
#define TRADE_H

#include <memory>

#include <guichan/actionlistener.hpp>
#include <guichan/selectionlistener.hpp>

#include "window.h"

#include "../guichanfwd.h"

class Inventory;
class Item;
class ItemContainer;
#ifdef EATHENA_SUPPORT
class Network;
#endif
class ScrollArea;

/**
 * Trade dialog.
 *
 * \ingroup Interface
 */
class TradeWindow : public Window, gcn::ActionListener, gcn::SelectionListener
{
    public:
        /**
         * Constructor.
         */
#ifdef TMWSERV_SUPPORT
        TradeWindow();
#else
        TradeWindow(Network *network);
#endif

        /**
         * Destructor.
         */
        ~TradeWindow();

        /**
         * Displays expected money in the trade window.
         */
        void setMoney(int quantity);

        /**
         * Add an item to the trade window.
         */
        void addItem(int id, bool own, int quantity);

        /**
         * Reset both item containers
         */
        void reset();

#ifdef EATHENA_SUPPORT
        /**
         * Add an item to the trade window.
         */
        void addItem(int id, bool own, int quantity, bool equipment);

        /**
         * Change quantity of an item.
         */
        void changeQuantity(int index, bool own, int quantity);

        /**
         * Increase quantity of an item.
         */
        void increaseQuantity(int index, bool own, int quantity);
#endif

        /**
         * Player received ok message from server
         */
#ifdef TMWSERV_SUPPORT
        void receivedOk();
#else
        void receivedOk(bool own);
#endif

        /**
         * Send trade packet.
         */
        void tradeItem(Item *item, int quantity);

        /**
         * Updates the labels and makes sure only one item is selected in
         * either my inventory or partner inventory.
         */
        void valueChanged(const gcn::SelectionEvent &event);

        /**
         * Called when receiving actions from the widgets.
         */
        void action(const gcn::ActionEvent &event);

        /**
         * Closes the Trade Window, as well as telling the server that the
         * window has been closed.
         */
        void close();

    private:
#ifdef TMWSERV_SUPPORT
        enum Status
        {
            PREPARING, /**< Players are adding items. */
            PROPOSING, /**< Local player is proposing a trade. */
            ACCEPTING  /**< Distant player is proposing a trade. */
        };

        /**
         * Sets the current status of the trade.
         */
        void setStatus(Status);
#endif

#ifdef EATHENA_SUPPORT
        Network *mNetwork;
#endif

        typedef const std::auto_ptr<Inventory> InventoryPtr;
        InventoryPtr mMyInventory;
        InventoryPtr mPartnerInventory;

        ItemContainer *mMyItemContainer;
        ItemContainer *mPartnerItemContainer;

        gcn::Label *mMoneyLabel;
        gcn::Button *mTradeButton;
#ifdef EATHENA_SUPPORT
        gcn::Button *mAddButton;
        gcn::Button *mOkButton;
#endif
        gcn::TextField *mMoneyField;

#ifdef TMWSERV_SUPPORT
        Status mStatus;
#else
        bool mOkOther, mOkMe;
#endif
};

extern TradeWindow *tradeWindow;

#endif
