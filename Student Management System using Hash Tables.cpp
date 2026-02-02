#include <iostream>
#include <string>
#include <fstream>
using namespace std;

struct department;
struct semester;
void addStudents(string d, string s, string i, string n, float g);

struct Student {
	string id;
	string name;
	float gpa;
	Student* next;
	Student* hashNext;
	department* dept;
	semester* sem;
};

struct department {
	string dept;
	Student* head1;
	Student* tail1;
	department* next;
};

struct semester {
	string sem;
	department* head2;
	department* tail2;
	semester* next;
};

struct HashTable {
	Student** table;
	int capacity;
	int size;
};

struct User
{
	string username;
	string password;
	User* next;
};

department* head2 = nullptr;
department* tail2 = nullptr;

semester* head3 = nullptr;
semester* tail3 = nullptr;

User* headUsers = nullptr;
User* tailUsers = nullptr;

HashTable* globalHashTable = nullptr;

bool isNumber(string str)
{
	for (char c : str)
	{
		if (!isdigit(c))
			return false;
	}
	return !str.empty();
}

bool isValidGPA(float gpa)
{
	return gpa >= 0.0 && gpa <= 4.0;
}

bool isValidDepartment(string dept)
{
	return dept == "CS" || dept == "Cys" || dept == "SE" || dept == "AI";
}

bool isValidSemester(string sem)
{
	return sem == "Spring2024" || sem == "Fall2024" || sem == "Spring2025" || sem == "Fall2025";
}

bool isValidName(string name)
{
	if (name.empty())
		return false;
	
	for (char c : name)
	{
		if (!isalpha(c) && c != ' ')
			return false;
	}
	return true;
}

void loadUsers(string filename)
{
	ifstream fin(filename);
	
	if (!fin)
		return;

	string line;
	while (getline(fin, line))
	{
		size_t pos = line.find(',');
		if (pos == string::npos)
			continue;

		string uname = line.substr(0, pos);
		string pwd = line.substr(pos + 1);

		User* u = new User{ uname, pwd, nullptr };
		if (!headUsers)
			headUsers = tailUsers = u;
		else
		{
			tailUsers->next = u;
			tailUsers = u;
		}
	}

	fin.close();
}

void saveUsers(string filename)
{
	ofstream fout(filename);
	User* cur = headUsers;

	while (cur != nullptr)
	{
		fout << cur->username << "," << cur->password << "\n";
		cur = cur->next;
	}
	fout.close();
}

bool login(string username, string password)
{
	User* cur = headUsers;
	
	while (cur)
	{
		if (cur->username == username && cur->password == password)
			return true;
		
		cur = cur->next;
	}
	return false;
}

bool registerUser(string username, string password)
{
	User* cur = headUsers;
	while (cur)
	{
		if (cur->username == username)
		{
			cout << "Error: Username already exists!\n";
			
			return false;
		}
		cur = cur->next;
	}

	User* u = new User{ username, password, nullptr };
	
	if (!headUsers)
		headUsers = tailUsers = u;
	else
	{
		tailUsers->next = u;
		tailUsers = u;
	}

	saveUsers("users.txt");
	cout << "Registration successful!\n";
	
	return true;
}

void saveToFile(string filename)
{
	ofstream fout(filename);
	if (!fout)
	{
		cout << "Error: Could not open file for writing.\n";
		
		return;
	}
	
	department* dept = head2;
	while (dept != nullptr)
	{
		Student* s = dept->head1;
		while (s != nullptr)
		{
			fout << s->id << ","
				<< s->name << ","
				<< s->gpa << ","
				<< s->dept->dept << ","
				<< s->sem->sem << "\n";
			
			s = s->next;
		}
		dept = dept->next;
	}

	fout.close();
	cout << "Data saved successfully to " << filename << "\n";
}

