#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <cstdio>
#include <sstream>

enum Grade
{
    A = 4,
    B = 3,
    C = 2,
    D = 1,
    F = 0,
    Ungraded = -1
};
// This enum represents which index a type of data is stored in. In context of a row in a csv file, which column it resides in.
enum Column
{
    id = 0,
    name = 1,
    email = 2,
    presentation = 3,
    essay = 4,
    term = 5
};
class StudentRecord
{
public:
    std::string name{};
    const std::string id{};
    std::string email{};
    StudentRecord()
    {
    }
    StudentRecord(std::string inID, std::string inName, std::string inEmail) : id{inID}, name{inName}, email{inEmail}
    {
    }
    StudentRecord(std::string inID, std::string inName, std::string inEmail, Grade inPresentationGrade, Grade inEssayGrade, Grade inTermProjectGrade)
        : id{inID}, name{inName}, email{inEmail},
          presentationGrade{inPresentationGrade}, essayGrade{inEssayGrade},
          termProjectGrade{inTermProjectGrade}
    {
    }
    Grade presentationGrade = Grade::Ungraded;
    Grade essayGrade = Grade::Ungraded;
    Grade termProjectGrade = Grade::Ungraded;
    void PrintRecord();
};
/*
* Uses the State Design Pattern to manage the state of the maintenance system.
Quite overkill for the scope of this project, and not fully-utilized because the
implementation requirements are simple enough.
*/
class SystemState
{
public:
    virtual void EnterState();
    virtual void ExitState();
    // MaintenanceSystem instance that is being represented within the state
    class MaintenanceSystem *maintenanceSystem;
};
void SystemState::EnterState()
{
}
void SystemState::ExitState()
{
}
class MenuState : public SystemState
{
public:
    virtual void EnterState() override;
    virtual void ExitState() override;
};
class NewRecordState : public SystemState
{
public:
    virtual void EnterState() override;
    virtual void ExitState() override;
};
class SearchingState : public SystemState
{
public:
    virtual void EnterState() override;
    virtual void ExitState() override;
};
class MaintenanceSystem
{
public:
    MaintenanceSystem();
    ~MaintenanceSystem();
    virtual void PromptUser();
    virtual void CreateRecord(const std::string inName, const std::string inID,const std::string inEmail);
    virtual void DeleteRecord(int rownum);
    virtual void EditRecord(int rownum);
    virtual void GotoState(class SystemState *newState);
    void PrintAllRecords();
    int FindRecordByID(const std::string inID);
    int FindRecordByIDNoPrint(const std::string inID);
    int FindRecordByName(const std::string inName);
    int FindRecordByEmail(const std::string inEmail);
    // few states the system can be in
    class MenuState *menuState;
    class NewRecordState *newRecordState;
    class SearchingState *searchingState;
    bool IsValidFieldInput(Column columnType, std::string input);
    const std::string GetRecordFilename() { return recordFilename; }

protected:
private:
    // File where record data is written to
    std::string recordFilename = "class_roll.csv";
    int FindRecordRowNum(const int colnum, const std::string input);
    int FindRecordRowNumNoPrint(const int colnum, const std::string input);
    void ReplaceFile(std::string replacedFilename, std::string successorFilename);
    void EditAllRecords();
    std::vector<std::string> SpliceCSVRow(std::string line);
    class SystemState *currentState;
    class SystemState *previousState;
};
void StudentRecord::PrintRecord()
{
    std::string printout = "[ ID: " + id + " | Name: " + name + " | Email: " + email + " | Essay Grade: " + std::to_string(essayGrade) + " |Presentation Grade: " + std::to_string(presentationGrade) + " | Term Project Grade: " +
                           std::to_string(termProjectGrade) + " ]";
    std::cout << printout << std::endl
              << "-----------------------------------------------------------------------------------------------------------" << std::endl;
}
MaintenanceSystem::MaintenanceSystem()
{
    // Having pointers to the maintenance system should be sufficient for the scope of the project
    menuState = new MenuState;
    menuState->maintenanceSystem = this;
    newRecordState = new NewRecordState;
    newRecordState->maintenanceSystem = this;
    searchingState = new SearchingState;
    searchingState->maintenanceSystem = this;
    currentState = nullptr;
    previousState = nullptr;
}
MaintenanceSystem::~MaintenanceSystem()
{
    delete menuState;
    delete newRecordState;
    delete searchingState;
}
/*Handles state transitions of the system*/
void MaintenanceSystem::GotoState(SystemState *newState)
{
    if (newState == nullptr)
    {
        std::cout << "Warning: newState is nullptr in MaintenanceSystem::GotoState"
                  << std::endl;
        return;
    }
    previousState = currentState;
    if (currentState != nullptr)
    {
        previousState->ExitState();
    }
    currentState = newState;
    currentState->EnterState();
}
// The main menu of the program
void MaintenanceSystem::PromptUser()
{
    std::cout << "\nOptions: " << std::endl;
    std::cout << " 0: Create student record" << std::endl;
    std::cout << " 1: Print all records" << std::endl;
    std::cout << " 2: Search for a student" << std::endl;
    std::cout << " 3: Edit field for all records" << std::endl;
    std::cout << " 4: Exit Program" << std::endl;

    std::string option_str; 
    int option;
    while (true) 
    {   
        std::cout << "\nSelection: ";
        std::cin >> option_str; 
        try {
            option = std::stoi(option_str);
            break;
        } catch (...) {
            std::cout << "Invalid option selected, please enter a number" << std::endl;
            continue;
        }
    }
    
    if (option > 4 || option < 0)
    {
        std::cout << "Invalid option selected, please try again" << std::endl;
        GotoState(menuState);
        return;
    }
    switch (option)
    {
    case 0:  // Create new record
        GotoState(newRecordState);
        break;
    case 1:  // Print all records
        PrintAllRecords();
        GotoState(menuState);
        break;
    case 2:  // Search for a single record
        GotoState(searchingState);
        break;
    case 3:  // Edit a field of all records at once
        EditAllRecords();
        break;
    case 4:  // Exit program
        exit(0);
        break;
    }
    return;
}
void MaintenanceSystem::CreateRecord(std::string inID, std::string inName, std::string inEmail)
{
    //@TODO - SHOULD convert records to a specific case when searching and creating entries, isn't required.
    StudentRecord Record = StudentRecord{inID, inName, inEmail};
    std::ofstream file;
    file.open(recordFilename, std::ios::app);
    file << inID << "," << inName << "," << inEmail << "," << Record.essayGrade << "," << Record.presentationGrade << "," << Record.termProjectGrade << std::endl;
    std::cout << "Record Created: ";
    Record.PrintRecord();
    file.close();
}
/*
* @Param replacedFilename - the original file that will be considered "old" after
new i/o operations.
* @Param successorFilename - the modified file that will be considered "new" after
new i/o operations.
The intention is not to rename a file, but copy old contents to a new file, and
maintain the same file name.
*/
void MaintenanceSystem::ReplaceFile(std::string replacedFilename, std::string
                                                                      successorFilename)
{
    // If anything goes wrong, a backupfile is generated containing the old information.
    std::string originalRenamedFilename = replacedFilename + ".bak";

    int result = std::rename(replacedFilename.c_str(),
                             originalRenamedFilename.c_str());
    if (result == 0)
    {
    }
    else
    {
        std::cout << "Failed to rename original file to backup name." << std::endl;
        return;
    }
    result = std::rename(successorFilename.c_str(), replacedFilename.c_str());
    if (result == 0)
    {
        if (std::remove(originalRenamedFilename.c_str()) == 0)
        {
        }
        else
        {
            std::cout << "Error with file operations when deleting record.[Removing original file]." << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to rename temp file." << std::endl;
    }
}
//@param rownum - the row (line) the record is found on.
void MaintenanceSystem::DeleteRecord(int rownum)
{
    std::ifstream originalFile;
    std::ofstream tempFile;
    originalFile.open(recordFilename);
    std::string tempFilename = "temp.csv";
    tempFile.open(tempFilename);
    std::string originalLine;
    std::string tempLine;
    int lineNumber = 1;
    while (std::getline(originalFile, originalLine))
    {
        if (lineNumber != rownum)
        {
            tempFile << originalLine << std::endl;
        }
        lineNumber++;
    }
    originalFile.close();
    tempFile.close();
    ReplaceFile(recordFilename, tempFilename);
}
//@param rownum - the row (line) the record is found on.
void MaintenanceSystem::EditRecord(int rownum)
{
    Column columnToModify;
    std::string modification;
    std::string printout = "Please select a data field to update:\n 1: ID\n 2: Name\n 3: Email\n 4: Presentation Grade\n 5: Essay Grade\n 6: Term Project Grade";
    std::cout << printout << std::endl;
    int option;
    std::string option_str;  
    do
    {
        std::cin >> option_str;
        try {
            option = std::stoi(option_str);
        } catch (...) {
            std::cout << "Invalid option selected, please enter a number" << std::endl;
            continue;
        }

        switch (option)
        {
        case 1:
            columnToModify = Column::id;
            break;
        case 2:
            columnToModify = Column::name;
            break;
        case 3:
            columnToModify = Column::essay;
            break;
        case 4:
            columnToModify = Column::presentation;
            break;
        case 5:
            columnToModify = Column::email;
            break;
        case 6:
            columnToModify = Column::term;
            break;
        }
        if (!(option > 0 && option <= 6))
        {
            std::cout << "Invalid Option. Please try again." << std::endl;
        }
    } while (!(option > 0 && option <= 6));
    do
    {
        std::cout << "Please enter the modification: " << std::endl;
        std::cin >> modification;
        if (!IsValidFieldInput(columnToModify, modification))
        {
            std::cout << "Invalid Column Data." << std::endl;
            return;
        }
    } while (!IsValidFieldInput(columnToModify, modification));
    std::ifstream originalFile;
    std::ofstream tempFile;
    originalFile.open(recordFilename);
    std::string tempFilename = "temp.csv";
    tempFile.open(tempFilename);
    std::string originalLine;
    std::string tempLine;
    std::stringstream ss;
    int lineNumber = 1;
    char delim = ',';
    while (std::getline(originalFile, originalLine))
    {
        if (lineNumber != rownum)
        {
            tempFile << originalLine << std::endl;
        }
        else
        {
            std::vector<std::string> row;
            row = SpliceCSVRow(originalLine);
            row[columnToModify] = modification;
            tempFile << row[Column::id] << "," << row[Column::name] << "," << row[Column::email] << "," << row[Column::presentation] << "," << row[Column::essay]
                     << "," << row[Column::term] << std::endl;
        }
        lineNumber++;
    }
    originalFile.close();
    tempFile.close();
    ReplaceFile(recordFilename, tempFilename);
    GotoState(menuState);
}
void MaintenanceSystem::PrintAllRecords()
{
    std::ifstream file;
    file.open(recordFilename);
    std::string line;
    std::cout << "\n\n\nList of all records: " << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    while (std::getline(file, line))
    {
        //@TODO if we care about fancy printing, use string stream
        std::cout << line << std::endl;
        std::cout << "------------------------------------------" << std::endl;
    }
    std::cout << "\n\n";
    file.close();
}
/*@param colnum - the column (datafield) we are searching by
@param input - the string we are searching for within row[colnum]
@return - an integer corresponding to the line the row was found on
*/
int MaintenanceSystem::FindRecordRowNum(int colnum, std::string input)
{
    std::ifstream file;
    file.open(recordFilename);
    std::string line;
    char delim = ',';
    int lineNumber = 1;
    int lineFound = -1;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, delim))
        {
            row.push_back(col);
        }
        // safety check to not try to access an element that doesnt exist
        if (colnum <= row.size() - 1 && row[colnum] == input)
        {
            lineFound = lineNumber;
            for (int i = 0; i < row.size(); i++)
            {
                std::cout << "Column " << i << ": " << row[i] << " | ";
            }
            std::cout << std::endl;
        }
        lineNumber++;
    }

    file.close();
    return lineFound;
}
int MaintenanceSystem::FindRecordRowNumNoPrint(int colnum, std::string input)
{
    std::ifstream file;
    file.open(recordFilename);
    std::string line;
    char delim = ',';
    int lineNumber = 1;
    int lineFound = -1;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, delim))
        {
            row.push_back(col);
        }
        lineNumber++; // Moved lineNumber++ to increment before the check, so the first line corresponds to lineNumber = 1
        if (colnum <= row.size() - 1 && row[colnum] == input)
        {
            lineFound = lineNumber;
            break;
        }
    }
    file.close();
    return lineFound;
}
// see FindRecordRowNum()
int MaintenanceSystem::FindRecordByID(std::string inID)
{
    return FindRecordRowNum(Column::id, inID);
}
int MaintenanceSystem::FindRecordByIDNoPrint(std::string inID)
{
    return FindRecordRowNumNoPrint(Column::id, inID);
}
// see FindRecordRowNum()
int MaintenanceSystem::FindRecordByName(std::string inName)
{
    return FindRecordRowNum(Column::name, inName);
}
// see FindRecordRowNum()
int MaintenanceSystem::FindRecordByEmail(std::string inEmail)
{
    return FindRecordRowNum(Column::email, inEmail);
}
void MaintenanceSystem::EditAllRecords()
{
    // Cannot modify fields containing search keys (i.e. UIDs, Names, or Emails). This would result in all records having identical information,
    // effectively making all records equally ambiguous.
    std::cout << "\nPlease select a valid field to modify:\n 1: Presentation Grade\n 2: Essay Grade\n 3: Term Project Grade\n";
    std::string option_str; 
    int option;

    while (true)
    {   
        std::cout << "Selection: ";
        std::cin >> option_str; 
        try {
            option = std::stoi(option_str);
            break;
        } catch (...) {
            std::cout << "\nInvalid option selected, please enter a number" << std::endl;
            continue;
        }
    }
    
    Column columnToModify;
    switch (option)
    {
    case 1:
        columnToModify = Column::presentation;
        break;
    case 2:
        columnToModify = Column::essay;
        break;
    case 3:
        columnToModify = Column::term;
        break;
    }
    if (!(option >= 1 && option <= 3))
    {
        std::cout << "\nEntered invalid menu option \n\n";
        EditAllRecords();
        return;
    }
    std::cout << "Please enter the modification: " << std::endl;
    std::string modification;
    std::cin >> modification;
    while (!IsValidFieldInput(columnToModify, modification))
    {
        std::cout << "Entered invalid field entry" << std::endl;
        std::cout << "Please enter the modification: " << std::endl;
        std::cin >> modification;
    }
    std::ifstream originalFile;
    std::ofstream tempFile;
    originalFile.open(recordFilename);
    std::string tempFilename = "temp.csv";
    tempFile.open(tempFilename);
    std::string originalLine;
    std::string tempLine;
    while (std::getline(originalFile, originalLine))
    {
        std::vector<std::string> row = SpliceCSVRow(originalLine);
        row[columnToModify] = modification;
        tempFile << row[Column::id] << "," << row[Column::name] << "," << row[Column::email] << "," << row[Column::presentation] << "," << row[Column::essay]
                 << "," << row[Column::term] << std::endl;
    }

    originalFile.close();
    tempFile.close();

    ReplaceFile(recordFilename, tempFilename);
    GotoState(menuState);
}
/*CSV line to splice. Splice is based on "," delimiter.
@return row - an array of strings with each index representing a column of the
csv file.
*/
std::vector<std::string> MaintenanceSystem::SpliceCSVRow(std::string line)
{
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string col;
    char delim = ',';
    while (std::getline(ss, col, delim))
    {
        row.push_back(col);
    }
    return row;
}
/*
 * @Param columnType - the field of data being input validated
 * @Param input - input to validate
 */
