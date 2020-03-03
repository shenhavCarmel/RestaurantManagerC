#include <vector>
#include <iostream>
#include "../include/Customer.h"
//
// Created by nofarcar@wincs.cs.bgu.ac.il on 11/11/18.
//


Customer::Customer(std::string c_name, int c_id) : name(c_name), id(c_id), isPresent(true), numOfOrders(0) {}

std::string Customer::getName() const {
    return name;
}

int Customer::getId() const {
    return id;
}

std::string Customer::toString() const {
    return (name + ", " + std:: to_string(id));
}

VegetarianCustomer::VegetarianCustomer(std::string name, int id) : Customer(name,id) {}

Customer *VegetarianCustomer::clone() {
    return new VegetarianCustomer(*this);
}

std::vector<int> VegetarianCustomer::order(const std::vector<Dish> &menu) {

    std::vector<int> customerOrder;

    int maxBvg = -1;
    int minVeg = -1;
    bool foundVeg = false;
    bool foundBvg = false;

    for (int i=0; i<static_cast<int>(menu.size()) && (!foundBvg || !foundVeg); i++) {
        if (!foundVeg && menu[i].getType() == VEG) {
            minVeg = i;
            foundVeg = true;
        }
        else if (!foundBvg && menu[i].getType() == BVG) {
            maxBvg = i;
            foundBvg = true;
        }

    }

    if (foundBvg && foundVeg) {
        for (int i=0; i<static_cast<int>(menu.size()); i++){
            if (menu[i].getType() == VEG){
                if (menu[i].getId() < menu[minVeg].getId())
                    minVeg = i;
            }
            else if (menu[i].getType() == BVG){
                if (menu[i].getPrice() > menu[maxBvg].getPrice())
                    maxBvg = i;
                else if (menu[i].getPrice() == menu[maxBvg].getPrice()) {
                    if (menu[i].getId() < menu[maxBvg].getId())
                        maxBvg = i;
                }
            }
        }
        customerOrder.push_back(menu[minVeg].getId());
        customerOrder.push_back(menu[maxBvg].getId());

        // update ordered count
        numOfOrders++;
    }
    return customerOrder;
}

std::string VegetarianCustomer::toString() const {
    return (this->getName() + ",veg");
}

CheapCustomer::CheapCustomer(std::string name, int id) : Customer(name,id) {}

Customer *CheapCustomer::clone() {
    return new CheapCustomer(*this);
}

std::vector<int> CheapCustomer::order(const std::vector<Dish> &menu) {

    std::vector<int> customerOrder;

    if (numOfOrders == 0) {
        int minDish = 0;
        for (int i=0; i<static_cast<int>(menu.size()); i++){
            if (menu[i].getPrice() < menu[minDish].getPrice())
                minDish = i;
            else if (menu[i].getPrice() == menu[minDish].getPrice()) {
                if (menu[i].getId() < menu[minDish].getId())
                    minDish = i;
            }
        }
        customerOrder.push_back(menu[minDish].getId());

        // update ordered count
        numOfOrders++;
    }

    return customerOrder;
}

std::string CheapCustomer::toString() const {
    return (this->getName() + ",chp");
}

SpicyCustomer::SpicyCustomer(std::string name, int id) : Customer(name,id)  {}

Customer *SpicyCustomer::clone() {
    return new SpicyCustomer(*this);
}

std::vector<int> SpicyCustomer::order(const std::vector<Dish> &menu) {

    std::vector<int> customerOrder;

    // if this is his first order
    if (numOfOrders == 0) {
        int maxSpicy = 0;
        for (int i=0; i<static_cast<int>(menu.size()); i++) {
            if (menu[i].getType() == SPC) {
                if (menu[i].getPrice() > menu[maxSpicy].getPrice())
                    maxSpicy = i;
                else if (menu[i].getPrice() == menu[maxSpicy].getPrice()) {
                    if (menu[i].getId() < menu[maxSpicy].getId())
                        maxSpicy = i;
                }
            }
        }
        customerOrder.push_back(menu[maxSpicy].getId());

        // update ordered count
        numOfOrders++;
    }
    else {
        int minAlcohol = 0;
        for (int i=0; i<static_cast<int>(menu.size()); i++) {
            if (menu[i].getType() != ALC) {
                if (menu[i].getPrice() < menu[minAlcohol].getPrice())
                    minAlcohol = i;
                else if (menu[i].getPrice() == menu[minAlcohol].getPrice()) {
                    if (menu[i].getId() < menu[minAlcohol].getId())
                        minAlcohol = i;
                }
            }
        }
        customerOrder.push_back(menu[minAlcohol].getId());
    }
    return customerOrder;
}

std::string SpicyCustomer::toString() const {
    return (this->getName() + ",spc");
}

AlchoholicCustomer::AlchoholicCustomer(std::string name, int id) : Customer(name,id), lastPricePaid(0), lastDishOrdered(0){}

Customer *AlchoholicCustomer::clone() {
    return new AlchoholicCustomer(*this);
}


std::vector<int> AlchoholicCustomer::order(const std::vector<Dish> &menu) {

    int minAlc=0;
    std::vector<int> customerOrder;
    for (int i=lastDishOrdered + 1 ; i<static_cast<int>(menu.size()); i++) {
        if (menu[i].getType() == ALC && menu[i].getPrice() == lastPricePaid) {
            minAlc = i;
            lastPricePaid = menu[minAlc].getPrice();
            lastDishOrdered = menu[minAlc].getId();
            customerOrder.push_back(menu[minAlc].getId());
            return customerOrder;
        }
    }
    bool foundALC = false;
    for (int j=0; j<static_cast<int>(menu.size()) && !foundALC; j++) {
        if (menu[j].getType() == ALC && menu[j].getPrice() > lastPricePaid) {
            minAlc = j;
            foundALC = true;
        }
    }
    if (!foundALC) {
        return customerOrder;
    }
    for (int i=minAlc ; i<static_cast<int>(menu.size()); i++) {
        if (menu[i].getType() == ALC && menu[i].getPrice() > lastPricePaid && menu[i].getPrice()<menu[minAlc].getPrice()) {
            minAlc = i;
        }
    }
    lastPricePaid = menu[minAlc].getPrice();
    lastDishOrdered = minAlc;
    customerOrder.push_back(minAlc);
    return customerOrder;

}

std::string AlchoholicCustomer::toString() const {
    return (this->getName() + ",alc");
}