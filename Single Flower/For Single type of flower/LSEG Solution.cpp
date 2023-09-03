#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> // Include this for sorting
#include <cctype>    // Include this for isspace function
#include <set> // Include this for std::set

using namespace std;

struct CSVRow {
    string ord; // New column
    string column1;
    string column2;
    string column3;
    string newColumn; // New column between column3 and column4
    string column4;
    string column5;
};

// Trim whitespace from the beginning and end of a string
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

        row.ord = "ord" + to_string(data.size() + 1); // Set the new column

        getline(ss, row.column1, ',');
        getline(ss, row.column2, ',');
        getline(ss, row.column3, ',');
        row.newColumn = "New"; // Set the new column value
        getline(ss, row.column4, ',');
        getline(ss, row.column5, ',');

        // Trim whitespace from column3 value
        trim(row.column3);

        data.push_back(row);
    }

    file.close();
    return data;
}

// Function to add a row to the appropriate table
void addToTable(vector<CSVRow>& buyTable, vector<CSVRow>& sellTable, const CSVRow& row) {
    if (row.column3 == "1") {
        buyTable.push_back(row);
    }
    else if (row.column3 == "2") {
        sellTable.push_back(row);
    }
}

// Function to write a row to the output.csv file
void writeLineToOutputCSV(const CSVRow& row) {
    std::ofstream csvFile("output.csv", std::ios_base::app); // Open in append mode

    if (!csvFile.is_open()) {
        std::cerr << "Error opening output.csv file." << std::endl;
        return;
    } 

    // Write the line to output.csv
    csvFile << row.ord << "," << row.column1 << "," << row.column2 << "," << row.column3 << ","
        << row.newColumn << "," << row.column4 << "," << row.column5 << std::endl;

    // Close the output.csv file
    csvFile.close();
}


bool validateOrder(CSVRow& row) {

    // Check if any required field is empty
    if (row.column1.empty() || row.column2.empty() || row.column3.empty() ||
        row.column4.empty() || row.column5.empty()) {
        row.newColumn = "Reject";
        //cout << "1" << "\n";
        return false;
    }

    // Trim whitespace from column2
    trim(row.column2);

    // Define a set of valid instruments
    std::set<std::string> validInstruments = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};

    // Check if column2 is not in the set of valid instruments
    if (validInstruments.find(row.column2) == validInstruments.end()) {
        row.newColumn = "Reject";
       //cout << "2" << "\n";
        return false;
    }

    // Trim whitespace from column3
    trim(row.column3);

    // Check for invalid Side (must be "1" or "2")
    if ((stoi(row.column3) > 2) || (stoi(row.column3) < 1)) {
        row.newColumn = "Reject";
        //cout << "3" << "\n";
        return false;
    }

    // Check if Price is not greater than 0
    if (stoi(row.column5) <= 0) {
        row.newColumn = "Reject";
        //cout << "4" << "\n";
        return false;
    }

    // Check if Quantity is not a multiple of 10
    if (stoi(row.column4) % 10 != 0) {
        row.newColumn = "Reject";
        //cout << "5" << "\n";
        return false;
    }

    // Check if Quantity is outside the range [10, 1000]
    if (stoi(row.column4) < 10 || stoi(row.column4) > 1000) {
        row.newColumn = "Reject";
       // cout << "6" << "\n";
        return false;
    }

    return true;
}



