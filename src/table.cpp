#include "global.h"
#include <dirent.h>
#include <iomanip>
#include <sstream>
#include <cstring>
/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
    this->oldMatrixName = tableName;
    this->readAccess = 0;
    this->writeAccess = 0;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    // string msg = "Size of int: " + to_string(sizeof(int)) + " ColumnCount: " + to_string(columnCount);
    // logger.log(msg.c_str());
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
            {
                string msg = "Size of int: " + to_string(sizeof(int)) + " ColumnCount: " + to_string(this->columnCount) + " MaxRowsPerBlock: " + to_string(this->maxRowsPerBlock);
                logger.log(msg.c_str());
                return true;
            }
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */

bool Table::loadMatrix()
{
    logger.log("Table::loadMatrix");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->blockifyMatrix())
        {
            return true;
        }
    }
    fin.close();
    return false;
}

vector<vector<int>> Table::readMatrix(string &fileName)
{
    vector<vector<int>> matrix;
    ifstream fin(fileName, ios::in);
    string line;

    while (getline(fin, line))
    {
        vector<int> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ','))
        {
            row.push_back(stoi(cell));
        }

        matrix.push_back(row);
    }
    return matrix;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */

// 1 2 3 4 5 6 6 6
// 7 8 9 10 11 12 12 12
// 13 14 15 16 17 18 18 18
// 19 20 21 22 23 24 24 24
// 25 26 27 28 29 30 30 30
// 31 32 33 34 35 36 36 36
// 37 38 39 40 41 42 43 44
// 45 46 47 48 49 50 51 52

bool Table::blockifyMatrix()
{
    logger.log("Matrix::blockify");
    vector<vector<int>> matrix = this->readMatrix(this->sourceFileName);
    int pageCounter = 0;
    int n = matrix.size();
    this->matDimensions = matrix.size();
    int m = MATRIX_SIZE;
    int numRows = (n + m - 1) / m;
    int numCols = (n + m - 1) / m;
    int rowIdx = 1, colIdx;
    for (int i = 0; i < numRows; i++)
    {
        colIdx = 1;
        for (int j = 0; j < numCols; j++)
        {
            int rowStart = i * m, rowEnd = min((i + 1) * m, n);
            int colStart = j * m, colEnd = min((j + 1) * m, n);
            vector<vector<int>> temp(rowEnd - rowStart, vector<int>(colEnd - colStart));
            int p = 0, q = 0;
            for (int x = i * m; x < min((i + 1) * m, n); x++)
            {
                for (int y = j * m; y < min((j + 1) * m, n); y++)
                {
                    temp[p][q++] = matrix[x][y];
                }
                p++;
                q = 0;
            }
            bufferManager.writePage(this->tableName, this->blockCount, temp, rowIdx, colIdx);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
            colIdx++;
            this->writeAccess++;
        }
        rowIdx++;
    }
    return true;
}

bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);

    // print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    // print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();

        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload()
{
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
    {
        bufferManager.deleteFile(this->sourceFileName);
    }
}

