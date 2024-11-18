#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

struct AccountNode {
    int id;
    string title;
    float balance;
    AccountNode* next;

    AccountNode(int accId, const string& accTitle, float accBalance)
        : id(accId), title(accTitle), balance(accBalance), next(NULL) {}
};

struct TransactionNode {
    int fromId;
    int toId;
    float amount;
    string description;
    string type;
    TransactionNode* next;

    TransactionNode(int from, int to, float amt, const string& desc, const string& transType)
        : fromId(from), toId(to), amount(amt), description(desc), type(transType), next(NULL) {}
};

class AccountList {
private:
    AccountNode* head;

public:
    AccountList() : head(NULL) {}

    void addAccount(int accId, const string& accTitle, float accBalance) {
        AccountNode* newNode = new AccountNode(accId, accTitle, accBalance);
        newNode->next = head;
        head = newNode;
    }

    void displayAccounts(int idToDisplay = -1) {
        AccountNode* current = head;
        cout << "Accounts:" << endl;
        while (current) {
            if (idToDisplay == -1 || current->id == idToDisplay) {
                cout << current->id << " " << current->title << " " << current->balance << endl;
                if (idToDisplay != -1) {
                    break;  // Break out of the loop if displaying a specific account
                }
            }
            current = current->next;
        }
        cout << endl;
    }

    AccountNode* getAccountById(int accId) {
        AccountNode* current = head;
        while (current) {
            if (current->id == accId) {
                return current;
            }
            current = current->next;
        }
        return NULL;
    }

    void updateAccountBalance(int accId, float newBalance) {
        AccountNode* account = getAccountById(accId);
        if (account) {
            account->balance = newBalance;
        }
    }

    AccountNode* getHead() const {
        return head;
    }
};

class TransactionList {
private:
    TransactionNode* head;

public:
    TransactionList() : head(NULL) {}

    void addTransaction(int fromId, int toId, float amount, const string& description, const string& transType) {
        TransactionNode* newNode = new TransactionNode(fromId, toId, amount, description, transType);
        newNode->next = head;
        head = newNode;
    }

    void displayTransactions(int idToDisplay = -1) {
        TransactionNode* current = head;
        cout << "Transactions:" << endl;
        while (current) {
            if (idToDisplay == -1 || current->fromId == idToDisplay || current->toId == idToDisplay) {
                cout << current->fromId << " " << current->toId << " " << current->amount << " "
                    << current->description << " " << current->type << endl;
            }
            current = current->next;
        }
        cout << endl;
    }

    TransactionNode* getLastTransaction() {
        return head;
    }
};

// to save data to file
void saveData(AccountList& accounts, TransactionList& transactions) {
    ofstream outFile("btsdata.csv");

    // to save accounts
    AccountNode* accCurrent = accounts.getHead();
    while (accCurrent) {
        outFile << "A," << accCurrent->id << "," << accCurrent->title << "," << accCurrent->balance << endl;
        accCurrent = accCurrent->next;
    }

    // to save transactions
    TransactionNode* transCurrent = transactions.getLastTransaction();
    while (transCurrent) {
        outFile << "T," << transCurrent->fromId << "," << transCurrent->toId << "," << transCurrent->amount << ","
            << transCurrent->description << "," << transCurrent->type << endl;
        transCurrent = transCurrent->next;
    }

    outFile.close();
}

// to load data from file
void loadData(AccountList& accounts, TransactionList& transactions) {
    ifstream inFile("btsdata.csv");
    string line, type;
    while (getline(inFile, line)) {
        stringstream ss(line);
        getline(ss, type, ',');  

        if (type == "A") {
            int id;
            float balance;
            string title;
            ss >> id;
            ss.ignore();  // Ignore the separator
            getline(ss, title, ',');
            ss >> balance;

            accounts.addAccount(id, title, balance);
        }
        else if (type == "T") {
            int fromId, toId;
            float amount;
            string description, transType;

            ss >> fromId;
            ss.ignore();  
            ss >> toId;
            ss.ignore();  
            ss >> amount;
            ss.ignore();  
            getline(ss, description, ',');
            getline(ss, transType);

            transactions.addTransaction(fromId, toId, amount, description, transType);
        }
    }
    inFile.close();
}

