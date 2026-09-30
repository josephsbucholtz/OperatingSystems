#ifndef ITEM_TO_PURCHASE_H
#define ITEM_TO_PURCHASE_H

typedef struct {
    int itemPrice;
    int itemQuantity;
    char itemName[50];
} ItemToPurchase;

void MakeItemBlank(ItemToPurchase* item);
void PrintItemCost(ItemToPurchase item);

#endif 
