#include<stdio.h>
#include<string.h>

#include "ItemToPurchase.h"

void MakeItemBlank(ItemToPurchase* item) {
    item->itemQuantity = 0;
    item->itemPrice = 0;
    strcpy(item->itemName, "none");
}

void PrintItemCost(ItemToPurchase item) {
    int total = item.itemQuantity * item.itemPrice;
    printf("%s %d @ $%d = $%d", item.itemName, item.itemQuantity, item.itemPrice, total);
}

