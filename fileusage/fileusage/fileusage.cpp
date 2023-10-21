/*
 *Name:	    Yaksh Patel
 *Date:	    25 Nov 2021
 *FileName: fileusage.cpp
 *Purpose:  Create a C++ Windows console application that lists file usage. Files will be counted and their sizes totaled.
			Files will be grouped by file extension.
*Version : 1.0.0
 */
#include <iostream>
#include <locale>
#include <sstream>
#include <iomanip>
#include <regex>
#include <string>
#include <algorithm>
#include <vector>
#include <functional>
using namespace std;
#include <filesystem>
using namespace std::filesystem;

// summary info
ostringstream summary;
unsigned totalFiles = 0, totalCount = 0;
uintmax_t totalSize = 0;

// class for extensions and extension info
class Extension {
public:
	path extension_;
	unsigned count_ = 1;
	uintmax_t size_;
	Extension(path ext, uintmax_t size) : extension_(ext), size_(size) {};
};

// overriding operator
inline bool operator == (Extension const& ext, path const& path) {
	return ext.extension_ == path;
}

// overriding operator
inline bool operator < (Extension const& lhs, Extension const& rhs) {
	return lhs.extension_ < rhs.extension_;
}

// overriding operator
inline bool operator > (Extension const& lhs, Extension const& rhs) {
	return rhs < lhs;
}

// sorting bool
inline bool descending_by_size(Extension const& lhs, Extension const& rhs) {
	return rhs.size_ > lhs.size_;
}

// sorting bool
inline bool ascending_by_size(Extension const& lhs, Extension const& rhs) {
	return lhs.size_ > rhs.size_;
}

// overriding operator
ostream& operator << (ostream& os, Extension const& extension) {
	os << extension.extension_ << " " << extension.count_ << " " << extension.size_ << endl;
	return os;
}

// print each extension
void printExtension(Extension const& ext, unsigned& first, unsigned& second, unsigned& third) {
	cout << setw(first) << right << ext.extension_ << " : " << setw(second) << right << ext.count_ << " : " << setw(third) << right << ext.size_ << endl;
}

//print line of dashes
void printLine(unsigned& first, unsigned& second, unsigned& third) {
	cout << string(first, '-') << "   " << string(second, '-') << "   " << string(third, '-') << endl;
}

// printing the reuslts
void print(vector<Extension> const& files, regex const& exp) {
	unsigned coloumnOne = 0, coloumnTwo = 0, coloumnThree = 0;
	unsigned totalExt = 0, totalCount = 0;
	uintmax_t totalSize = 0;
	// getting dynamically aligned columns
	for (auto f : files) {
		if (regex_match(f.extension_.string(), exp)) {
			if (f.extension_.string().size() > coloumnOne)
				coloumnOne = f.extension_.string().size();
			if (to_string(f.count_).length() > coloumnTwo)
				coloumnTwo = to_string(f.count_).length();
			if (to_string(f.size_).length() > coloumnThree)
				coloumnThree = to_string(f.size_).length();
			totalExt++;
			totalCount += f.count_;
			totalSize += f.size_;
		}
	}
	coloumnOne += 5, coloumnTwo += 5, coloumnThree += 5;
	// aligned results
	cout << setw(coloumnOne) << right << "Ext" << " : " << setw(coloumnTwo) << right << "#" << " : " << setw(coloumnThree) << right << "Total" << endl;
	printLine(coloumnOne, coloumnTwo, coloumnThree);
	for (auto f : files) {
		if (regex_match(f.extension_.string(), exp))
			printExtension(f, coloumnOne, coloumnTwo, coloumnThree);
	}
	printLine(coloumnOne, coloumnTwo, coloumnThree);
	cout << setw(coloumnOne) << right << totalExt << " : " << setw(coloumnTwo) << right << totalCount << " : " << setw(coloumnThree) << right << totalSize << endl;
	// gathering info for summary
	++totalFiles;
	totalCount += totalCount;
	totalSize += totalSize;
	summary << setw(15) << right << totalCount << " : " << setw(20) << right << totalSize << endl;
}

// printing summary
void printSummary() {
	unsigned coloumnOne = 15, coloumnTwo = 15, coloumnThree = 20;
	cout << setw(coloumnOne) << right << "Ext" << " : " << setw(coloumnTwo) << right << "#" << " : " << setw(coloumnThree) << right << "Total" << endl;
	printLine(coloumnOne, coloumnTwo, coloumnThree);
	cout << summary.str();
	printLine(coloumnOne, coloumnTwo, coloumnThree);
	cout << setw(coloumnOne) << right << totalFiles << " : " << setw(coloumnTwo) << right << totalCount << " : " << setw(coloumnThree) << right << totalSize << endl;
}

// function for performing scan
void scan(vector<Extension>& files, path const& f, regex const& ext) {
	directory_iterator d(f), e;
	while (d != e) {
		if (!is_directory(d->status()) && regex_match(d->path().extension().string(), ext)) {
			if (find(files.begin(), files.end(), d->path().extension()) != files.end()) {
				(*find(files.begin(), files.end(), d->path().extension())).count_++;
				(*find(files.begin(), files.end(), d->path().extension())).size_ += file_size(d->path());
			}
			else {
				Extension current(d->path().extension(), file_size(d->path()));
				files.push_back(current);
			}
		}
		++d;
	}
}

