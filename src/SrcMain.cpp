#include "SrcMain.h"
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include "Node.h"
#include <fstream>
#include <cstring>

extern int proccparse(); // NOLINT
struct yy_buffer_state; // NOLINT
extern void procc_flush_buffer(yy_buffer_state* b); // NOLINT
extern int procclex_destroy(); // NOLINT
extern FILE* proccin; // NOLINT

extern int gLineNumber;
extern NProgram* gProgram;
bool gSyntaxError = false;

// CHANGE ANYTHING ABOVE THIS LINE AT YOUR OWN RISK!!!!

int ProcessCommandArgs(int argc, const char* argv[])
{
	gLineNumber = 1;
	gSyntaxError = false;
	if (argc < 2)
	{
		std::cout << "You must pass the input file as a command line parameter." << std::endl;
		return 1;
	}

	// Read input from the first command line parameter
	proccin = fopen(argv[1], "r");
	if (proccin == nullptr)
	{
		std::cout << "File not found: " << argv[1] << std::endl;
		return 1;
	}
	procc_flush_buffer(nullptr);

	// Start the parse
	proccparse();

	if (gProgram != nullptr && argc == 3)
	{
		// TODO: Add any needed code for Parts 2, 3, and 4!!!
        std::ofstream ofile("ast.txt");
        gProgram->OutputAST(ofile, 0);
        if (std::strstr(argv[2], "emit") != nullptr){
            CodeContext context;
            gProgram->CodeGen(context);
            context.OutputEmit();
        }
        else if (std::strstr(argv[2], "reg") != nullptr){
            CodeContext context;
            gProgram->CodeGen(context);
            
            //implement Generate Interval
            std::map<std::string, int> start;
            std::map<std::string, int> end;
            int index = 0;
            for (std::vector<std::string> &paramsVec : context.params){
                for (std::string &str : paramsVec){
                    if (str[0] != '%') { continue; }
                    if (start.find(str) == start.end()){
                        start[str] = index;
                        end[str] = index;
                    }else{
                        end[str] = index;
                    }
                }
                ++index;
            }
            
            //Linear Scan
            std::map<std::string, int> mapToRealReg;
            std::vector<bool> regAvailable;
            regAvailable.resize(8, true);
            
            for (unsigned int i = 0; i < context.ops.size(); ++i){
                for (auto &temp : end){
                    if (temp.second == i){
                        regAvailable[mapToRealReg[temp.first]] = true;
                    }
                }
                for (std::string param : context.params[i]){
                    //the param did not occur yet
                    if (param[0] == '%' && mapToRealReg.find(param) == mapToRealReg.end()){
                        for (int i = 1; i < 8; ++i){
                            if (regAvailable[i]){
                                mapToRealReg[param] = i;
                                regAvailable[i] = false;
                                break;
                            }
                        }
                    }
                }
            }
            
            //print reg.txt;
            std::ofstream ofile("reg.txt");
            ofile << "INTERVALS:" << std::endl;
            for (int i = 0 ; i < context.nextReg; ++i){
                std::string param = '%' + std::to_string(i);
                int startTime = start[param];
                int endTime = end[param];
                ofile << param << ':' << startTime << ',' << endTime << std::endl;
            }
            ofile << "ALLOCATION:" << std::endl;
            for (int i = 0; i < context.nextReg; ++i)
            {
                std::string param = "%" + std::to_string(i);
                ofile << param << ':' << 'r' << mapToRealReg[param] << std::endl;
            }
            ofile.close();
            
            //print emit.txt
            ofile.open("emit.txt");
            for (unsigned int i = 0; i < context.ops.size(); ++i){
                ofile << context.ops[i];
                if (!context.params.empty()){
                    for (unsigned int j = 0 ; j < context.params[i].size(); ++j){
                        std::string vr = context.params[i][j];
                        if (j == 0){
                            if (vr[0] == '%'){
                                ofile << ' ' << 'r' << mapToRealReg[vr];
                            }else{
                                ofile << ' ' << vr;
                            }
                        }else{
                            if (vr[0] == '%'){
                                ofile << "," << 'r' << mapToRealReg[vr];
                            }else{
                                ofile << "," << vr;
                            }
                        }
                    }
                    ofile << std::endl;
                }
            }
        }
        ofile.close();
	}
	else
	{
		// (Just a useful separator for debug cout statements in grammar)
		std::cout << "**********************************************\n";
	}

	// Close the file stream
	fclose(proccin);
	// Destroy lexer so it reinitializes
	procclex_destroy();
	// Return 1 if syntax error, 0 otherwise
	return static_cast<int>(gSyntaxError);
}

void proccerror(const char* s) // NOLINT
{
	std::cout << s << " on line " << gLineNumber << std::endl;
	gSyntaxError = true;
}
