#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include <iterator>
using namespace std;

struct Date {
    int year;
    int month;
};

//Update CurrentDate.txt
void updateDate(int NumOfMonths)
{
    int NumOfYears = 0;
    ifstream F("CurrentDate.txt");
    Date current;
    F >> current.month >> current.year;
    F.close();

    current.month = current.month + NumOfMonths;
    if (current.month > 12) 
    {
        NumOfYears = current.month / 12;
        current.year = current.year + NumOfYears;
        current.month = current.month % 12;
    }

    ofstream Out("CurrentDate.txt");
    Out << current.month << " " << current.year;
    Out.close();

}

void getDate(Date& current)
{
    ifstream F("CurrentDate.txt");
    F >> current.month >> current.year;
    F.close();
}

class Transaction
{
private:
    float Value;
    static int NextID;
    int id;
    float Tax;
    string Ttype;
public:

    float getValue() { return this->Value; }
    void setTax(float tax) { this->Tax = tax; }
    float getTax() { return this->Tax; }
    void setTtype(string temp) { this->Ttype = temp; }

    Transaction()
    {
        this->Value = 0;
        this->Tax = 0;
    }

    Transaction(float value)
    {
        this->Value = value;
        this->id = NextID++;
        this->Tax = 0;
    }

    void Display()
    {
        cout << "ID: " << this->id << " || Type: " << this->Ttype << " || Valoare: " << this->Value << " || Tax: " << this->Tax << endl;
    }
};

int Transaction::NextID = 1;

class AbstractAccount {
protected:
    string ClientName;
    float Balance;
    Date OpeningDate;
    vector<Transaction> TransHistory;
    int ID;
public:
    virtual void addTransaction(Transaction t) 
    {
        this->Balance = this->Balance + t.getValue();
        TransHistory.push_back(t);

    }

    vector<Transaction> getAllTransactions() {
        vector<Transaction> temp;
        temp = TransHistory;
        return temp;
    }

    float getBalance() { return this->Balance; }
    void setID(int id) { this->ID = id; }

    bool BalanceCheck(Transaction T)
    {
        if (this->Balance + T.getValue() < 0)
        {
            cout << "Not enough money" << endl;
            return false;
        }
        else return true;

    }


    virtual void DisplayTransactions()
    {
        for (int i = 0; i < TransHistory.size(); i++)
        {
            TransHistory[i].Display();
        }
    }

    AbstractAccount() {
        this->ClientName = "No Owner";
        this->Balance = 0;
        OpeningDate.year = 1900;
        OpeningDate.month = 1;
    }

    AbstractAccount(string name, float bal, Date od) {
        this->ClientName = name;
        this->Balance = bal;
        this->OpeningDate = od;
    }

    virtual void InterestUpdate(int n) {}

    virtual void Display()
    {
        cout << "------------------------------------" << endl;
        cout << "Account ID: " << ID << endl;
        cout << "Client name: " << ClientName << endl;
        cout << "Account Balance: " << Balance << endl;
        cout << "Month and Year: " << OpeningDate.month << "/" << \
            OpeningDate.year << endl;
    }

    friend ostream& operator<<(ostream& out, const AbstractAccount& acc);
};

ostream& operator<<(ostream& out, const AbstractAccount& acc)
{
    out << "------------------------------------" << endl;
    out << "Client name: " << acc.ClientName << endl;
    out << "Account Balance: " << acc.Balance << endl;
    out << "Month and Year: " << acc.OpeningDate.month << "/" << \
        acc.OpeningDate.year << endl;
    out << "------------------------------------" << endl;
    return out;
}

class CurrentAccount : public AbstractAccount {
protected:
    int FreeTransactions;
public:

    void addTransaction(Transaction t) {
        this->Balance = this->Balance + t.getValue();
        if (t.getValue() < 0) {
            if (FreeTransactions == 0)
            {
                cout << "No more Free Transactions. Tax = " << t.getValue() / 100 << endl;
                t.setTax(t.getValue() / 100);
                this->Balance += t.getTax();
                t.setTtype("Debit");
            }
            else
            {
                cout << "Test";
                updateFree();
                t.setTtype("Debit");
            }
        }
        else
            t.setTtype("Credit");

        TransHistory.push_back(t);
    }

    int getFreebies() { return this->FreeTransactions; }

    void DisplayTransactions()
    {
        for (int i = 0; i < TransHistory.size(); i++)
        {
            TransHistory[i].Display();
        }
    }

    void updateFree()
    {
        this->FreeTransactions = this->FreeTransactions - 1;
    }

    CurrentAccount() : AbstractAccount() {
        this->FreeTransactions = 0;
    }

    CurrentAccount(string name, float bal, Date od, int MaxTransactions) :
        AbstractAccount(name, bal, od) {
        this->FreeTransactions = MaxTransactions;
    }