// function for performing recursive scan
void rscan(vector<Extension>& files, path const& f, regex const& ext) {
	for (recursive_directory_iterator d(f), e; d != e; ++d) {
		if (!is_directory(d->status()) && regex_match(d->path().extension().string(), ext)) {
			if (find(files.begin(), files.end(), d->path().extension()) != files.end()) {
				(*find(files.begin(), files.end(), d->path().extension())).count_++;
				(*find(files.begin(), files.end(), d->path().extension())).size_ += file_size(d->path());
			}
			else {
				Extension current(d->path().extension(), file_size(d->path()));
				files.push_back(current);
			}
		}
	}
}


int main(int argc, char* argv[]) {
	
	summary.imbue(locale(""));
	cout.imbue(locale(""));
	cout << "fileusage.exe (c) 2021, Yaksh Patel\n\n";

	if (argc > 4) {
		cout << "Incorrect number of command line arguments.\nFor help: fileusage.exe -h\n";
		return EXIT_FAILURE;
	}
	
	vector<Extension> files;
	
	if (argc == 1) {
		cout << "All files: " << current_path() << endl;
		rscan(files, current_path(), (regex)".*");
		sort(files.begin(), files.end());
		print(files, (regex)".*");
		return EXIT_SUCCESS;
	}

	regex c_switch("c"), cpp_switch("\\+"), java_switch("j"), sharp_switch("#"),
		web_switch("w"), x_switch("x"), sum_switch("s"), rec_switch("r"), rev_switch("R"),
		sort_switch("S"), v_switch("v"), help_switch("h");

	if (!(regex_search(argv[1], (regex)"-c*"))) {
		cout << "All files: " << argv[1] << endl;
		scan(files, argv[1], (regex)".*");
		print(files, (regex)".*");
		return EXIT_SUCCESS;
	}

	if (regex_search(argv[1], help_switch)) {
		cout << "Usage: fileusage [-hrRsSvc+#jw(x regularexpression)] [folder]\n\n";
		return EXIT_SUCCESS;
	}

	regex c_ext(R"r(\.(c|h)$)r"), cpp_ext(R"r(\.(cc|cp|cpp|c\+\+|cxx|hpp|hxx)$)r"),
		java_ext(R"r(\.(class|j|jad|jar|java|jsp|ser)$)r"), sharp_ext(R"r(\.(cs|vb|jsl)$)r"),
		web_ext(R"r(\.(htm|html|html5|js|jse|jsc)$)r"), x_ext;

	// determining the path
	path myPath;
	if (argc == 2)
		myPath = current_path();
	else {
		if (regex_search(argv[1], x_switch)) {
			if (argc == 4)
				myPath = argv[3];
			else
				myPath = current_path();
		}
		else
			myPath = argv[2];
	}

	// recursive or not
	void(*pf)(vector<Extension>&, path const&, regex const&) = scan;
	if (regex_search(argv[1], rec_switch))
		pf = rscan;

	// regex extension for scan
	string scanExtensions = "\\.(";
	if (regex_search(argv[1], c_switch))
		scanExtensions += "c|h|";
	if (regex_search(argv[1], cpp_switch))
		scanExtensions += "cc|cp|cpp|c\\+\\+|cxx|hpp|hxx|";
	if (regex_search(argv[1], java_switch))
		scanExtensions += "class|j|jad|jar|java|jsp|ser|";
	if (regex_search(argv[1], sharp_switch))
		scanExtensions += "cs|vb|jsl|";
	if (regex_search(argv[1], web_switch))
		scanExtensions += "htm|html|html5|js|jse|jsc|";
	if (regex_search(argv[1], x_switch)) {
		string temp = argv[2];
		scanExtensions += temp.substr(2, temp.length() - 2);
		temp = temp.substr(2, temp.length() - 2);
		x_ext = R"r(\.)r" + temp;
	}
	else
		scanExtensions = scanExtensions.substr(0, scanExtensions.length() - 2);
	scanExtensions += ")$";
	
	// actual scan
	pf(files, myPath, (regex)scanExtensions);

	// sorting
	if (regex_search(argv[1], rev_switch) && regex_search(argv[1], sort_switch))
		sort(files.begin(), files.end(), descending_by_size);
	else if (regex_search(argv[1], rev_switch))
		sort(files.begin(), files.end(), greater<Extension>());
	else if (regex_search(argv[1], sort_switch))
		sort(files.begin(), files.end(), ascending_by_size);
	else
		sort(files.begin(), files.end());

	// printing the results
	if (regex_search(argv[1], c_switch)) {
		cout << "C files: " << myPath << endl;
		summary << setw(15) << right << "C files" << " : ";
		print(files, c_ext);
		cout << endl;
	}
	if (regex_search(argv[1], cpp_switch)) {
		cout << "C++ files: " << myPath << endl;
		summary << setw(15) << right << "C++ files" << " : ";
		print(files, cpp_ext);
		cout << endl;
	}
	if (regex_search(argv[1], java_switch)) {
		cout << "Java files: " << myPath << endl;
		summary << setw(15) << right << "Java files" << " : ";
		print(files, java_ext);
		cout << endl;
	}
	if (regex_search(argv[1], sharp_switch)) {
		cout << "C# files: " << myPath << endl;
		summary << setw(15) << right << "C# files" << " : ";
		print(files, sharp_ext);
		cout << endl;
	}
	if (regex_search(argv[1], web_switch)) {
		cout << "Web files: " << myPath << endl;
		summary << setw(15) << right << "Web files" << " : ";
		print(files, web_ext);
		cout << endl;
	}
	if (regex_search(argv[1], x_switch)) {
		string temp = argv[2];
		temp += " files: ";
		cout << temp << myPath << endl;
		summary << setw(15) << right << temp << " : ";
		print(files, x_ext);
		cout << endl;
	}
	if (regex_search(argv[1], sum_switch)) {
		cout << "All searches: " << myPath << endl;
		printSummary();
	}

}