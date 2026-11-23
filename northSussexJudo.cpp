#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
using namespace std;


const int    WEEKS_PER_MONTH              = 4;    
const int    MAX_COACHING_HOURS_PER_WEEK  = 5;   
const int    MAX_COMPETITIONS_PER_MONTH   = 1;    
const double COACHING_RATE_PER_HOUR       = 9.50;
const double COMPETITION_FEE              = 22.00;

const int PLAN_COUNT     = 3;
const int CATEGORY_COUNT = 7;

struct PlanInfo {
    string name;
    int    sessionsPerWeek;
    double feePerWeek;
    bool   canCompete;      
};

struct CategoryInfo {
    string name;
    double lowerKg;    
    double upperKg;           
};

const PlanInfo PLANS[PLAN_COUNT] = {
    {"Beginner",     2, 25.00, false},
    {"Intermediate", 3, 30.00, true},
    {"Elite",        5, 35.00, true}
};

const CategoryInfo CATEGORIES[CATEGORY_COUNT] = {
    {"Extra-lightweight (up to 60 kg)",   0.0,   60.0},
    {"Half-lightweight (60-66 kg)",       60.0,  66.0},
    {"Lightweight (66-73 kg)",            66.0,  73.0},
    {"Half-middleweight (73-81 kg)",      73.0,  81.0},
    {"Middleweight (81-90 kg)",           81.0,  90.0},
    {"Half-heavyweight (90-100 kg)",      90.0,  100.0},
    {"Heavyweight (over 100 kg)",         100.0, 999.0}
};


string formatMoney(double amount) {
    ostringstream out;
    out << "GBP " << fixed << setprecision(2) << amount;
    return out.str();
}


class Athlete {
private:
    string name;
    int    planIndex;
    double weightKg;
    int    categoryIndex;
    int    competitions;
    int    weeklyCoachingHours;

public:
    Athlete() : name(""), planIndex(0), weightKg(0.0), categoryIndex(0),
                competitions(0), weeklyCoachingHours(0) {}

    Athlete(string n, int plan, double weight, int category,
            int comps, int coachingHours)
        : name(n), planIndex(plan), weightKg(weight),
          categoryIndex(category), competitions(comps),
          weeklyCoachingHours(coachingHours) {}


    string getName() const { return name; }
    string getPlanName() const { return PLANS[planIndex].name; }

    
    double getTrainingCost() const {
        return PLANS[planIndex].feePerWeek * WEEKS_PER_MONTH;
    }

    
    double getCoachingCost() const {
        return weeklyCoachingHours * WEEKS_PER_MONTH * COACHING_RATE_PER_HOUR;
    }


    double getCompetitionCost() const {
        return competitions * COMPETITION_FEE;
    }

    double getTotalCost() const {
        return getTrainingCost() + getCoachingCost() + getCompetitionCost();
    }

    
    string getWeightComparison() const {
        const CategoryInfo &cat = CATEGORIES[categoryIndex];
        ostringstream out;
        out << fixed << setprecision(1);
        if (weightKg > cat.upperKg) {
            out << "OVER the category limit by " << (weightKg - cat.upperKg)
                << " kg (" << weightKg << " kg vs " << cat.upperKg << " kg)";
        } else if (weightKg <= cat.lowerKg) {
            out << "UNDER the category minimum by "
                << (cat.lowerKg - weightKg) << " kg - a lighter category "
                << "may be more suitable";
        } else {
            out << "Within the category (" << weightKg << " kg)";
        }
        return out.str();
    }


