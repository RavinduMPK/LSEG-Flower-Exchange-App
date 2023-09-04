#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <set>
#include <chrono>

using namespace std;

struct CSVRow {
    string ord;
    string column1;
    string column2;
    string column3;
    string newColumn;
    string column4;
    string column5;
};

void trim(string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start != string::npos && end != string::npos) {
        s = s.substr(start, end - start + 1);
    }
    else {
        s.clear();
    }
}

vector<CSVRow> readCSV(const string& filename) {
    vector<CSVRow> data;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        CSVRow row;

        row.ord = "ord" + to_string(data.size() + 1);

        getline(ss, row.column1, ',');
        getline(ss, row.column2, ',');
        getline(ss, row.column3, ',');
        row.newColumn = "New";
        getline(ss, row.column4, ',');
        getline(ss, row.column5, ',');

        trim(row.column3);

        data.push_back(row);
    }

    file.close();
    return data;
}

void addToTable(vector<CSVRow>& buyTable, vector<CSVRow>& sellTable, const CSVRow& row) {
    if (row.column3 == "1") {
        buyTable.push_back(row);
    }
    else if (row.column3 == "2") {
        sellTable.push_back(row);
    }
}

void writeLineToOutputCSV(const CSVRow& row) {
    std::ofstream csvFile("execution_rep.csv", std::ios_base::app);

    if (!csvFile.is_open()) {
        std::cerr << "Error opening execution_rep.csv file." << std::endl;
        return;
    }

    csvFile << row.ord << "," << row.column1 << "," << row.column2 << "," << row.column3 << ","
        << row.newColumn << "," << row.column4 << "," << row.column5 << std::endl;

    csvFile.close();
}

bool validateOrder(CSVRow& row) {
    if (row.column1.empty() || row.column2.empty() || row.column3.empty() ||
        row.column4.empty() || row.column5.empty()) {
        row.newColumn = "Reject";
        return false;
    }

    trim(row.column2);

    std::set<std::string> validInstruments = { "Rose", "Lavender", "Lotus", "Tulip", "Orchid" };

    if (validInstruments.find(row.column2) == validInstruments.end()) {
        row.newColumn = "Reject";
        return false;
    }

    trim(row.column3);

    if ((stoi(row.column3) > 2) || (stoi(row.column3) < 1)) {
        row.newColumn = "Reject";
        return false;
    }

    if (stoi(row.column5) <= 0) {
        row.newColumn = "Reject";
        return false;
    }

    if (stoi(row.column4) % 10 != 0) {
        row.newColumn = "Reject";
        return false;
    }

    if (stoi(row.column4) < 10 || stoi(row.column4) > 1000) {
        row.newColumn = "Reject";
        return false;
    }

    return true;
}