string Table::getMatrixTempFileName(string &matrixName, int rowIdx, int colIdx)
{
    string fileName = "../data/temp/" + matrixName + "_Page_R" + to_string(rowIdx) + "_C" + to_string(colIdx);
    return fileName;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::matrixUnload()
{
    logger.log("Table::~matrixUnload");
    int matrixDim = this->matDimensions;
    int rows = (matrixDim + MATRIX_SIZE - 1) / (MATRIX_SIZE);
    int cols = rows;
    for (int i = 1; i <= rows; i++)
    {
        for (int j = 1; j <= cols; j++)
        {
            string fileName = getMatrixTempFileName(this->tableName, i, j);
            bufferManager.deleteFile(fileName);
        }
    }
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

/**
 * @brief Function that finds the transpose of a square matrix
 *
 */
void Table::transposeSqaureMatrix(vector<vector<int>> &elements)
{
    int n = elements.size();
    int m = elements[0].size();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < m; j++)
            swap(elements[j][i], elements[i][j]);
}

/**
 * @brief Function that finds the transpose of a rectangular matrix
 */
void Table::transposeRectMatrix(vector<vector<int>> &elements, vector<vector<int>> &resultMatrix)
{
    int n = elements.size();
    int m = elements[0].size();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            resultMatrix[j][i] = elements[i][j];
}

/**
 * @brief Function that writes the transpose matrix back to the file
 */
void Table::writeBackToFile(string &FileName, vector<vector<int>> &resultMatrix)
{
    std::ofstream file(FileName, std::ios::trunc);
    for (int i = 0; i < resultMatrix.size(); ++i)
    {
        for (int j = 0; j < resultMatrix[0].size(); j++)
        {
            file << resultMatrix[i][j];
            if (j != resultMatrix[0].size() - 1)
                file << ",";
        }
        file << endl;
    }
    file.close();
}

/**
 * @brief Function that transposes the matrix stored in the form
 * of a table
 *
 */
void Table::transpose()
{
    logger.log("Table::transpose");
    int totalfile = this->blockCount + 1;
    int totalRows, totalCol;
    totalCol = totalRows = sqrt(totalfile);
    int pageId = 0;
    string matrixFileName = this->tableName;
    for (int r = 1; r <= totalRows; r++)
    {
        for (int c = r; c <= totalCol; c++)
        {
            if (r == c)
            {
                string diagonalFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> elements = this->readMatrix(diagonalFileName);
                int rows = elements.size();
                int cols = elements[0].size();
                if (rows == cols)
                {
                    this->transposeSqaureMatrix(elements);
                    this->writeBackToFile(diagonalFileName, elements);
                }
                else
                {
                    vector<vector<int>> resultMatrix(cols, vector<int>(rows));
                    this->transposeRectMatrix(elements, resultMatrix);
                    this->writeBackToFile(diagonalFileName, resultMatrix);
                }
            }
            else
            {
                string firstFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> firstMatrixElements = this->readMatrix(firstFileName);
                string SecondFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(c) + "_C" + to_string(r);
                vector<vector<int>> SecondMatrixElements = this->readMatrix(SecondFileName);
                {
                    int rows = firstMatrixElements.size();
                    int cols = firstMatrixElements[0].size();
                    if (rows == cols)
                    {
                        this->transposeSqaureMatrix(firstMatrixElements);
                        this->writeBackToFile(SecondFileName, firstMatrixElements);
                    }
                    else
                    {
                        vector<vector<int>> resultMatrix(cols, vector<int>(rows));
                        this->transposeRectMatrix(firstMatrixElements, resultMatrix);
                        this->writeBackToFile(SecondFileName, resultMatrix);
                    }
                }
                {
                    int rows = SecondMatrixElements.size();
                    int cols = SecondMatrixElements[0].size();
                    if (rows == cols)
                    {
                        this->transposeSqaureMatrix(SecondMatrixElements);
                        this->writeBackToFile(firstFileName, SecondMatrixElements);
                    }
                    else
                    {
                        vector<vector<int>> resultMatrix(cols, vector<int>(rows));
                        this->transposeRectMatrix(SecondMatrixElements, resultMatrix);
                        this->writeBackToFile(firstFileName, resultMatrix);
                    }
                }
                this->readAccess++;
                this->writeAccess++;
            }
            pageId++;
            this->readAccess++;
            this->writeAccess++;
        }
    }
}

/**
 * @brief Function checks diagonal symmetry
 *
 * @returns bool
 */
bool Table::checkSymmetryInDiagonalFiles(vector<vector<int>> &firstMatrix, vector<vector<int>> &secondMatrix)
{
    for (int r = 0; r < firstMatrix.size(); r++)
    {
        for (int c = 0; c < firstMatrix[0].size(); c++)
        {
            if (firstMatrix[r][c] != secondMatrix[c][r])
            {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Functions checks symmetry of a matrix
 *
 * @returns bool
 */
bool Table::checkSymmetry()
{
    int totalfile = this->blockCount + 1;
    int totalRows, totalCol;
    totalCol = totalRows = sqrt(totalfile);
    string matrixFileName = this->tableName;
    for (int r = 1; r <= totalRows; r++)
    {
        for (int c = r; c <= totalCol; c++)
        {
            if (r == c)
            {
                string diagonalFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> elements = this->readMatrix(diagonalFileName);
                this->readAccess++;
                int rows = elements.size();
                int cols = elements[0].size();
                if (!this->checkSymmetryInDiagonalFiles(elements, elements))
                    return false;
            }
            else
            {
                string firstFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> firstMatrixElements = this->readMatrix(firstFileName);
                string SecondFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(c) + "_C" + to_string(r);
                vector<vector<int>> SecondMatrixElements = this->readMatrix(SecondFileName);
                this->readAccess += 2;
                if (!this->checkSymmetryInDiagonalFiles(firstMatrixElements, SecondMatrixElements))
                    return false;
            }
        }
    }
    return true;
}

/**
 * @brief Function computes element-wise subtraction between A and A'
 *
 */
void Table::performOperation(vector<vector<int>> &matrix1, vector<vector<int>> &matrix2, vector<vector<int>> &result)
{
    for (int r = 0; r < matrix1.size(); r++)
        for (int c = 0; c < matrix1[0].size(); c++)
            result[r][c] = matrix1[r][c] - matrix2[c][r];
}

/**
 * @brief Function changes the sign of element after compute operation
 *
 */
void Table::changeSignOfMatrix(vector<vector<int>> &result)
{
    for (int r = 0; r < result.size(); r++)
        for (int c = 0; c < result[0].size(); c++)
            if (result[r][c] != 0)
                result[r][c] = -1 * result[r][c];
}

/**
 * @brief Function which does subtraction between A and A'
 *
 */
bool Table::compute()
{
    int totalFiles = this->blockCount + 1;
    int totalRows, totalCol;
    totalCol = totalRows = sqrt(totalFiles);
    string matrixFileName = this->tableName;
    for (int r = 1; r <= totalRows; r++)
    {
        for (int c = r; c <= totalCol; c++)
        {
            if (r == c)
            {
                string diagonalFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> elements = this->readMatrix(diagonalFileName);
                this->readAccess++;
                vector<vector<int>> result(elements.size(), vector<int>(elements[0].size()));
                this->performOperation(elements, elements, result);

                fstream fout;
                string resultFileName = "../data/temp/" + matrixFileName + "_RESULT" + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);

                fout.open(resultFileName, ios::out | ios::app);
                this->writeBackToFile(resultFileName, result);
                this->writeAccess++;
            }
            else
            {
                string firstFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                vector<vector<int>> firstMatrixElements = this->readMatrix(firstFileName);

                string SecondFileName = "../data/temp/" + matrixFileName + "_Page" + "_R" + to_string(c) + "_C" + to_string(r);
                vector<vector<int>> SecondMatrixElements = this->readMatrix(SecondFileName);
                this->readAccess += 2;
                vector<vector<int>> result(firstMatrixElements.size(), vector<int>(firstMatrixElements[0].size()));
                this->performOperation(firstMatrixElements, SecondMatrixElements, result);

                fstream f1out;
                string resultFirstFileName = "../data/temp/" + matrixFileName + "_RESULT" + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
                f1out.open(resultFirstFileName, ios::out | ios::app);

                fstream f2out;
                string resultSecondFileName = "../data/temp/" + matrixFileName + "_RESULT" + "_Page" + "_R" + to_string(c) + "_C" + to_string(r);
                f2out.open(resultSecondFileName, ios::out | ios::app);

                this->writeBackToFile(resultFirstFileName, result);
                int rows = result.size();
                int cols = result[0].size();
                if (rows == cols)
                {
                    this->transposeSqaureMatrix(result);
                    this->changeSignOfMatrix(result);
                    this->writeBackToFile(resultSecondFileName, result);
                }
                else
                {
                    vector<vector<int>> resultMatrix(cols, vector<int>(rows));
                    this->transposeRectMatrix(result, resultMatrix);
                    this->changeSignOfMatrix(resultMatrix);
                    this->writeBackToFile(resultSecondFileName, resultMatrix);
                }
                this->writeAccess += 2;
            }
        }
    }
    return true;
}

/**
 * @brief Function that exports the matrix onto the disk
 *
 */
void Table::makeMatrixPermanent()
{
    logger.log("Table::makeMatrixPermanent");

    int totalfiles = this->blockCount + 1;
    int totalRows, totalCol;
    totalCol = totalRows = sqrt(totalfiles);
    // File which is already stored in the data folder
    // string matrixFileName = this->tableName;
    string matrixFileName = this->oldMatrixName;

    int matrixSize = this->matDimensions;

    int rowsCovered = 0;
    int rowsInOneFile = MATRIX_SIZE;
    string exportedFileName = "../data/" + matrixFileName + ".csv";
    // delete if file already exist
    const char *filename = exportedFileName.c_str();
    std::remove(filename);
    exportedFileName = "../data/" + this->tableName + ".csv";

    // create a new csv file
    fstream f1out;
    f1out.open(exportedFileName, ios::out | ios::app);

    bool flag = false;
    for (int r = 1; r <= totalRows; r++)
    {
        vector<vector<int>> temp;
        for (int c = 1; c <= totalCol; c++)
        {
            int rowIdx = 0;
            string fileName = "../data/temp/" + this->tableName + "_Page" + "_R" + to_string(r) + "_C" + to_string(c);
            fstream filePtr(fileName, ios::in);
            string word, line;
            if (filePtr.is_open())
            {
                while (getline(filePtr, line))
                {
                    vector<int> temp1;
                    stringstream str(line);
                    while (getline(str, word, ','))
                    {
                        if (c == 1)
                            temp1.push_back(stoi(word));
                        else
                            temp[rowIdx].push_back(stoi(word));
                    }
                    rowIdx++;
                    if (c == 1)
                        temp.push_back(temp1);
                }
            }
            this->readAccess++;
        }
        std::ofstream outFile(exportedFileName, std::ios::app);
        for (int i = 0; i < temp.size(); ++i)
        {
            if (flag)
                outFile << std::endl;
            for (int j = 0; j < temp[0].size(); j++)
            {
                flag = true;
                outFile << temp[i][j];
                if (j < temp[0].size() - 1)
                    outFile << ",";
            }
        }

        outFile.close();
    }
}

bool Table::renameFilesInDirectory(const string &oldMatrixName, const string &newMatrixName, const string &directoryPath)
{
    const char *dirPath = directoryPath.c_str();
    DIR *dir = opendir(dirPath);
    if (!dir)
    {
        cout << "Failed to open directory" << endl;
        return false;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
        if (entry->d_type == DT_REG)
        {
            string fName = entry->d_name;
            string fileName = directoryPath + entry->d_name;
            if (fName.find(oldMatrixName) == 0)
            {
                string newFileName = newMatrixName + fName.substr(oldMatrixName.size());
                newFileName = directoryPath + newFileName;
                if (rename(fileName.c_str(), newFileName.c_str()))
                {
                    cout << "Failed to rename" << endl;
                    closedir(dir);
                    return false;
                }
            }
        }
    }
    closedir(dir);
    return true;
}

/**
 * @brief Renames the matrix
 *
 */
void Table::renameMatrix(string &newMatrixName)
{
    logger.log("Table::renameMatrix");
    string oldMatrixName = this->tableName;

    if (tableCatalogue.renameMatrix(oldMatrixName, newMatrixName))
    {
        string directoryPath = "../data/temp/";
        if (this->renameFilesInDirectory(oldMatrixName, newMatrixName, directoryPath))
        {
            this->tableName = newMatrixName;
            this->isMatrixRenamed = true;
            // tableCatalogue.deleteTable(oldMatrixName);
        }
    }
}

/**
 * @brief Prints the matrix. Number of rows to be printed = min(actual_rows, 20)
 *
 */
void Table::printMatrix()
{
    logger.log("Table::printMatrix");
    int totalFiles = this->blockCount;
    string matrixFileName = this->tableName;
    int rowFiles = sqrt(totalFiles);
    int rowPrint = min(this->matDimensions, (int)PRINT_COUNT);
    vector<vector<string>> matrixValues;
    int rowsCovered = 0;
    int row = 1, flag = 0;
    for (row; row <= rowFiles; row++)
    {
        if (rowsCovered >= rowPrint)
        {
            row--;
            break;
        }
        string fileName = "../data/temp/" + matrixFileName + "_Page_R" + to_string(row) + "_C" + to_string(1);
        fstream filePtr(fileName, ios::in);
        string word, line;
        if (filePtr.is_open())
        {
            while (getline(filePtr, line))
            {
                if (rowsCovered < rowPrint)
                {
                    int columnCount = 0;
                    vector<string> temp;
                    stringstream str(line);
                    while (getline(str, word, ','))
                    {
                        if (columnCount < rowPrint)
                        {
                            temp.push_back(word);
                            columnCount++;
                        }
                    }
                    matrixValues.push_back(temp);
                    rowsCovered++;
                }
                else
                    flag = 1;
            }
        }
        this->readAccess++;
        if (flag)
            break;
    }
    int totalFileCoveredInRow = min(row, rowFiles);
    int totalColumnCoveredTillNow = matrixValues[0].size();
    int columnCount = totalColumnCoveredTillNow;
    for (int c = 2; c <= rowFiles and columnCount < rowPrint; c++)
    {
        int rowNumber = 0;
        for (int r = 1; r <= totalFileCoveredInRow; r++)
        {
            string fileName = "../data/temp/" + matrixFileName + "_Page_R" + to_string(r) + "_C" + to_string(c);
            fstream filePtr(fileName, ios::in);
            string word, line;
            if (filePtr.is_open())
            {
                while (getline(filePtr, line))
                {
                    int count = columnCount;
                    stringstream str(line);
                    while (getline(str, word, ','))
                    {

                        if (count < rowPrint && rowNumber < rowPrint)
                        {
                            matrixValues[rowNumber].push_back(word);
                            count++;
                        }
                        else
                            break;
                    }

                    rowNumber++;
                }
            }
            this->readAccess++;
        }
        columnCount = matrixValues[0].size();
    }
    for (int i = 0; i < matrixValues.size(); i++)
    {
        for (int j = 0; j < matrixValues[0].size(); j++)
        {
            cout << matrixValues[i][j] << " ";
        }
        cout << endl;
    }
}

void Table::setReadWriteZero()
{
    this->readAccess = this->writeAccess = 0;
}

/**
 * @brief Function that print:
 * Number of blocks read
 * Number of blocks written
 * Number of blocks accessed
 *
 */
void Table::getBlockStatistics()
{
    cout << "Number of blocks read: " << this->readAccess << endl;
    cout << "Number of blocks write: " << this->writeAccess << endl;
    cout << "Number of blocks accessed: " << this->readAccess + this->writeAccess << endl;
}

/**
 * @brief Copies the sorted content from newFile to oldFile
*/
bool copySortedContent(string &sourceFileName, string &destinationFileName) {

    ifstream sourceFile(sourceFileName);
    if (!sourceFile.is_open()) {
        cerr<<"Error opening source file."<<endl;
        return false;
    }

    ofstream destinationFile(destinationFileName);
    if (!destinationFile.is_open()) {
        cerr<<"Error opening destination file."<<endl;
        sourceFile.close(); // Close the source file if the destination file cannot be opened.
        return false;
    }

    string line;
    while(getline(sourceFile, line)) {
        destinationFile<<line<<endl;
    }

    sourceFile.close();
    destinationFile.close();
    return true;
}

/**
 * @brief Structure which acts as a comparator
 * for priority queue
*/
struct QueueComparator {
    const vector<int>& columnIndex;
    const vector<bool>& isAscending;

    QueueComparator(const vector<int>& columnIndex, const vector<bool>& isAscending)
        : columnIndex(columnIndex), isAscending(isAscending) {}

    bool operator()(const pair<vector<int>, pair<int, int> > &left, const pair<vector<int>, pair<int, int> > &right) const {
        for (size_t i = 0; i < columnIndex.size(); ++i) {
            int col = columnIndex[i];
            bool ascending = isAscending[i];

            if (left.first[col] < right.first[col])
                return ascending;
            else if (left.first[col] > right.first[col])
                return !ascending;
        }
        // If all columns are equal, maintain relative ordering.
        int pageLeft = left.second.first;
        int pageRight = right.second.first;
        return pageLeft > pageRight;
    }
};

/**
 * @brief Structure that sorts:
 * This struct helps us to sort
 * the rows of a page based on column_indices and sortStrategy
 * while maintaining relative order in case two rows are exactly the same
 *
 */
struct CustomComparator {
    const vector<vector<int>>& data;
    const vector<int>& colIndices;
    const vector<bool>& ascending;
    const vector<vector<int> >& copyArr;
    
    CustomComparator(const vector<vector<int>>& data, const vector<int>& colIndices, const vector<bool>& ascending, const vector<vector<int> > &copyArr)
        : data(data), colIndices(colIndices), ascending(ascending), copyArr(copyArr) {}
    
    bool operator()(const vector<int>& a, const vector<int>& b) const {
        for (size_t i = 0; i < colIndices.size(); ++i) {
            int colIndex = colIndices[i];
            if (a[colIndex] != b[colIndex]) {
                return ascending[i] ? a[colIndex] < b[colIndex] : a[colIndex] > b[colIndex];
            }
        }

        int aIt = find(copyArr.begin(), copyArr.end(), a) - copyArr.begin();
        int bIt = find(copyArr.begin(), copyArr.end(), b) - copyArr.begin();
        return aIt < bIt;
    }
};

void SortMergePages(Table* &resTable, vector<vector<vector<int> > > &pages, vector<int> &sortColumnIndices, vector<bool> &isAscending) {
    // Sorting step
    for(int i = 0; i < pages.size(); i++) {
        vector<vector<int> > copyArr = pages[i];
        CustomComparator comparator(pages[i], sortColumnIndices, isAscending, copyArr);
        // Sort the data using the custom comparator
        sort(pages[i].begin(), pages[i].end(), comparator);
    }
    vector<bool> reversedIsAscending;
    for(bool it: isAscending) {
        reversedIsAscending.push_back(!it);
    }
    // Merging step
    // {row, {pageIdx, rowIdx}}
    priority_queue<pair<vector<int>, pair<int, int> > , vector<pair<vector<int>, pair<int, int> > >, QueueComparator> pq(QueueComparator(sortColumnIndices, reversedIsAscending));
    for(int i = 0; i < pages.size(); i++) {
        pq.push({pages[i][0], {i, 0}});
    }

    while(!pq.empty()) {
        int pageIdx = pq.top().second.first, rowIdx = pq.top().second.second;
        pq.pop();
        resTable->writeRow<int>(pages[pageIdx][rowIdx]);
        if(rowIdx + 1 < pages[pageIdx].size()) {
            pq.push({pages[pageIdx][rowIdx + 1], {pageIdx, rowIdx + 1}});
        }
    }
    resTable->blockify();
    return;
}

void MergeTables(Table* &resTable, vector<Table*> &tables, vector<int> &sortColumnIndices, vector<bool> &isAscending) {
    vector<Cursor> cursors;
    for(int i = 0; i < tables.size(); i++) {
        cursors.push_back(tables[i]->getCursor());
    }

    vector<bool> reversedIsAscending;
    for(bool it: isAscending) {
        reversedIsAscending.push_back(!it);
    }
    // {vector<int>, {cursorIdx, 0}}
    priority_queue<pair<vector<int>, pair<int, int> > , vector<pair<vector<int>, pair<int, int> > >, QueueComparator> pq(QueueComparator(sortColumnIndices, reversedIsAscending));
    for(int i = 0; i < cursors.size(); i++) {
        vector<int> row = cursors[i].getNext();
        if(!row.empty()) {
            pq.push({row, {i, 0}});
        }
    }
    while(!pq.empty()) {
        int cursorIdx = pq.top().second.first;
        vector<int> row = pq.top().first;
        pq.pop();
        resTable->writeRow<int>(row);
        vector<int> nextRow = cursors[cursorIdx].getNext();
        if(!nextRow.empty()) {
            pq.push({nextRow, {cursorIdx, 0}});
        }
    }
    resTable->blockify();
}

/**
 * @brief Sorts the matrix based on the provided columns
 * SORT Student BY Stud_age IN DESC
 *
 */
void Table::sortRelation(vector<string> &sortAttributes, vector<string> &sortAttributesStrategy, string resultRelationName = "")
{
    logger.log("Table::sortRelation");
    string originalTableName = this->tableName;

    Table *resultTable;
    if(resultRelationName == "") resultTable = new Table("SortResult", this->columns);
    else resultTable = new Table(resultRelationName, this->columns);
    tableCatalogue.insertTable(resultTable);

    // Extract the index to attributes to be sorted
    vector<int> sortColumnIndices;
    for(int i = 0; i < sortAttributes.size(); i++) {
        sortColumnIndices.push_back(this->getColumnIndex(sortAttributes[i]));
    }

    // Maintain the asc/desc vector
    vector<bool> isAscending;
    for(int i = 0; i < sortAttributesStrategy.size(); i++) {
        isAscending.push_back(sortAttributesStrategy[i] == "ASC");
    }

    int allottedBuffer = BLOCK_COUNT_SORT;
    int M = allottedBuffer - 1;

    int blockCount = this->blockCount;
    int blockSet = 0;
    int phase = 0;
    int i = 0;
    vector<Table* > tables;

    while(i < blockCount) {
        vector<vector<vector<int> > > pages;
        int j = 0;
        while(j < M & i + j < blockCount) {
            Page temp = bufferManager.getPage(this->tableName, i + j);
            Page *page = &temp;
            // Get all the rows from page
            vector<vector<int> > rows = temp.getAllRows();
            pages.push_back(rows);
            j++;
        }
        i += j;
        Table *resTable = new Table("temp_file_" + to_string(blockSet) + "_phase_" + to_string(phase), this->columns);
        tableCatalogue.insertTable(resTable);
        SortMergePages(resTable, pages, sortColumnIndices, isAscending);

        tables.push_back(resTable);
        blockSet++;
    }
    blockSet = 0;
    phase++;
    while(tables.size() > 1) {
        vector<Table *> resTables;
        int i = 0;
        while(i < tables.size()) {
            vector<Table *> tempTables;
            int j = 0;
            while(j < M && i + j < tables.size()) {
                tempTables.push_back(tables[i + j]);
                j++;
            }
            i += j;
            Table* resTable = new Table("temp_file_" + to_string(blockSet) + "_phase_" + to_string(phase), this->columns);
            tableCatalogue.insertTable(resTable);
            MergeTables(resTable, tempTables, sortColumnIndices, isAscending);
            resTables.push_back(resTable);
            for(int k = 0; k < tempTables.size(); k++) {
                tableCatalogue.deleteTable(tempTables[k]->tableName);
            }
            blockSet++;
        }
        phase++;
        blockSet = 0;
        tables = resTables;
    }

    if(tables.size() == 1) {
        Table *sortedTable = tables[0];
        Cursor cursor = sortedTable->getCursor();
        vector<int> row = cursor.getNext();
        while(!row.empty()) {
            resultTable->writeRow<int>(row);
            row = cursor.getNext();
        }
        tableCatalogue.deleteTable(sortedTable->tableName);
        resultTable->blockify();
    }

    // Only in case the operation is SORT do this
    // In case the operation is ORDER BY don't do this
    if(resultRelationName == "") {
    // Copy the content of <newTable>_Page<id> to <table>_Page<id>
    // SortResult_Page0 -> Student_Page0
        for(int k = 0; k < blockCount; k++) {
            Page newPage = bufferManager.getPage(resultTable->tableName, k);
            vector<vector<int> > newData = newPage.getAllRows();
            // Removing the oldPage if it exists in the pool
            Page oldPage = bufferManager.getPage(this->tableName, k);
            bufferManager.eraseFromPool(oldPage.pageName);
            oldPage.updatePage(newData);
        }
        // Deleting the new relation
        tableCatalogue.deleteTable(resultTable->tableName);
    }
    return;
}