void loadFromFile(string filename)
{
	ifstream fin(filename);
	
	if (!fin)
	{
		cout << "No saved data found.\n";
		
		return;
	}
	
	string line;
	while (getline(fin, line))
	{
			if (line.empty())
				continue;

		size_t pos1 = line.find(',');
		size_t pos2 = line.find(',', pos1 + 1);
		size_t pos3 = line.find(',', pos2 + 1);
		size_t pos4 = line.find(',', pos3 + 1);
		string id = line.substr(0, pos1);
		string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
		float gpa = stof(line.substr(pos2 + 1, pos3 - pos2 - 1));
		string dept = line.substr(pos3 + 1, pos4 - pos3 - 1);
		string sem = line.substr(pos4 + 1);

		addStudents(dept, sem, id, name, gpa);
	}
	
	fin.close();
	cout << "Data loaded successfully from " << filename << "\n\n\n";
}

int indexConverter(string id, int capacity)
{
	int hash = 0;
	for (int i = 0; i < id.length(); i++)
		hash += id[i];

	return hash % capacity;
}

HashTable* createHashTable(int capacity)
{
	HashTable* ht = new HashTable;
	ht->capacity = capacity;
	ht->size = 0;
	ht->table = new Student * [capacity];

	for (int i = 0; i < capacity; i++)
	{
		ht->table[i] = nullptr;
	}

	cout << "Hash table created with " << capacity << " slots\n\n";

	return ht;
}

Student* searchStudentHash(HashTable* ht, string id)
{
	if (ht == nullptr)
		return nullptr;

	int index = indexConverter(id, ht->capacity);

	Student* cur = ht->table[index];

	while (cur != nullptr)
	{
		if (cur->id == id)
			return cur;

		cur = cur->hashNext;
	}

	return nullptr;
}

bool deleteStudentHash(HashTable* ht, string id)
{
	if (ht == nullptr)
		return false;

	int index = indexConverter(id, ht->capacity);

	Student* cur = ht->table[index];
	Student* prev = nullptr;

	while (cur != nullptr)
	{
		if (cur->id == id)
		{
			if (prev == nullptr)
				ht->table[index] = cur->hashNext;
			else
				prev->hashNext = cur->hashNext;

			return true;
		}

		prev = cur;
		cur = cur->hashNext;
	}

	return false;
}

bool insertStudentHash(HashTable* ht, Student* student)
{
	if (ht == nullptr)
	{
		cout << "Hash Table Not Found!\n\n";

		return false;
	}

	if (searchStudentHash(ht, student->id) != nullptr)
		return false;

	int index = indexConverter(student->id, ht->capacity);

	student->hashNext = ht->table[index];
	ht->table[index] = student;
	ht->size++;

	return true;
}


void addDepartments(string d)
{
	department* temp1 = head2;
	while (temp1 != nullptr)
	{
		if (temp1->dept == d)
		{
			cout << "Department Already Exists!\n\n";

			return;
		}

		temp1 = temp1->next;
	}

	department* temp = new department;
	temp->dept = d;
	temp->head1 = nullptr;
	temp->tail1 = nullptr;
	temp->next = nullptr;

	if (head2 == nullptr)
	{
		head2 = tail2 = temp;
	}
	else
	{
		tail2->next = temp;
		tail2 = temp;
	}
}

department* findDepartment(string d)
{
	if (head2 == nullptr)
	{
		cout << "NO Department Found!\n\n";

		return nullptr;
	}

	department* temp = head2;

	while (temp != nullptr)
	{
		if (temp->dept == d)
			return temp;

		temp = temp->next;
	}

	return nullptr;
}

void addSemester(string s)
{
	semester* temp1 = head3;
	while (temp1 != nullptr)
	{
		if (temp1->sem == s)
		{
			cout << "Semester Already Exists!\n\n";

			return;
		}

		temp1 = temp1->next;
	}

	semester* temp = new semester;
	temp->sem = s;
	temp->head2 = nullptr;
	temp->tail2 = nullptr;
	temp->next = nullptr;

	if (head3 == nullptr)
	{
		head3 = tail3 = temp;
	}
	else
	{
		tail3->next = temp;
		tail3 = temp;
	}
}

