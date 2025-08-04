#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <algorithm>
#include <limits>

using namespace std;

/* Zach Fizet
   SNHU CS-320
   Project 2 - Adaptive Data Structures + Strategy Pattern + Unit Tests
   4/14/2025
*/

// Struct to represent a course
struct Course {
    string courseId;
    string title;
    vector<string> prerequisites;
};

// Strategy interface to support different storage backends
class ICourseStorage {
public:
    virtual void addCourse(const Course& course) = 0;
    virtual const Course* getCourse(const string& courseId) const = 0;
    virtual vector<Course> getAllCourses() const = 0;
    virtual ~ICourseStorage() {}
};

// Strategy 1: Unordered map for fast lookups (O(1) average)
class HashCourseStorage : public ICourseStorage {
private:
    unordered_map<string, Course> courseMap;

public:
    void addCourse(const Course& course) override {
        courseMap[course.courseId] = course;
    }

    const Course* getCourse(const string& courseId) const override {
        auto it = courseMap.find(courseId);
        if (it != courseMap.end()) {
            return &it->second;
        }
        return nullptr;
    }

    vector<Course> getAllCourses() const override {
        vector<Course> courses;
        for (const auto& pair : courseMap) {
            if (pair.first.find("CS") == 0)
                courses.push_back(pair.second);
        }
        sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
            return a.courseId < b.courseId;
        });
        return courses;
    }
};

// Strategy 2: Tree map for sorted storage (O(log n) inserts, sorted output)
class TreeCourseStorage : public ICourseStorage {
private:
    map<string, Course> courseTree;

public:
    void addCourse(const Course& course) override {
        courseTree[course.courseId] = course;
    }

    const Course* getCourse(const string& courseId) const override {
        auto it = courseTree.find(courseId);
        if (it != courseTree.end()) {
            return &it->second;
        }
        return nullptr;
    }

    vector<Course> getAllCourses() const override {
        vector<Course> courses;
        for (const auto& pair : courseTree) {
            if (pair.first.find("CS") == 0)
                courses.push_back(pair.second);
        }
        return courses; // Already sorted
    }
};

// Display a course and its prerequisites
void displayCourse(const Course& course, const ICourseStorage& storage) {
    cout << course.courseId << ": " << course.title << endl;
    if (!course.prerequisites.empty()) {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            string preId = course.prerequisites[i];
            cout << preId;
            const Course* prereq = storage.getCourse(preId);
            if (prereq != nullptr) {
                cout << " (" << prereq->title << ")";
            }
            if (i < course.prerequisites.size() - 1)
                cout << ", ";
        }
        cout << endl;
    } else {
        cout << "Prerequisites: None" << endl;
    }
}

// Load course data from file into the current storage strategy
void loadCourses(const string& filename, ICourseStorage& storage) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string word;
        Course course;

        if (!getline(ss, course.courseId, ',')) continue;
        if (!getline(ss, course.title, ',')) continue;
        while (getline(ss, word, ',')) {
            course.prerequisites.push_back(word);
        }

        storage.addCourse(course);
    }

    file.close();
    cout << "Courses loaded successfully." << endl;
}

// Print CS courses in sorted order
void printSortedCourses(const ICourseStorage& storage) {
    vector<Course> courses = storage.getAllCourses();
    cout << "\n--- Computer Science Course List ---\n";
    for (const auto& course : courses) {
        cout << course.courseId << ": " << course.title << endl;
    }
    cout << endl;
}

// Prompt user to find and view a course
void findCourse(const ICourseStorage& storage) {
    cout << "Enter course ID: ";
    string courseId;
    cin >> courseId;

    const Course* course = storage.getCourse(courseId);
    if (course) {
        displayCourse(*course, storage);
    } else {
        cout << "Course not found." << endl;
    }
}

// Ask user which storage strategy they want to use
unique_ptr<ICourseStorage> chooseStrategy() {
    cout << "Select data structure strategy:\n";
    cout << "1. Fast lookups (unordered_map)\n";
    cout << "2. Sorted storage (map)\n";
    cout << "Enter choice: ";

    int strategy;
    cin >> strategy;

    if (strategy == 2) {
        cout << "Using map for sorted access.\n";
        return make_unique<TreeCourseStorage>();
    } else {
        cout << "Using unordered_map for fast access.\n";
        return make_unique<HashCourseStorage>();
    }
}

// Unit test runner for both strategies
void runTests() {
    cout << "\n--- Running Unit Tests ---\n";

    auto testStorage = [](const string& name, unique_ptr<ICourseStorage> storage) {
        cout << "Testing " << name << "...\n";

        Course cs101 = { "CS101", "Intro to Computer Science", {} };
        Course cs102 = { "CS102", "Data Structures", { "CS101" } };

        storage->addCourse(cs101);
        storage->addCourse(cs102);

        const Course* result = storage->getCourse("CS101");
        if (result && result->title == "Intro to Computer Science") {
            cout << "[PASS] getCourse(\"CS101\")\n";
        } else {
            cout << "[FAIL] getCourse(\"CS101\")\n";
        }

        result = storage->getCourse("MATH101");
        if (!result) {
            cout << "[PASS] getCourse(\"MATH101\") not found\n";
        } else {
            cout << "[FAIL] getCourse(\"MATH101\") should not exist\n";
        }

        vector<Course> csCourses = storage->getAllCourses();
        if (csCourses.size() == 2 && csCourses[0].courseId < csCourses[1].courseId) {
            cout << "[PASS] getAllCourses() sorted order\n";
        } else {
            cout << "[FAIL] getAllCourses() sorting or size issue\n";
        }

        cout << endl;
    };

    testStorage("HashCourseStorage", make_unique<HashCourseStorage>());
    testStorage("TreeCourseStorage", make_unique<TreeCourseStorage>());
}

// Menu interface
void displayMenu() {
    unique_ptr<ICourseStorage> storage = chooseStrategy();
    bool isLoaded = false;
    string filename;

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Load course data\n";
        cout << "2. Print sorted list of CS courses\n";
        cout << "3. Print course information\n";
        cout << "4. Run unit tests\n";
        cout << "9. Exit\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input, please try again.\n";
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Enter file name: ";
            cin >> filename;
            loadCourses(filename, *storage);
            isLoaded = true;
            break;

        case 2:
            if (!isLoaded) {
                cout << "Please load course data first.\n";
            } else {
                printSortedCourses(*storage);
            }
            break;

        case 3:
            if (!isLoaded) {
                cout << "Please load course data first.\n";
            } else {
                findCourse(*storage);
            }
            break;

        case 4:
            runTests();
            break;

        case 9:
            cout << "Exiting program." << endl;
            return;

        default:
            cout << "Invalid selection.\n";
        }
    }
}

// Program entry point
int main() {
    displayMenu();
    return 0;
}
