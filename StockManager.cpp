#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;
class Item
{
private:
	int id;
	char name[100];
	char gender;
	char type[100];
	int price;
	long stock;
public:
	void disp()
	{
		cout<<"---------------------------------------------\n";
		cout<<"Id:\t"<<id<<"\n";
		cout<<"Name:\t"<<name<<"\n";
		cout<<"Gender:\t"<<gender<<"\n";
		cout<<"Type:\t"<<type<<"\n";
		cout<<"Price:\t"<<price<<"\n";
		cout<<"Stock:\t"<<stock<<"\n";
		cout<<"---------------------------------------------\n";
	}
	Item()
	{}
	Item(int id, char name[], char gender, char type[], int price, long stock)
	{
		this->id = id;
		strcpy(this->name, name);
		this->gender = gender;
		strcpy(this->type, type);
		this->price = price;
		this->stock = stock;
	}
	bool change_stock(int amount)
	{
		if(amount + stock < 0)
		{
			cout<<"Not enough stock to sell\n";
			return false;
		}
		else
			stock += amount;
		return true;
	}
	int get_id()
	{return id;}
	long get_stock()
	{return stock;}
};
class User
{
private:
	char username[100];
	char password[100];
	char name[100];
	int access;
public:
	User(char user[], char pass[], char n[], int acc)
	{
		strcpy(username, user);
		strcpy(password, pass);
		strcpy(name, n);
		access = acc;
	}
	User()
	{}
	User(int a)
	{
	}
	char *get_name()
	{
		return name;
	}
	void init(char user[], char pass[], char n[], int acc)
	{
		strcpy(username, user);
		strcpy(password, pass);
		strcpy(name, n);
		access = acc;
	}
	bool check(char user[], char pass[])
	{
		if(strcmp(user, username) == 0 && strcmp(password, pass) == 0)
			return true;
		return false;
	}
	bool match(char user[])
	{
		return !strcmp(user, username);
	}
	bool hasAccess(int level)
	{
		return access >= level;
	}
};
class ItemDB
{
private:
	Item *items;
	int n;
public:
	ItemDB()
	{
		n = 0;
		items = NULL;
	}
	void init()
	{
		if (ifstream("items.dat", ios::binary) == false)
		{
			fstream f("items.dat", ios::binary | ios::out);
			f.close();
		}
		fstream f("items.dat", ios::binary | ios::in | ios::out);
		f.seekp(0, ios::end);
		n = f.tellp() / sizeof(Item);
		if(n == 0)
			return;
		items = new Item[n];
		f.seekp(0, ios::beg);
		for(int i = 0; i < n; ++i)
		{
			f.read((char *)&items[i], sizeof(Item));
		}
		f.close();
	}
	bool updateStock(int id, int amount)
	{
		int i;
		bool flag = false;
		for(i = 0; i < n; ++i)
		{
			if(items[i].get_id() == id)
			{
				flag = items[i].change_stock(amount);
			}
		}
		if(flag == true)
		{
			fstream f("items.dat", ios::binary | ios::in | ios::out);
			f.seekp(i * sizeof(Item), ios::beg);
			f.write((char *)&items[i], sizeof(Item));
			f.close();
		}
		return flag;
	}
	void viewStock()
	{
		if(n == 0)
			cout<<"No stock\n";
		for(int i = 0; i < n; ++i)
			items[i].disp();
	}
};
class UserDB
{
private:
	User *users;
	int n;
public:
	UserDB()
	{
		n = 0;
		users = NULL;
	}
	void init()
	{
		if (ifstream("users.dat", ios::binary) == false)
		{
			fstream f("users.dat", ios::binary | ios::out);
			User def("root\0", "root\0", "Default\0", 15);
			f.write((char*)&def, sizeof(def));
			f.close();
		}
		fstream f("users.dat", ios::binary | ios::in | ios::out);
		f.seekp(0, ios::end);
		n = f.tellp() / sizeof(User);
		users = new User[n];
		f.seekp(0, ios::beg);
		for(int i = 0; i < n; ++i)
		{
			f.read((char *)&users[i], sizeof(User));
		}
	}
	User* login(char user[], char pass[])
	{
		for(int i = 0; i < n; ++i)
		{
			if(users[i].check(user, pass))
				return &users[i];
		}
		return NULL;
	}
	bool exists(char user[])
	{
		for(int i = 0; i < n; ++i)
			if(users[i].match(user))
				return true;
		return false;
	}

};
int main()
{
	// Fill User Database
	UserDB U;
	U.init();

	// Store currently logged in user
	User *me;

	// Fill Item Database
	ItemDB I;
	I.init();

	// Make User Login
	int flag = 0;
	do
	{
		char user[100], pass[100];
		cout<<"Enter Username:\n";
		cin.get(user, 100);
		cin.get();
		cout<<"Enter Password:\n";
		cin.get(pass, 100);
		cin.get();
		me = U.login(user, pass);
		if(me != NULL)
			flag = 1;
		else
			cout<<"Invalid login credentials\n";
	}while(flag == 0);

	// Successful login
	cout<<"Welcome, "<<me->get_name()<<".\n";
	int opt;
	do
	{
		if(me->hasAccess(1))
			cout<<"1. View stock\n";
		if(me->hasAccess(2))
			cout<<"2. Bill item\n";
		if(me->hasAccess(3))
			cout<<"3. Add stock\n";
		if(me->hasAccess(4))
			cout<<"4. Add new item\n";
		if(me->hasAccess(5))
			cout<<"5. Create new user\n";
		cout<<"0. Exit\n";
		cin>>opt;
		if(!me->hasAccess(opt))
			opt = 100;
		switch(opt)
		{
		case 1:
			I.viewStock();
			break;
		case 2:
			{
				int id, amount;
				cout<<"Enter id of item to bill\n";
				cin>>id;
				cout<<"Enter amount sold\n"; 
				cin>>amount;
				bool f = I.updateStock(id, -1 * amount);
				if(f == false)
					cout<<"Transaction failed\n";
				else
					cout<<"Transaction successful\n";
			}
			break;
		case 3:
			{
				int id, amount;
				cout<<"Enter id of item\n";
				cin>>id;
				cout<<"Enter increase in stock\n";
				cin>>amount;
				if(amount < 0)
				{
					cout<<"Error, amount must be a positive value\n";
					break;
				}
				bool f = I.updateStock(id, amount);
				if(f == false)
					cout<<"Transaction failed\n";
				else
					cout<<"Transaction successful\n";
			}
			break;
		case 4:
			{
				int id;
				cout<<"Enter ID of new item\n";
				cin>>id;
				if(I.updateStock(id, 0))
				{
					cout<<"Item already exists\n";
					break;
				}
				char name[100];
				char gender;
				char type[100];
				int price;
				long stock;
				cout<<"Enter name of product\n";
				cin>>name;
				cout<<"Enter gender of clothing (M = Male, F = Female, U = Unisex)\n";
				cin>>gender;
				cout<<"Enter product type\n";
				cin>>type;
				cout<<"Enter price\n";
				cin>>price;
				cout<<"Enter stock\n";
				cin>>stock;
				if(stock < 0)
				{
					cout<<"Error stock must be at least 0\n";
					break;
				}
				Item NEW(id, name, gender, type, price, stock);
				fstream f("items.dat", ios::binary | ios::in | ios::out);
				f.seekp(0, ios::end);
				f.write((char*)&NEW, sizeof(Item));
				f.close();
				I.init();
			}
			break;
		case 5:
			{
				char username[100], password[100], name[100];
				int access;
				cin.get();
				cout<<"Enter username\n";
				cin.get(username, 100);
				cin.get();
				if(U.exists(username))
				{
					cout<<"User exists!\n";
					break;
				}
				cout<<"Enter password\n";
				cin.get(password, 100);
				cin.get();
				cout<<"Enter name of user\n";
				cin.get(name, 100);
				cin.get();
				cout<<"Enter access privileges\n1: View Stock (Assistant)\n2: Bill Items (Cashier)\n3: Add Stock (Stock Manager)\n4: Add New Item (Fashion Curator)\n5: Create New User (Manager)\n";
				cin>>access;
				fstream f("users.dat", ios::binary | ios::in | ios::out);
				f.seekp(0, ios::end);
				User NEW(username, password, name, access);
				f.write((char*)&NEW, sizeof(User));
				f.close();
				U.init();
			}
			break;
		case 0:
			break;
		default:
			cout<<"Option not found\n";
		}
		cout<<"\n";
	}while(opt != 0);
}
