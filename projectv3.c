#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct F_item {
    char name[100];
    float price;
    int quantity;
    struct F_item* next;
};

struct Order {
    char item_name[100];
    int quantity;
    struct Order* next;
};

struct contract {
    struct Order* orders;
    struct contract* next;
};

struct F_item* createF_item(const char* name, float price, int quantity) {
    struct F_item* newItem = (struct F_item*)malloc(sizeof(struct F_item));
    strncpy(newItem->name, name, sizeof(newItem->name) - 1);
    newItem->name[sizeof(newItem->name) - 1] = '\0'; // Ensure null-terminated string
    newItem->price = price;
    newItem->quantity = quantity;
    newItem->next = NULL;
    return newItem;
}

struct Order* createOrder(const char* name, int quantity) {
    struct Order* newOrder = (struct Order*)malloc(sizeof(struct Order));
    strncpy(newOrder->item_name, name, sizeof(newOrder->item_name) - 1);
    newOrder->item_name[sizeof(newOrder->item_name) - 1] = '\0'; // Ensure null-terminated string
    newOrder->quantity = quantity;
    newOrder->next = NULL;
    return newOrder;
}

void displayMenu(struct F_item* menu) {
    struct F_item* current = menu;
    printf("Food Menu:\n");
    while (current != NULL) {
        printf("%s - Price: %.2f - Quantity in Stock: %d\n", current->name, current->price, current->quantity);
        current = current->next;
    }
    printf("\n");
}

struct Order* placeOrder(struct F_item* menu, struct Order* orders) {
    displayMenu(menu);

    char item_name[100];
    int quantity;
    printf("Enter the name of the item you want to order: ");
    scanf("%s", item_name);
    printf("Enter the quantity: ");
    scanf("%d", &quantity);

    struct F_item* currentItem = menu;
    while (currentItem != NULL && strcmp(currentItem->name, item_name) != 0) {
        currentItem = currentItem->next;
    }

    if (currentItem == NULL) {
        printf("Item not found in the menu.\n");
        return orders;
    }

    if (quantity > currentItem->quantity) {
        printf("Insufficient quantity in stock.\n");
        return orders;
    }

    currentItem->quantity -= quantity;

    struct Order* newOrder = createOrder(item_name, quantity);
    if (orders == NULL) {
        orders = newOrder;
    } else {
        struct Order* lastOrder = orders;
        while (lastOrder->next != NULL) {
            lastOrder = lastOrder->next;
        }
        lastOrder->next = newOrder;
    }

    printf("Order placed successfully!\n");
    return orders;
}

void displayOrder(struct Order* orders) {
    printf("Order Summary:\n");
    struct Order* currentOrder = orders;
    while (currentOrder != NULL) {
        printf("%s - Quantity: %d\n", currentOrder->item_name, currentOrder->quantity);
        currentOrder = currentOrder->next;
    }
    printf("\n");
}

void displayTotalCost(struct Order* orders, struct F_item* menu) {
    float totalCost = 0;
    struct Order* currentOrder = orders;

    while (currentOrder != NULL) {
        struct F_item* currentItem = menu;
        while (currentItem != NULL && strcmp(currentItem->name, currentOrder->item_name) != 0) {
            currentItem = currentItem->next;
        }

        if (currentItem != NULL) {
            totalCost += currentItem->price * currentOrder->quantity;
        }

        currentOrder = currentOrder->next;
    }

    printf("Total Cost: %.2f\n", totalCost);
}

struct Order* remove_item(struct Order* orders, struct F_item* menu) {
    if (orders == NULL) {
        printf("Order is empty. Nothing to remove.\n");
        return orders;
    }

    char item_name[100];
    printf("Enter the name of the item to remove: ");
    scanf("%s", item_name);

    struct Order* currentOrder = orders;
    struct Order* prevOrder = NULL;

    while (currentOrder != NULL && strcmp(currentOrder->item_name, item_name) != 0) {
        prevOrder = currentOrder;
        currentOrder = currentOrder->next;
    }

    if (currentOrder == NULL) {
        printf("Item not found in the order.\n");
        return orders;
    }

    struct F_item* currentItem = menu;
    while (currentItem != NULL && strcmp(currentItem->name, item_name) != 0) {
        currentItem = currentItem->next;
    }

    currentItem->quantity += currentOrder->quantity;

