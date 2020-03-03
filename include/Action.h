#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include "Customer.h"

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};

//Forward declaration
class Restaurant;

class BaseAction{
public:
    BaseAction();
    ActionStatus getStatus() const;
    virtual void act(Restaurant& restaurant)=0;
    virtual std::string toString() const=0;
    std::string getFullStatus();
    virtual BaseAction* clone() = 0;
    virtual ~BaseAction();
protected:
    void complete();
    void error(std::string errorMsg);
    std::string getErrorMsg() const;
private:
    std::string errorMsg;
    ActionStatus status;
    std::string fullStatus;
};


class OpenTable : public BaseAction {
public:
    OpenTable(int id, std::vector<Customer *> &customersList);
    void act(Restaurant &restaurant);
    std::string toString() const;
    OpenTable* clone();

    // 1. Destructor
    virtual ~OpenTable();

    // 2. Copy Constructor
    OpenTable(const OpenTable &other);

    // 3. Move Constructor
    OpenTable(OpenTable&& other);

private:
    const int tableId;
    std::vector<Customer *> customers;
 //   std::string tableString;
};


class Order : public BaseAction {
public:
    Order(int id);
    void act(Restaurant &restaurant);
    std::string toString() const;
    Order* clone();
private:
    const int tableId;
};


class MoveCustomer : public BaseAction {
public:
    MoveCustomer(int src, int dst, int customerId);
    void act(Restaurant &restaurant);
    std::string toString() const;
    MoveCustomer* clone();
private:
    const int srcTable;
    const int dstTable;
    const int id;
};


class Close : public BaseAction {
public:
    Close(int id);
    void act(Restaurant &restaurant);
    std::string toString() const;
    Close* clone();
private:
    const int tableId;
};


class CloseAll : public BaseAction {
public:
    CloseAll();
    void act(Restaurant &restaurant);
    std::string toString() const;
    CloseAll* clone();
private:
};


class PrintMenu : public BaseAction {
public:
    PrintMenu();
    void act(Restaurant &restaurant);
    std::string toString() const;
    PrintMenu* clone();
private:
};


class PrintTableStatus : public BaseAction {
public:
    PrintTableStatus(int id);
    void act(Restaurant &restaurant);
    std::string toString() const;
    PrintTableStatus* clone();
private:
    const int tableId;
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();
    void act(Restaurant &restaurant);
    std::string toString() const;
    PrintActionsLog* clone();
private:
};


class BackupRestaurant : public BaseAction {
public:
    BackupRestaurant();
    void act(Restaurant &restaurant);
    std::string toString() const;
    BackupRestaurant* clone();
private:
};


class RestoreResturant : public BaseAction {
public:
    RestoreResturant();
    void act(Restaurant &restaurant);
    std::string toString() const;
    RestoreResturant* clone();

};


#endif