    void printBill() const {
        const PlanInfo &plan = PLANS[planIndex];
        cout << "\n=============================================\n";
        cout << " Athlete : " << name << "\n";
        cout << "=============================================\n";
        cout << left << setw(34) << " Training plan"
             << plan.name << " (" << plan.sessionsPerWeek << " sessions/week)\n";
        cout << left << setw(34) << " Category"
             << CATEGORIES[categoryIndex].name << "\n\n";

        cout << left << setw(34) << " Item" << right << setw(14) << "Cost" << "\n";
        cout << " ---------------------------------------------\n";
        cout << left << setw(34)
             << (" " + plan.name + " plan x " + to_string(WEEKS_PER_MONTH) + " weeks")
             << right << setw(14) << formatMoney(getTrainingCost()) << "\n";
        cout << left << setw(34)
             << (" Private coaching " + to_string(weeklyCoachingHours * WEEKS_PER_MONTH) + " hrs")
             << right << setw(14) << formatMoney(getCoachingCost()) << "\n";
        cout << left << setw(34)
             << (" Competitions x " + to_string(competitions))
             << right << setw(14) << formatMoney(getCompetitionCost()) << "\n";
        cout << " ---------------------------------------------\n";
        cout << left << setw(34) << " TOTAL FOR THE MONTH"
             << right << setw(14) << formatMoney(getTotalCost()) << "\n\n";
        cout << " Weight check: " << getWeightComparison() << "\n";
        cout << "=============================================\n";
    }
};


void swapAthletes(Athlete &a, Athlete &b) {
    Athlete temp = a;
    a = b;
    b = temp;
}

