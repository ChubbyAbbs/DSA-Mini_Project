#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <algorithm> // for max
using namespace std;

struct Maintenance {
    string date;
    string description;
    double cost;
    Maintenance* left;
    Maintenance* right;
    int height;
    Maintenance(string d, string desc, double c = 0)
        : date(d), description(desc), cost(c), left(nullptr), right(nullptr), height(1) {}
};

class MaintenanceAVLTree {
public:
    Maintenance* root;
    MaintenanceAVLTree() : root(nullptr) {}

    int height(Maintenance* N) {
        return N ? N->height : 0;
    }

    int getBalance(Maintenance* N) {
        return N ? height(N->left) - height(N->right) : 0;
    }

    Maintenance* rotateRight(Maintenance* y) {
        Maintenance* x = y->left;
        Maintenance* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    Maintenance* rotateLeft(Maintenance* x) {
        Maintenance* y = x->right;
        Maintenance* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Maintenance* insert(Maintenance* node, string date, string description, double cost) {
        if (!node) return new Maintenance(date, description, cost);
        if (date < node->date)
            node->left = insert(node->left, date, description, cost);
        else if (date > node->date)
            node->right = insert(node->right, date, description, cost);
        else
            return node;

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && date < node->left->date) return rotateRight(node);
        if (balance < -1 && date > node->right->date) return rotateLeft(node);
        if (balance > 1 && date > node->left->date) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && date < node->right->date) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void insert(string date, string description, double cost) {
        root = insert(root, date, description, cost);
    }

    void inOrder(Maintenance* root) {
        if (root) {
            inOrder(root->left);
            cout << "Date: " << root->date << " | Description: " << root->description << " | Cost: $" << root->cost << endl;
            inOrder(root->right);
        }
    }

    void displayRecords() {
        if (!root) {
            cout << "No maintenance records found." << endl;
        } else {
            cout << "=== Maintenance Records (by Date) ===" << endl;
            inOrder(root);
            cout << "======================================" << endl;
        }
    }

    Maintenance* search(Maintenance* node, string date) {
        if (!node || node->date == date) return node;
        if (date < node->date) return search(node->left, date);
        return search(node->right, date);
    }

    void searchRecord(string date) {
        Maintenance* result = search(root, date);
        if (result) {
            cout << "=== Record Found ===" << endl;
            cout << "Date: " << result->date << " | Description: " << result->description << " | Cost: $" << result->cost << endl;
            cout << "====================" << endl;
        } else {
            cout << "No maintenance record found for date: " << date << endl;
        }
    }

    void deleteRecord(string date) {
        root = deleteNode(root, date);
    }

    Maintenance* minValueNode(Maintenance* node) {
        Maintenance* current = node;
        while (current->left)
            current = current->left;
        return current;
    }

    Maintenance* deleteNode(Maintenance* root, string date) {
        if (!root) return root;

        if (date < root->date)
            root->left = deleteNode(root->left, date);
        else if (date > root->date)
            root->right = deleteNode(root->right, date);
        else {
            if (!root->left || !root->right) {
                Maintenance* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else {
                    *root = *temp;
                }
                delete temp;
            } else {
                Maintenance* temp = minValueNode(root->right);
                root->date = temp->date;
                root->description = temp->description;
                root->cost = temp->cost;
                root->right = deleteNode(root->right, temp->date);
            }
        }

        if (!root) return root;

        root->height = 1 + max(height(root->left), height(root->right));
        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0) return rotateRight(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0) return rotateLeft(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    void exportRecordsToFile(string filename) {
        ofstream file(filename);
        if (file.is_open()) {
            file << "=== Maintenance Records (by Date) ===" << endl;
            exportRecords(root, file);
            file << "======================================" << endl;
            file.close();
            cout << "Records exported to " << filename << endl;
        } else {
            cout << "Failed to open file: " << filename << endl;
        }
    }

    void exportRecords(Maintenance* root, ofstream& file) {
        if (root) {
            exportRecords(root->left, file);
            file << "Date: " << root->date << " | Description: " << root->description << " | Cost: $" << root->cost << endl;
            exportRecords(root->right, file);
        }
    }

    void updateRecord(string date, string newDescription, double newCost) {
        Maintenance* node = search(root, date);
        if (node) {
            node->description = newDescription;
            node->cost = newCost;
            cout << "Record updated for " << date << "." << endl;
        } else {
            cout << "No record found for date: " << date << endl;
        }
    }
};

bool isValidChoice(int& choice) {
    if (cin.fail()) {
        cout << "Invalid input! Please enter a number between 1 and 7." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool isValidCost(double& cost) {
    if (cin.fail()) {
        cout << "Invalid input for cost! Please enter a valid numeric value." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

int main() {
    map<string, MaintenanceAVLTree> vehicleRecords;
    int choice;
    string model, date, description, filename;
    double cost;

    do {
        cout << "\n--- Car Maintenance Record System ---" << endl;
        cout << "1. Add Maintenance Record\n2. Display All Records\n3. Search for a Record by Date\n4. Delete a Record by Date\n5. Export Records to File\n6. Update Maintenance Record\n7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (!isValidChoice(choice)) continue;

        switch (choice) {
        case 1:
            cout << "Enter vehicle model: ";
            cin >> model;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter maintenance date (YYYY-MM-DD): ";
            cin >> date;
            cin.ignore();
            cout << "Enter maintenance description: ";
            getline(cin, description);
            cout << "Enter cost: ";
            cin >> cost;
            if (!isValidCost(cost)) continue;
            vehicleRecords[model].insert(date, description, cost);
            cout << "Maintenance record added successfully for " << model << "." << endl;
            break;

        case 2:
            cout << "Enter vehicle model to display records: ";
            cin >> model;
            if (vehicleRecords.find(model) != vehicleRecords.end())
                vehicleRecords[model].displayRecords();
            else
                cout << "No records found for model: " << model << endl;
            break;

        case 3:
            cout << "Enter vehicle model: ";
            cin >> model;
            cout << "Enter date to search (YYYY-MM-DD): ";
            cin >> date;
            if (vehicleRecords.find(model) != vehicleRecords.end())
                vehicleRecords[model].searchRecord(date);
            else
                cout << "No records found for model: " << model << endl;
            break;

        case 4:
            cout << "Enter vehicle model: ";
            cin >> model;
            cout << "Enter date to delete (YYYY-MM-DD): ";
            cin >> date;
            if (vehicleRecords.find(model) != vehicleRecords.end()) {
                vehicleRecords[model].deleteRecord(date);
                cout << "Record deleted if it existed." << endl;
            } else {
                cout << "No records found for model: " << model << endl;
            }
            break;

        case 5:
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter filename to export records: ";
            getline(cin, filename);
            cout << "Enter vehicle model: ";
            getline(cin, model);
            if (vehicleRecords.find(model) != vehicleRecords.end())
                vehicleRecords[model].exportRecordsToFile(filename);
            else
                cout << "No records found for model: " << model << endl;
            break;

        case 6:
            cout << "Enter vehicle model: ";
            cin >> model;
            cout << "Enter date of record to update (YYYY-MM-DD): ";
            cin >> date;
            cin.ignore();
            cout << "Enter new description: ";
            getline(cin, description);
            cout << "Enter new cost: ";
            cin >> cost;
            if (!isValidCost(cost)) continue;
            if (vehicleRecords.find(model) != vehicleRecords.end())
                vehicleRecords[model].updateRecord(date, description, cost);
            else
                cout << "No records found for model: " << model << endl;
            break;

        case 7:
            cout << "Exiting the system. Thank you!" << endl;
            break;

        default:
            cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 7);

    return 0;
}
