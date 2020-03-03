//
// Created by nofarcar@wincs.cs.bgu.ac.il on 11/11/18.
//
#include "../include/Table.h"
#include "../include/Customer.h"

#include <iostream>
#include <vector>

Table::Table(int t_capacity): capacity(t_capacity), open(false),customersList(0), orderList(), currOrderList(){}

// Copy constructor
Table::Table(Table &other) : capacity(other.capacity), open(other.open), customersList(), orderList(), currOrderList()  {
    //customersList.clear();

    for(Customer* cust:other.customersList){
        customersList.push_back(cust->clone());
    }
    orderList.clear();
    for(OrderPair order:other.orderList){
        orderList.push_back(order);
    }
    currOrderList.clear();
    for(OrderPair order:other.currOrderList){
        currOrderList.push_back(order);
    }
}

// Copy assignment
Table& Table::operator=(const Table &other) {
    if (this != &other)
    {
        clear();

        capacity = other.getCapacity();
        open = other.open;

        orderList.clear();
        for(OrderPair order:other.orderList){
            orderList.push_back(order);
        }

        customersList.clear();
        for(Customer* cust:other.customersList){
            customersList.push_back(cust->clone());
        }

        currOrderList.clear();
        for(OrderPair currOrder:other.currOrderList){
            currOrderList.push_back(currOrder);
        }

    }
    return *this;
}

// Destructor
Table::~Table() { clear(); }

void Table::clear()
{
    for(Customer* cust : customersList){
        delete cust;
    }
    customersList.clear();
    orderList.clear();
    currOrderList.clear();
}


// Move Constructor
Table::Table(Table &&other) : capacity(std::move(other.capacity)), open(std::move(other.open)), customersList(), orderList(), currOrderList()  {

    //clear();
    orderList.clear();
    for(OrderPair order:other.orderList){
        orderList.push_back(order);
    }
    currOrderList.clear();
    for(OrderPair currOrder:other.currOrderList){
        currOrderList.push_back(currOrder);
    }
     /*
    customersList.clear();
    for (int i=0; i < static_cast<int>(other.getCustomers().size()); i++) {
        customersList.push_back((other.getCustomers()[i]));
    }*/

    customersList = other.getCustomers();

    other.customersList.clear();
    other.orderList.clear();
    other.currOrderList.clear();
}

// Move Assignment
Table& Table::operator=(Table&& other) {

    if (this != &other) {

        clear();
        orderList.clear();
        for(OrderPair order:other.orderList){
            orderList.push_back(order);
        }
        currOrderList.clear();
        for(OrderPair currOrder:other.currOrderList){
            currOrderList.push_back(currOrder);
        }

        /*
        customersList.clear();
        for (int i=0; i< static_cast<int>(other.getCustomers().size()); i++) {
            customersList.push_back((other.getCustomers()[i]));
        }*/

        customersList = other.getCustomers();

        open = other.isOpen();
        capacity = other.getCapacity();

        other.customersList.clear();
        other.orderList.clear();
        other.currOrderList.clear();


    }
    return *this;
}

Table* Table::clone() {
    return new Table(*this);
}

void Table::closeTable() {
    this->open = false;
    clear();
}

std::vector<OrderPair> Table::getCurrOrderList() {
    return currOrderList;
}

int Table::getBill() {
    int bill = 0;
    for (int i=0; i< static_cast<int>(orderList.size()); i++) {
        bill = bill + orderList[i].second.getPrice();
    }
    return bill;
}

int Table::getCapacity() const {
    return this->capacity;
}

Customer * Table::getCustomer(int id) {
    for (int i=0; i< static_cast<int>(customersList.size()); i++) {
        if (customersList[i]->getId() == id)
            return customersList[i];
    }

    return nullptr;
}

std::vector<Customer *> & Table::getCustomers() {
    return this->customersList;
}

std::vector<OrderPair> & Table::getOrders() {
    return this->orderList;
}

bool Table::isOpen() {
    return open;
}

void Table::openTable() {
    open = true;
}

void Table::order(const std::vector <Dish> &menu) {
    currOrderList.clear();

    // get the new orders from all customers in the table
    for (int i=0; i< static_cast<int>(customersList.size()); i++) {
        std::vector<int> customerDishes;
        customerDishes = customersList[i]->order(menu);
        for (int j=0; j< static_cast<int>(customerDishes.size()); j++) {
            OrderPair tmpPair(customersList[i]->getId(),menu[customerDishes[j]]);
            orderList.push_back(tmpPair);
            currOrderList.push_back(tmpPair);
        }
    }
}

void Table::removeCustomer(int id) {
    bool found = false;
    for (int i = 0; i < static_cast<int> (customersList.size()) && !found; i++) {
        if (customersList[i]->getId() == id) {
            //delete customersList[i];
            customersList.erase(customersList.begin()+i);
            found = true;
        }
    }
    ///
    std::vector <OrderPair> tempV;
    for(OrderPair o : orderList){
        if(static_cast<int>(o.first) != id){
            tempV.push_back(o);
        }
    }

    orderList.clear();
    for(OrderPair o:tempV){
        orderList.push_back(o);
    }
    tempV.clear();

    if(customersList.empty()){
        this->open = false;
    }
}

void Table::addCustomer(Customer* customer) {
    if (capacity - customersList.size() <=0) {
        std::cout << "full capacity, cannot add customer" << std::endl;
    }
    else {
        this->customersList.push_back(customer);
    }

}