semester* findSemester(string s)
{
	if (head3 == nullptr)
	{
		cout << "NO Semester Found!\n\n";

		return nullptr;
	}

	semester* temp = head3;

	while (temp != nullptr)
	{
		if (temp->sem == s)
			return temp;

		temp = temp->next;
	}

	return nullptr;
}

void addStudents(string d, string s, string i, string n, float g)
{
	semester* SEM = findSemester(s);

	if (SEM == nullptr)
	{
		cout << "Semester Not Found!\n\n";

		return;
	}

	department* DEPT = findDepartment(d);

	if (DEPT == nullptr)
	{
		cout << "Department Not Found!\n\n";

		return;
	}

	if (searchStudentHash(globalHashTable, i) != nullptr)
	{
		cout << "Student with ID " << i << " Already exists!\n\n";

		return;
	}

	Student* temp = new Student;
	temp->id = i;
	temp->name = n;
	temp->gpa = g;
	temp->sem = SEM;
	temp->dept = DEPT;
	temp->next = nullptr;
	temp->hashNext = nullptr;

	insertStudentHash(globalHashTable, temp);

	if (DEPT->head1 == nullptr)
		DEPT->head1 = DEPT->tail1 = temp;
	else
	{
		DEPT->tail1->next = temp;
		DEPT->tail1 = temp;
	}

	cout << "Student " << n << " added to " << d << " department!\n\n";
}

void update(string id)
{
	Student* temp = searchStudentHash(globalHashTable, id);

	if (temp == nullptr)
	{
		cout << "Student Not Found!\n\n";

		return;
	}

	float newGPA;

	cout << "Enter New GPA: ";
	cin >> newGPA;

	if (cin.fail())
	{
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "Error: GPA must be a number!\n";
		
		return;
	}

	if (!isValidGPA(newGPA))
	{
		cout << "Error: GPA must be between 0.0 and 4.0!\n";
		
		return;
	}

	temp->gpa = newGPA;
	cout << "GPA updated for student " << temp->name << " (ID: " << id << ")\n\n";
}

void searchStudent(string id)
{
	Student* temp = searchStudentHash(globalHashTable, id);

	if (temp == nullptr)
	{
		cout << "Student Not Found!\n\n";

		return;
	}

	cout << "\n===== Student Found =====\n"
		<< "ID: " << temp->id << "\n"
		<< "Name: " << temp->name << "\n"
		<< "GPA: " << temp->gpa << "\n"
		<< "========================\n\n";
}

void deleteStudent(string id)
{
	Student* temp = searchStudentHash(globalHashTable, id);

	if (temp == nullptr)
	{
		cout << "Student Not Found!\n\n";

		return;
	}

	department* DEPT = temp->dept;
	Student* cur = DEPT->head1;
	Student* prev = nullptr;

	while (cur != nullptr)
	{
		if (cur->id == id)
		{
			if (prev == nullptr)
				DEPT->head1 = cur->next;
			else
				prev->next = cur->next;

			cout << "Student with ID: " << id << " has been Deleted!\n\n";

			deleteStudentHash(globalHashTable, id);
			delete cur;

			return;
		}

		prev = cur;
		cur = cur->next;
	}
}

void display(string d)
{
	department* dept = findDepartment(d);

	if (dept == nullptr) {
		cout << "Department not found!\n\n";
		return;
	}

	cout << "\n===== Students in " << d << " Department =====\n";

	if (dept->head1 == nullptr) {
		cout << "No students in this department.\n";
	}
	else {
		Student* temp = dept->head1;
		int count = 1;
		while (temp != nullptr) {
			cout << count << ". ID: " << temp->id
				<< "\t| Name: " << temp->name
				<< "\t| GPA: " << temp->gpa
				<< "\t| Department: " << temp->dept->dept
				<< "\t| Semester: " << temp->sem->sem << "\n";
			temp = temp->next;
			count++;
		}
	}

	cout << "==========================================\n\n";
}

