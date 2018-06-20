#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char ** argv)
{
	if (argc != 2) return 1;
	std::ifstream s(argv[1]);
	if (!s.is_open()) {
		std::cerr << "fail to open file!\n";
		return 1;
	}
	std::string l;
	while (std::getline(s,l)) if (l.substr(0,6) == "class ") {
		std::istringstream ii(l);
		std::string n;
		ii >> n >> n;
		std::cout << "#include \"" << argv[1] << "\"\n";
		std::cout << "#include <sim/runner.hh>\n";
		std::cout << "class Runner" << n << " :\n";
		std::cout << "\tprivate " << n << ",\n";
		std::cout << "\tvirtual public Runner\n";
		std::cout << "{\n";
		std::cout << "public:\n";
		std::cout << "\tRunner" << n << "(int argc, char ** argv, char ** envp) :\n";
		std::cout << "\t\tRunner(argc, argv, envp)\n";
		std::cout << "\t{}\n";
		std::cout << "};\n";
		std::cout << "int main(int argc, char ** argv, char ** envp) {\n";
		std::cout << "\tRunner" << n << " obj(argc, argv, envp);\n";
		std::cout << "\treturn obj.execute();\n";
		std::cout << "}\n";
	}
	return 0;
}