bool authenticatePassword() {
    const string correctPassword = "dsa";
    const int maxAttempts = 5;
    int attempts = 0;

    while (attempts < maxAttempts) {
        string password;
        cout << "Enter the password to display all accounts/transactions: ";
        cin >> password;

        if (password == correctPassword) {
            return true;
        }
        else {
            cout << "Incorrect password. Please try again." << endl;
            attempts++;
        }
    }

    cout << "Too many incorrect attempts. Exiting program." << endl;
    exit(1);
}

int main() {
    AccountList accounts;
    TransactionList transactions;
    // loading data
    loadData(accounts, transactions);

    int choice;
    bool processing = false;

    do {
        if (!processing) {
            cout << "1. Create Account\n2. Perform Transaction\n3. Display Accounts\n4. Display Transactions\n0. Exit" << endl;
            cin >> choice;

            // Check for valid input
            while (cin.fail()) {
                cout << "Invalid input. Please enter a valid integer." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> choice;
            }
        }
        else {
            cout << "System is busy processing. Please wait..." << endl;

            continue;
        }

        switch (choice) {
        case 1: {
            int id;
            float balance;
            string title;
            cout << "Enter Account ID: ";
            cin >> id;

            while (cin.fail()) {
                cout << "Invalid input. Please enter a valid integer for Account ID." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> id;
            }
            cin.ignore();

            cout << "Enter Account Title: ";
            getline(cin, title);

            cout << "Enter Account Balance: ";
            cin >> balance;

            while (cin.fail()) {
                cout << "Invalid input. Please enter a valid float for Account Balance." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> balance;
            }

            accounts.addAccount(id, title, balance);
            saveData(accounts, transactions);
            break;
        }
        case 2: {
            int fromId, toId;
            float amount;
            string description;

            // Input validation and loop until valid sender account ID is entered
            while (true) {
                cout << "Enter Sender Account ID: ";
                if (!(cin >> fromId)) {
                    cout << "Invalid input. Please enter a valid integer for Sender Account ID." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    break;
                }
            }

            while (true) {
                cout << "Enter Receiver Account ID: ";
                if (!(cin >> toId)) {
                    cout << "Invalid input. Please enter a valid integer for Receiver Account ID." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    break;
                }
            }

            // Input validation and loop until valid transaction amount is entered
            while (true) {
                cout << "Enter Transaction Amount: ";
                if (!(cin >> amount)) {
                    cout << "Invalid input. Please enter a valid float for Transaction Amount." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    break;
                }
            }

            cout << "Enter Transaction Description: ";
            cin.ignore();
            getline(cin, description);

            AccountNode* fromAccount = accounts.getAccountById(fromId);
            AccountNode* toAccount = accounts.getAccountById(toId);

            if (fromAccount && toAccount && fromAccount->balance >= amount) {
                string Gettype;
                cout << "Enter Transaction Type: ";
                getline(cin, Gettype);
                transactions.addTransaction(fromId, toId, amount, description, Gettype);

                // Update account balances
                fromAccount->balance -= amount;
                toAccount->balance += amount;

                // Update in the l-list
                accounts.updateAccountBalance(fromAccount->id, fromAccount->balance);
                accounts.updateAccountBalance(toAccount->id, toAccount->balance);

                cout << "Transaction successful." << endl;
            }
            else {
                cout << "Transaction failed. Insufficient funds or invalid account ID." << endl;
            }
            break;
        }
        case 3: {
            int idToDisplay;

            while (true) {
                cout << "Enter Account ID to display (or enter 0 to display all with password): ";
                if (cin >> idToDisplay) {
                    if (idToDisplay >= 0) {
                        break;
                    }
                }

                cout << "Invalid input. Please enter a valid integer (or 0)." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            if (idToDisplay == 0) {
                if (authenticatePassword()) {
                    accounts.displayAccounts();
                }
            }
            else {
                accounts.displayAccounts(idToDisplay);
            }
            break;
        }
        case 4: {
            int idToDisplay;

            while (true) {
                cout << "Enter Account ID to display (or enter 0 to display all with password): ";
                if (cin >> idToDisplay) {
                    if (idToDisplay >= 0) {
                        break;
                    }
                }

                cout << "Invalid input. Please enter a valid integer (or 0)." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            if (idToDisplay == 0) {
                if (authenticatePassword()) {
                    transactions.displayTransactions();
                }
            }
            else {
                transactions.displayTransactions(idToDisplay);
            }
            break;
        }
        case 0:
            // Saving data to file before exiting
            saveData(accounts, transactions);
            cout << "Exiting program." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}