    void Display()
    {
        AbstractAccount::Display();
        cout << "Max number of free transactions: " << FreeTransactions << endl;
        cout << "------------------------------------" << endl;
    }

    friend istream& operator>>(istream& in, CurrentAccount& acc);
    friend ostream& operator<<(ostream& out, const CurrentAccount& acc);
};

ostream& operator<<(ostream& out, const CurrentAccount& acc)
{
    out << "------------------------------------" << endl;
    out << "Client name: " << acc.ClientName << endl;
    out << "Account Balance: " << acc.Balance << endl;
    out << "Month and Year: " << acc.OpeningDate.month << "/" << \
        acc.OpeningDate.year << endl;
    out << "Max number of free transactions: " << acc.FreeTransactions << endl;
    out << "------------------------------------" << endl;
    return out;
}

istream& operator>>(istream& in, CurrentAccount& acc)
{
    fstream data("CurrentDate.txt");
    cout << "Client Name: ";
    getchar();
    getline(in, acc.ClientName);
    cout << "Initial deposit: ";
    in >> acc.Balance;
    try
    {
        if (acc.Balance <= 0) throw 1;
    }
    catch(int x)
    {
        while (acc.Balance <= 0)
        {
            cout << "Initial Balance error.\nNew value >0 ";
            in >> acc.Balance;
        }
    }
    data >> acc.OpeningDate.month;
    data >> acc.OpeningDate.year;
    cout << "Maximum free allowed transactions: ";
    in >> acc.FreeTransactions;
    try
    {
        if (acc.FreeTransactions <= 0) throw 1;
    }
    catch (int x)
    {
        while (acc.FreeTransactions <= 0)
        {
            cout << "Free Transactions error.\nNew value >0 ";
            in >> acc.Balance;
        }
    }
    return in;
}

class EconomyAccount : public AbstractAccount {
protected:
    float Interest;
    int Months;
    //Numarul de luni trecute de la ultimul update al soldului
    int MonPassed;
public:

    float getInterest() { return this->Interest; }

    void addTransaction(Transaction t) {
        this->Balance = this->Balance + t.getValue();
        if (t.getValue() > 0)
        {
            t.setTtype("Credit");
        }
        if (t.getValue() < 0)
        {
            t.setTtype("Debit");
        }
        TransHistory.push_back(t);
    }

    void DisplayTransactions()
    {
        for (int i = 0; i < TransHistory.size(); i++)
        {
            TransHistory[i].Display();
        }
    }

    void InterestUpdate(int n)
    {
        this->MonPassed = this->MonPassed + n;
        while (Months <= MonPassed)
        {
            Transaction temp = Transaction(this->Balance * this->Interest);
            cout << this->ClientName << " Old balance: " << this->Balance << endl;
            cout << this->ClientName << " New balance: " << this->Balance + this->Balance * this->Interest << endl;
            this->MonPassed = this->MonPassed - Months;
            addTransaction(temp);
        }
    }

    EconomyAccount() : AbstractAccount() {
        this->Interest = 0;
        this->Months = 0;
    }

    EconomyAccount(string name, float bal, Date od, float inter, \
        int months) : AbstractAccount(name, bal, od) {
        this->Interest = inter;
        this->Months = months;
    }

    void Display()
    {
        AbstractAccount::Display();
        cout << "Interest: " << Interest << " ||| Every " << Months << " Months" << endl;
        cout << "------------------------------------" << endl;
    }

    friend istream& operator>>(istream& in, EconomyAccount& acc);
    friend ostream& operator<<(ostream& out, const EconomyAccount& acc);
};

ostream& operator<<(ostream& out, const EconomyAccount& acc)
{
    out << "------------------------------------" << endl;
    out << "Client name: " << acc.ClientName << endl;
    out << "Account Balance: " << acc.Balance << endl;
    out << "Month and Year: " << acc.OpeningDate.month << "/" << \
        acc.OpeningDate.year << endl;
    out << "Interest: " << acc.Interest << " || Every " << acc.Months << " Months" << endl;
    out << "------------------------------------" << endl;
    return out;
}

