#pragma once
#include "Main.hpp"

using namespace catapi;

int main() {
	CatApi->Done();
}

void Main::Done() {
	std::cout << "CatApi fact collector DB\n";
	if (fs::exists("./CatApi.db")) {
		int FunctionChosen = ChooseFunction();
		if (FunctionChosen == 1) {
			GetFact();
		}
		else if (FunctionChosen == 2) {
			GetAllFactEntries();
		}
		else {
			std::cout << "That wasn't a valid option!\n Please try again\n";
			Done();
		}
	}
	else {
		GetFact();
	}
}

int Main::ChooseFunction() {
	std::cout << "Input 1 to insert a new fact into the DB\n";
	std::cout << "Input 2 to see all entries\n";
	std::cout << "Input: ";
	int chosen;
	std::cin >> chosen;
	std::cout << "\n";
	return chosen;
}

void Main::GetFact() {
	Document jsonreader;
	cpr::Response r = cpr::Get(cpr::Url{ "https://catfact.ninja/fact" });

	if (jsonreader.Parse(r.text.c_str()).HasParseError()) {
		std::cout << "Error parsing json\n";
	}
	else {
		if (jsonreader.HasMember("fact")) {
			std::cout << "Fact received from the api\n";
			InsertFact(jsonreader["fact"].GetString());
		}
		else {
			GetFact();
			return;
		}
	}
}

void Main::InsertFact(string fact) {
	sqlite3* db;
	bool dbexists = true;

	if (!fs::exists("./CatApi.db")) {
		dbexists = false;
	}

	if (sqlite3_open("CatApi.db", &db) == SQLITE_OK) {
		std::cout << "Connection with database established! Inserting fact\n";

		if (!dbexists) {
			string sql = "CREATE TABLE FACTS(FACT TEXT NOT NULL);";
			int err;
			char* errmsg;
			try {
				err = sqlite3_exec(db, sql.c_str(), NULL, 0, &errmsg);

				if (err != SQLITE_OK) {
					std::cout << "Error creating table " << errmsg;
					sqlite3_free(errmsg);
					_sleep(5000);
					exit(-1);
				}
			}
			catch (const std::exception& e) {
				std::cout << e.what();
				_sleep(5000);
				exit(-1);
			}
		}
		if (NoDuplicates(fact)) {
			string sql = "insert into FACTS VALUES ('" + fact + "');";
			int err;
			char* errmsg;

			try {
				err = sqlite3_exec(db, sql.c_str(), NULL, 0, &errmsg);

				if (err != SQLITE_OK) {
					std::cout << "Error inserting fact " << errmsg;
					sqlite3_free(errmsg);
					_sleep(5000);
					exit(-1);
				}
			}
			catch (const std::exception& e) {
				std::cout << e.what();
				_sleep(5000);
				exit(-1);
			}
			sqlite3_close(db);
			std::cout << "Fact: " << fact << " was inserted into the database\n \n";
			Done();
		}
		else {
			std::cout << "Connection with database failed " << sqlite3_errmsg(db) << "\n";
			sqlite3_close(db);
			_sleep(5000);
			exit(-1);
		}
		sqlite3_close(db);
	}
}

bool Dupe = false;
int CheckDupe(void* notUsed, int resultsn, char** notUsed1, char** notUsed2) {
	if (resultsn <= 0) {
		Dupe = false;
	}
	else {
		Dupe = true;
	}
	return 0;
}

bool Main::NoDuplicates(string fact) {
	sqlite3* db;

	if (sqlite3_open("CatApi.db", &db) == SQLITE_OK) {
		string sql = "select * from FACTS WHERE fact = '" + fact + "';";
		int err;
		char* errmsg;

		try {
			err = sqlite3_exec(db, sql.c_str(), CheckDupe, NULL, &errmsg);

			if (err != SQLITE_OK) {
				std::cout << "Error checking for dupe " << errmsg;
				sqlite3_free(errmsg);
				_sleep(5000);
				exit(-1);
			}
		}
		catch (const std::exception& e) {
			std::cout << "Connection with database failed " << sqlite3_errmsg(db) << "\n";
			sqlite3_close(db);
			_sleep(5000);
			exit(-1);
		}
		sqlite3_close(db);
		if (Dupe) {
			return false;
		}
		else { return true; }
	}
	return true;
}
// End of function 1
char* SelectResult[9999];
int resultsn;
bool first = true;
int GetResults(void* NotUsed, int argc, char** argv, char** azColName) {
	resultsn = argc;
	if (first) {
		std::cout << "Printing all entries: \n \n";
		first = false;
	}

	for (int i = 0; i < resultsn; i++) {
		std::cout << argv[i] << "\n \n";
	}
	return 0;
}

void Main::GetAllFactEntries() {
	sqlite3* db;

	if (sqlite3_open("CatApi.db", &db) == SQLITE_OK) {
		string sql = "select * from FACTS;";
		int err;
		char* errmsg;

		try {
			err = sqlite3_exec(db, sql.c_str(), GetResults, 0, &errmsg);

			if (err != SQLITE_OK) {
				std::cout << "Error checking for duplicate " << errmsg;
				sqlite3_free(errmsg);
				_sleep(5000);
				exit(-1);
			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			_sleep(5000);
			exit(-1);
		}
		sqlite3_close(db);
		first = true;
		Done();
	}

}