    if (prevOrder == NULL) {
        orders = currentOrder->next;
    } else {
        prevOrder->next = currentOrder->next;
    }

    free(currentOrder);
    printf("%s removed from the order.\n", item_name);

    return orders;
}

void generateAndPrintBill(struct Order* orders, struct F_item* menu) {
    printf("Generating Bill...\n");
    printf("\n========== Bill ==========\n");

    float totalCost = 0;
    struct Order* currentOrder = orders;

    while (currentOrder != NULL) {
        struct F_item* currentItem = menu;
        while (currentItem != NULL && strcmp(currentItem->name, currentOrder->item_name) != 0) {
            currentItem = currentItem->next;
        }

        if (currentItem != NULL) {
            float itemCost = currentItem->price * currentOrder->quantity;
            totalCost += itemCost;
            printf("%s - Quantity: %d - Cost: %.2f\n", currentOrder->item_name, currentOrder->quantity, itemCost);
        }

        currentOrder = currentOrder->next;
    }

    printf("\nTotal Cost: %.2f\n", totalCost);
    printf("==========================\n");
}

struct F_item* addNewItemToMenu(struct F_item* menu) {
    char name[100];
    float price;
    int quantity;

    printf("Enter the name of the new item: ");
    scanf("%s", name);
    printf("Enter the price of the new item: ");
    scanf("%f", &price);
    printf("Enter the quantity in stock for the new item: ");
    scanf("%d", &quantity);

    struct F_item* newItem = createF_item(name, price, quantity);

    if (menu == NULL) {
        menu = newItem;
    } else {
        struct F_item* lastItem = menu;
        while (lastItem->next != NULL) {
            lastItem = lastItem->next;
        }
        lastItem->next = newItem;
    }

    printf("%s added to the menu.\n", name);
    return menu;
}

struct contract* createcontract(struct Order* orders) {
    struct contract* newcontract = (struct contract*)malloc(sizeof(struct contract));
    newcontract->orders = orders;
    newcontract->next = NULL;
    return newcontract;
}

void pushcontract(struct contract** stack, struct Order* orders) {
    struct contract* newcontract = createcontract(orders);
    newcontract->next = *stack;
    *stack = newcontract;
}

struct Order* pop_stack(struct contract** stack) {
    if (*stack == NULL) {
        return NULL; // Stack is empty
    }

    struct contract* topcontract = *stack;
    *stack = topcontract->next;
    struct Order* orders = topcontract->orders;
    free(topcontract);
    return orders;
}

void displayOrderHistory(struct contract* stack) {
    printf("Order History:\n");
    int orderNumber = 1;
    while (stack != NULL) {
        printf("Order %d:\n", orderNumber++);
        displayOrder(stack->orders);
        stack = stack->next;
    }
    printf("\n");
}

int main() {
    struct F_item* menu = createF_item("Burger", 5.0, 10);
    menu->next = createF_item("Pizza", 8.0, 8);
    menu->next->next = createF_item("Pasta", 7.0, 12);

    struct Order* orders = NULL;
    struct contract* stack = NULL;

    while (1) {
        int choice;
        printf("Restaurant Management System\n");
        printf("1. Place an order\n");
        printf("2. Display order\n");
        printf("3. Calculate total cost\n");
        printf("4. Remove an item from the order\n");
        printf("5. Generate and Print Bill\n");
        printf("6. View Available Items and Quantities\n");
        printf("7. Add a new item to the menu\n");
        printf("8. View Order History\n");
        printf("9. Undo Last Order\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                orders = placeOrder(menu, orders);
                break;
            case 2:
                displayOrder(orders);
                break;
            case 3:
                displayTotalCost(orders, menu);
                break;
            case 4:
                orders = remove_item(orders, menu);
                break;
            case 5:
                generateAndPrintBill(orders, menu);
                break;
            case 6:
                displayMenu(menu);
                break;
            case 7:
                menu = addNewItemToMenu(menu);
                break;
            case 8:
                displayOrderHistory(stack);
                break;
            case 9:
                if (stack != NULL) {
                    struct Order* removedOrder = pop_stack(&stack);
                    if (removedOrder != NULL) {
                        printf("Undoing the last order...\n");
                        orders = removedOrder;
                        displayOrder(orders);
                    }
                } else {
                    printf("No orders to undo.\n");
                }
                break;
            case 10:
                printf("Exiting... Thank you!\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }

        pushcontract(&stack, orders);
    }

    return 0;
}
