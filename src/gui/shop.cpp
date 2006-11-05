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

#include "shop.h"
#include "../utils/tostring.h"
#include "../resources/itemmanager.h"

ShopItems::~ShopItems()
{
    clear();
}

int ShopItems::getNumberOfElements()
{
    return mItemsShop.size();
}

std::string ShopItems::getElementAt(int i)
{
    return mItemsShop.at(i).name;
}

void ShopItems::addItem(short id, int price)
{
    ITEM_SHOP item_shop;

    item_shop.name = itemDb->getItemInfo(id).getName()
                     + " " + toString(price) + " GP";
    item_shop.price = price;
    item_shop.id = id;
    item_shop.image = itemDb->getItemInfo(id).getImage();

    mItemsShop.push_back(item_shop);
}

ITEM_SHOP ShopItems::at(int i)
{
    return mItemsShop.at(i);
}

void ShopItems::push_back(ITEM_SHOP item_shop)
{
    mItemsShop.push_back(item_shop);
}

void ShopItems::clear()
{
    mItemsShop.clear();
}
