#ifndef TABLE_H_
#define TABLE_H_

#include <vector>
#include "Customer.h"
#include "Dish.h"

typedef std::pair<int, Dish> OrderPair;

class Table{
public:

    Table(int t_capacity);
    int getCapacity() const;
    void addCustomer(Customer* customer);
    void removeCustomer(int id);
    Customer* getCustomer(int id);
    std::vector<Customer*>& getCustomers();
    std::vector<OrderPair>& getOrders();
    void order(const std::vector<Dish> &menu);
    void openTable();
    void closeTable();
    int getBill();
    bool isOpen();
    void clear();
    Table* clone();
    void copy(int other_capacity, bool other_open, std::vector<Customer*> other_customersList, std::vector<OrderPair> other_orderList, std::vector<OrderPair> getCurrOrderList);
    std::vector<OrderPair> getCurrOrderList();

    // 1. Destructor
    ~Table();

    // 2. Copy Constructor
    Table(Table &other);

    // 3. Move Constructor
    Table(Table &&other);

    // 4. Copy Assignment
    Table& operator=(const Table &other);

    // 5. Move Assignment
    Table& operator=(Table &&other);

private:
    int capacity;
    bool open;
    std::vector<Customer*> customersList;
    std::vector<OrderPair> orderList; //A list of pairs for each order in a table - (customer_id, Dish)
    std::vector<OrderPair> currOrderList;
};

#endif