#include <time.h>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;

int randInt(int min, int max)
{
    return (rand() % (max - min + 1) + min);
}

int main(int argc, char **argv)
{
    fstream file("generated_input.txt", ios::out);
    srand(time(NULL));
    int N = 100000;
    for (int i = 0; i < N; i++) {
        int id, type, year, month, date, hour, minute;
        id = randInt(1, 9999);
        type = randInt(0, 1);
        year = randInt(1000, 3000);
        month = randInt(10, 12);
        date = randInt(10, 31);
        hour = randInt(10, 23);
        minute = randInt(10, 59);
        file << id << ',';
        if (type)
            file << "sign-out";
        else
            file << "sign-in";
        file << ',' << year << month << date << hour << minute;
        if (i != N - 1)
            file << '\n';
        /* code */
    }
}