istream& operator>>(istream& in, EconomyAccount& acc) {
    fstream data("CurrentDate.txt");
    cout << "Client Name: ";
    getchar();
    getline(in, acc.ClientName);
    cout << "Initial deposit: ";
    in >> acc.Balance;
    try
    {
        if (acc.Balance <= 0) throw 1;
    }
    catch (int x)
    {
        while (acc.Balance <= 0)
        {
            cout << "Initial Balance error.\nNew value >0: ";
            in >> acc.Balance;
        }
    }
    data >> acc.OpeningDate.month;
    data >> acc.OpeningDate.year;
    cout << "Interest in 0.XX format: ";
    in >> acc.Interest; 
    try
    {
        if (acc.Interest < 0) throw 1;
        if (acc.Interest > 1) throw 1;
    }
    catch (int x)
    {
        while (acc.Interest < 0 || acc.Interest > 1)
        {
            cout << "Interest error.\nNew value 0<Value<1: ";
            in >> acc.Interest;
        }
    }
    cout << "Months Interest: ";
    in >> acc.Months;
    try
    {
        if (acc.Months != 3 && acc.Months != 6 && acc.Months != 12) throw 1;
    }
    catch (int x)
    {
        while (acc.Months != 3 && acc.Months != 6 && acc.Months != 12)
        {
            cout << "Interest error.\nNew value 3 6 or 12: ";
            in >> acc.Months;
        }
    }
    return in;
}

template<class T>
class AccAdministration {
protected:
    unordered_map<int, vector<Transaction>> AccData;
    int ID;
    static int NextID;
    vector<T> Accounts;
public:
    int getID() { return this->ID; }
    T getAccount(int i) { return Accounts[i]; }


    //Update Interest for every EconomyAccount
    void EconomyTimeUp(int n)
    {
        updateDate(n);
        for (int i = 0; i < Accounts.size(); i++)
        {
            if (typeid(*Accounts[i]) == typeid(EconomyAccount))
            {
                Accounts[i]->InterestUpdate(n);
                auto it = AccData.begin();
                for (it; it != AccData.end(); it++) {
                    if (it->first == i+1) {
                        cout << "test";
                        it->second = Accounts[i]->getAllTransactions();
                    }
                }
            }
        }
    }


    void addAccount(T acc)
    {
        this->ID = NextID++;
        AccData.insert(pair<int, vector<Transaction>>(ID, vector<Transaction>()));
        Accounts.push_back(acc);
        acc->setID(ID);
    }

    void DisplayAccounts()
    {
        for (int i = 0; i < Accounts.size(); i++)
        {
            Accounts[i]->Display();
        }
    }

    void addTransaction(int id, Transaction& t)
    {
        if (Accounts[id - 1]->BalanceCheck(t))
        {
            auto it = AccData.begin();
            for (it; it != AccData.end(); it++) {
                if (it->first == id) {
                    it->second.push_back(t);
                    Accounts[id - 1]->addTransaction(t);
                }
            }
        }
        else
            cout << "Error";
    }

    void DisplayTransactions(int id)
    {
        auto it = AccData.begin();
        for (it; it != AccData.end(); it++)
        {
            if (it->first == id) {
                vector<Transaction>::iterator tt;
                for (tt = it->second.begin(); tt != it->second.end(); tt++)
                {
                    tt->Display();
                }
            }
        }
    }
};


template<class T>
int AccAdministration<T>::NextID = 1;

void menu()
{
    cout << "1 - Economy Account" << endl;
    cout << "2 - Current Account" << endl;
    cout << "3 - Update Date" << endl;
    cout << "4 - Display Accounts" << endl;
    cout << "5 - New Transaction" << endl;
    cout << "6 - All transactions for ID" << endl;
    cout << "7 - Menu Display" << endl;
    cout << "0 - Exit" << endl;
}

int main() 
{
    AccAdministration<AbstractAccount*> BancaX;
    int x = 7;
    Date Present;
    getDate(Present);
    cout << "Present: ";
    cout << Present.month << "/" << Present.year << endl;

    while (x != 0)
    {
        if (x == 1)
        {
            auto* ea = new EconomyAccount();
            cin >> *ea;
            BancaX.addAccount(ea);
        }
        else if (x == 2)
        {
            auto* ca = new CurrentAccount();
            cin >> *ca;
            BancaX.addAccount(ca);
        }
        else if (x == 3)
        {
            int v;
            cout << "Months: ";
            cin >> v;
            try {
                if (v <= 0) throw 1;
            }
            catch (int x)
            {
                while (v <= 0)
                {
                    cout << "Can't be <=0\nNew value: ";
                    cin >> v;
                }
            }
            BancaX.EconomyTimeUp(v);
        }
        else if (x == 4)
        {
            BancaX.DisplayAccounts();
        }
        else if (x == 5)
        {
            int val, tempid;
            cout << "ID of Account: ";
            cin >> tempid;
            cout << "Transaction value: ";
            cin >> val;
            Transaction tempt = Transaction(val);
            BancaX.addTransaction(tempid, tempt);
        }
        else if (x == 6)
        {
            int tempid;
            cout << "ID of Acount: ";
            cin >> tempid;
            BancaX.DisplayTransactions(tempid);
        }
        else if (x == 7)
        {

            cout << endl << endl;
            menu();
        }
        else cout << "Wrong Input" << endl;

        cout << "Choice: ";
        cin >> x;
    }
    

}