bool MaintenanceSystem::IsValidFieldInput(Column columnType, std::string input)
{
    int idCharLimit = 10;
    int nameCharLimit = 40;
    int emailCharLimit = 40;
    int lowestGrade = Grade::Ungraded;
    int highestGrade = Grade::A;
    int conv;
    switch (columnType)
    {
    case Column::id:
        return input.size() == idCharLimit;
        break;
    case Column::name:
        return input.size() <= nameCharLimit;
        break;
    case Column::email:
        return input.size() <= emailCharLimit;
        break;
    case Column::presentation:
        conv = std::stoi(input);
        return conv <= highestGrade && conv >= lowestGrade;
        break;
    case Column::essay:
        conv = std::stoi(input);
        return conv <= highestGrade && conv >= lowestGrade;
        break;
    case Column::term:
        conv = std::stoi(input);
        return conv <= highestGrade && conv >= lowestGrade;
        break;
    }
    return false;
}
void NewRecordState::EnterState()
{
    std::cout << "Create New Student Record" << std::endl;
    std::cout << "-------------------------" << std::endl
              << std::endl;
    //@TODO: optionally: ability for user to exit menu at any time
    // requirements mention nothing about handling duplicate records.
    std::string idNumber;
    int idCharRequirement = 10; // realistically declaring these does not reflect values defined inside the IsValidFieldInput - bad practice
    do
    {
        std::cout << "1. Enter ID Number (" << idCharRequirement << "Character Requirement): " << std::endl;
        std::cin >> idNumber;
        int selectedRow = maintenanceSystem->FindRecordByID(idNumber);
        if (selectedRow > 0)
        {
            std::cout << "The ID " << idNumber << " already exists. Please choose another ID Number." << std::endl;
            idNumber = "0";
        }
        if (!maintenanceSystem->IsValidFieldInput(Column::id, idNumber))
        {
            std::cout << "ID Numbers have a " << idCharRequirement << " Character Requirement." << std::endl;
        }
    } while (!maintenanceSystem->IsValidFieldInput(Column::id, idNumber));
    std::string name;
    int nameCharLimit = 40;
    do
    {
        std::cout << "2. Enter Name (up to" << nameCharLimit << "Characters): " << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore anything left in the input buffer
        std::getline(std::cin, name);                                       // Use getline to read the full line, which may contain spaces

        if (!maintenanceSystem->IsValidFieldInput(Column::name, name))
        {
            std::cout << "Names have a limit of " << nameCharLimit << "characters." << std::endl;
        }
    } while (!maintenanceSystem->IsValidFieldInput(Column::name, name));
    int emailCharLimit = 40;
    std::string email;
    do
    {
        std::cout << "3. Enter Email (up to" << emailCharLimit << "Characters):" << std::endl;
        std::getline(std::cin, email); // Use getline here as well

        if (!maintenanceSystem->IsValidFieldInput(Column::email, email))
        {
            std::cout << "Emails have a limit of " << emailCharLimit << " characters." << std::endl;
        }
    } while (!maintenanceSystem->IsValidFieldInput(Column::email, email));
    maintenanceSystem->CreateRecord(idNumber, name, email);
    maintenanceSystem->GotoState(maintenanceSystem->menuState);
}
void NewRecordState::ExitState()
{
}
void MenuState::EnterState()
{
    maintenanceSystem->PromptUser();
}
void MenuState::ExitState()
{
}
void SearchingState::EnterState()
{
    std::string printout = "Please select a field to search by:\n 1. ID: \n 2. Name: \n 3. Email: \n 0: Exit to main menu";
    std::cout << printout << std::endl;
    std::string option_str; 
    int option;

    while (true) 
    {
        std::cout << "\nSelection: ";
        std::cin >> option_str; 
        try {
            option = std::stoi(option_str);
            if (option > 3 || option < 0)
            {
                std::cout << "Invalid option selected, please try again" << std::endl;
                continue;
            } 
            break;
        } catch (...) {
            std::cout << "Invalid option selected, please enter a number" << std::endl;
            continue;
        }
    }

    std::string inputString;
    // Line of file, the row is on
    int selectedRow;
    switch (option)
    {
    case 0:
        maintenanceSystem->GotoState(maintenanceSystem->menuState);
        break;
    case 1:
        std::cout << "Enter a ID to Search by: ";
        std::cin >> inputString;
        selectedRow = maintenanceSystem->FindRecordByID(inputString);
        break;
    case 2:
        std::cout << "Enter a Name to Search by: ";
        std::cin >> inputString;
        selectedRow = maintenanceSystem->FindRecordByName(inputString);
        std::cout << "Retrieved record" << std::endl;
        break;
    case 3:
        std::cout << "Enter a Email to Search by: ";
        std::cin >> inputString;
        selectedRow = maintenanceSystem->FindRecordByEmail(inputString);
        break;
    }
    int firstLine = 1;
    if (selectedRow >= firstLine) // a row must be on a line >=1
    {
        std::cout << "Record found" << std::endl;
    }
    else
    {
        std::cout << "Record not found" << std::endl;
        maintenanceSystem->GotoState(maintenanceSystem->menuState);
    }
    printout = "Available Actions:\n 1: Edit Record: \n 2: Delete Record: \n 0: Exit to main menu";
    std::cout << "\n\n" << printout;
    
    while (true) 
    {
        std::cout << "\nSelection: ";
        std::cin >> option_str;
        try {
            option = std::stoi(option_str);
            if (option > 2 || option < 0)
            {
                std::cout << "Invalid option selected, please try again" << std::endl;
                continue;
            }
            break;
        } catch (...) {
            std::cout << "Invalid option selected, please enter a number" << std::endl;
            continue;
        }
    }

    switch (option)
    {
    case 0:
        maintenanceSystem->GotoState(maintenanceSystem->menuState);
        break;
    case 1:
        maintenanceSystem->EditRecord(selectedRow);
        break;
    case 2:
        printout = "Are you sure you want to delete this record?\n 1: YES, 0: NO";
        std::cout << printout << std::endl;
        while (true)
        {
            std::cout << "\nSelection: ";
            std::cin >> option_str;
            try {
                option = std::stoi(option_str);
                if (option > 1 || option < 0)
                {
                    std::cout << "Invalid option selected, please try again" << std::endl;
                    continue;
                }
                break;
            } catch (...) {
                std::cout << "Invalid option selected, please enter a number" << std::endl;
                continue;
            }
        }
        if (option == 1)
        {
            maintenanceSystem->DeleteRecord(selectedRow);
        }
        else if (option == 0)
        {
            std::cout << "Canceled deletion of record." << std::endl;
        }
        maintenanceSystem->GotoState(maintenanceSystem->menuState);
        break;
    }
    maintenanceSystem->GotoState(maintenanceSystem->menuState);
}
void SearchingState::ExitState()
{
}
int main()
{
    auto timeNow = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(timeNow);
    std::cout << "\n\n\n\n\nClass-Roll Maintenance System - " << std::ctime(&currentTime) 
              << "--------------------------------------------------------\n"
              << "(Note: please make all selections using integer number inputs)\n\n";
    MaintenanceSystem *systemInstance = new MaintenanceSystem();
    systemInstance->GotoState(systemInstance->menuState);
    return 0;
}
