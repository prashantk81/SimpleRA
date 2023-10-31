#include "cursor.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT).
 *
 */
class Table
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string tableName = "";
    vector<string> columns;
    vector<unsigned int> distinctValuesPerColumnCount;
    unsigned int columnCount = 0;
    long long int rowCount = 0;
    unsigned int blockCount = 0;
    unsigned int maxRowsPerBlock = 0;
    vector<unsigned int> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;
    int matDimensions = 0;
    bool isMatrix = false;
    bool isMatrixRenamed = false;
    string newMatrixName = "";
    string oldMatrixName = "";
    int readAccess = 0;
    int writeAccess = 0;
    bool extractColumnNames(string firstLine);
    bool blockify();
    bool blockifyMatrix();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(string tableName, vector<string> columns);
    bool load();
    bool loadMatrix();
    vector<vector<int> > readMatrix(string &fileName);
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void renameMatrix(string &newMatrixName);
    bool renameFilesInDirectory(const string &oldMatrixName, const string &newMatrixName, const string &directoryPath);
    void print();
    void printMatrix();
    void transpose();
    void transposeSqaureMatrix(vector<vector<int> > &elements);
    void transposeRectMatrix(vector<vector<int> > &elements, vector<vector<int> > &resultMatrix);
    void writeBackToFile(string &FileName, vector<vector<int> > &resultMatrix);
    bool checkSymmetryInDiagonalFiles(vector<vector<int> > &firstMatrix, vector<vector<int> > &secondMatrix);
    bool checkSymmetry();
    void performOperation(vector<vector<int> > &matrix1, vector<vector<int> > &matrix2, vector<vector<int> > &result);
    void changeSignOfMatrix(vector<vector<int> > &result);
    bool compute();
    void makePermanent();
    void makeMatrixPermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();
    void matrixUnload();
    string getMatrixTempFileName(string &matrixName, int rowIdx, int colIdx);
    void setReadWriteZero();
    void getBlockStatistics();
    void sortRelation(vector<string> &sortAttributes, vector<string> &sortAttributesStrategy, string resultRelationName);

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Table::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Table::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};