void matchOrders(vector<CSVRow>& buyTable, vector<CSVRow>& sellTable, int indicator) {
    for (auto buyRow = buyTable.begin(); buyRow != buyTable.end();) {
        if (buyRow->newColumn != "New" && buyRow->newColumn != "PFill") {
            ++buyRow;
            continue;
        }

        for (auto sellRow = sellTable.begin(); sellRow != sellTable.end();) {
            if (sellRow->newColumn != "New" && sellRow->newColumn != "PFill") {
                ++sellRow;
                continue;
            }

            if (indicator == 2) {
                    if (stoi(sellRow->column5) <= stoi(buyRow->column5)) {
                        if (stoi(sellRow->column4) > stoi(buyRow->column4)) {
                            buyRow->newColumn = "Fill";

                            CSVRow newSellRow;
                            newSellRow.ord = sellRow->ord;
                            newSellRow.column1 = sellRow->column1;
                            newSellRow.column2 = sellRow->column2;
                            newSellRow.column3 = sellRow->column3;
                            newSellRow.newColumn = "PFill";
                            newSellRow.column4 = buyRow->column4;
                            newSellRow.column5 = buyRow->column5;

                            sellRow->column4 = to_string(stoi(sellRow->column4) - stoi(buyRow->column4));
                            sellRow->newColumn = "PFill";

                            writeLineToOutputCSV(newSellRow);
                            writeLineToOutputCSV(*buyRow);
                            ++sellRow;
                        }
                        else if (stoi(sellRow->column4) == stoi(buyRow->column4)) {
                            buyRow->newColumn = "Fill";
                            sellRow->newColumn = "Fill";

                            sellRow->column5 = buyRow->column5;

                            writeLineToOutputCSV(*sellRow);
                            writeLineToOutputCSV(*buyRow);
                            ++sellRow;
                        }
                        else {
                            sellRow->newColumn = "Fill";
                            sellRow->column5 = buyRow->column5;

                            CSVRow newBuyRow;
                            newBuyRow.ord = buyRow->ord;
                            newBuyRow.column1 = buyRow->column1;
                            newBuyRow.column2 = buyRow->column2;
                            newBuyRow.column3 = buyRow->column3;
                            newBuyRow.newColumn = "PFill";
                            newBuyRow.column4 = sellRow->column4;
                            newBuyRow.column5 = buyRow->column5;

                            buyRow->column4 = to_string(stoi(buyRow->column4) - stoi(sellRow->column4));
                            buyRow->newColumn = "PFill";

                            writeLineToOutputCSV(*sellRow);
                            writeLineToOutputCSV(newBuyRow);
                            ++sellRow;
                        }
                    }

                    else {
                        if (sellRow->newColumn == "New") {
                            writeLineToOutputCSV(*sellRow);
                            break;
                        }
                        else {
                            break;
                        }
                    }
                
                
            }

            else if (indicator == 1) {
                    if (stoi(sellRow->column5) <= stoi(buyRow->column5)) {
                        if (stoi(buyRow->column4) > stoi(sellRow->column4)) {
                            sellRow->newColumn = "Fill";

                            CSVRow newBuyRow;
                            newBuyRow.ord = buyRow->ord;
                            newBuyRow.column1 = buyRow->column1;
                            newBuyRow.column2 = buyRow->column2;
                            newBuyRow.column3 = buyRow->column3;
                            newBuyRow.newColumn = "PFill";
                            newBuyRow.column4 = sellRow->column4;
                            newBuyRow.column5 = sellRow->column5;

                            buyRow->column4 = to_string(stoi(buyRow->column4) - stoi(sellRow->column4));
                            buyRow->newColumn = "PFill";

                            writeLineToOutputCSV(newBuyRow);
                            writeLineToOutputCSV(*sellRow);
                            ++sellRow;
                        }
                        else if (stoi(buyRow->column4) == stoi(sellRow->column4)) {
                            sellRow->newColumn = "Fill";
                            buyRow->newColumn = "Fill";

                            buyRow->column5 = sellRow->column5;

                            writeLineToOutputCSV(*buyRow);
                            writeLineToOutputCSV(*sellRow);
                            ++sellRow; //check
                        }
                        else {
                            buyRow->newColumn = "Fill";
                            buyRow->column5 = sellRow->column5;

                            CSVRow newSellRow;
                            newSellRow.ord = sellRow->ord;
                            newSellRow.column1 = sellRow->column1;
                            newSellRow.column2 = sellRow->column2;
                            newSellRow.column3 = sellRow->column3;
                            newSellRow.newColumn = "PFill";
                            newSellRow.column4 = buyRow->column4;
                            newSellRow.column5 = sellRow->column5;

                            sellRow->column4 = to_string(stoi(sellRow->column4) - stoi(buyRow->column4));
                            sellRow->newColumn = "PFill";

                            writeLineToOutputCSV(*buyRow);
                            writeLineToOutputCSV(newSellRow); 
                            ++sellRow;
                        }
                    }
                    else {
                        if (buyRow->newColumn == "New") {
                            writeLineToOutputCSV(*buyRow);
                            break;
                        }
                        else {
                            break;
                        }
                    }
                
            }
        }

        if (buyRow->newColumn == "New" || buyRow->newColumn == "PFill") {
            ++buyRow;
        }
    }

    buyTable.erase(
        std::remove_if(buyTable.begin(), buyTable.end(),
            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
        buyTable.end());

    sellTable.erase(
        std::remove_if(sellTable.begin(), sellTable.end(),
            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
        sellTable.end());
}

void writeExecutionTimeToOutputCSV(long long executionTime) {
    std::ofstream csvFile("execution_rep.csv", std::ios_base::app); // Open in append mode

    if (!csvFile.is_open()) {
        std::cerr << "Error opening execution_rep.csv file." << std::endl;
        return;
    }

    // Write the execution time to output.csv
    csvFile << ".........................................................." << endl;
    csvFile << "Execution Time (microseconds): " << executionTime << std::endl;

    // Close the output.csv file
    csvFile.close();
}


int main() {
    string filename = "orders.csv";
    vector<CSVRow> csvData = readCSV(filename);

    vector<CSVRow> buyTable;
    vector<CSVRow> sellTable;

    std::ofstream csvFile("execution_rep.csv");

    if (!csvFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    //csvFile << "Order ID,Client Order,Instrument, Side, Exec Status, Quantity, Price" << std::endl;

    // Measure the start time
    auto start = std::chrono::high_resolution_clock::now();

    for (const CSVRow& row : csvData) {
        bool isValid = validateOrder(const_cast<CSVRow&>(row));

        if (!isValid) {
            writeLineToOutputCSV(row);
            continue;
        }

        addToTable(buyTable, sellTable, row);

        if (buyTable.empty() || sellTable.empty()) {
            writeLineToOutputCSV(row);
            continue;
        }
        else {

            // Sort buy table in Descending order based on integer conversion of column5
            sort(buyTable.begin(), buyTable.end(), [](const CSVRow& a, const CSVRow& b) {
                return stoi(a.column5) > stoi(b.column5); // Descending order
                });


            // Sort sell table in Ascending order based on integer conversion of column5
            sort(sellTable.begin(), sellTable.end(), [](const CSVRow& a, const CSVRow& b) {
                return stoi(a.column5) < stoi(b.column5); // Ascending order
                });

            const CSVRow& lastRow = row;
            int indicator = stoi(lastRow.column3);
            //cout << indicator << "\n";


            matchOrders(buyTable, sellTable, indicator);
        }
    }

    // Measure the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the execution time in milliseconds
    auto executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Write the execution time to the output CSV
    writeExecutionTimeToOutputCSV(executionTime);

    csvFile.close();

    return 0;
}