int main() {
    string filename = "example_6.csv"; // Change this to your .csv file's path
    vector<CSVRow> csvData = readCSV(filename);

    vector<CSVRow> buyTable;  // For column 3 value 1
    vector<CSVRow> sellTable; // For column 3 value 2

    // Create and open the .csv file
    std::ofstream csvFile("output.csv");

    // Check if the file is open
    if (!csvFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    // Write header row
    csvFile << "Order ID,Client Order,Instrument, Side, Exec Status, Quantity, Price" << std::endl;

    for (const CSVRow& row : csvData) {

        
        // Validate the current row
        bool isValid = validateOrder(const_cast<CSVRow&>(row));

        // Check the validation result
        if (!isValid) {
            // If the row is invalid, it has already been marked as "Reject" in the newColumn
            // Write the rejected row to output.csv
            writeLineToOutputCSV(row);
            continue;
        }

        // Add the row to buy or sell Table
        addToTable(buyTable, sellTable, row);

        // Check either buyTable or sellTable is empty
        if (buyTable.empty() || sellTable.empty()) {
            // if this condition satisfies, push a copy of this line to output.csv file and read the next line in .csv file
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
            

            
            
            



            // Here, check whether the line that was read lastly is a sell item (if row.column3 == "2") or a buy item (if row.column3 == "1").
            const CSVRow& lastRow = row; // Get the last-read row


            if (lastRow.column3 == "2") {
                for (auto sellRow = sellTable.begin(); sellRow != sellTable.end() && ((sellRow->newColumn == "New")|| (sellRow->newColumn == "PFill")); ++sellRow) {
                    for (CSVRow& buyRow : buyTable) {
                        // Compare the sell price and buy price after converting to integers
                        if (stoi(sellRow->column5) <= stoi(buyRow.column5)) {
                            // Check the amount of the orders after converting to integers
                            if (stoi(sellRow->column4) > stoi(buyRow.column4)) {
                                buyRow.newColumn = "Fill";

                                // Create another row in the sell table by changing newColumn = "Pfill", column5 = buyRow.column5, and column4 = buyRow.column4
                                CSVRow newSellRow;
                                newSellRow.ord = sellRow->ord; // Set a new order ID
                                newSellRow.column1 = sellRow->column1;
                                newSellRow.column2 = sellRow->column2;
                                newSellRow.column3 = sellRow->column3;
                                newSellRow.newColumn = "PFill";
                                newSellRow.column4 = buyRow.column4;
                                newSellRow.column5 = buyRow.column5;

                                // Update the existing sell row
                                sellRow->column4 = to_string(stoi(sellRow->column4) - stoi(buyRow.column4));
                                sellRow->newColumn = "PFill";

                                // Write newSellRow to output.csv
                                writeLineToOutputCSV(newSellRow);

                                // Write buyRow to output.csv
                                writeLineToOutputCSV(buyRow);

                                if (stoi(sellRow->column4) == 0) {
                                    break;
                                }
                            }

                            else if (stoi(sellRow->column4) == stoi(buyRow.column4)) {
                                buyRow.newColumn = "Fill";
                                sellRow->newColumn = "Fill";

                                sellRow->column5 = buyRow.column5;

                                // Write newSellRow to output.csv
                                writeLineToOutputCSV(*sellRow);

                                // Write buyRow to output.csv
                                writeLineToOutputCSV(buyRow);

                                break;
                            }
                            // If sell order is less than buy order
                            else {
                                sellRow->newColumn = "Fill";

                                // Create another row in the buy table by changing newColumn = "PFill" and column4 = sellRow.column4
                                CSVRow newBuyRow;
                                newBuyRow.ord = buyRow.ord; // Set a new order ID
                                newBuyRow.column1 = buyRow.column1;
                                newBuyRow.column2 = buyRow.column2;
                                newBuyRow.column3 = buyRow.column3;
                                newBuyRow.newColumn = "Pfill";
                                newBuyRow.column4 = sellRow->column4;
                                newBuyRow.column5 = buyRow.column5;

                                // Update the existing sell row
                                buyRow.column4 = to_string(stoi(buyRow.column4) - stoi(sellRow->column4));
                                buyRow.newColumn = "PFill";

                                // Write newSellRow to output.csv
                                writeLineToOutputCSV(newBuyRow);

                                // Write buyRow to output.csv
                                writeLineToOutputCSV(*sellRow);

                                //Create an iterator pointing to the first row(header) in buyTable
                                /*auto sellRowIterator = sellTable.begin();

                                // Erase the first row (header)
                                buyTable.erase(sellRowIterator);*/
                                

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

                    // Here I delete rows that have row.column4 = "Fill" in both buy table and sell table.
                    // After processing, remove rows with "Fill" in the newColumn
                    buyTable.erase(
                        std::remove_if(buyTable.begin(), buyTable.end(),
                            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
                        buyTable.end());

                    sellTable.erase(
                        std::remove_if(sellTable.begin(), sellTable.end(),
                            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
                        sellTable.end());

                }
               
            }

            else if (lastRow.column3 == "1") {
                for (auto buyRow = buyTable.begin(); buyRow != buyTable.end() && ((buyRow->newColumn == "New") || (buyRow->newColumn == "PFill")); ++buyRow) {
                    for (CSVRow& sellRow : sellTable) {
                        // Compare the sell price and buy price after converting to integers
                        if (stoi(sellRow.column5) <= stoi(buyRow->column5)) {
                            // Check the amount of the orders after converting to integers
                            if (stoi(buyRow->column4) > stoi(sellRow.column4)) {
                                sellRow.newColumn = "Fill";

                                // Create another row in the buy table by changing newColumn = "Pfill", column5 = sellRow.column5, and column4 = sellRow.column4
                                CSVRow newBuyRow;
                                newBuyRow.ord = buyRow->ord; // Set a new order ID
                                newBuyRow.column1 = buyRow->column1;
                                newBuyRow.column2 = buyRow->column2;
                                newBuyRow.column3 = buyRow->column3;
                                newBuyRow.newColumn = "PFill";
                                newBuyRow.column4 = sellRow.column4;
                                newBuyRow.column5 = sellRow.column5;

                                // Update the existing buy row
                                buyRow->column4 = to_string(stoi(buyRow->column4) - stoi(sellRow.column4));
                                buyRow->newColumn = "PFill";

                                // Write newBuyRow to output.csv
                                writeLineToOutputCSV(newBuyRow);

                                // Write sellRow to output.csv
                                writeLineToOutputCSV(sellRow);

                                /*if (stoi(buyRow->column4) == 0) {
                                    break;
                                }*/
                            }

                            else if (stoi(buyRow->column4) == stoi(sellRow.column4)) {
                                sellRow.newColumn = "Fill";
                                buyRow->newColumn = "Fill";

                                buyRow->column5 = sellRow.column5;

                                // Write newBuyRow to output.csv
                                writeLineToOutputCSV(*buyRow);

                                // Write sellRow to output.csv
                                writeLineToOutputCSV(sellRow);

                                break;
                            }
                            // If buy order is less than sell order
                            else {
                                buyRow->newColumn = "Fill";

                                // Create another row in the sell table by changing newColumn = "PFill" and column4 = buyRow.column4
                                CSVRow newSellRow;
                                newSellRow.ord = sellRow.ord; // Set a new order ID
                                newSellRow.column1 = sellRow.column1;
                                newSellRow.column2 = sellRow.column2;
                                newSellRow.column3 = sellRow.column3;
                                newSellRow.newColumn = "Pfill";
                                newSellRow.column4 = buyRow->column4;
                                newSellRow.column5 = sellRow.column5;

                                // Update the existing sell row
                                sellRow.column4 = to_string(stoi(sellRow.column4) - stoi(buyRow->column4));
                                sellRow.newColumn = "PFill";

                                // Write newSellRow to output.csv
                                writeLineToOutputCSV(newSellRow);

                                // Write buyRow to output.csv
                                writeLineToOutputCSV(*buyRow);

                                //Create an iterator pointing to the first row(header) in buyTable
                                /*auto sellRowIterator = sellTable.begin();

                                // Erase the first row (header)
                                buyTable.erase(sellRowIterator); */

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

                    // Here I delete rows that have row.column4 = "Fill" in both buy table and sell table.
                    // After processing, remove rows with "Fill" in the newColumn
                    buyTable.erase(
                        std::remove_if(buyTable.begin(), buyTable.end(),
                            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
                        buyTable.end());

                    sellTable.erase(
                        std::remove_if(sellTable.begin(), sellTable.end(),
                            [](const CSVRow& row) { return row.newColumn == "Fill"; }),
                        sellTable.end());


                    // Check sellTable and buyTable
                    /*cout << "Sell Table:" << endl;
                    for (const CSVRow& row : sellTable) {
                        cout << row.ord << ", " << row.column1 << ", " << row.column2 << ", " << row.column3 << ", "
                            << row.newColumn << ", " << row.column4 << ", " << row.column5 << endl;
                    }

                    cout << "Buy Table:" << endl;
                    for (const CSVRow& row : buyTable) {
                        cout << row.ord << ", " << row.column1 << ", " << row.column2 << ", " << row.column3 << ", "
                            << row.newColumn << ", " << row.column4 << ", " << row.column5 << endl;
                    }
                    */

                }
            }
        }
    }

    // Close the file
    csvFile.close();

   /* cout << "Buy Table:" << endl;
    for (const CSVRow& row : buyTable) {
        cout << row.ord << ", " << row.column1 << ", " << row.column2 << ", " << row.column3 << ", "
            << row.newColumn << ", " << row.column4 << ", " << row.column5 << endl;
    }

    cout << "Sell Table:" << endl;
    for (const CSVRow& row : sellTable) {
        cout << row.ord << ", " << row.column1 << ", " << row.column2 << ", " << row.column3 << ", "
            << row.newColumn << ", " << row.column4 << ", " << row.column5 << endl;
    }
    
    */
    return 0;
}
