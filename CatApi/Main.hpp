#pragma once
#include <iostream>
#include <rapidjson/document.h>
#include <cpr/cpr.h>
#include <sqlite3.h>
#include <filesystem>

using namespace rapidjson;
using std::string;
namespace fs = std::filesystem;

namespace catapi {
	class Main {
	public:
		void Done();
		int ChooseFunction();
		// Function 1
		void GetFact();
		void InsertFact(string fact);
		bool NoDuplicates(string fact);
		// Function 2
		void GetAllFactEntries();
	};

	inline std::unique_ptr<Main> CatApi;
}