int main()
{
	globalHashTable = createHashTable(50);

	addDepartments("CS");
	addDepartments("Cys");
	addDepartments("SE");
	addDepartments("AI");

	addSemester("Spring2024");
	addSemester("Fall2024");
	addSemester("Spring2025");
	addSemester("Fall2025");

	loadFromFile("students.txt");

	int choice;
	string dept, sem, id, name;
	float gpa;

	string currentUser;
	int authChoice;

	loadUsers("users.txt");

	do {
		cout << "===== Welcome =====\n";
		cout << "1. Login\n2. Register\n0. Exit\nEnter choice: ";
		cin >> authChoice;

		cin.ignore();

		if (authChoice == 1)
		{
			string username, password;
			cout << "Username: ";
			getline(cin, username);
			cout << "Password: ";
			getline(cin, password);

			if (login(username, password))
			{
				cout << "Login successful! Welcome, " << username << "\n";
				currentUser = username;
				break;
			}
			else
			{
				cout << "Invalid username or password.\n";
			}

		}
		else if (authChoice == 2)
		{
			string username, password;
			cout << "Choose a Username: ";
			getline(cin, username);
			cout << "Choose a Password: ";
			getline(cin, password);

			registerUser(username, password);

		}
		else if (authChoice == 0)
		{
			cout << "Exiting Program...\n";
			return 0;
		}
		else
		{
			cout << "Invalid Choice! Try Again.\n";
		}
	} while (true);

	do {
		cout << "\n===== Student Management System =====\n";
		cout << "1. Add Student\n"
			<< "2. Search Student by ID\n"
			<< "3. Update Student GPA\n"
			<< "4. Delete Student\n"
			<< "5. Display Students in Department\n"
			<< "0. Exit\n"
			<< "Enter your choice: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
		{
			cin.ignore();
			cout << "Enter Semester (Spring2024, Fall2024, Spring2025, Fall2025): ";
			getline(cin, sem);
			if (!isValidSemester(sem))
			{
				cout << "Error: Invalid Semester! Choose from Spring2024, Fall2024, Spring2025, Fall2025.\n";
				break;
			}

			cout << "Enter Department (CS, Cys, SE, AI): ";
			getline(cin, dept);
			if (!isValidDepartment(dept))
			{
				cout << "Error: Invalid Department! Choose from CS, Cys, SE, AI.\n";
				break;
			}

			cout << "Enter Student ID (numbers only): ";
			getline(cin, id);
			if (!isNumber(id))
			{
				cout << "Error: Student ID must contain numbers only!\n";
				break;
			}
			if (searchStudentHash(globalHashTable, id))
			{
				cout << "Error: Student ID already exists!\n";
				break;
			}

			cout << "Enter Student Name: ";
			getline(cin, name);
			if (!isValidName(name))
			{
				cout << "Error: Invalid Name! Only letters and spaces allowed.\n";
				break;
			}

			cout << "Enter GPA (0.0 - 4.0): ";
			cin >> gpa;

			if (cin.fail())
			{
				cin.clear();
				cin.ignore(10000, '\n');
				cout << "Error: GPA must be a number!\n";
				break;
			}

			if (!isValidGPA(gpa))
			{
				cin.ignore(10000, '\n');
				cout << "Error: GPA must be between 0.0 and 4.0!\n";
				break;
			}

			cin.ignore(10000, '\n');

			addStudents(dept, sem, id, name, gpa);
			break;
		}

		case 2:
			cout << "Enter Student ID to Search: ";
			cin >> id;
			searchStudent(id);
			break;

		case 3:
			cout << "Enter Student ID to Update GPA: ";
			cin >> id;
			update(id);
			break;

		case 4:
			cout << "Enter Student ID to Delete: ";
			cin >> id;
			deleteStudent(id);
			break;

		case 5:
			cout << "Enter Department Name to Display Students: ";
			cin >> dept;
			display(dept);
			break;

		case 0:
			saveToFile("students.txt");
			cout << "Exiting Program...\n";
			break;

		default:
			cout << "Invalid Choice! Try Again.\n";
		}

	} while (choice != 0);

	return 0;
}