int partition(vector<Athlete> &arr, int low, int high) {
    double pivot = arr[high].getTotalCost();
    int i = low - 1;
    for (int j = low; j < high; j++) {         
        if (arr[j].getTotalCost() > pivot) {   
            i++;
            swapAthletes(arr[i], arr[j]);
        }
    }
    swapAthletes(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<Athlete> &arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


string readLine(const string &prompt) {
    string line;
    cout << prompt;
    if (!getline(cin, line)) {       
        cout << "\nInput closed. Goodbye!\n";
        exit(0);
    }
    return line;
}

string readName(const string &prompt) {
    while (true) {
        string value = readLine(prompt);
        size_t start = value.find_first_not_of(" \t");
        if (start == string::npos) {
            cout << "  Error: the name cannot be empty. Please try again.\n";
            continue;
        }
        size_t end = value.find_last_not_of(" \t");
        return value.substr(start, end - start + 1);
    }
}

int readInt(const string &prompt, int minValue, int maxValue) {
    while (true) {
        string line = readLine(prompt);
        try {
            size_t used = 0;
            int value = stoi(line, &used);
            if (used != line.size()) {
                throw invalid_argument("trailing characters");
            }
            if (value < minValue || value > maxValue) {
                cout << "  Error: enter a whole number from " << minValue
                     << " to " << maxValue << ".\n";
                continue;
            }
            return value;
        } catch (const exception &) {
            cout << "  Error: '" << line << "' is not a valid whole number.\n";
        }
    }
}

double readDouble(const string &prompt, double minValue, double maxValue) {
    while (true) {
        string line = readLine(prompt);
        try {
            size_t used = 0;
            double value = stod(line, &used);
            if (used != line.size()) {
                throw invalid_argument("trailing characters");
            }
            if (value < minValue || value > maxValue) {
                cout << "  Error: enter a number from " << minValue
                     << " to " << maxValue << ".\n";
                continue;
            }
            return value;
        } catch (const exception &) {
            cout << "  Error: '" << line << "' is not a valid number.\n";
        }
    }
}


Athlete createAthlete() {
    string name = readName("Enter athlete name: ");

    cout << "\nTraining plans:\n";
    for (int i = 0; i < PLAN_COUNT; i++) {
        cout << "  " << (i + 1) << ". " << PLANS[i].name << " - "
             << PLANS[i].sessionsPerWeek << " sessions/week, "
             << formatMoney(PLANS[i].feePerWeek) << " per week\n";
    }
    int plan = readInt("Choose plan (1-" + to_string(PLAN_COUNT) + "): ",
                       1, PLAN_COUNT) - 1;

    double weight = readDouble("Enter current weight in kg: ", 20.0, 250.0);

    cout << "\nCompetition weight categories:\n";
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        cout << "  " << (i + 1) << ". " << CATEGORIES[i].name << "\n";
    }
    int category = readInt("Choose category (1-" + to_string(CATEGORY_COUNT) + "): ",
                           1, CATEGORY_COUNT) - 1;

    int competitions = 0;
    if (PLANS[plan].canCompete) {
        competitions = readInt("Competitions entered this month (0-"
                               + to_string(MAX_COMPETITIONS_PER_MONTH) + "): ",
                               0, MAX_COMPETITIONS_PER_MONTH);
    } else {
        cout << "\n  Note: only Intermediate and Elite athletes can enter "
                "competitions. Competitions set to 0.\n";
    }

    int coachingHours = 0;
    char answer = ' ';
    while (answer != 'y' && answer != 'n') {
        string reply = readLine("Add private coaching? (y/n): ");
        answer = reply.empty() ? ' ' : (char)tolower(reply[0]);
        if (reply.size() != 1 || (answer != 'y' && answer != 'n')) {
            cout << "  Error: please type y or n.\n";
            answer = ' ';
        }
    }
    if (answer == 'y') {
        coachingHours = readInt("Private coaching hours per week (0-"
                                + to_string(MAX_COACHING_HOURS_PER_WEEK) + "): ",
                                0, MAX_COACHING_HOURS_PER_WEEK);
    }

    return Athlete(name, plan, weight, category, competitions, coachingHours);
}


void displayAthletes(const vector<Athlete> &athletes) {
    cout << "\n" << left << setw(5) << "No" << setw(22) << "Name"
         << setw(15) << "Plan" << right << setw(14) << "Monthly total" << "\n";
    cout << string(56, '-') << "\n";
    for (size_t i = 0; i < athletes.size(); i++) {
        cout << left << setw(5) << (i + 1)
             << setw(22) << athletes[i].getName()
             << setw(15) << athletes[i].getPlanName()
             << right << setw(14) << formatMoney(athletes[i].getTotalCost())
             << "\n";
    }
}

void displayRanked(const vector<Athlete> &athletes) {
    vector<Athlete> copy = athletes;         
    quickSort(copy, 0, (int)copy.size() - 1);
    cout << "\nAthletes ranked by monthly cost (highest first):";
    displayAthletes(copy);
}

void showMenu() {
    cout << "\n------ North Sussex Judo ------\n"
         << "1. Register a new athlete\n"
         << "2. Show an athlete's monthly bill\n"
         << "3. List all athletes\n"
         << "4. Rank athletes by monthly cost (quicksort)\n"
         << "0. Quit\n";
}


int main() {
    vector<Athlete> athletes;
    
    athletes.push_back(Athlete("Aiko Tanaka",     2, 59.5, 0, 1, 2));
    athletes.push_back(Athlete("Ben Carter",      1, 68.0, 1, 1, 0));
    athletes.push_back(Athlete("Chloe Evans",     0, 72.4, 2, 0, 1));
    athletes.push_back(Athlete("Dev Patel",       2, 80.2, 3, 1, 5));
    athletes.push_back(Athlete("Emma Wilson",     1, 85.0, 4, 0, 3));
    athletes.push_back(Athlete("Femi Adeyemi",    0, 101.5, 6, 0, 0));

    cout << " North Sussex Judo Fee Management System";

    while (true) {
        showMenu();
        int choice = readInt("Choose an option: ", 0, 4);

        if (choice == 0) {
            cout << "Goodbye!\n";
            return 0;
        } else if (choice == 1) {
            Athlete newAthlete = createAthlete();
            athletes.push_back(newAthlete);
            newAthlete.printBill();
        } else if (choice == 2) {
            displayAthletes(athletes);
            int index = readInt("Enter athlete number: ", 1, (int)athletes.size());
            athletes[index - 1].printBill();
        } else if (choice == 3) {
            displayAthletes(athletes);
        } else if (choice == 4) {
            displayRanked(athletes);
        }
    }
}
