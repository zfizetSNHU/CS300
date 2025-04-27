#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

/* Zach Fizet
	SNHU CS-320
	Project 2
	4/14/2025
*/

using namespace std;

// Course object

struct Course {
	string courseId;
	string title;
	vector<string> prerequisites;
};

// Load course data from CSV file into hash map

void loadCourses(const string& filename, unordered_map<string, Course>& courseMap) {
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Error: Could not open file'" << filename << "'.\n" << endl;
		return;
	}

	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		string word;
		Course course;


		// get course ID
		if (!getline(ss, course.courseId, ',')) continue;

		// get course title
		if (!getline(ss, course.title, ',')) continue;

		// get prerequisites
		while (getline(ss, word, ',')) {
			course.prerequisites.push_back(word);
		}

		// store course in hash map
		courseMap[course.courseId] = course;
	}

	file.close();
	cout << "Courses loaded successfully." << endl;
}

// display a course and its prerequisites (if applicable)

void displayCourse(const Course& course, const unordered_map<string, Course>& courseMap) {
	cout << course.courseId << ": " << course.title << endl;
	if (!course.prerequisites.empty()) {
		cout << "Prerequisites: ";
		for (size_t i = 0; i < course.prerequisites.size(); ++i) {
			string preId = course.prerequisites[i];
			cout << preId;
			if (courseMap.count(preId)) {
				cout << " (" << courseMap.at(preId).title << ")";
			}
			if (i < course.prerequisites.size() - 1)
				cout << ", ";
		}
		cout << endl;
	}
	else {
		cout << "Prerequisites: None" << endl;
	}
}
// method for printing courses in a sorted order
void printSortedCourses(const unordered_map<string, Course>& courseMap) {
	vector<string> sortedKeys;

	// find and collect all course IDs that start with "CS"
	for (const auto& pair : courseMap) {
		if (pair.first.find("CS") == 0) {
			sortedKeys.push_back(pair.first);
		}
	}

	// sort course IDs
	sort(sortedKeys.begin(), sortedKeys.end());

	// display sorted CS course list
	cout << "\n--- Computer Science Course List ---\n";
	for (const string& courseId : sortedKeys) {
		cout << courseId << ": " << courseMap.at(courseId).title << endl;
	}
	cout << endl;
}

// Find and display a specific course
void findCourse(const unordered_map<string, Course>& courseMap) {
	cout << "Enter course ID: ";
	string courseId;
	cin >> courseId;

	auto it = courseMap.find(courseId);
	if (it != courseMap.end()) {
		displayCourse(it->second, courseMap);
	}
	else {
		cout << "Course not found." << endl;
	}
}

// Menu interface

void displayMenu() {
	unordered_map<string, Course> courseMap;
	string filename;
	bool isLoaded = false;
	
	// while loop for displaying menu options
	while (true) {
		cout << "\n Menu:\n";
		cout << "1. Load Course data\n";
		cout << "2. Print sorted list of CS courses\n";
		cout << "3. Print course information\n";
		cout << "9 Exit\n";
		cout << "Choose an option: ";


		// assigns the next input as the choice made by the user
		// corresponds to the switch cases below
		int choice;
		cin >> choice;

		// added for error handling invalid inputs
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input, please select a valid option.\n";
			continue;
		}

		// option one for loading file - utilizes the loadCourses method
		switch (choice) {
		case 1:
			cout << "Enter file name: ";
			cin >> filename;
			loadCourses(filename, courseMap);
			isLoaded = true;
			break;


		// option 2 - checks if the course data is loaded and then prints the courses via the sorted courses method
		case 2:
			if (!isLoaded) {
				cout << "Error, load course data first.\n";
			}
			else {
				printSortedCourses(courseMap);
			}
			break;

		// option 3 - checks for loaded course data and then finds a course based off the course ID, utilizing the findCourse method
		case 3:
			if (!isLoaded) {
				cout << "Error, load course data first.\n";
			}
			else {
				findCourse(courseMap);
			}
			break;

		// case for exiting program
		case 9:
			cout << "Exiting program." << endl;
			return;

		// default case - error handling if an option is invalid
		default:
			cout << "Input invalid, please select a valid option." << endl;
		}
	}
}
// main method for program - runs the displayMenu method
int main() {
	displayMenu();
	return 0;
}