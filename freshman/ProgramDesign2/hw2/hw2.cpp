#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class employee
{
public:
    int id;
    int age;
    char gender;
    int salary;

    employee(int Id, int Age, char Gender, int Salary)
    {
        id = Id;
        age = Age;
        gender = Gender;
        salary = Salary;
    }

    void print()
    {
        cout << "id:" << id << ", age:" << age << ", gender:" << gender
             << ", salary:" << salary << '\n';
    }

    bool greater(employee e)
    {
        return (salary > e.salary || (salary == e.salary && age > e.age) ||
                (salary == e.salary && age == e.age && gender == 'm' &&
                 e.gender == 'f') ||
                (salary == e.salary && age == e.age && gender == 'm' &&
                 e.gender == 'f' && id > e.id));
    }
};


int main(int argc, char **argv)
{
    fstream file(argv[1], ios::in);
    vector<employee> emp;

    if (file.is_open()) {
        string cur[4];
        while (getline(file, cur[0], ','), getline(file, cur[1], ','),
               getline(file, cur[2], ','), getline(file, cur[3], '\n')) {
            employee emp1(stoi(cur[0]), stoi(cur[1]), cur[2][0], stoi(cur[3]));
            emp.push_back(emp1);
        }
    }
    // sort
    for (size_t i = 1; i < emp.size(); i++) {
        if (emp[i - 1].greater(emp[i])) {
            iter_swap(emp.begin() + i - 1, emp.begin() + i);
            i = 0;
        }
    }
    // print ans
    int cur_salary = emp[0].salary;
    cout << emp[0].salary;
    for (auto &a : emp) {
        if (cur_salary != a.salary) {
            cout << '\n' << a.salary;
            cur_salary = a.salary;
        }
        if (a.salary == cur_salary) {
            cout << "," << a.id;
        }
    }
    cout << '\n';
    return 0;
}